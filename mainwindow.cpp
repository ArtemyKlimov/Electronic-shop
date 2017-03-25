#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDate>
#include <QMessageBox>
#include <QSqlQuery>
#include <QFile>
#include <QFileDialog>
#include "add_data.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//В конструкторе задаем основные параметры главного окна. Инициализируем переменные.
    ui->setupUi(this);
    //ставим текущую дату как верхнюю границу поиска по времени поставки
    ui->dateEdit_till->setDate(QDate::currentDate());
    //настраиваем окна поиска по цене: вводить можно только 6 цифр от 0 до 9
    QRegExp exp("[0-9]{0,6}");
    ui->lineEdit_from->setValidator(new QRegExpValidator(exp, this));
    ui->lineEdit_till->setValidator(new QRegExpValidator(exp, this));
    //если таблица пуста - кнопка вывода в эксель недоступна.
    ui->pushButton->setEnabled(false);
    //подключаем базу данных.
    connectDB();
    //если не были созданы объекты классов добавления данных, рисоания графиков и заказа
    //соответсвующие переменные имеют ложные значения
    tableIsCreated = false;
    drawerWasCalled = false;
    AddingWasCalled = false;
    OrderWasCalled = false;

}
void MainWindow::connectDB(){
    //на всякий случай, если было ранее создано подключение к БД, закрываем его
    mydb.close();
    //указываем драйвер подкючения к БД.  В нашем случае SQLITE
    mydb = QSqlDatabase::addDatabase("QSQLITE");
    //указываем путь к БД. В данном случае - корневая директория.
    mydb.setDatabaseName("mybase.db3");
    //открываем БД
    mydb.open();
    QSqlQuery qry;
    //Создаем запрос  - выбираем все записи из базы
    qry.exec("SELECT * FROM products");
    //Все записи из столбцов "тип" и "страна" добавляем в контейнер QSet (чтобы избежать дублирования)
    //, потом добавляем в выпадающий список
    while(qry.next()){
        type_set.insert(qry.value(1).toString());
        country_set.insert(qry.value(7).toString());
    }

    ui->comboBox_country->clear();
    ui->comboBox_type->clear();
    ui->comboBox_type->addItem("не выбрано");
    ui->comboBox_country->addItem("не выбрано");
    for(it = type_set.begin(); it != type_set.end(); ++it){
         ui->comboBox_type->addItem(*it);
       }
    for(it = country_set.begin(); it !=country_set.end(); ++it)
        ui->comboBox_country->addItem(*it);
}

MainWindow::~MainWindow()
{
    mydb.close();
    delete ui;
}

void MainWindow::on_pushButton_showRes_clicked()
{
    //Данная функция вызывается при нажатии на кнопку "Вывести результат"
    //создаем фильтр из введенных пользователем данных и направляем запрос к БД
    QString filter1 = "type LIKE '%" + ui->comboBox_type->currentText() + "%'";
    QString filter2 = "model LIKE '%" + ui->lineEdit_model->text() + "%'";
    QString filter3 = "brand LIKE '%" + ui->lineEdit_brand->text() + "%'";
    QString filter4 = "price >= " + ui->lineEdit_from->text();
    QString filter5 = "price <= " + ui->lineEdit_till->text();
    QString filter6 = "volume >= " + ui->spinBox_from->text();
    QString filter7 = "volume <= " + ui->spinBox_till->text();
    QString filter8 = "country LIKE '%" + ui->comboBox_country->currentText() + "%'";
    QString filter9 = "date >=  '"+ui->dateEdit_from->text()+"'";
    QString filter10 = "date <= '"+ui->dateEdit_till->text()+"'";
    QString text;
    //Создаем модель
    model = new QSqlTableModel();
    model->setTable("products");
    //Если какое-то поле было изменено, то применяем соотвутствующий фильтр
    if(ui->comboBox_type->currentText() != "не выбрано" ){
        text.append(filter1);
    }
    if(ui->lineEdit_model->isModified()){
        if(text.length() != 0) text.append(" and ");
        text.append(filter2);
    }
    if(ui->lineEdit_brand->isModified()){
        if(text.length() != 0) text.append(" and ");
        text.append(filter3);
    }
    if(ui->lineEdit_from->text() != 0){
        if(text.length() != 0) text.append(" and ");
        text.append(filter4);
    }
    if(ui->lineEdit_till->text() != 0){
        if(text.length() != 0) text.append(" and ");
        text.append(filter5);
    }
    if(ui->spinBox_from->text() != 0){
        if(text.length() != 0) text.append(" and ");
        text.append(filter6);
    }
    if(ui->spinBox_till->text() != 0){
        if(text.length() != 0) text.append(" and ");
        text.append(filter7);
    }
    if(ui->comboBox_country->currentText() != "не выбрано" ){
        if(text.length() != 0) text.append(" and ");
        text.append(filter8);
    }
    text.append(" and " + filter9 + " and " + filter10);

    qDebug()<<text;
    //применяем фильтр
    model->setFilter(text);
    model->select();
    //настраиваем отображение фильтра в табице
    model->setHeaderData(0,Qt::Horizontal,"№ в базе");
    model->setHeaderData(1,Qt::Horizontal,"Изделие");
    model->setHeaderData(2,Qt::Horizontal,"Модель");
    model->setHeaderData(3,Qt::Horizontal,"Бренд");
    model->setHeaderData(4,Qt::Horizontal,"Дата");
    model->setHeaderData(5,Qt::Horizontal,"Кол-во");
    model->setHeaderData(6,Qt::Horizontal,"Цена, $");
    model->setHeaderData(7,Qt::Horizontal,"Страна");
    //передаем модель в таблицу
    ui->tableView->setModel(model);
    //подгоняем границы ячеек под содержимое
    ui->tableView->resizeColumnsToContents();
    //не показываем сетку таблицы
    ui->tableView->setShowGrid(false);
    //настраиваем отображение таблицы. Если нажата ячейка, выделяется вся строка.
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //первый столбец с индексами изделий не отображаем
    ui->tableView->setColumnHidden(0, true);
    ui->tableView->show();
    ui->pushButton->setEnabled(true);
    tableIsCreated = true;
    //вызываем функцию подведения итогов
    setTotal();

}

