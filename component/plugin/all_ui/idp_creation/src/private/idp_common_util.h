#ifndef IDP_COMMON_UTIL_H
#define IDP_COMMON_UTIL_H
#include <QString>
#include <QList>
#include <QSharedPointer>

struct MixDNumberData {
    QList<QList<int>> dNums;
    QString viewDNum;
};

struct IdpMExeParamDetail;
struct IdpMpTableModeData;
struct IdpMpPosListData;
QString toViewDNumber(const QList<IdpMExeParamDetail>&);
void toViewDNumber(const QList<IdpMExeParamDetail>&, QString&);


void collectViewMpName(const QList<IdpMExeParamDetail>& list, QString& mpName);
void collectViewDName(const QList<QSharedPointer<IdpMpPosListData>>& list, QString& dName);
void collectViewDName(const QList<QList<int>>& list, QString& dName);
QString collectViewMpName(const QList<IdpMExeParamDetail>& list);
QString collectViewDName(const QList<QSharedPointer<IdpMpPosListData>>& list);
QString collectViewDName(const QList<QList<int>>& list);


void collectDList(const QList<IdpMExeParamDetail>& list, QList<QList<int>>& dList);
#endif // IDP_COMMON_UTIL_H
