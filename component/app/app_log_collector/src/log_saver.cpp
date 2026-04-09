#include "log_saver.h"
#include "cdsem_path_util.h"
#include "log_table_sql_config.h"
#include "sem_common.h"
#include "sem_exception.h"

#ifdef QT_DEBUG
#include <cas_lock.h>
#include <QStandardPaths>
#include "sem_file_reader.h"
#define CONNECT_PATH_ID +cdsem_test::readPathId()
#define DISCONNECT_PATH_ID(STR) cdsem_test::removePathId(STR)
namespace cdsem_test {
    QString _pathId;
    CasLock pathIdLock;
    QString writePathId(const QString& path) {
        QString id = QString::number(QDateTime::currentMSecsSinceEpoch(), 16).toUpper();
        sem::saveStringToFile(id, path);
        return id;
    }

    QString& readPathId() {
        if (_pathId.size() > 0) {
            return _pathId;
        }
        CasLocker locker(pathIdLock);
        if (_pathId.size() > 0) {
            return _pathId;
        }
        QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/hengren_cdsem_log_path.txt";
        _pathId = sem::readFileAsQString(path);
        if (_pathId.isEmpty()) {
            _pathId = writePathId(path);
        }
        return _pathId;
    }

    QString removePathId(const QString& path) {
        QString pathId = readPathId();
        if (path.endsWith(pathId)) {
            return path.left(path.length() - pathId.length());
        }
        return path;
    }
};
#else
#define CONNECT_PATH_ID
#define DISCONNECT_PATH_ID(STR) STR
#endif

inline int getTableNumber(const QDate& date) {
    return date.year() * 100 + date.month();
}

inline int getTableNumber(const QDateTime& datetime) {
    return getTableNumber(datetime.date());
}

