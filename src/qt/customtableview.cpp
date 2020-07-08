/**
 * @brief Implements the custom functionality of the QTableView such as copy-paste and key events
 * @author Yongkie Wiyogo
 */

#include "customtableview.h"
#include <QApplication>
#include <QClipboard>
#include <QDate>
#include "../Config.h"
// Customizing the QTableView

CustomTableView::CustomTableView(QWidget *parent)
    : QTableView(parent)
{
}

CustomTableView::~CustomTableView() {}

void CustomTableView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier)
    {
        QApplication::clipboard()->setText(this->currentIndex().data().toString());
    }
    else if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_V)
    {
        model()->setData(currentIndex(), QApplication::clipboard()->text());
    }
    else if (event->key() == Qt::Key_Return)
    {
        // we captured the Enter key press, now we need to move to the next row
        qint32 row = currentIndex().row();
        qint32 col = currentIndex().column();

        if (col >= model()->columnCount() - 1)
        {
            col = 0;
            row++;
        }
        else
        {
            col++;
        }

        if (row >= model()->rowCount())
            row = 0;

        // if (state() == QAbstractItemView::EditingState)
        // {
        // if we are editing, confirm and move to the row below
        QModelIndex oNextIndex = model()->index(row, col);
        setCurrentIndex(oNextIndex);
        selectionModel()->select(oNextIndex, QItemSelectionModel::ClearAndSelect);
        // }
        // else
        // {
        //     if we're not editing, start editing
        //     edit(currentIndex());
        // }
    }
    else
    {
        // any other key was pressed, inform base class
        QTableView::keyPressEvent(event);
    }
}


QString DateDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    return locale.toString(value.toDate(), Config::DATE_FORMAT.c_str());
}
