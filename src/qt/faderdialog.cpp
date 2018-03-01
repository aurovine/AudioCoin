#include "faderdialog.h"

#include <QApplication>
#include <QDialog>
#include <QLayout>
#include <QShowEvent>

FaderDialog::FaderDialog(QWidget *parent) :
    QDialog(parent),
    result(0)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::WindowStaysOnTopHint);

    fadeInAnimation = new QPropertyAnimation(this, "windowOpacity");
    fadeInAnimation->setEasingCurve(QEasingCurve::InCubic);
    fadeInAnimation->setDuration(250);
    fadeInAnimation->setStartValue(0.0);
    fadeInAnimation->setEndValue(1.0);
 
    fadeOutAnimation = new QPropertyAnimation(this, "windowOpacity");
    fadeOutAnimation->setEasingCurve(QEasingCurve::OutCubic);
    fadeOutAnimation->setDuration(250);
    fadeOutAnimation->setStartValue(1.0);
    fadeOutAnimation->setEndValue(0.0);

    connect(fadeOutAnimation, SIGNAL(finished()), this, SLOT(closeDialog()));
}

void FaderDialog::showEvent(QShowEvent *)
{
    fadeInAnimation->start();
}

void FaderDialog::done(int result)
{
    this->result = result;
    fadeOutAnimation->start();
}

void FaderDialog::closeDialog()
{
    QDialog::done(result);
}
