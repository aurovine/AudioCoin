#include "settingspage.h"
#include "ui_settingspage.h"

#include "guiconstants.h"
#include "bitcoinunits.h"
#include "monitoreddatamapper.h"
#include "netbase.h"
#include "optionsmodel.h"

#include <QDir>
#include <QIntValidator>
#include <QLocale>
#include <QMessageBox>

SettingsPage::SettingsPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsPage),
    model(0),
    mapper(0),
    fRestartWarningDisplayed_Proxy(false),
    fRestartWarningDisplayed_Lang(false),
    fProxyIpValid(true),
    fProxyPortValid(true)
{
    ui->setupUi(this);

    /* Network elements init */
#ifndef USE_UPNP
    ui->mapPortUpnp->setEnabled(false);
#endif

#ifdef Q_OS_MAC
    ui->proxyIp->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->proxyPort->setAttribute(Qt::WA_MacShowFocusRect, 0);

    // disable windows specific setting checkboxes
    ui->minimizeToTray->setEnabled(false);
    ui->minimizeOnClose->setEnabled(false);
#endif

    ui->proxyIp->setEnabled(false);
    ui->proxyPort->setEnabled(false);
    ui->proxyPort->setValidator(new QIntValidator(1, 65535, this));

    connect(ui->connectSocks, SIGNAL(toggled(bool)), ui->proxyIp, SLOT(setEnabled(bool)));
    connect(ui->connectSocks, SIGNAL(toggled(bool)), ui->proxyPort, SLOT(setEnabled(bool)));
    connect(ui->connectSocks, SIGNAL(clicked(bool)), this, SLOT(showRestartWarning_Proxy()));

    ui->proxyIp->installEventFilter(this);

    /* Display elements init */
    QDir translations(":translations");
    ui->lang->addItem(QString("(") + tr("default") + QString(")"), QVariant(""));
    foreach(const QString &langStr, translations.entryList())
    {
        QLocale locale(langStr);

        /** check if the locale name consists of 2 parts (language_country) */
        if(langStr.contains("_"))
        {
#if QT_VERSION >= 0x040800
            /** display language strings as "native language - native country (locale name)", e.g. "Deutsch - Deutschland (de)" */
            ui->lang->addItem(locale.nativeLanguageName() + QString(" - ") + locale.nativeCountryName() + QString(" (") + langStr + QString(")"), QVariant(langStr));
#else
            /** display language strings as "language - country (locale name)", e.g. "German - Germany (de)" */
            ui->lang->addItem(QLocale::languageToString(locale.language()) + QString(" - ") + QLocale::countryToString(locale.country()) + QString(" (") + langStr + QString(")"), QVariant(langStr));
#endif
        }
        else
        {
#if QT_VERSION >= 0x040800
            /** display language strings as "native language (locale name)", e.g. "Deutsch (de)" */
            ui->lang->addItem(locale.nativeLanguageName() + QString(" (") + langStr + QString(")"), QVariant(langStr));
#else
            /** display language strings as "language (locale name)", e.g. "German (de)" */
            ui->lang->addItem(QLocale::languageToString(locale.language()) + QString(" (") + langStr + QString(")"), QVariant(langStr));
#endif
        }
    }

    ui->unit->setModel(new BitcoinUnits(this));

    /* Widget-to-option mapper */
    mapper = new MonitoredDataMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setOrientation(Qt::Vertical);

    /* enable save button when data modified */
    connect(mapper, SIGNAL(viewModified()), this, SLOT(enableSaveButton()));
    /* disable save button when new data loaded */
    connect(mapper, SIGNAL(currentIndexChanged(int)), this, SLOT(disableSaveButton()));
    /* setup/change UI elements when proxy IP is invalid/valid */
    // connect(this, SIGNAL(proxyIpValid(QValidatedLineEdit *, bool)), this, SLOT(handleProxyIpValid(QValidatedLineEdit *, bool)));
}

SettingsPage::~SettingsPage()
{
    delete ui;
}

