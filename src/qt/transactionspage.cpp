#include "transactionspage.h"
#include "ui_transactionspage.h"

#include "transactionfilterproxy.h"
#include "transactionrecord.h"
#include "walletmodel.h"
#include "addresstablemodel.h"
#include "transactiontablemodel.h"
#include "bitcoinunits.h"
#include "csvmodelwriter.h"
#include "transactiondescdialog.h"
#include "editaddressdialog.h"
#include "optionsmodel.h"
#include "guiutil.h"

#include <QDoubleValidator>
#include <QHeaderView>
#include <QMessageBox>
#include <QPoint>
#include <QMenu>
#include <QLabel>
#include <QDateTimeEdit>
#include <QStyledItemDelegate>
#include <QPainter>

#define ROW_HEIGHT  50

class TransactionsPageItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    TransactionsPageItemDelegate(): QStyledItemDelegate(), unit(BitcoinUnits::BTC)
    {

    }

    inline void paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index ) const
    {
        // painter->save();

        // painter->setRenderHint(QPainter::Antialiasing, true);

        // QRect mainRect = option.rect;
        
        // painter->setPen(Qt::black);
        
        // painter->drawText(mainRect, Qt::AlignRight|Qt::AlignVCenter, "hahaha");

        // painter->restore();
        QStyledItemDelegate::paint(painter, option, index);
    }

    inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QSize(ROW_HEIGHT, ROW_HEIGHT);
    }

    int unit;
};
#include "transactionspage.moc"

TransactionsPage::TransactionsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransactionsPage),
    model(0),
    transactionsdelegate(new TransactionsPageItemDelegate())
    // transactionProxyModel(0)
{
    ui->setupUi(this);

#ifdef Q_OS_MAC
    ui->addressWidget->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->amountWidget->setAttribute(Qt::WA_MacShowFocusRect, 0);
#endif

    ui->transactionView->setItemDelegate(transactionsdelegate);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0,0,0,0);
#ifdef Q_OS_MAC
    hlayout->setSpacing(5);
    hlayout->addSpacing(26);
#else
    hlayout->setSpacing(0);
    hlayout->addSpacing(23);
#endif

    ui->dateWidget->setItemDelegate(new QStyledItemDelegate());
    ui->dateWidget->addItem(tr("All"), All);
    ui->dateWidget->addItem(tr("Today"), Today);
    ui->dateWidget->addItem(tr("This week"), ThisWeek);
    ui->dateWidget->addItem(tr("This month"), ThisMonth);
    ui->dateWidget->addItem(tr("Last month"), LastMonth);
    ui->dateWidget->addItem(tr("This year"), ThisYear);
    ui->dateWidget->addItem(tr("Range..."), Range);

    ui->dateFrom->setDisplayFormat("dd/MM/yy");
    ui->dateFrom->setCalendarPopup(true);
    ui->dateFrom->setMinimumWidth(100);
    ui->dateFrom->setDate(QDate::currentDate().addDays(-7));

    ui->dateTo->setDisplayFormat("dd/MM/yy");
    ui->dateTo->setCalendarPopup(true);
    ui->dateTo->setMinimumWidth(100);
    ui->dateTo->setDate(QDate::currentDate());

    // Hide by default
    ui->dateRangeWidget->setVisible(false);

    // Notify on change
    connect(ui->dateFrom, SIGNAL(dateChanged(QDate)), this, SLOT(dateRangeChanged()));
    connect(ui->dateTo, SIGNAL(dateChanged(QDate)), this, SLOT(dateRangeChanged()));

    ui->typeWidget->setItemDelegate(new QStyledItemDelegate());

    ui->typeWidget->addItem(tr("All"), TransactionFilterProxy::ALL_TYPES);
    ui->typeWidget->addItem(tr("Received with"), TransactionFilterProxy::TYPE(TransactionRecord::RecvWithAddress) |
                                                 TransactionFilterProxy::TYPE(TransactionRecord::RecvFromOther));
    ui->typeWidget->addItem(tr("Sent to"), TransactionFilterProxy::TYPE(TransactionRecord::SendToAddress) |
                                           TransactionFilterProxy::TYPE(TransactionRecord::SendToOther));
    ui->typeWidget->addItem(tr("To yourself"), TransactionFilterProxy::TYPE(TransactionRecord::SendToSelf));
    ui->typeWidget->addItem(tr("Mined"), TransactionFilterProxy::TYPE(TransactionRecord::Generated));
    ui->typeWidget->addItem(tr("Other"), TransactionFilterProxy::TYPE(TransactionRecord::Other));

