/**
 * @brief Declares the custom functionality class of the QTableView
 * @author Yongkie Wiyogo
 */

#ifndef CUSTOMTABLEVIEW_H
#define CUSTOMTABLEVIEW_H

#include <QKeyEvent>
#include <QStyledItemDelegate>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

class CustomTableView : public QTableView
{
    Q_OBJECT

   public:
    CustomTableView(QWidget* parent = nullptr);
    ~CustomTableView();

   private slots:
    void keyPressEvent(QKeyEvent* event);
};

// DateDelegate class allows the column sort by date in the QTableView
class DateDelegate : public QStyledItemDelegate
{
    QString displayText(const QVariant& value, const QLocale& locale) const;
};
#endif