#ifndef TEMPLATE_MANAGER_CONTROLLER_H
#define TEMPLATE_MANAGER_CONTROLLER_H

#include "relation_template_manager.h"
#include "ui_data_manager_creation_global.h"

class TemplateInfoSearchDTO;
struct TemplateManagerControllerDataHelper;
class UI_DATA_MANAGER_EXPORT TemplateManagerController : public RelationTemplateManager {
    Q_OBJECT
private:
    TemplateManagerControllerDataHelper* dataHelper;

    void create();

    void createMenu();
    void createActionMenu();
    void createAlignmentMenu();
    void createAddressMenu();
    void createMpMenu();
    void createMpIpMenu();

    void openCreateEditor(const QString& title, int type);

    void localRefreshList(const TemplateInfoSearchDTO& param, quint64 selectedId = 0);
    void refreshList(int, quint64 selectedId = 0);
    void refreshList(const TemplateInfoSearchDTO& param, quint64 selectedId = 0);

    void openRenameEditor(quint64 id, const QString& text, int type);

public:
    explicit TemplateManagerController(QWidget* parent = nullptr);
    ~TemplateManagerController() noexcept;

    void refreshAlignmentList(quint64 selectedId = 0);
    void refreshAddressingList(quint64 selectedId = 0);
    void refreshMPList(quint64 selectedId = 0);
    void refreshMPIPList(quint64 selectedId = 0);
    void deleteTemplate(int type);

    void enableApAction(bool = true);
    void enableMsAction(bool = true);
    void enableMpAction(bool = true);
    void enableMpIpAction(bool = true);

    //type:
    //0 - search AP
    //1 - search MS
    //2 - search MP
    //3 - search MPIP
    //input:
    //0 - name
    //1 - comment
    void openSearchView(char type, char input);

signals:
    void openAlignmentEditor(quint64 id, const QString& title);
    void openAddressingEditor(quint64 id, const QString& title);
    void openMpEditor(quint64 id, const QString& title);
    void openMpIpEditor(quint64 id, const QString& title);
};

#endif // TEMPLATE_MANAGER_CONTROLLER_H
