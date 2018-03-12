#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include "faderdialog.h"

namespace Ui {
    class AboutDialog;
}
class ClientModel;

/** "About" dialog box */
class AboutDialog : public FaderDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

    void setModel(ClientModel *model);
private:
    Ui::AboutDialog *ui;

private slots:
    void on_buttonBox_accepted();
};

#endif // ABOUTDIALOG_H
