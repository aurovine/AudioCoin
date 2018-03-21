#include "editaddressdialog.h"
#include "ui_editaddressdialog.h"

#include "base58.h"
#include "addresstablemodel.h"
#include "guiutil.h"
#include "guiconstants.h"
#include "optionsmodel.h"

#include <QDataWidgetMapper>
#include <QMessageBox>

EditAddressDialog::EditAddressDialog(Mode mode, QWidget *parent) :
    FaderDialog(parent),
    ui(new Ui::EditAddressDialog), mapper(0), mode(mode), model(0)
{
    ui->setupUi(this);

#ifdef Q_OS_MAC
    ui->labelEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->addressEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
#endif

#if QT_VERSION >= 0x040700
    /* Do not move this to the XML file, Qt before 4.7 will choke on it */
    ui->labelEdit->setPlaceholderText(tr("Address label"));
    ui->addressEdit->setPlaceholderText(tr("Address"));
#endif

    GUIUtil::setupAddressWidget(ui->addressEdit, this);

    switch(mode)
    {
    case NewReceivingAddress:
        ui->titleLabel->setText(tr("New receiving address"));
        setWindowTitle(tr("New receiving address"));
        ui->titleIcon->setPixmap(QPixmap(":/icons/material/white/plus"));
        ui->addressEdit->hide();
        ui->addressLabel->hide();
        break;
    case NewSendingAddress:
        ui->titleLabel->setText(tr("New sending address"));
        setWindowTitle(tr("New sending address"));
        ui->titleIcon->setPixmap(QPixmap(":/icons/material/white/plus"));
        ui->confirmButton->setEnabled(false);
        break;
    case EditReceivingAddress:
        ui->titleLabel->setText(tr("Edit receiving address"));
        setWindowTitle(tr("Edit receiving address"));
        ui->titleIcon->setPixmap(QPixmap(":/icons/material/white/edit"));
        ui->addressEdit->setEnabled(false);
        break;
    case EditSendingAddress:
        ui->titleLabel->setText(tr("Edit sending address"));
        ui->titleIcon->setPixmap(QPixmap(":/icons/material/white/edit"));
        setWindowTitle(tr("Edit sending address"));
        break;
    }

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    connect(ui->addressEdit, &QLineEdit::textChanged, this, &EditAddressDialog::textChanged);
    connect(ui->cancelButton, &QPushButton::clicked, this, &EditAddressDialog::reject);
    connect(ui->confirmButton, &QPushButton::clicked, this, &EditAddressDialog::accept);

    if (fUseBlackTheme)
        ui->buttonWidget->setStyleSheet("#buttonWidget { background-color: #212121; }");
    else
        ui->buttonWidget->setStyleSheet("#buttonWidget { background-color: #092f41; }");

    ui->cancelButton->setIcon(QIcon(fUseBlackTheme ? ":/icons/material/white/close" : ":/icons/material/black/close"));
}

EditAddressDialog::~EditAddressDialog()
{
    delete ui;
}

void EditAddressDialog::textChanged(const QString &address)
{
    if(!model)
        return;

    bool valid = CBitcoinAddress(address.toStdString()).IsValid();
    ui->addressEdit->setValid(address.isEmpty() ? true : valid);
    ui->confirmButton->setEnabled(valid);
}

void EditAddressDialog::setModel(AddressTableModel *model)
{
    this->model = model;
    if(!model)
        return;

    mapper->setModel(model);
    mapper->addMapping(ui->labelEdit, AddressTableModel::Label);
    mapper->addMapping(ui->addressEdit, AddressTableModel::Address);
}

void EditAddressDialog::loadRow(int row)
{
    mapper->setCurrentIndex(row);
}

bool EditAddressDialog::saveCurrentRow()
{
    if(!model)
        return false;

    switch(mode)
    {
    case NewReceivingAddress:
    case NewSendingAddress:
        address = model->addRow(
                mode == NewSendingAddress ? AddressTableModel::Send : AddressTableModel::Receive,
                ui->labelEdit->text(),
                ui->addressEdit->text());
        break;
    case EditReceivingAddress:
    case EditSendingAddress:
        if(mapper->submit())
        {
            address = ui->addressEdit->text();
        }
        break;
    }
    return !address.isEmpty();
}

void EditAddressDialog::accept()
{
    if(!model)
        return;

    if(!saveCurrentRow())
    {
        switch(model->getEditStatus())
        {
        case AddressTableModel::OK:
            // Failed with unknown reason. Just reject.
            break;
        case AddressTableModel::NO_CHANGES:
            // No changes were made during edit operation. Just reject.
            break;
        case AddressTableModel::INVALID_ADDRESS:
            QMessageBox::warning(this, windowTitle(),
                tr("The entered address \"%1\" is not a valid Audiocoin address.").arg(ui->addressEdit->text()),
                QMessageBox::Ok, QMessageBox::Ok);
            break;
        case AddressTableModel::DUPLICATE_ADDRESS:
            QMessageBox::warning(this, windowTitle(),
                tr("The entered address \"%1\" is already in the address book.").arg(ui->addressEdit->text()),
                QMessageBox::Ok, QMessageBox::Ok);
            break;
        case AddressTableModel::WALLET_UNLOCK_FAILURE:
            QMessageBox::critical(this, windowTitle(),
                tr("Could not unlock wallet."),
                QMessageBox::Ok, QMessageBox::Ok);
            break;
        case AddressTableModel::KEY_GENERATION_FAILURE:
            QMessageBox::critical(this, windowTitle(),
                tr("New key generation failed."),
                QMessageBox::Ok, QMessageBox::Ok);
            break;

        }
        return;
    }
    done(QDialog::Accepted);
}

QString EditAddressDialog::getAddress() const
{
    return address;
}

void EditAddressDialog::setAddress(const QString &address)
{
    this->address = address;
    ui->addressEdit->setText(address);
}
