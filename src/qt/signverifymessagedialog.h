#ifndef SIGNVERIFYMESSAGEDIALOG_H
#define SIGNVERIFYMESSAGEDIALOG_H

#include <QDialog>
#include "faderdialog.h"

namespace Ui {
    class SignVerifyMessageDialog;
}
class WalletModel;

class SignVerifyMessageDialog : public FaderDialog
{
    Q_OBJECT

public:
    enum Mode {
        Sign,
        Verify
    };

    explicit SignVerifyMessageDialog(Mode mode, QString addr, QWidget *parent = 0);
    ~SignVerifyMessageDialog();

    void setModel(WalletModel *model);
    void setAddress_SM(QString address);
    void setAddress_VM(QString address);

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::SignVerifyMessageDialog *ui;
    WalletModel *model;

private slots:
    /* sign message */
    void on_addressBookButton_SM_clicked();
    void on_pasteButton_SM_clicked();
    void on_signMessageButton_SM_clicked();
    void on_copySignatureButton_SM_clicked();
    void on_clearButton_SM_clicked();
    void on_addressIn_SM_textChanged(const QString &address);
    void on_addressIn_VM_textChanged(const QString &address);

    /* verify message */
    void on_addressBookButton_VM_clicked();
    void on_verifyMessageButton_VM_clicked();
    void on_clearButton_VM_clicked();
};

#endif // SIGNVERIFYMESSAGEDIALOG_H
