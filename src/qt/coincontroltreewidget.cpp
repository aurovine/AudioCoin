#include "coincontroltreewidget.h"
#include "coincontroldialog.h"

CoinControlTreeWidget::CoinControlTreeWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    this->parent = parent;
}

void CoinControlTreeWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) // press spacebar -> select checkbox
    {
        event->ignore();
        int columnIndex = 0;

        if (this->currentItem())
            this->currentItem()->setCheckState(columnIndex, ((this->currentItem()->checkState(columnIndex) == Qt::Checked) ? Qt::Unchecked : Qt::Checked));
    }
    else if (event->key() == Qt::Key_Enter) // press ok -> confirm dialog
    {
        event->ignore();
        CoinControlDialog *coinControlDialog = (CoinControlDialog *)parent;
        coinControlDialog->accept();
    }
    else
    {
        this->QTreeWidget::keyPressEvent(event);
    }
}