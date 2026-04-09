#include "idp_message_box_helper.h"
#include "cde90_message_box.h"
#include "sem_global.h"

QSharedPointer<CDE90MessageBox> idpTemplateShowYesNoBox(
    QWidget* parent,
    const QString& title,
    const QString& text,
    bool okMark,
    const std::function<bool(int)>& callback) {
    QSharedPointer<CDE90MessageBox> box;
    if (!MainThreadInvoker::getInvoker().isMainThread()) {
        MainThreadInvoker::getInvoker().blockRun(
            [parent, &title, &text, okMark, &callback, &box]() mutable {
                box = idpTemplateShowYesNoBox(parent, title, text, okMark, callback);
            });
    }
    box.reset(new CDE90MessageBox(parent), sem::commonDeleteLater);
    box->setWindowTitle(title);
    box->setText(text);
    box->addLeftButton("OK", okMark);
    box->addRightButton("Cancel");
    box->setResizable(false);
    box->setCallback(callback);
    box->setWindowModality(Qt::NonModal);
    box->show();
    return box;
}
