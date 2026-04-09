#ifndef RECIPE_DATA_DAO_H
#define RECIPE_DATA_DAO_H
#include "abstract_dao.h"
#include "general_database_dao_global.h"
#include "recipe_data_entity.h"

class SEM_DAO_EXPORT RecipeDataDao : public AbstractDao {
private:
    qint32 checkAndSave(RecipeDataEntity &);

public:
    qint32 save(RecipeDataEntity &);
    qint32 updateTopId(quint64 id, quint64 topId, int* errCode = 0);
    qint32 saveWithTransaction(RecipeDataEntity &);

    RecipeDataEntity get(quint64 id, qint32 *errCode = nullptr);
    RecipeDataEntity getSimpleInfo(quint64 id, qint32 *errCode = nullptr);

    quint64 getRecipeIdByName(quint64 classId, const QString& name, qint32* errCode = nullptr);
    RecipeDataEntity getSimpleRecipeByName(quint64 classId, const QString& name, qint32* errCode = nullptr);

    QList<RecipeDataEntity> searchList(const RecipeSearchDTO &param, qint32 *errCode = nullptr);

    qint32 updateRecipeNameOnly(quint64 id, const QString &title);

    // 如果当前版本是有效版本，则会同时删除历史版本？
    qint32 deleteRecipeById(quint64 id);

    qint32 deleteRecipeByClass(quint64 classId);
};

#endif // RECIPE_DATA_DAO_H
