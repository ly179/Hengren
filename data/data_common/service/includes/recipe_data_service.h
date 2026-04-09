#ifndef RECIPE_DATA_SERVICE_H
#define RECIPE_DATA_SERVICE_H

#include "general_database_service_global.h"
#include "iservice.h"
#include "recipe_data_entity.h"
#include "recipe_data_model.h"

struct OperationResult;
struct RecipeDataServiceHelper;

class SEM_SERVICE_EXPORT RecipeDataService : public IService {
    friend struct RecipeDataServiceHelper;
private:
    RecipeDataServiceHelper* helper;

public:
    RecipeDataService();
    ~RecipeDataService() noexcept;

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

    //擦除recipe
    void eraseRecipe(qint64 id, bool removeIdw, bool removeIdp, OperationResult *result = nullptr);

    Recipe_CopyDataResult copyNew(const Recipe_CopyDataParam& param);
};

#endif // RECIPE_DATA_SERVICE_H
