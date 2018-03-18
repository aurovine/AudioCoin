#include "qcustombutton.h"

#include "guiconstants.h"

#include <QEvent>

QCustomButton::QCustomButton(QWidget *parent) :
    QPushButton(parent)
{
    setStyleSheet(BUTTON_STYLE);
    installEventFilter(this);

#ifdef Q_OS_MAC
    setAttribute(Qt::WA_LayoutUsesWidgetRect, 1);
#endif
    
}

bool QCustomButton::eventFilter(QObject *object, QEvent *event)
{
    QCustomButton *button = qobject_cast<QCustomButton*>(object);
    if (!button) {
        return false;
    }

    if (button->objectName() == "cancelButton" || button->objectName() == "closeButton" || button->objectName() == "signCancelButton" || button->objectName() == "verifyCancelButton")
    {
        if (event->type() == QEvent::Enter) {
            button->setIcon(QIcon(":/icons/material/white/close"));
        }

        if (event->type() == QEvent::Leave) {
            button->setIcon(QIcon(":/icons/material/black/close"));
        }
    }

    return QPushButton::eventFilter(object, event);
}
