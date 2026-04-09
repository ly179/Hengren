#ifndef FILE_MANAGER_CONTROLLER_H
#define FILE_MANAGER_CONTROLLER_H
#include "ui_data_manager_creation_global.h"
#include "relation_file_manager.h"

class ClassSearchDTO;
class IDW_WaferSearchDTO;
class IdpSearchDTO;
class RecipeSearchDTO;
class HistoryViewController;
struct FileManagerDataHelper;

class UI_DATA_MANAGER_EXPORT FileManagerController : public RelationFileManager {
    Q_OBJECT
private:
    FileManagerDataHelper* dataHelper = nullptr;
    void createMenu();
    void createActionMenu();
    void createClassMenu();
    void createIDWMenu();
    void createIDPMenu();
    void createRecipeMenu();

    void refreshClassData(quint64 selectedId = 0);
    void refreshClassData(const ClassSearchDTO& param, quint64 selectedId = 0);
    void openClassEditor(quint64 id, const QString&);
    void openClassCopyEditor(quint64 id);
    void openIDWCreateEditor(quint64 classId, const QString& className);
    void openIDWRenameEditor(quint64 idwId, const QString& text);
    void openIDWCopyEditor(quint64 id);
    void openIDPRenameEditor(quint64 idpId, const QString& text);
    void openIDPCopyEditor(quint64 id);
    void openRecipeRenameEditor(quint64 recipeId, const QString& text);
    void openIdpCreateEditor(quint64 classId, quint64 idwId);
    void openRecipeCreateEditor(quint64 classId);
    void openRecipeCopyEditor(quint64 recipeId);
    //0 - search class
    //1 - search idw
    //2 - search idp
    //3 - search recipe
    void openSearchView(char);

    void createHistoryView();

    void create();

    void updateLoadingBtnStatus(int i, bool succ);

public:
    explicit FileManagerController(QWidget* parent = nullptr);
    ~FileManagerController() noexcept;

    void refreshIDWData(quint64 classId, quint64 selectedId = 0);
    //classId = 0表示刷新当前选中的class的IDW，大于0时，如果与当前选中的classId相同则刷新，否则不刷新
    void refreshSelectedIDWData(quint64 classId = 0, quint64 selectedId = 0);
    void enableIDWAction(bool = true);
    void refreshIDWData(const IDW_WaferSearchDTO& param, quint64 selectedId = 0);
    void refreshIDPData(quint64 classId, quint64 selectedId = 0);
    //classId = 0表示刷新当前选中的class的IDP，大于0时，如果与当前选中的classId相同则刷新，否则不刷新
    void refreshSelectedIDPData(quint64 classId, quint64 idwTopId, quint64 selectedId = 0);
    void refreshIDPData(const IdpSearchDTO& param, quint64 selectedId = 0);
    void enableIDPAction(bool = true);
    void refreshRecipeData(quint64 classId, quint64 selectedId = 0);
    //classId = 0表示刷新当前选中的class的IDP，大于0时，如果与当前选中的classId相同则刷新，否则不刷新
    void refreshSelectedRecipeData(quint64 classId, quint64 selectedId = 0);
    void refreshRecipeData(const RecipeSearchDTO& param, quint64 selectedId = 0);
    void enableRecipeAction(bool = true);

signals:
    void openIDWEditor(quint64 classId, const QString& className, quint64 id, const QString& text, bool allowCoverSave = true);
    void openIdpEditor(quint64 classId, quint64 id, const QString& idpName, quint64 idwId, bool allowCoverSave = true);
    void openRecipeEditor(quint64 classId, quint64 id, const QString& recipeName, bool allowCoverSave = true);
    void openRecipeEraser(quint64 classId);

private slots:
    void openClassEditor();
};

#endif // FILE_MANAGER_CONTROLLER_H
