#ifndef FADERDIALOG_H
#define FADERDIALOG_H

#include <QDialog>
#include <QShowEvent>
#include <QPropertyAnimation>

class FaderDialog : public QDialog
{
    Q_OBJECT

public:
    FaderDialog(QWidget *parent = 0);

    void showEvent(QShowEvent *);
    void done(int result);

private:
    int result;
    QWidget *parent;

    QPropertyAnimation *fadeInAnimation;
    QPropertyAnimation *fadeOutAnimation;

private slots:
    void closeDialog();

};

#endif /* FADERDIALOG */
