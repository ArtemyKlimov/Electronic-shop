#include "add_data.h"
#include "ui_add_data.h"
#include <QSqlQuery>
#include <QMessageBox>

Add_data::Add_data(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Add_data)
{
    //В конструкторе задаем основные параметры окна добавления данных в БД.
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    QRegExp exp("[0-9]{0,6}");
    ui->lineEdit_price->setValidator(new QRegExpValidator(exp, this));
    ui->dateEdit->setDate(QDate::currentDate());
    //Кнопка добавления данных недоступна, пока не заполнены все поля
    ui->pushButton->setEnabled(false);
    //связываем все поля с кнопкой:
    connect(ui->lineEdit_brand, SIGNAL(textChanged(QString)), this, SLOT(checkLineEdits()));
    connect(ui->lineEdit_country, SIGNAL(textChanged(QString)), this, SLOT(checkLineEdits()));
    connect(ui->lineEdit_price, SIGNAL(textChanged(QString)), this, SLOT(checkLineEdits()));
    connect(ui->lineEdit_type, SIGNAL(textChanged(QString)), this, SLOT(checkLineEdits()));
    connect(ui->lineEdit_model, SIGNAL(textChanged(QString)), this, SLOT(checkLineEdits()));
}

Add_data::~Add_data()
{
    delete ui;
}

void Add_data::checkLineEdits(){
    //Данная функция работает как сигнал. Если все поля заполнены,
    //то переменная "ok" принимает истинное значение
    //и кнопка разблокируется
    bool ok = !ui->lineEdit_brand->text().isEmpty()
            && !ui->lineEdit_type->text().isEmpty()
            && !ui->lineEdit_price->text().isEmpty()
            && !ui->lineEdit_model->text().isEmpty()
            && !ui->lineEdit_country->text().isEmpty();

        ui->pushButton->setEnabled(ok);
}



void Add_data::on_pushButton_clicked()
{
    //Если кнопка нажата, то в БД добавляется строка с введенными данными.
    if(ui->lineEdit_brand !=0 && ui->lineEdit_type !=0 && ui->lineEdit_country !=0
            && ui->lineEdit_price !=0)  ui->pushButton->setEnabled(true);
    QSqlQuery *qry=  new QSqlQuery();
    qry->prepare("INSERT INTO products(type, model, brand, date, volume, price, country) VALUES(:type, :model, :brand, :date, :volume, :price, :country)");
    qry->bindValue(":type", ui->lineEdit_type->text());
    qry->bindValue(":model", ui->lineEdit_model->text());
    qry->bindValue(":brand", ui->lineEdit_brand->text());
    qry->bindValue(":date", ui->dateEdit->text());
    qry->bindValue(":volume", ui->spinBox_volume->text());
    qry->bindValue(":price", ui->lineEdit_price->text());
    qry->bindValue(":country", ui->lineEdit_country->text());
    QMessageBox *mes = new QMessageBox();
    if(!qry->exec()){
        mes->setText("Не удалось добавить данные");
    }
    else{
        mes->setText("Товар добавлен в базу");
        emit refresh_table();
        close();
    }
    mes->show();
}


void Add_data::on_pushButton_2_clicked()
{
    close();
}
