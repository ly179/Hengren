#ifndef RECIPE_DATA_MODEL_H
#define RECIPE_DATA_MODEL_H
#include <QList>
#include "cdsem_pack_base.h"
#include "float_number.h"
#include "sem_common_data_model.h"
#include "file_copy_data_model.h"

struct FemMeasureModel {
    FloatNumber<3> foucsStart;
    FloatNumber<3> foucsStep;
    FloatNumber<3> exposureStart;
    FloatNumber<3> exposureStep;
    // 0 - x:focus,y:exposure
    //1 - x:exposure,y:focus
    //2 - focus only
    int femType = 0;
    //0 - from lower left chip
    //1 - from center chip
    int startChip = 0;
};

struct OmSemScopeCorrectionModel {
    FloatNumber1 targetRangeX;
    FloatNumber1 targetRangeY;
    FloatNumber1 targetRangeDiffX;
    FloatNumber1 targetRangeDiffY;
    bool allowAdjustment = false;
    bool enableOmSemScopeCorrection = false;
};

struct ZSensorOffsetModel {
    Integer32 offset;
    Integer32 lsb;
    bool allowAdjustment = false;
    bool enableZSensorOffset = false;
};

struct CDCheckModel {
    quint64 refMsrId = 0;
    //TODO Measure输入框填的什么数据类型未知
    Integer32 nextMsrNo;
    bool enableCDCheck = false;
};

struct StageMonitorModel {
    //TODO 暂定为3个精度，具体未知
    FloatNumber<3> range;
    bool enableStageMonitor = false;
};

struct PgMonitorModel {
    //TODO 其他参数未知
    bool enablePgMonitor = false;
};

struct MaintenanceDataModel {
    OmSemScopeCorrectionModel omSemScopeCorrection;
    ZSensorOffsetModel zSensorOffset;
    CDCheckModel cdCheck;
    StageMonitorModel stageMonitor;
    PgMonitorModel pgMonitor;
};

struct RecipeProcessModel {
    int executeMode = 0;
    int manualAssist = 0;
    int imageSave = 0;
};

struct RecipeMeasureResultModel {
    Integer32 succRate;
    bool diskSave = false;
    bool sendToHost = false;
    bool printOut = false;
    bool networkTransfer = false;
    int confirm = 0;
    int method = 0;
    int limitCheck = 0;
};

struct RecipeEnvAdditionSettingModel {
    bool editIdpAfterWaferAlign = false;
    bool presetBC = false;
    bool retrySemAlignWithAFCC = false;
    bool retryAddressWithAFCC = false;
    bool retryPatternRegWithAFCC = false;
    bool reMeasureErrorSite = false;
    bool periodicWaferAlign = false;
    Integer32 ionizerIrradition;
};

struct RecipeEnvironmentModel {
    RecipeMeasureResultModel measureResult;
    RecipeProcessModel waferAlign;
    RecipeProcessModel measureAdderss;
    RecipeProcessModel measureExe;
    RecipeEnvAdditionSettingModel additionSetting;
};

struct RecipeDataModel {
    RecipeEnvironmentModel env;
    MaintenanceDataModel maintenance;
    FemMeasureModel fem;
    QList<int> selectedSlots;
    quint64 classId = 0;
    quint64 recipeId = 0;
    quint64 idwId = 0;
    quint64 idwTopId = 0;
    quint64 idpId = 0;
    quint64 idpTopId = 0;
    Integer32 semConditionNum;
    //0 - HighOxide 1 - Normal 2 - ArF
    int waferProperty = 0;
    int special = 0;
    bool autoCalibration = false;
};

struct Recipe_CopyDataResult: public OperationResult {
    quint64 newId = 0;
    quint64 newClassId = 0;
};

struct Recipe_CopyDataParam: public SimpleDataCopyParam {
    QString toClassName;
    QString toIdwName;
    QString toIdpName;
    QString toIdwOfIdpName;

    bool overwrite = false;
};

#endif // RECIPE_DATA_MODEL_H
