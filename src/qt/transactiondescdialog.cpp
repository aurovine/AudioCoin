#include "transactiondescdialog.h"
#include "ui_transactiondescdialog.h"

#include "transactiontablemodel.h"
#include "optionsmodel.h"

#include <QModelIndex>

TransactionDescDialog::TransactionDescDialog(const QModelIndex &idx, QWidget *parent) :
    FaderDialog(parent),
    ui(new Ui::TransactionDescDialog)
{
    ui->setupUi(this);
    QString desc = idx.data(TransactionTableModel::LongDescriptionRole).toString();
    ui->detailText->setHtml(desc);

    if (fUseBlackTheme)
        ui->buttonWidget->setStyleSheet("#buttonWidget { background-color: #212121; }");
    else
        ui->buttonWidget->setStyleSheet("#buttonWidget { background-color: #092f41; }");

    ui->closeButton->setIcon(QIcon(fUseBlackTheme ? ":/icons/material/white/close" : ":/icons/material/black/close"));
}

void TransactionDescDialog::on_closeButton_clicked()
{
    reject();
}

TransactionDescDialog::~TransactionDescDialog()
{
    delete ui;
}
