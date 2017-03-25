#include "drawgraph.h"
#include "ui_drawgraph.h"
#include <QVector>
#include <algorithm>

DrawGraph::DrawGraph(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrawGraph)
{
      ui->setupUi(this);
    updateModel();
    QSqlQuery qry;
    qry.exec("SELECT * FROM products");
    while(qry.next()){
        types_set.insert(qry.value(1).toString());
    }
    ui->comboBox_types->addItem("не выбрано");
    for(it = types_set.begin(); it != types_set.end(); ++it){
         ui->comboBox_types->addItem(*it);
       }
    connect(ui->comboBox_types, SIGNAL(currentTextChanged(QString)), this, SLOT(updateModel()));
    connect(ui->comboBox_models, SIGNAL(currentTextChanged(QString)), this, SLOT(update_if_model_was_selected()));
    customPlot = new QCustomPlot();
    ui->gridLayout->addWidget(customPlot, 0, 0, 1, 1);
    //устанавливаем возможность увеличивать-уменьшать график, передвигать по оис "x"
    customPlot->setInteraction(QCP::iRangeZoom, true);
    customPlot->setInteraction(QCP::iRangeDrag, true);
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal);
    customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot->xAxis->setDateTimeFormat("dd.MM.yy");
    customPlot->yAxis->setRange(0,100);
    customPlot->xAxis->setTickLabelFont(QFont(QFont().family(),8));
    customPlot->yAxis->setTickLabelFont(QFont(QFont().family(),8));
    customPlot->xAxis->setAutoTickStep(true);
    customPlot->yAxis->setTickLabelColor(QColor(Qt::red));
    customPlot->legend->setVisible(true);
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
    graphic = new QCPGraph(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(graphic);
    graphic->setName("Поступление товара");
    graphic->setPen(QPen(QColor(Qt::red)));
    graphic->setAntialiased(false);
    //выводим график в виде вертикальных линий
    graphic->setLineStyle(QCPGraph::lsImpulse);
    graphic->setAdaptiveSampling(true);
}

void DrawGraph::on_pushButton_clicked()
{
    QVector<double> volume;
    QVector<double> supply_date;
    QModelIndex idx;
    QModelIndex idx_date;
    for(int i = 0; i <model_for_graph->rowCount(); ++i){
        idx = model_for_graph->index(i,5);
        volume.push_back(idx.data(Qt::DisplayRole).toDouble());
        idx_date = model_for_graph->index(i,4);
        supply_date.push_back(idx_date.data(Qt::DisplayRole).toDateTime().toTime_t());
    }
    double max_y = *std::max_element(volume.constBegin(), volume.constEnd());
    double max_x = *std::max_element(supply_date.constBegin(), supply_date.constEnd());
    double min_x = *std::min_element(supply_date.constBegin(), supply_date.constEnd());
    graphic->setData(supply_date, volume);
    customPlot->rescaleAxes();
    //прибавляем сверху 5 единиц к оси y
    customPlot->yAxis->setRange(0,max_y+5);
    customPlot->xAxis->setRange(min_x-24*7*3*3600, max_x+24*7*3*3600);
    //отрисовываем
    customPlot->replot();
}

void DrawGraph::updateModel(){
    filter1 = "type LIKE '%" + ui->comboBox_types->currentText() + "%'";
    model_for_graph = new QSqlTableModel();
    model_for_graph->setTable("products");
    model_for_graph->setFilter(filter1);
    model_for_graph->select();
    QModelIndex idx;
    ui->comboBox_models->clear();
    ui->comboBox_models->addItem("не выбрано");
    models_set.clear();
    for(int i = 0; i<model_for_graph->rowCount(); ++i){
        idx = model_for_graph->index(i,2);
        models_set.insert(idx.data(Qt::DisplayRole).toString());
    }
    if((ui->comboBox_types->currentText() != "не выбрано")&&(ui->comboBox_types->currentText() != "")){
        for(it2 = models_set.begin(); it2 != models_set.end(); ++it2){
            ui->comboBox_models->addItem(*it2);
        }
    }
}

void DrawGraph::update_if_model_was_selected(){

    filter1 = "type LIKE '%" + ui->comboBox_types->currentText() + "%'";
    if(ui->comboBox_models->currentText()!="не выбрано" && ui->comboBox_models->currentText()!="" ){
        filter2 = " and model LIKE '%" + ui->comboBox_models->currentText() + "'";
        filter1.append(filter2);

    }
     model_for_graph->setFilter(filter1);

    qDebug()<<filter1;
}

DrawGraph::~DrawGraph()
{
    delete ui;
}