int LogSaver::save(LogRecordEntity& line) {
    SqlConnectionHolder connection = getConnectionHolder();
    if (connection.openError()) {
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString tableName = getLogRecordTableName(getTableNumber(line.getLogCreateTime()));
    SqlQueryHolder holder = connection.exec(
        QString("insert into ") + tableName
        + QString(" (catalog, log_level, log_content, file_name, file_line, log_create_time) values (?, ?, ?, ?, ? ,?)"));
    if (line.getCatalog().isEmpty()) {
        line.setCatalog("default");
    }
    holder.addBindValue(line.getCatalog());
    holder.addBindValue(line.getLogLevel());
    holder.addBindValue(line.getLogContent());
    holder.addBindValue(line.getFileName());
    holder.addBindValue(line.getFileLine());
    holder.addBindValue(line.getLogCreateTime());
    if (!holder.exec()) {
        //TODO if it's table not exits error, create table and retry
        QSqlError err = holder.lastError();
        if (err.nativeErrorCode() == "1146") {
            return SQL_TABLE_NOT_EXISTS;
        }
        return SQL_UPDATE_ERR_CODE;
    }
    line.setId(holder.lastInsertId().toULongLong());
    return 0;
}

int LogSaver::save(const QList<LogRecordEntity>& list, const QDate& date) {
    if (list.isEmpty()) {
        return 0;
    }
    SqlConnectionHolder connection = getConnectionHolder();
    if (connection.openError()) {
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString tableName = getLogRecordTableName(getTableNumber(date));
    QString sql = QString("insert into ")
                      .append(tableName)
                      .append(" (catalog, log_level, log_content, file_name, file_line, log_create_time) values ");

    for (int i = 0; i < list.size(); ++i) {
        if (i > 0) {
            sql.append(" , ");
        }
        sql.append(" (?, ?, ?, ?, ?, ?) ");
    }
    SqlQueryHolder holder = connection.exec(sql);
    for (int i = 0; i < list.size(); ++i) {
        const LogRecordEntity& line = list[i];
        if (line.getCatalog().isEmpty()) {
            holder.addBindValue("default");
        } else {
            holder.addBindValue(line.getCatalog());
        }
        holder.addBindValue(line.getLogLevel());
        holder.addBindValue(line.getLogContent());
        holder.addBindValue(line.getFileName());
        holder.addBindValue(line.getFileLine());
        holder.addBindValue(line.getLogCreateTime());
    }
    if (!holder.exec()) {
        //TODO if it's table not exits error, create table and retry
        QSqlError err = holder.lastError();
        if (err.nativeErrorCode() == "1146") {
            return SQL_TABLE_NOT_EXISTS;
        }
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

int LogSaver::update(const LogRecordEntity& entity, const QDate& date) {
    SqlConnectionHolder connection = getConnectionHolder();
    if (connection.openError()) {
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString tableName = getLogRecordTableName(getTableNumber(date));
    QString sql = QString("update ").append(tableName).append(" set log_content = ? where id = ?");
    SqlQueryHolder holder = connection.exec(sql);
    holder.bindValue(0, entity.getLogContent());
    holder.bindValue(1, entity.getId());
    if (!holder.exec()) {
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

int runGenSql(SqlConnectionHolder& connection, const QStringList& sqlList) {
    if (sqlList.isEmpty()) {
        qCritical() << "runGenSql: No SQL config found.";
        return SQL_TABLE_CREATE_ERROR_CODE;
    }
    for (const QString& sql : sqlList) {
        SqlQueryHolder holder = connection.exec(sql);
        if (!holder.exec()) {
            return SQL_TABLE_CREATE_ERROR_CODE;
        }
    }
    return 0;
}

int LogSaver::generalTable(const QDate& date) {
    SqlConnectionHolder connection = getConnectionHolder();
    if (connection.openError()) {
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    return runGenSql(connection, getLogRecordTableSql(connection.getType(), getTableNumber(date)));
}

int LogSaver::generalTable(const QDateTime& datetime) {
    SqlConnectionHolder connection = getConnectionHolder();
    if (connection.openError()) {
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    return runGenSql(connection, getLogRecordTableSql(connection.getType(), getTableNumber(datetime)));
}

int LogSaver::saveFilePos(LogFileScanEntity& entity, const QDate& date) {
    SqlConnectionHolder connection = getConnectionHolder();
    if (connection.openError()) {
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString path = entity.getFilePath();
    fixFilePath(path);
    entity.setFilePath(path);
    if (entity.getId() == 0) {
        SqlQueryHolder holder = connection.exec(
            "select id from hengre_log_file_scan where file_path = ?");
        holder.bindValue(0, path);
        if (!holder.exec()) {
            return SQL_QUERY_ERR_CODE;
        }
        if (holder.next()) {
            entity.setId(holder.value(0).toULongLong());
        }
    }
    entity.setRecordTable(getLogRecordTableName(getTableNumber(date)));
    entity.setCreateTime(QDateTime());
    entity.setEditTime(QDateTime());
    if (entity.getId() == 0) {
        SqlQueryHolder holder = connection.exec(
            "insert into hengre_log_file_scan (file_path, scaned_length, record_table, "
            "last_record_id) values (?, ?, ?, ?)");
        holder.bindValue(0, entity.getFilePath() CONNECT_PATH_ID);
        holder.bindValue(1, entity.getScanedLength());
        holder.bindValue(2, entity.getRecordTable());
        holder.bindValue(3, entity.getLastRecordId());
        if (!holder.exec()) {
            return SQL_UPDATE_ERR_CODE;
        }
        entity.setId(holder.lastInsertId().toULongLong());
    } else {
        SqlQueryHolder holder = connection.exec("update hengre_log_file_scan set last_record_id = "
                                                "?, scaned_length = ?, file_path = ? where id = ?");
        holder.bindValue(0, entity.getLastRecordId());
        holder.bindValue(1, entity.getScanedLength());
        holder.bindValue(2, entity.getFilePath() CONNECT_PATH_ID);
        holder.bindValue(3, entity.getId());
        if (!holder.exec()) {
            return SQL_UPDATE_ERR_CODE;
        }
    }
    return 0;
}

LogFileScanEntity LogSaver::getFilePos(const QString& path, int* errCode) {
    LogFileScanEntity entity;
    SqlConnectionHolder connection = getConnectionHolder();
    if (connection.openError()) {
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return entity;
    }
    sem::AfterGc gc([this]() { close(); });
    QString filePath = path;
    fixFilePath(filePath);
    SqlQueryHolder holder = connection.exec(
        "select id, file_path, scaned_length, record_table, last_record_id from "
        "hengre_log_file_scan where file_path = ?");
    holder.bindValue(0, filePath CONNECT_PATH_ID);
    if (!holder.exec()) {
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return entity;
    }
    if (!holder.next()) {
        if (errCode) {
            *errCode = SQL_QUERY_DATA_NOT_EXISTS;
        }
        return entity;
    }
    if (errCode) {
        *errCode = 0;
    }
    entity.setId(holder.value(0).toULongLong());
    entity.setFilePath(DISCONNECT_PATH_ID(holder.value(1).toString()));
    entity.setScanedLength(holder.value(2).toLongLong());
    entity.setRecordTable(holder.value(3).toString());
    entity.setLastRecordId(holder.value(4).toULongLong());
    return entity;
}

QList<LogFileScanEntity> LogSaver::allFilePos(qint32* errCode) {
    QList<LogFileScanEntity> list;
    SqlConnectionHolder connection = getConnectionHolder();
    if (connection.openError()) {
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return list;
    }
    sem::AfterGc gc([this]() { close(); });
    SqlQueryHolder holder = connection.exec(
        "select id, file_path, scaned_length, record_table, last_record_id from "
        "hengre_log_file_scan");
    if (!holder.exec()) {
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return list;
    }
    if (errCode) {
        *errCode = 0;
    }
    LogFileScanEntity entity;
#ifdef QT_DEBUG
    QString path;
#endif
    while (holder.next()) {
        entity.setId(holder.value(0).toULongLong());
#ifdef QT_DEBUG
        path = holder.value(1).toString();
        if (!path.endsWith(cdsem_test::readPathId())) continue;
        entity.setFilePath(DISCONNECT_PATH_ID(path));
#else
        entity.setFilePath(holder.value(1).toString());
#endif
        entity.setScanedLength(holder.value(2).toLongLong());
        entity.setRecordTable(holder.value(3).toString());
        entity.setLastRecordId(holder.value(4).toULongLong());
        list.append(entity);
    }
    return list;
}

int LogSaver::deleteFilePosRecord(quint64 id) {
    SqlConnectionHolder connection = getConnectionHolder();
    if (connection.openError()) {
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    SqlQueryHolder holder = connection.exec("delete from hengre_log_file_scan where id = ?");
    holder.bindValue(0, id);
    if (!holder.exec()) {
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}
