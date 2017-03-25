#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QtSql>
#include <QDebug>
#include <QFileInfo>
#include <QSet>
#include "add_data.h"
#include "ui_add_data.h"
#include "drawgraph.h"
#include "ui_drawgraph.h"
#include "order.h"
#include "ui_order.h"

typedef QSqlDatabase mydb;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private slots:
    void on_pushButton_showRes_clicked();

    void on_pushButton_add_clicked();

    void on_pushButton_delete_clicked();

    void on_action_3_triggered();

    void obr_refresh_table();

    void on_action_2_triggered();

    void on_Push_Button2_clicked();

    void on_action_triggered();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase mydb;
    QSqlTableModel *model;
    QSet<QString> type_set;
    QSet<QString> country_set;
    QSet<QString>::iterator it;
   // Add_data *new_data;
    DrawGraph *drawer;
    Order *new_order;
    void setTotal();
    void connectDB();
    bool tableIsCreated;
    bool drawerWasCalled;
    bool AddingWasCalled;
    bool OrderWasCalled;
};

#endif // MAINWINDOW_H
