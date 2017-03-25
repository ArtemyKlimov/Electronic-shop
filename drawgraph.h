#ifndef DRAWGRAPH_H
#define DRAWGRAPH_H

#include <QWidget>
#include <QSqlTableModel>
#include <QtSql>
#include <QSet>
#include "qcustomplot.h"

namespace Ui {
class DrawGraph;
}

class DrawGraph : public QWidget
{
    Q_OBJECT

public:
    explicit DrawGraph(QWidget *parent = 0);
    ~DrawGraph();
private slots:
    void updateModel();
    void update_if_model_was_selected();
    //void slotRangeChanged(const QCPRange &newRange);

    void on_pushButton_clicked();

signals:
   // void refresh_boxes();

private:
    Ui::DrawGraph *ui;
    QSqlTableModel *model_for_graph;
    QSet<QString> types_set;
    QSet<QString> models_set;
    QSet<QString>::iterator it;
    QSet<QString>::iterator it2;
    QCustomPlot *customPlot;
    QCPGraph *graphic;
    QString filter1;
    QString filter2;

};

#endif // DRAWGRAPH_H
