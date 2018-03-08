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

protected:
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    /* enable save button */
    void enableSaveButton();
    /* disable save button */
    void disableSaveButton();
    void on_saveButton_clicked();
    void on_proxyIp_textChanged(const QString &text);

    void showRestartWarning_Proxy();
    void showRestartWarning_Lang();
    void updateDisplayUnit();
    void handleProxyIpValid(QValidatedLineEdit *object, bool fState);

signals:
    void proxyIpValid(QValidatedLineEdit *object, bool fValid);

private:
    Ui::SettingsPage *ui;
    OptionsModel *model;
    MonitoredDataMapper *mapper;
    bool fRestartWarningDisplayed_Proxy;
    bool fRestartWarningDisplayed_Lang;
    bool fProxyIpValid;
};

#endif // SETTINGSPAGE_H
