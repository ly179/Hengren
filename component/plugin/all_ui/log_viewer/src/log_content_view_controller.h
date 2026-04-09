#ifndef LOG_CONTENT_VIEW_CONTROLLER_H
#define LOG_CONTENT_VIEW_CONTROLLER_H
#include <QWidget>

namespace cdsem {
class UiLogContentViewController;
}

class HighlightLogTextarea;
struct LogContentViewDataHelper;
class LogContentViewController : public QWidget{
    Q_OBJECT
private:
    QScopedPointer<LogContentViewDataHelper> dataHelper;
    cdsem::UiLogContentViewController* ui = nullptr;

    void create();
    void createAction();

    bool eventFilter(QObject* object, QEvent* event) override;
public:
     explicit LogContentViewController(QWidget* parent = nullptr);
     ~LogContentViewController() noexcept override;

    void setLogView(HighlightLogTextarea*);

    void pushFilterHistory(const QString& text, int i = -1);

    void searchText(const QString& text, bool enableCase, bool enableRegular);

signals:
    void doFilter(const QString& text, bool enableCase, bool enableRegular);
    void doSearch(const QString& text, bool enableCase, bool enableRegular);
};
#endif
