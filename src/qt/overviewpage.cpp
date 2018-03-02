#include "overviewpage.h"
#include "ui_overviewpage.h"

#include "clientmodel.h"
#include "walletmodel.h"
#include "bitcoinunits.h"
#include "optionsmodel.h"
#include "transactiontablemodel.h"
#include "transactionfilterproxy.h"
#include "guiutil.h"
#include "guiconstants.h"

#include <QAbstractItemDelegate>
#include <QPainter>

#define DECORATION_SIZE 44
#define NUM_ITEMS 10

class TxViewDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    TxViewDelegate(): QAbstractItemDelegate(), unit(BitcoinUnits::BTC)
    {

    }

    inline void paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index ) const
    {
        painter->save();

        painter->setRenderHint(QPainter::Antialiasing, true);
        
        int pad = 20;

        QRect mainRect = option.rect;
        QRect contentRect(mainRect.left() + pad, mainRect.top() + pad / 2, mainRect.width() - 2 * pad, mainRect.height() - pad);

        int halfheight = (contentRect.height() - pad) / 2;

        QColor color = option.palette.color(QPalette::Text);

        QFont font;
        font.setFamily(QStringLiteral("Open Sans"));
        font.setPointSize(12);
        font.setBold(false);
        // font.setCapitalization(QFont::AllUppercase);
        painter->setFont(font);

        QRect txTypeRect(contentRect.left(), contentRect.top(), 24, contentRect.height());
        QRect dateRect(txTypeRect.right() + pad, contentRect.top(), 100, contentRect.height());
        QRect addressRect(dateRect.right() + pad, contentRect.top(), contentRect.width() - 384, contentRect.height());
        QRect amountRect(addressRect.right() + pad, contentRect.top(), 200, contentRect.height());

        qint64 amount = index.data(TransactionTableModel::AmountRole).toLongLong();

        bool confirmed = index.data(TransactionTableModel::ConfirmedRole).toBool();

        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        icon.paint(painter, txTypeRect);

        painter->setPen(fUseBlackTheme ? QColor(255, 255, 255) : QColor(75, 75, 75));
        // painter->drawText(txTypeRect, Qt::AlignHCenter|Qt::AlignVCenter, qvariant_cast<QString>(index.data(TransactionTableModel::TransactionType)));

        QString dateStr = GUIUtil::dateTimeStr(index.data(TransactionTableModel::DateRole).toDateTime());
        QString addressStr = index.data(Qt::DisplayRole).toString();
        QString amountStr = BitcoinUnits::formatWithUnit(unit, amount, true);

        painter->drawText(dateRect, Qt::AlignHCenter|Qt::AlignVCenter, dateStr);
        painter->drawText(addressRect, Qt::AlignLeft|Qt::AlignVCenter, addressStr);

        if (fUseBlackTheme)
        {
            painter->setPen(Qt::white);
        }
        else if (!confirmed)
        {
            painter->setPen(COLOR_UNCONFIRMED);
            amountStr = QString("[") + amountStr + QString("]");
        }
        else if (amount < 0)
        {
            painter->setPen(COLOR_NEGATIVE);
        }
        
        painter->drawText(amountRect, Qt::AlignRight|Qt::AlignVCenter, amountStr);

        painter->restore();
    }

    inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QSize(DECORATION_SIZE, DECORATION_SIZE);
    }

    int unit;

};
#include "overviewpage.moc"

OverviewPage::OverviewPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OverviewPage),
    clientModel(0),
    walletModel(0),
    currentBalance(-1),
    currentStake(0),
    currentUnconfirmedBalance(-1),
    currentImmatureBalance(-1),
    txdelegate(new TxViewDelegate()),
    filter(0)
{
    ui->setupUi(this);

    // Recent transactions
    ui->listTransactions->setItemDelegate(txdelegate);
    // ui->listTransactions->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
    ui->listTransactions->setMaximumHeight(NUM_ITEMS * DECORATION_SIZE);
    ui->listTransactions->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

#ifdef Q_OS_MAC
    ui->listTransactions->setAttribute(Qt::WA_MacShowFocusRect, false);
#endif

    int countSelectIndex = ui->transactionCountSelect->findText(QString::number(NUM_ITEMS));
    if (countSelectIndex != -1)
        ui->transactionCountSelect->setCurrentIndex(countSelectIndex);

    connect(ui->listTransactions, SIGNAL(clicked(QModelIndex)), this, SLOT(handleTransactionClicked(QModelIndex)));
    // connect(ui->transactionCountSelect, SIGNAL(currentTextChanged(QString)), this, SLOT(updateListFilter(QString)));

    // init "out of sync" warning labels
    ui->totalBalanceSyncStatus->setText("(" + tr("OUT OF SYNC") + ")");
    ui->balanceSyncStatus->setText("(" + tr("OUT OF SYNC") + ")");
    ui->transactionsSyncStatus->setText("(" + tr("OUT OF SYNC") + ")");

    // start with displaying the "out of sync" warnings
    showOutOfSyncWarning(true);

    if (fUseBlackTheme)
    {
        const char* whiteLabelQSS = "QLabel { color: rgb(255,255,255); }";
        ui->labelSpendableAmount->setStyleSheet(whiteLabelQSS);
        ui->labelStakeAmount->setStyleSheet(whiteLabelQSS);
        ui->labelUnconfirmedAmount->setStyleSheet(whiteLabelQSS);
        // ui->labelImmature->setStyleSheet(whiteLabelQSS);
        ui->labelTotal->setStyleSheet(whiteLabelQSS);
    }
}

