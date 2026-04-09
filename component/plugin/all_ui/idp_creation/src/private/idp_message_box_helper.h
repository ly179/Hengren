#ifndef IDP_MESSAGE_BOX_HELPER_H
#define IDP_MESSAGE_BOX_HELPER_H
#include <QSharedPointer>
#include "functional"
class QWidget;
class QString;
class CDE90MessageBox;
QSharedPointer<CDE90MessageBox> idpTemplateShowYesNoBox(QWidget* parent,
                                                        const QString& title,
                                                        const QString& text,
                                                        bool okMark,
                                                        const std::function<bool(int)>& callback);
#endif // IDP_MESSAGE_BOX_HELPER_H
