#ifndef QTHEMEDPANEL_H
#define QTHEMEDPANEL_H

#include <QWidget>
#include <QPainter>

class QThemedPanel : public QWidget
{
    Q_OBJECT

public:
    explicit QThemedPanel(QWidget *parent = 0);

private:
    void paintEvent(QPaintEvent *);
};

#endif // QTHEMEDPANEL_H
