#ifndef ADD_DATA_H
#define ADD_DATA_H

#include <QWidget>
#include <QtSql>
#include <QSqlTableModel>


namespace Ui {
class Add_data;
}

class Add_data : public QWidget
{
    Q_OBJECT

public:
    explicit Add_data(QWidget *parent = 0);
    ~Add_data();

private slots:
    void on_pushButton_clicked();
    void checkLineEdits();
    void on_pushButton_2_clicked();

signals:
    void refresh_table();

private:
    Ui::Add_data *ui;
    QSqlTableModel *model;
};

#endif // ADD_DATA_H
