/**
 * @brief Declares the custom functionality class of the QTableView
 * @author Yongkie Wiyogo
 */

#ifndef CUSTOMTABLEVIEW_H
#define CUSTOMTABLEVIEW_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QTableView>
#include <QKeyEvent>

class CustomTableView : public QTableView
{
  Q_OBJECT

public:
  CustomTableView(QWidget *parent = nullptr);
  ~CustomTableView();

private slots:
  void keyPressEvent(QKeyEvent *event);
};

#endif