#if QT_VERSION >= 0x040700
    /* Do not move this to the XML file, Qt before 4.7 will choke on it */
    ui->addressWidget->setPlaceholderText(tr("Enter address or label to search"));
    ui->amountWidget->setPlaceholderText(tr("Min amount"));
#endif

    ui->amountWidget->setValidator(new QDoubleValidator(0, 1e20, 8, this));

    // int width = ui->transactionView->verticalScrollBar()->sizeHint().width();

    // Cover scroll bar width with spacing
// #ifdef Q_OS_MAC
//     hlayout->addSpacing(width+2);
// #else
//     hlayout->addSpacing(width);
// #endif
    
    // Actions
    QAction *copyAddressAction = new QAction(tr("Copy address"), this);
    QAction *copyLabelAction = new QAction(tr("Copy label"), this);
    QAction *copyAmountAction = new QAction(tr("Copy amount"), this);
    QAction *copyTxIDAction = new QAction(tr("Copy transaction ID"), this);
    QAction *editLabelAction = new QAction(tr("Edit label"), this);
    QAction *showDetailsAction = new QAction(tr("Show transaction details"), this);

    contextMenu = new QMenu();
    contextMenu->addAction(copyAddressAction);
    contextMenu->addAction(copyLabelAction);
    contextMenu->addAction(copyAmountAction);
    contextMenu->addAction(copyTxIDAction);
    contextMenu->addAction(editLabelAction);
    contextMenu->addAction(showDetailsAction);

    // Connect actions
    connect(ui->dateWidget, SIGNAL(activated(int)), this, SLOT(chooseDate(int)));
    connect(ui->typeWidget, SIGNAL(activated(int)), this, SLOT(chooseType(int)));
    connect(ui->addressWidget, SIGNAL(textChanged(QString)), this, SLOT(changedPrefix(QString)));
    connect(ui->amountWidget, SIGNAL(textChanged(QString)), this, SLOT(changedAmount(QString)));

    connect(ui->transactionView, SIGNAL(doubleClicked(QModelIndex)), this, SIGNAL(doubleClicked(QModelIndex)));
    connect(ui->transactionView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualMenu(QPoint)));

    connect(copyAddressAction, SIGNAL(triggered()), this, SLOT(copyAddress()));
    connect(copyLabelAction, SIGNAL(triggered()), this, SLOT(copyLabel()));
    connect(copyAmountAction, SIGNAL(triggered()), this, SLOT(copyAmount()));
    connect(copyTxIDAction, SIGNAL(triggered()), this, SLOT(copyTxID()));
    connect(editLabelAction, SIGNAL(triggered()), this, SLOT(editLabel()));
    connect(showDetailsAction, SIGNAL(triggered()), this, SLOT(showDetails()));
}

