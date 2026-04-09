#ifndef UI_JOB_CONTROLLER
#define UI_JOB_CONTROLLER
#include <QtGlobal>
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
#include "inner_code_editor.h"
#ifndef CODER_EDITOR_DEP
#define CODER_EDITOR_DEP __declspec(dllimport)
#endif
#include "code_editor_controller.h"
#include <QList>
#endif

#include "cde90_layout_helper.h"
#include "cde90_button.h"
#include <QWidget>

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiJobController;
struct UiJobControllerPath;
}

struct cdsem::UiJobControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiJobControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "job_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "job_controller";
#endif
        path = proHuiDir + "/" + huiFileName + ".hui";
    }
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    //该目录用于实时加载和保存hui文件
    void setHuiDir(const QString& dir) {
        huiDir = dir;
        path = huiDir + "/" + huiFileName + ".hui";
    }

    //该目录用于编辑器实时更新header
    void setCodeDir(const QString& dir) {
        sourceCodeDir = dir;
    }
#endif

};
class cdsem::UiJobController {
private:
    const cdsem::UiJobControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    QWidget* jobItemContent = nullptr;
    CDE90ButtonBox* stopBtn = nullptr;
    CDE90ButtonBox* pauseBtn = nullptr;
    CDE90ButtonBox* interruptBtn = nullptr;
    CDE90ButtonBox* queueBtn = nullptr;
    CDE90ButtonBox* closeBtn = nullptr;
    UiJobController() = default;
    ~UiJobController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
, &__allWidgets
#endif
        );
        jobItemContent = helper.get<QWidget>("jobItemContent");
        stopBtn = helper.get<CDE90ButtonBox>("stopBtn");
        pauseBtn = helper.get<CDE90ButtonBox>("pauseBtn");
        interruptBtn = helper.get<CDE90ButtonBox>("interruptBtn");
        queueBtn = helper.get<CDE90ButtonBox>("queueBtn");
        closeBtn = helper.get<CDE90ButtonBox>("closeBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiJobControllerPath& path) {
        return getCodeEditor<cdsem::UiJobControllerPath>(path);
    }

#endif

};
#endif
