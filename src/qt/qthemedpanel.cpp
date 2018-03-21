#include "qthemedpanel.h"

#include "optionsmodel.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOption>

QThemedPanel::QThemedPanel(QWidget *parent) :
    QWidget(parent)
{
    if (fUseBlackTheme)
      setStyleSheet("QThemedPanel { background-color: #292c30; } ");
    else {
      setStyleSheet("QThemedPanel { background-color: white; } ");
    }
}

void QThemedPanel::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