void MainWindow::setTotal(){
    //данная функция подсчитывает количество наденного таблицу товара и суммирует стоимость
    int total_products = 0;
    int total_summ = 0;
    int products;

    QModelIndex idx;
    for(int i = 0; i<ui->tableView->model()->rowCount(); ++i){
        idx = model->index(i,5);
        products = idx.data(Qt::DisplayRole).toInt();
        total_products +=products;
        idx = model->index(i, 6);
        total_summ += idx.data(Qt::DisplayRole).toInt()* products;
    }
    QString s = QString::number(total_products);
    ui->label_total_volume->setText(s);
    s = QString::number(total_summ);
    ui->label_total_summ->setText(s);
}


void MainWindow::on_pushButton_add_clicked()
{

    Add_data *new_data = new Add_data();
    //связываем добавление новых данных в БД с текущим отображением данных в таблице.
    connect(new_data, SIGNAL(refresh_table()), this, SLOT(obr_refresh_table()));
    new_data->show();
    AddingWasCalled =true;
}

void MainWindow::on_pushButton_delete_clicked()
{
    //функция запускается при нажатии на кнопку "удалить"
    //проверка, указаны ли данные, которые необходимо удалить
    if(!tableIsCreated){
        QMessageBox::warning(this, tr("Empty query"), tr("Укажите удаляемый элемент"));
        return;
    }
    QModelIndex idx = model->index(ui->tableView->currentIndex().row(), 0);
    int id_to_delete = idx.data(Qt::DisplayRole).toInt();
    QSqlQuery *qry = new QSqlQuery();
    qry->prepare("DELETE FROM products WHERE id =:id");
    qry->bindValue(":id", id_to_delete);
    QMessageBox *mes = new QMessageBox();
    if(qry->exec()){
        mes->setText("Товар удален из базы");
        mes->show();
    }
    ui->tableView->show();
    //вызываем функцию отображения результатов таблицы.
    on_pushButton_showRes_clicked();
}

void MainWindow::obr_refresh_table(){
    //функция обновляет подключение к базе, имитирует нажатие на
    connectDB();
    on_pushButton_showRes_clicked();
}


void MainWindow::on_action_3_triggered()
{
    //если был нажат элемент меню "О программе", то вызыаем диалоговое окно с информацией:
    QMessageBox::information(this, tr("О программе"),
                             tr("Ищите товары в магазине, пользуясь поиском\n"
                                "Добавляйте и удаляйте товары в магазине\n"
                                "Смотрите статистику поступления товаров на склад\n"
                                "Заказывайте товар в нашем магазине"));
}

void MainWindow::on_action_2_triggered()
{
    drawerWasCalled = true;
    drawer = new DrawGraph();
    drawer->show();
}

void MainWindow::on_Push_Button2_clicked()
{
    close();
}

void MainWindow::on_action_triggered()
{
    new_order = new Order();
    new_order->show();
    OrderWasCalled = true;
}

void MainWindow::on_pushButton_clicked(){
   QFileDialog Filepath;
   QString save_to = Filepath.getSaveFileName(0, "Save File", "MyTable", "*.txt;;*.csv");
   if(!save_to.isEmpty()){
       QFile CSVFile(save_to);
       //открываем файл по заданному адресу
       if(CSVFile.open( QIODevice::WriteOnly ))
       {
           QTextStream textStream( &CSVFile );
           QStringList stringList;
           stringList<<"\" \"";  //пропускаем 0-строку и 0-стобец
           for(int c = 1; c < ui->tableView->horizontalHeader()->count(); ++c){
              stringList<<"\""+ui->tableView->model()->headerData(c, Qt::Horizontal).toString()+"\"";
           }
            textStream<<stringList.join(";")+"\n";

           for( int row = 0; row < ui->tableView->model()->rowCount(); row++ )
           {
               stringList.clear();
               stringList << "\""+ui->tableView->model()->headerData(row, Qt::Vertical).toString()+"\"";
               for( int column = 1; column < 8; column++ )
               {
                   stringList<<ui->tableView->model()->data(ui->tableView->model()->index(row, column), Qt::DisplayRole).toString();
               }
               textStream << stringList.join( ';' )+"\n";
           }
           CSVFile.close();
       }
   }
}