void SettingsPage::setModel(OptionsModel *model)
{
    this->model = model;

    if(model)
    {
        connect(model, SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));

        mapper->setModel(model);
        setMapper();
        mapper->toFirst();
    }

    /* update the display unit, to not use the default ("BTC") */
    updateDisplayUnit();

    /* warn only when language selection changes by user action (placed here so init via mapper doesn't trigger this) */
    connect(ui->lang, SIGNAL(valueChanged()), this, SLOT(showRestartWarning_Lang()));

    /* disable save button after settings are loaded as there is nothing to save */
    disableSaveButton();
}

void SettingsPage::setMapper()
{
    /* Main */
    mapper->addMapping(ui->transactionFee, OptionsModel::Fee);
    mapper->addMapping(ui->reserveBalance, OptionsModel::ReserveBalance);
    mapper->addMapping(ui->bitcoinAtStartup, OptionsModel::StartAtStartup);

    /* Network */
    mapper->addMapping(ui->mapPortUpnp, OptionsModel::MapPortUPnP);

    mapper->addMapping(ui->connectSocks, OptionsModel::ProxyUse);
    mapper->addMapping(ui->proxyIp, OptionsModel::ProxyIP);
    mapper->addMapping(ui->proxyPort, OptionsModel::ProxyPort);

    /* Window */
#ifndef Q_OS_MAC
    mapper->addMapping(ui->minimizeToTray, OptionsModel::MinimizeToTray);
    mapper->addMapping(ui->minimizeOnClose, OptionsModel::MinimizeOnClose);
#endif

    /* Display */
    mapper->addMapping(ui->lang, OptionsModel::Language);
    mapper->addMapping(ui->unit, OptionsModel::DisplayUnit);
    mapper->addMapping(ui->coinControlFeatures, OptionsModel::CoinControlFeatures);
    mapper->addMapping(ui->useBlackTheme, OptionsModel::UseBlackTheme);
}

void SettingsPage::enableSaveButton()
{
    /* prevent enabling of the save buttons when data modified, if there is an invalid proxy address present */
    if (fProxyIpValid)
        ui->saveButton->setEnabled(true);
}

void SettingsPage::disableSaveButton()
{
    ui->saveButton->setEnabled(false);
}

void SettingsPage::on_saveButton_clicked()
{
    mapper->submit();
    disableSaveButton();
    // accept();
}

void SettingsPage::showRestartWarning_Proxy()
{
    if(!fRestartWarningDisplayed_Proxy)
    {
        QMessageBox::warning(this, tr("Warning"), tr("This setting will take effect after restarting Audiocoin."), QMessageBox::Ok);
        fRestartWarningDisplayed_Proxy = true;
    }
}

void SettingsPage::showRestartWarning_Lang()
{
    if(!fRestartWarningDisplayed_Lang)
    {
        QMessageBox::warning(this, tr("Warning"), tr("This setting will take effect after restarting Audiocoin."), QMessageBox::Ok);
        fRestartWarningDisplayed_Lang = true;
    }
}

void SettingsPage::updateDisplayUnit()
{
    if(model)
    {
        /* Update transactionFee with the current unit */
        ui->transactionFee->setDisplayUnit(model->getDisplayUnit());
    }
}

void SettingsPage::on_proxyIp_textChanged(const QString &text)
{
    CService addr;

    fProxyIpValid = LookupNumeric(text.toStdString().c_str(), addr);
    if(fProxyIpValid)
    {
        enableSaveButton();
    }
    else
    {
        disableSaveButton();
    }

    ui->proxyIp->setValid(fProxyIpValid);
    updateStatusLabel();
}

void SettingsPage::on_proxyPort_textChanged(const QString &text)
{
    fProxyPortValid = !text.isEmpty();

    if(fProxyPortValid)
    {
        enableSaveButton();
    }
    else
    {
        disableSaveButton();
    }

    ui->proxyPort->setValid(fProxyPortValid);
    updateStatusLabel();
}

void SettingsPage::updateStatusLabel()
{
    if(!fProxyIpValid && !fProxyPortValid)
    {
        ui->statusLabel->setText(tr("The supplied proxy address and port are invalid"));
    }
    else if(!fProxyIpValid)
    {
        ui->statusLabel->setText(tr("The supplied proxy address is invalid"));
    }
    else if(!fProxyPortValid)
    {
        ui->statusLabel->setText(tr("The supplied proxy port is invalid"));
    }
    else
    {
        ui->statusLabel->clear();
    }
}
