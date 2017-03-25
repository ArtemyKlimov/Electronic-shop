#ifndef ORDER_H
#define ORDER_H

#include <QWidget>
#include <QSet>
#include <QtSql>
#include <QMap>
#include <QSqlQueryModel>
namespace Ui {
class Order;
}

class Order : public QWidget
{
    Q_OBJECT

public:
    explicit Order(QWidget *parent = 0);
    ~Order();

private slots:
    void on_pushButton_clicked();
    void is_summ_input();
    void on_tableView_clicked(const QModelIndex &index);

private:
    Ui::Order *ui;  
    QSqlQueryModel *order_model;
    QMultiMap<double, int> wares_map;
    QMultiMap<double, int>::iterator it2;
    QSet<QString> types_set;
    QSet<QString>::iterator it;
    int availible_summ;
    void count_n_of_wares(const QModelIndex &index);
};

#endif // ORDER_H
