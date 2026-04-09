#include "idp_common_util.h"
#include "idp_data_model.h"
#include <QSet>

QString toViewDNumber(const QList<IdpMExeParamDetail>& list) {
    QString dName;
    toViewDNumber(list, dName);
    return dName;
}

void toViewDNumber(const QList<IdpMExeParamDetail>& subList, QString& dName) {
    dName.clear();
    QSet<int> dChecker;
    for (const IdpMExeParamDetail& d : subList) {
        for (const int& dN: d.dNumbers) {
            if (dChecker.contains(dN)) continue;
            if (dName.length() > 0) dName.append(",");
            if (dChecker.size() < 2) dName.append(QString::number(dN));
            else if (dChecker.size() == 2) {
                dName.append(",*");
                break;
            }
            dChecker.insert(dN);
        }
        if (dChecker.size() > 2) break;
    }
}

void collectViewMpName(const QList<IdpMExeParamDetail>& list, QString& mpName) {
    mpName.clear();
    for (int i = 0; i < list.size(); ++ i) {
        const IdpMExeParamDetail& d1 = list[i];
        if (i == 0) {
            mpName = d1.mpName;
        }
        if (i < list.size() - 1) {
            const IdpMExeParamDetail& d2 = list[i + 1];
            if (d1.mpName != d2.mpName) {
                mpName.append(",*");
                break;
            }
        }
    }
}

void collectViewDName(const QList<QSharedPointer<IdpMpPosListData>>& list, QString& dName) {
    dName.clear();
    QSet<int> dChecker;
    for (int i = 0; i < list.size(); ++ i) {
        const QSharedPointer<IdpMpPosListData>& data = list[i];
        for (const QList<int>& dNumbers : data->dNumbers) {
            for (const int& d : dNumbers) {
                if (dChecker.isEmpty()) {
                    dName = QString::number(d);
                    dChecker.insert(d);
                    continue;
                } else if (dChecker.contains(d)) {
                    continue;
                }
                dChecker.insert(d);
                if (dChecker.size() > 2) {
                    dName.append(",*");
                    return;
                }
                dName.append(QString(",%1").arg(QString::number(d)));
            }
        }
    }
}

void collectViewDName(const QList<QList<int>>& list, QString& dName) {
    dName.clear();
    QSet<int> dChecker;
    for (const QList<int>& dNumbers : list) {
        for (const int& d : dNumbers) {
            if (dChecker.isEmpty()) {
                dName = QString::number(d);
                dChecker.insert(d);
                continue;
            } else if (dChecker.contains(d)) {
                continue;
            }
            dChecker.insert(d);
            if (dChecker.size() > 2) {
                dName.append(",*");
                return;
            }
            dName.append(QString(",%1").arg(QString::number(d)));
        }
    }
}

QString collectViewMpName(const QList<IdpMExeParamDetail>& list) {
    QString mpName;
    collectViewMpName(list, mpName);
    return mpName;
}

QString collectViewDName(const QList<QSharedPointer<IdpMpPosListData>>& list) {
    QString dName;
    collectViewDName(list, dName);
    return dName;
}

QString collectViewDName(const QList<QList<int>>& list) {
    QString dName;
    collectViewDName(list, dName);
    return dName;
}

void collectDList(const QList<IdpMExeParamDetail>& list, QList<QList<int>>& dList) {
    dList.clear();
    if (list.isEmpty()) return;
    dList.reserve(list.size());
    for (int i = 0; i < list.size(); ++ i) {
        dList.append(list[i].dNumbers);
    }
}
