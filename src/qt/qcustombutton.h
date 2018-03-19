#ifndef QCUSTOMBUTTON_H
#define QCUSTOMBUTTON_H

#include <QPushButton>

class QCustomButton : public QPushButton
{
    Q_OBJECT

public:
    explicit QCustomButton(QWidget *parent = 0);

private slots:
    bool eventFilter(QObject *, QEvent *event);
};

#endif // QCUSTOMBUTTON_H
