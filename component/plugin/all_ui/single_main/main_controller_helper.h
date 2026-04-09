#ifndef MAIN_CONTROLLER_HELPER_H
#define MAIN_CONTROLLER_HELPER_H

#include <QObject>

class QWidget;
class ManualControlController;
class FileManagerController;
class IdwWaferEditor;
class IdpDataEditorController;
class TemplateManagerController;
class APTemplateEditController;
class MSTemplateEditController;
class MPTemplateEditController;
class TemplateMpipEditController;
class RecipeEditorController;
class RecipeEraserController;
class JobController;
class WorksheetController;
class StageController;
class LockLoaderController;
class AmpEditController;
class UserListController;
class RoleListController;
class MainController;
class MsrManagerController;
class ImageManagerController;

class MainControllerHelper : public QObject{
    Q_OBJECT
private:

    MainController* parent;

    ManualControlController* manualController = nullptr;
    FileManagerController* fileManagerController = nullptr;
    TemplateManagerController* templateManagerController = nullptr;
    IdwWaferEditor* idwWaferEditor = nullptr;
    IdpDataEditorController* idpDataEditor = nullptr;
    RecipeEditorController* recipeDataEditor = nullptr;
    APTemplateEditController* apTemplateEditController = nullptr;
    MSTemplateEditController* msTemplateEditController = nullptr;
    MPTemplateEditController* mpTemplateEditController = nullptr;
    TemplateMpipEditController* mpipEditController = nullptr;
    RecipeEraserController* recipeEraserController = nullptr;
    JobController* jobController = nullptr;
    WorksheetController* workSheetController = nullptr;
    StageController* stageController = nullptr;
    LockLoaderController* loaderController = nullptr;
    UserListController* userController = nullptr;
    RoleListController* roleController = nullptr;
    MsrManagerController* msrController = nullptr;
    ImageManagerController* imageController = nullptr;


public:
    explicit MainControllerHelper(QObject* = nullptr);

    virtual ~MainControllerHelper() noexcept;

    void setMainWidget(MainController* parent);

    void openManualController();

    void openFileController();

    void openImageOperationController();
    void openWorkSheetController();
    void openLoaderController();

    void openAmpController();
    void openMSRController();

    void openImageController();

signals:
    void manualControllerVisible(bool);
    void fileManagerControllerVisible(bool);
    void recipeControllerVisible(bool);
    void worksheetControllerVisible(bool);
    void loaderControllerVisible(bool);
    void msrControllerVisible(bool);
    void imageControllerVisible(bool);
};

#endif // MAIN_CONTROLLER_HELPER_H
