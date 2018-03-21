#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "clientmodel.h"
#include "optionsmodel.h"

#include "version.h"

AboutDialog::AboutDialog(QWidget *parent) :
    FaderDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    connect(ui->confirmButton, SIGNAL(clicked()), this, SLOT(accept()));

    if (fUseBlackTheme)
        ui->buttonWidget->setStyleSheet("#buttonWidget { background-color: #212121; }");
    else
        ui->buttonWidget->setStyleSheet("#buttonWidget { background-color: #092f41; }");
}

void AboutDialog::setModel(ClientModel *model)
{
    if(model)
    {
        ui->versionLabel->setText(model->formatFullVersion());
    }
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_buttonBox_accepted()
{
    close();
}
