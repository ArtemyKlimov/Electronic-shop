#include "order.h"
#include "ui_order.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDebug>

Order::Order(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Order)
{
    //В конструкторе задаем основные параметры окна заказа.
    //Задаем выпадающему списку значения из графы "тип" в БД.
    //Значения задаются через QSet, чтобы не было повторений
    ui->setupUi(this);
    //Запрещаем пользователю менять размер окна
    setFixedSize(420,370);
    ui->pushButton->setEnabled(false);
    QSqlQuery qry;
    qry.exec("SELECT * FROM products");
    while(qry.next()){
        types_set.insert(qry.value(1).toString());
    }
    for(it = types_set.begin(); it != types_set.end(); ++it){
         ui->comboBox->addItem(*it);
    }
    QRegExp exp("[0-9]{0,6}");
    ui->lineEdit->setValidator(new QRegExpValidator(exp, this));
    //Пока данные не выбраны, результат не показываем
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(is_summ_input()));
}

void Order::is_summ_input(){
    //Кнопка вывести значений доступна только когда
    //пользователь ввел необходимую сумму
    bool ok = !ui->lineEdit->text().isEmpty();
    ui->pushButton->setEnabled(ok);
}

Order::~Order()
{
    delete ui;
}

void Order::on_pushButton_clicked(){
//заносим в таблицу все товары выбранной категории из БД.
//Задаем вид таблицы и отрисовываем ее
    order_model = new QSqlQueryModel();
    availible_summ = ui->lineEdit->text().toInt();
    //Задаем запрос к БД. Если модель совпадает - группируем значения, суммируя количество.
    order_model->setQuery("SELECT model, brand, SUM(volume) FROM products WHERE type = '" + ui->comboBox->currentText()+ "' GROUP BY model");
    order_model->setHeaderData(0, Qt::Horizontal,"Модель");
    order_model->setHeaderData(1, Qt::Horizontal,"Производитель");
    order_model->setHeaderData(2, Qt::Horizontal,"Остаток на складе");
    ui->tableView->setModel(order_model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->show();
}

void Order::on_tableView_clicked(const QModelIndex &index)
{
    count_n_of_wares(index);
}

void Order::count_n_of_wares(const QModelIndex &index){
//Функция подсчета результата: сколько товара пользователь может приобрести
//на введенную сумму. Вызывается каждый раз когда пользователь кликает по строке таблицы.
//Отрисовывает результат под таблицей.
//Результат максимально доступное количество товара на складе магазина.
//Сначала предлагаются самые дешевые модели, потом дороже итд.
    QSqlQuery query;
    QString model = ui->tableView->model()->index(index.row(), 0).data().toString();
    query.exec("SELECT * FROM products WHERE model = '" + model+ "'");
    double price;
    int volume;
    while(query.next()){
        price = query.value(6).toDouble();
        volume = query.value(5).toInt();
        wares_map.insert(price, volume);

    }
    int sum =0, n_of_wares = 0;
    //В QMultiMap, где ключом является цена товара, а значением количество устройств,
    //добавляем все значения БД выбранной модели. Контейнер сам сортирует значения от
    //наименьшего к наибольшему.
    //Пробегаемся в цикле по QMultiMap. С каждой итерацией переменную sum увеличиваем
    //на стоимость устройства. Итерируем n_of_wares пока, работают циклы. Выходим из обоих
    //циклов, если sum больше указанной пользователем суммы.
    for(it2 = wares_map.begin(); it2 !=wares_map.end(); ++it2)
    {
        for(int i = 0; i < it2.value(); ++i){
            sum+=it2.key();
            if (sum>availible_summ) break;
                n_of_wares++;
        }
        if (sum>availible_summ) break;
    }
    //Выводим полученный результат в виде строки:
    //Вам доступно %количество% %тип_устройства% %бренд% %модель%
    QString ending;
    //В зависимости от кол-ва торваов меняем окончание у типа изделия: Товар-товаров - товара
    int end = n_of_wares % 10;
    if(end == 1){
        ending = "";
    }
    else if((end ==2)||(end==3)||(end == 4)){
        ending = "а";
    }
    else{
        ending = "ов";
    }
    if((n_of_wares %100 == 11) || (n_of_wares %100 ==12)||(n_of_wares %100== 13) ||(n_of_wares %100==14)) ending = "ов";
    QString s = QString::number(n_of_wares);
    QString brand = ui->tableView->model()->index(index.row(), 1).data().toString();
    QString result = "Вам доступно: "+s+" "+ui->comboBox->currentText()+ending+" "+brand+" "+model+" для заказа";
    ui->label_result->setText(result);
    wares_map.clear();
}