void TransactionsPage::setModel(WalletModel *model)
{
    this->model = model;
    if(model)
    {
        transactionProxyModel = new TransactionFilterProxy(this);
        transactionProxyModel->setSourceModel(model->getTransactionTableModel());
        transactionProxyModel->setDynamicSortFilter(true);
        transactionProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
        transactionProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

        transactionProxyModel->setSortRole(Qt::EditRole);

        ui->transactionView->setModel(transactionProxyModel);
        ui->transactionView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->transactionView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        ui->transactionView->setSortingEnabled(true);
        ui->transactionView->sortByColumn(TransactionTableModel::Date, Qt::DescendingOrder);
        ui->transactionView->verticalHeader()->hide();

        ui->transactionView->horizontalHeader()->resizeSection(TransactionTableModel::Status, 54);
        ui->transactionView->horizontalHeader()->resizeSection(TransactionTableModel::Date, 155);
        ui->transactionView->horizontalHeader()->resizeSection(TransactionTableModel::Type, 155);
        ui->transactionView->horizontalHeader()->setResizeMode(TransactionTableModel::ToAddress, QHeaderView::Stretch);
        ui->transactionView->horizontalHeader()->resizeSection(TransactionTableModel::Amount, 220);
    }
}

TransactionsPage::~TransactionsPage()
{
    delete ui;
}

void TransactionsPage::chooseDate(int idx)
{
    if(!transactionProxyModel)
        return;
    QDate current = QDate::currentDate();
    ui->dateRangeWidget->setVisible(false);
    switch(ui->dateWidget->itemData(idx).toInt())
    {
    case All:
        transactionProxyModel->setDateRange(
                TransactionFilterProxy::MIN_DATE,
                TransactionFilterProxy::MAX_DATE);
        break;
    case Today:
        transactionProxyModel->setDateRange(
                QDateTime(current),
                TransactionFilterProxy::MAX_DATE);
        break;
    case ThisWeek: {
        // Find last Monday
        QDate startOfWeek = current.addDays(-(current.dayOfWeek()-1));
        transactionProxyModel->setDateRange(
                QDateTime(startOfWeek),
                TransactionFilterProxy::MAX_DATE);

        } break;
    case ThisMonth:
        transactionProxyModel->setDateRange(
                QDateTime(QDate(current.year(), current.month(), 1)),
                TransactionFilterProxy::MAX_DATE);
        break;
    case LastMonth:
        transactionProxyModel->setDateRange(
                QDateTime(QDate(current.year(), current.month()-1, 1)),
                QDateTime(QDate(current.year(), current.month(), 1)));
        break;
    case ThisYear:
        transactionProxyModel->setDateRange(
                QDateTime(QDate(current.year(), 1, 1)),
                TransactionFilterProxy::MAX_DATE);
        break;
    case Range:
        ui->dateRangeWidget->setVisible(true);
        dateRangeChanged();
        break;
    }
}

void TransactionsPage::chooseType(int idx)
{
    if(!transactionProxyModel)
        return;
    transactionProxyModel->setTypeFilter(
        ui->typeWidget->itemData(idx).toInt());
}

void TransactionsPage::changedPrefix(const QString &prefix)
{
    if(!transactionProxyModel)
        return;
    transactionProxyModel->setAddressPrefix(prefix);
}

void TransactionsPage::changedAmount(const QString &amount)
{
    if(!transactionProxyModel)
        return;
    qint64 amount_parsed = 0;
    if(BitcoinUnits::parse(model->getOptionsModel()->getDisplayUnit(), amount, &amount_parsed))
    {
        transactionProxyModel->setMinAmount(amount_parsed);
    }
    else
    {
        transactionProxyModel->setMinAmount(0);
    }
}

void TransactionsPage::exportClicked()
{
    // CSV is currently the only supported format
    QString filename = GUIUtil::getSaveFileName(
            this,
            tr("Export Transaction Data"), QString(),
            tr("Comma separated file (*.csv)"));

    if (filename.isNull()) return;

    CSVModelWriter writer(filename);

    // name, column, role
    writer.setModel(transactionProxyModel);
    writer.addColumn(tr("Confirmed"), 0, TransactionTableModel::ConfirmedRole);
    writer.addColumn(tr("Date"), 0, TransactionTableModel::DateRole);
    writer.addColumn(tr("Type"), TransactionTableModel::Type, Qt::EditRole);
    writer.addColumn(tr("Label"), 0, TransactionTableModel::LabelRole);
    writer.addColumn(tr("Address"), 0, TransactionTableModel::AddressRole);
    writer.addColumn(tr("Amount"), 0, TransactionTableModel::FormattedAmountRole);
    writer.addColumn(tr("ID"), 0, TransactionTableModel::TxIDRole);

    if(!writer.write())
    {
        QMessageBox::critical(this, tr("Error exporting"), tr("Could not write to file %1.").arg(filename),
                              QMessageBox::Abort, QMessageBox::Abort);
    }
}

