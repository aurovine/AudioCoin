#ifndef TRANSACTIONSPAGE_H
#define TRANSACTIONSPAGE_H

#include <QWidget>

namespace Ui {
    class TransactionsPage;
}

class WalletModel;
class TransactionsPageItemDelegate;
class TransactionFilterProxy;

QT_BEGIN_NAMESPACE
class QModelIndex;
class QMenu;
QT_END_NAMESPACE

class TransactionsPage : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionsPage(QWidget *parent = 0);
    ~TransactionsPage();

    void setModel(WalletModel *model);

    // Date ranges for filter
    enum DateEnum
    {
        All,
        Today,
        ThisWeek,
        ThisMonth,
        LastMonth,
        ThisYear,
        Range
    };

private:
    Ui::TransactionsPage *ui;
    WalletModel *model;
    QMenu *contextMenu;
    
    TransactionsPageItemDelegate *transactionsdelegate;
    TransactionFilterProxy *transactionProxyModel;

private slots:
    void contextualMenu(const QPoint &);
    void dateRangeChanged();
    void showDetails();
    void copyAddress();
    void editLabel();
    void copyLabel();
    void copyAmount();
    void copyTxID();

signals:
    void doubleClicked(const QModelIndex&);

public slots:
    void chooseDate(int idx);
    void chooseType(int idx);
    void changedPrefix(const QString &prefix);
    void changedAmount(const QString &amount);
    void exportClicked();
    void focusTransaction(const QModelIndex&);

};

#endif // TRANSACTIONSPAGE_H
