#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QDialog>

namespace Ui {
class SettingsPage;
}
class OptionsModel;
class MonitoredDataMapper;
class QValidatedLineEdit;

/** Preferences dialog. */
class SettingsPage : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = 0);
    ~SettingsPage();

    void setModel(OptionsModel *model);
    void setMapper();

private slots:
    /* enable save button */
    void enableSaveButton();
    /* disable save button */
    void disableSaveButton();

    void updateStatusLabel();
    void on_saveButton_clicked();
    void on_proxyIp_textChanged(const QString &text);
    void on_proxyPort_textChanged(const QString &text);

    void showRestartWarning_Proxy();
    void showRestartWarning_Lang();
    void updateDisplayUnit();

// signals:
//     void proxyIpValid(QValidatedLineEdit *object, bool fValid);

private:
    Ui::SettingsPage *ui;
    OptionsModel *model;
    MonitoredDataMapper *mapper;
    bool fRestartWarningDisplayed_Proxy;
    bool fRestartWarningDisplayed_Lang;
    bool fProxyIpValid;
    bool fProxyPortValid;
};

#endif // SETTINGSPAGE_H