void OverviewPage::on_transactionCountSelect_currentTextChanged(const QString &text)
{
    int count = text.toInt();

    filter = new TransactionFilterProxy();
    filter->setSourceModel(this->walletModel->getTransactionTableModel());
    filter->setLimit(count);
    filter->setDynamicSortFilter(true);
    filter->setSortRole(Qt::EditRole);
    filter->setShowInactive(false);
    filter->sort(TransactionTableModel::Status, Qt::DescendingOrder);
    ui->listTransactions->setModel(filter);

    ui->listTransactions->setMaximumHeight(count * DECORATION_SIZE);
}

void OverviewPage::handleTransactionClicked(const QModelIndex &index)
{
    if(filter)
        emit transactionClicked(filter->mapToSource(index));
}

OverviewPage::~OverviewPage()
{
    delete ui;
}

void OverviewPage::setBalance(qint64 balance, qint64 stake, qint64 unconfirmedBalance, qint64 immatureBalance)
{
    int unit = walletModel->getOptionsModel()->getDisplayUnit();
    currentBalance = balance;
    currentStake = stake;
    currentUnconfirmedBalance = unconfirmedBalance;
    currentImmatureBalance = immatureBalance;
    ui->labelSpendableAmount->setText(BitcoinUnits::formatWithUnit(unit, balance));
    ui->labelStakeAmount->setText(BitcoinUnits::formatWithUnit(unit, stake));
    ui->labelUnconfirmedAmount->setText(BitcoinUnits::formatWithUnit(unit, unconfirmedBalance));
    // ui->labelImmature->setText(BitcoinUnits::formatWithUnit(unit, immatureBalance));
    ui->labelTotal->setText(BitcoinUnits::formatWithUnit(unit, balance + stake + unconfirmedBalance + immatureBalance));

    // only show immature (newly mined) balance if it's non-zero, so as not to complicate things
    // for the non-mining users
    // bool showImmature = immatureBalance != 0;
    // ui->labelImmature->setVisible(showImmature);
    // ui->labelImmatureText->setVisible(showImmature);
}

void OverviewPage::setClientModel(ClientModel *model)
{
    this->clientModel = model;
    if(model)
    {
        // Show warning if this is a prerelease version
        connect(model, SIGNAL(alertsChanged(QString)), this, SLOT(updateAlerts(QString)));
        updateAlerts(model->getStatusBarWarnings());
    }
}

void OverviewPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
    if(model && model->getOptionsModel())
    {
        // Set up transaction list
        filter = new TransactionFilterProxy();
        filter->setSourceModel(model->getTransactionTableModel());
        filter->setLimit(NUM_ITEMS);
        filter->setDynamicSortFilter(true);
        filter->setSortRole(Qt::EditRole);
        filter->setShowInactive(false);
        filter->sort(TransactionTableModel::Status, Qt::DescendingOrder);

        ui->listTransactions->setModel(filter);
        ui->listTransactions->setModelColumn(TransactionTableModel::ToAddress);

        // Keep up to date with wallet
        setBalance(model->getBalance(), model->getStake(), model->getUnconfirmedBalance(), model->getImmatureBalance());
        connect(model, SIGNAL(balanceChanged(qint64, qint64, qint64, qint64)), this, SLOT(setBalance(qint64, qint64, qint64, qint64)));

        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
    }

    // update the display unit, to not use the default ("BTC")
    updateDisplayUnit();
}

void OverviewPage::updateDisplayUnit()
{
    if(walletModel && walletModel->getOptionsModel())
    {
        if(currentBalance != -1)
            setBalance(currentBalance, walletModel->getStake(), currentUnconfirmedBalance, currentImmatureBalance);

        // Update txdelegate->unit with the current unit
        txdelegate->unit = walletModel->getOptionsModel()->getDisplayUnit();

        ui->listTransactions->update();
    }
}

void OverviewPage::updateAlerts(const QString &warnings)
{
    this->ui->labelAlerts->setVisible(!warnings.isEmpty());
    this->ui->labelAlerts->setText(warnings);
}

void OverviewPage::showOutOfSyncWarning(bool fShow)
{
    ui->totalBalanceSyncStatus->setVisible(fShow);
    ui->balanceSyncStatus->setVisible(fShow);
    ui->transactionsSyncStatus->setVisible(fShow);
}
