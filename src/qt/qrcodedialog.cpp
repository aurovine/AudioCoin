#include "qrcodedialog.h"
#include "ui_qrcodedialog.h"

#include "bitcoinunits.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "optionsmodel.h"

#include <QPixmap>
#include <QUrl>

#include <qrencode.h>

QRCodeDialog::QRCodeDialog(const QString &addr, const QString &label, bool enableReq, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QRCodeDialog),
    model(0),
    address(addr)
{
    ui->setupUi(this);

    setWindowTitle(QString("%1").arg(address));

    ui->chkReqPayment->setVisible(enableReq);
    ui->amountLabel->setVisible(enableReq);
    ui->amount->setVisible(enableReq);

    ui->label->setText(label);

    ui->saveButton->setEnabled(false);

    genCode();
}

QRCodeDialog::~QRCodeDialog()
{
    delete ui;
}

void QRCodeDialog::setModel(OptionsModel *model)
{
    this->model = model;

    if (model)
        connect(model, SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));

    // update the display unit, to not use the default ("BTC")
    updateDisplayUnit();
}

void QRCodeDialog::genCode()
{
    QString uri = getURI();

    if (uri != "")
    {
        ui->lblQRCode->setText("");

        QRcode *code = QRcode_encodeString(uri.toUtf8().constData(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
        if (!code)
        {
            ui->lblQRCode->setText(tr("Error encoding URI into QR Code."));
            return;
        }
        myImage = QImage(code->width + 8, code->width + 8, QImage::Format_RGB32);
        myImage.fill(0xffffff);
        unsigned char *p = code->data;
        for (int y = 0; y < code->width; y++)
        {
            for (int x = 0; x < code->width; x++)
            {
                myImage.setPixel(x + 4, y + 4, ((*p & 1) ? 0x0 : 0xffffff));
                p++;
            }
        }
        QRcode_free(code);

        ui->lblQRCode->setPixmap(QPixmap::fromImage(myImage).scaled(300, 300));

        ui->outUri->setPlainText(uri);
    }
}

QString QRCodeDialog::getURI()
{
    QString ret = QString("audiocoin:%1").arg(address);
    int paramCount = 0;

    ui->outUri->clear();

    if (ui->chkReqPayment->isChecked())
    {
        if (ui->amount->validate())
        {
            // even if we allow a non BTC unit input in amount, we generate the URI with BTC as unit (as defined in BIP21)
            ret += QString("?amount=%1").arg(BitcoinUnits::format(BitcoinUnits::BTC, ui->amount->value()));
            paramCount++;
        }
        else
        {
            ui->saveButton->setEnabled(false);
            ui->lblQRCode->setText(tr("The entered amount is invalid, please check."));
            return QString("");
        }
    }

    if (!ui->label->text().isEmpty())
    {
        QString lbl(QUrl::toPercentEncoding(ui->label->text()));
        ret += QString("%1label=%2").arg(paramCount == 0 ? "?" : "&").arg(lbl);
        paramCount++;
    }

    if (!ui->message->text().isEmpty())
    {
        QString msg(QUrl::toPercentEncoding(ui->message->text()));
        ret += QString("%1message=%2").arg(paramCount == 0 ? "?" : "&").arg(msg);
        paramCount++;
    }

    // limit URI length to prevent a DoS against the QR-Code dialog
    if (ret.length() > MAX_URI_LENGTH)
    {
        ui->saveButton->setEnabled(false);
        ui->lblQRCode->setText(tr("Resulting URI too long, try to reduce the text for label / message."));
        return QString("");
    }

    ui->saveButton->setEnabled(true);
    return ret;
}

void QRCodeDialog::on_amount_textChanged()
{
    genCode();
}

void QRCodeDialog::on_label_textChanged()
{
    genCode();
}

void QRCodeDialog::on_message_textChanged()
{
    genCode();
}

void QRCodeDialog::on_saveButton_clicked()
{
    QString fn = GUIUtil::getSaveFileName(this, tr("Save QR Code"), QString(), tr("PNG Images (*.png)"));
    if (!fn.isEmpty())
        myImage.scaled(EXPORT_IMAGE_SIZE, EXPORT_IMAGE_SIZE).save(fn);
}

void QRCodeDialog::on_chkReqPayment_toggled(bool fChecked)
{
    if (!fChecked)
        // if chkReqPayment is not active, don't display amount as invalid
        ui->amount->setValid(true);

    genCode();
}

void QRCodeDialog::updateDisplayUnit()
{
    if (model)
    {
        // Update amount with the current unit
        ui->amount->setDisplayUnit(model->getDisplayUnit());
    }
}