void TransactionsPage::contextualMenu(const QPoint &point)
{
    QModelIndex index = ui->transactionView->indexAt(point);
    if(index.isValid())
    {
        contextMenu->exec(QCursor::pos());
    }
}

void TransactionsPage::copyAddress()
{
    GUIUtil::copyEntryData(ui->transactionView, 0, TransactionTableModel::AddressRole);
}

void TransactionsPage::copyLabel()
{
    GUIUtil::copyEntryData(ui->transactionView, 0, TransactionTableModel::LabelRole);
}

void TransactionsPage::copyAmount()
{
    GUIUtil::copyEntryData(ui->transactionView, 0, TransactionTableModel::FormattedAmountRole);
}

void TransactionsPage::copyTxID()
{
    GUIUtil::copyEntryData(ui->transactionView, 0, TransactionTableModel::TxIDRole);
}

void TransactionsPage::editLabel()
{
    if(!ui->transactionView->selectionModel() ||!model)
        return;
    QModelIndexList selection = ui->transactionView->selectionModel()->selectedRows();
    if(!selection.isEmpty())
    {
        AddressTableModel *addressBook = model->getAddressTableModel();
        if(!addressBook)
            return;
        QString address = selection.at(0).data(TransactionTableModel::AddressRole).toString();
        if(address.isEmpty())
        {
            // If this transaction has no associated address, exit
            return;
        }
        // Is address in address book? Address book can miss address when a transaction is
        // sent from outside the UI.
        int idx = addressBook->lookupAddress(address);
        if(idx != -1)
        {
            // Edit sending / receiving address
            QModelIndex modelIdx = addressBook->index(idx, 0, QModelIndex());
            // Determine type of address, launch appropriate editor dialog type
            QString type = modelIdx.data(AddressTableModel::TypeRole).toString();

            EditAddressDialog dlg(type==AddressTableModel::Receive
                                         ? EditAddressDialog::EditReceivingAddress
                                         : EditAddressDialog::EditSendingAddress,
                                  this);
            dlg.setModel(addressBook);
            dlg.loadRow(idx);
            dlg.exec();
        }
        else
        {
            // Add sending address
            EditAddressDialog dlg(EditAddressDialog::NewSendingAddress,
                                  this);
            dlg.setModel(addressBook);
            dlg.setAddress(address);
            dlg.exec();
        }
    }
}

void TransactionsPage::showDetails()
{
    if(!ui->transactionView->selectionModel())
        return;
    QModelIndexList selection = ui->transactionView->selectionModel()->selectedRows();
    if(!selection.isEmpty())
    {
        TransactionDescDialog dlg(selection.at(0));
        dlg.exec();
    }
}

void TransactionsPage::dateRangeChanged()
{
    if(!transactionProxyModel)
        return;
    transactionProxyModel->setDateRange(
            QDateTime(ui->dateFrom->date()),
            QDateTime(ui->dateTo->date()).addDays(1));
}

void TransactionsPage::focusTransaction(const QModelIndex &idx)
{
    if(!transactionProxyModel)
        return;
    QModelIndex targetIdx = transactionProxyModel->mapFromSource(idx);
    ui->transactionView->scrollTo(targetIdx);
    ui->transactionView->setCurrentIndex(targetIdx);
    ui->transactionView->setFocus();
}
