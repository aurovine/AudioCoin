#include "signverifymessagedialog.h"
#include "ui_signverifymessagedialog.h"

#include "addressbookpage.h"
#include "base58.h"
#include "guiutil.h"
#include "init.h"
#include "main.h"
#include "optionsmodel.h"
#include "walletmodel.h"
#include "wallet.h"

#include <QClipboard>

#include <string>
#include <vector>

SignVerifyMessageDialog::SignVerifyMessageDialog(Mode mode, QString addr, QWidget *parent) :
    FaderDialog(parent),
    ui(new Ui::SignVerifyMessageDialog),
    model(0)
{
    ui->setupUi(this);

#ifdef Q_OS_MAC
    ui->addressIn_SM->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->messageIn_SM->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->signatureOut_SM->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->addressIn_VM->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->messageIn_VM->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->signatureIn_VM->setAttribute(Qt::WA_MacShowFocusRect, 0);
#endif

#if (QT_VERSION >= 0x040700)
    /* Do not move this to the XML file, Qt before 4.7 will choke on it */
    ui->addressIn_SM->setPlaceholderText(tr("Audiocoin address (e.g. AeAiLyyJHVZEi92rzk3HjSRWRAUDGeLXWf)"));
    ui->signatureOut_SM->setPlaceholderText(tr("Click \"Sign Message\" to generate signature"));

    ui->addressIn_VM->setPlaceholderText(tr("Audiocoin address (e.g. AeAiLyyJHVZEi92rzk3HjSRWRAUDGeLXWf)"));
    ui->signatureIn_VM->setPlaceholderText(tr("Audiocoin signature"));
#endif

    setContentsMargins(0, 0, 0, 0);

    // ui->SignVerifyMessageDialogLayout->setSizeConstraint(QLayout::SetFixedSize);

    GUIUtil::setupAddressWidget(ui->addressIn_SM, this);
    GUIUtil::setupAddressWidget(ui->addressIn_VM, this);

    ui->addressIn_SM->installEventFilter(this);
    ui->messageIn_SM->installEventFilter(this);
    ui->signatureOut_SM->installEventFilter(this);
    ui->addressIn_VM->installEventFilter(this);
    ui->messageIn_VM->installEventFilter(this);
    ui->signatureIn_VM->installEventFilter(this);

    ui->signatureOut_SM->setFont(GUIUtil::bitcoinAddressFont());
    ui->signatureIn_VM->setFont(GUIUtil::bitcoinAddressFont());

    switch (mode)
    {
        case Sign:
            setAddress_SM(addr);
            ui->verifyMessageWidget->hide();
            setWindowTitle(tr("Signatures - Sign a Message"));
            break;
        case Verify:
            setAddress_VM(addr);
            ui->signMessageWidget->hide();
            setWindowTitle(tr("Signatures - Verify a Message"));
            break;
    }

    // QApplication::processEvents();

    resize(width(), sizeHint().height());
    move(parent->frameGeometry().center() - QPoint(width() / 2, height() / 2));

    ui->signMessageButton_SM->setEnabled(false);
    ui->verifyMessageButton_VM->setEnabled(false);

    connect(ui->signCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->verifyCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

SignVerifyMessageDialog::~SignVerifyMessageDialog()
{
    delete ui;
}

void SignVerifyMessageDialog::setModel(WalletModel *model)
{
    this->model = model;
}

void SignVerifyMessageDialog::setAddress_SM(QString address)
{
    if (!address.isEmpty())
    {
        ui->addressIn_SM->setText(address);
        ui->messageIn_SM->setFocus();
    }
}

void SignVerifyMessageDialog::setAddress_VM(QString address)
{
    if (!address.isEmpty())
    {
        ui->addressIn_VM->setText(address);
        ui->messageIn_VM->setFocus();
    }
}

void SignVerifyMessageDialog::on_addressBookButton_SM_clicked()
{
    if (model && model->getAddressTableModel())
    {
        AddressBookPage dlg(AddressBookPage::DialogMode, AddressBookPage::ReceivingTab, this);
        dlg.setModel(model->getAddressTableModel());
        if (dlg.exec())
        {
            setAddress_SM(dlg.getReturnValue());
        }
    }
}

void SignVerifyMessageDialog::on_pasteButton_SM_clicked()
{
    setAddress_SM(QApplication::clipboard()->text());
}

void SignVerifyMessageDialog::on_signMessageButton_SM_clicked()
{
    if (!model)
        return;

    /* Clear old signature to ensure users don't get confused on error with an old signature displayed */
    ui->signatureOut_SM->clear();

    CBitcoinAddress addr(ui->addressIn_SM->text().toStdString());

    CKeyID keyID;
    if (!addr.GetKeyID(keyID))
    {
        ui->addressIn_SM->setValid(false);
        ui->statusLabel_SM->setStyleSheet("QLabel { text-transform: uppercase; color: red; }");
        ui->statusLabel_SM->setText(tr("The entered address does not refer to a key"));
        return;
    }

    WalletModel::UnlockContext ctx(model->requestUnlock());
    if (!ctx.isValid())
    {
        ui->statusLabel_SM->setStyleSheet("QLabel { text-transform: uppercase; color: red; }");
        ui->statusLabel_SM->setText(tr("Wallet unlock was cancelled"));
        return;
    }

    CKey key;
    if (!pwalletMain->GetKey(keyID, key))
    {
        ui->statusLabel_SM->setStyleSheet("QLabel { text-transform: uppercase; color: red; }");
        ui->statusLabel_SM->setText(tr("Private key for the entered address is not available"));
        return;
    }

    CDataStream ss(SER_GETHASH, 0);
    ss << strMessageMagic;
    ss << ui->messageIn_SM->document()->toPlainText().toStdString();

    std::vector<unsigned char> vchSig;
    if (!key.SignCompact(Hash(ss.begin(), ss.end()), vchSig))
    {
        ui->statusLabel_SM->setStyleSheet("QLabel { text-transform: uppercase; color: red; }");
        ui->statusLabel_SM->setText(tr("Message signing failed"));
        return;
    }

    ui->statusLabel_SM->setStyleSheet("QLabel { text-transform: uppercase; color: green; }");
    ui->statusLabel_SM->setText(tr("Message signed"));

    ui->signatureOut_SM->setText(QString::fromStdString(EncodeBase64(&vchSig[0], vchSig.size())));
}

void SignVerifyMessageDialog::on_copySignatureButton_SM_clicked()
{
    QApplication::clipboard()->setText(ui->signatureOut_SM->text());
    ui->statusLabel_SM->setStyleSheet("QLabel { text-transform: uppercase; color: green; }");
    ui->statusLabel_SM->setText(tr("Copied to clipboard"));
}

void SignVerifyMessageDialog::on_clearButton_SM_clicked()
{
    ui->addressIn_SM->clear();
    ui->messageIn_SM->clear();
    ui->signatureOut_SM->clear();
    ui->statusLabel_SM->clear();

    ui->addressIn_SM->setFocus();
}

void SignVerifyMessageDialog::on_addressBookButton_VM_clicked()
{
    if (model && model->getAddressTableModel())
    {
        AddressBookPage dlg(AddressBookPage::DialogMode, AddressBookPage::SendingTab, this);
        dlg.setModel(model->getAddressTableModel());
        if (dlg.exec())
        {
            setAddress_VM(dlg.getReturnValue());
        }
    }
}

void SignVerifyMessageDialog::on_verifyMessageButton_VM_clicked()
{
    CBitcoinAddress addr(ui->addressIn_VM->text().toStdString());

    CKeyID keyID;
    if (!addr.GetKeyID(keyID))
    {
        ui->addressIn_VM->setValid(false);
        ui->statusLabel_VM->setStyleSheet("QLabel { text-transform: uppercase; color: red; }");
        ui->statusLabel_VM->setText(tr("The entered address does not refer to a key"));
        return;
    }

    bool fInvalid = false;
    std::vector<unsigned char> vchSig = DecodeBase64(ui->signatureIn_VM->text().toStdString().c_str(), &fInvalid);

    if (fInvalid)
    {
        ui->signatureIn_VM->setValid(false);
        ui->statusLabel_VM->setStyleSheet("QLabel { text-transform: uppercase; color: red; }");
        ui->statusLabel_VM->setText(tr("The signature could not be decoded"));
        return;
    }

    CDataStream ss(SER_GETHASH, 0);
    ss << strMessageMagic;
    ss << ui->messageIn_VM->document()->toPlainText().toStdString();

    CPubKey pubkey;
    if (!pubkey.RecoverCompact(Hash(ss.begin(), ss.end()), vchSig))
    {
        ui->signatureIn_VM->setValid(false);
        ui->statusLabel_VM->setStyleSheet("QLabel { text-transform: uppercase; color: red; }");
        ui->statusLabel_VM->setText(tr("The signature did not match the message digest"));
        return;
    }

    if (!(CBitcoinAddress(pubkey.GetID()) == addr))
    {
        ui->statusLabel_VM->setStyleSheet("QLabel { text-transform: uppercase; color: red; }");
        ui->statusLabel_VM->setText(tr("Message verification failed"));
        return;
    }

    ui->statusLabel_VM->setStyleSheet("QLabel { text-transform: uppercase; color: green; }");
    ui->statusLabel_VM->setText(tr("Message verified"));
}

void SignVerifyMessageDialog::on_clearButton_VM_clicked()
{
    ui->addressIn_VM->clear();
    ui->signatureIn_VM->clear();
    ui->messageIn_VM->clear();
    ui->statusLabel_VM->clear();

    ui->addressIn_VM->setFocus();
}

void SignVerifyMessageDialog::on_addressIn_SM_textChanged(const QString &address)
{
    if(!model)
        return;

    bool valid = CBitcoinAddress(address.toStdString()).IsValid();
    ui->addressIn_SM->setValid(address.isEmpty() ? true : valid);
    ui->signMessageButton_SM->setEnabled(valid);
}

void SignVerifyMessageDialog::on_addressIn_VM_textChanged(const QString &address)
{
    if(!model)
        return;

    bool valid = CBitcoinAddress(address.toStdString()).IsValid();
    ui->addressIn_VM->setValid(address.isEmpty() ? true : valid);
    ui->verifyMessageButton_VM->setEnabled(valid);
}

bool SignVerifyMessageDialog::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn)
    {
        if (ui->verifyMessageWidget->isHidden())
        {
            /* Clear status message on focus change */
            ui->statusLabel_SM->clear();

            /* Select generated signature */
            if (object == ui->signatureOut_SM)
            {
                ui->signatureOut_SM->selectAll();
                return true;
            }
        }
        else if (ui->signMessageWidget->isHidden())
        {
            /* Clear status message on focus change */
            ui->statusLabel_VM->clear();
        }
    }
    return QDialog::eventFilter(object, event);
}
