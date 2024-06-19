#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QString>
#include <QCalendar>
#include <QMessageBox>
#include <QDate>
#include <QFile>
#include <QFileDialog>
#include <xlsxdocument.h>

using namespace QXlsx;

//---------------------------

QDate today = QDate::currentDate();
QCalendar convert = QCalendar(QCalendar::System::Jalali);
QString selected = "-1";

//---------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->calendarWidget->setCalendar(convert);

    ////////////////////////////////////////////

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.db");
    if (!db.open())
        QMessageBox::critical(this,"","not");

    db.exec("create table datas(ID integer primary key, date text, title text, exit text, login text)");

    ////////////////////////////////////////////

    reload();

}

MainWindow::~MainWindow()
{
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    selected = QString::number(index.row()+1);
}


void MainWindow::on_calendarWidget_clicked(const QDate &date)
{
    ui->le_date->setText(date.toString("yyyy/MM/dd",convert));
    ui->stackedWidget->setCurrentIndex(1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_act_new_triggered()
{
    ui->le_date->setText(today.toString("yyyy/MM/dd",convert));
    ui->le_title->clear();
    ui->le_exit->clear();
    ui->le_login->clear();

    ui->stackedWidget->setCurrentIndex(1);
    selected = "-1";
}


void MainWindow::on_act_edit_triggered()
{    
    if (selected != "-1")
    {        
        QSqlQuery q;
        q.exec("select date,title,exit,login from datas where ID = '"+selected+"'");
        q.next();

        ui->le_date->setText(q.value(0).toString());
        ui->le_title->setText(q.value(1).toString());
        ui->le_exit->setText(q.value(2).toString());
        ui->le_login->setText(q.value(3).toString());

        ui->stackedWidget->setCurrentIndex(1);
    }
    else
        QMessageBox::critical(this,"یک خانه انتخاب کنید!","لطفا یک سطر از جدول را انتخاب کنید");
}


void MainWindow::on_act_delete_triggered()
{

    if (selected != "-1")
    {
        QMessageBox::StandardButton btn;
        btn = QMessageBox::critical(this,"حذف یک سطر !!","این سطر از جدول حذف شود ؟؟",QMessageBox::Yes|QMessageBox::No);
        if (btn == QMessageBox::Yes)
        {
            QSqlQuery("delete from datas where ID = '"+selected+"'");
            QSqlQuery("update datas set ID=ID-1 where ID > '"+selected+"'");

            reload();
        }
    }
    else
        QMessageBox::critical(this,"یک خانه انتخاب کنید!","لطفا یک سطر از جدول را انتخاب کنید");
}


void MainWindow::on_act_allDelete_triggered()
{
    QMessageBox::StandardButton btn;
    btn = QMessageBox::critical(this,"پاک کردن کل اطلاعات !!","کل اطلاعات حذف شود ؟؟",QMessageBox::Yes|QMessageBox::No);
    if (btn == QMessageBox::Yes)
    {
        QFile exist("data.db");
        if (exist.exists())
            if (exist.remove())
                QMessageBox::critical(this,"فایل پاک شد","");
            else
                QMessageBox::critical(this,"فایل پاک نشد","");
        else
            QMessageBox::critical(this,"فایل نیست","");
    }
}


void MainWindow::on_act_inExcel_triggered()
{
    QString address = QFileDialog::getSaveFileName(this,"","hesabdari.xlsx");
    if (address != "")
    {
        Document xlsx(address);

        xlsx.write("D1","جمع کل");
        xlsx.write("C1",ui->le_total->text());
        xlsx.write("E3","ردیف");
        xlsx.write("D3","تاریخ");
        xlsx.write("C3","شرح");
        xlsx.write("B3","برداشت");
        xlsx.write("A3","واریز");

        QSqlQuery q("select * from datas");
        int i=4;
        while (q.next())
        {
            xlsx.write("E"+QString::number(i),q.value(0).toString());
            xlsx.write("D"+QString::number(i),q.value(1).toString());
            xlsx.write("C"+QString::number(i),q.value(2).toString());
            xlsx.write("B"+QString::number(i),q.value(3).toString());
            xlsx.write("A"+QString::number(i),q.value(4).toString());
            i++;
        }
        xlsx.write("E1",i-4);

        xlsx.setColumnWidth(1,13);
        xlsx.setColumnWidth(2,13);
        xlsx.setColumnWidth(3,25);
        xlsx.setColumnWidth(4,12);
        xlsx.setColumnWidth(5,5);

        if (xlsx.save())
            QMessageBox::critical(this,"اکسل ذخیره شد !","فایل اکسل با موفقیت خروجی گرفته شد");
        else
            QMessageBox::critical(this,"خطا در ذخیره اکسل !","ذخیره اکسل با خطا مواجه شد !!");

    }
}


void MainWindow::on_act_ofExcel_triggered()
{
    QString address = QFileDialog::getOpenFileName(this,"","hesabdari.xlsx");
    if (address != "")
    {
        QMessageBox::StandardButton btn;
        btn = QMessageBox::critical(this,"بازنویسی اطلاعات !","آیا اطلاعات جدید جایگزین داده های قبل شود ؟؟",QMessageBox::Yes|QMessageBox::No);
        if (btn == QMessageBox::Yes)
        {
            QSqlQuery("delete from datas");

            Document xlsx(address);

            for(int i=1; i<=xlsx.read("E1").toInt(); i++)
            {
                QString data1 = xlsx.read("E"+QString::number(i+3)).toString();
                QString data2 = xlsx.read("D"+QString::number(i+3)).toString();
                QString data3 = xlsx.read("C"+QString::number(i+3)).toString();
                QString data4 = xlsx.read("B"+QString::number(i+3)).toString();
                QString data5 = xlsx.read("A"+QString::number(i+3)).toString();
                QSqlQuery("insert into datas values('"+data1+"','"+data2+"','"+data3+"','"+data4+"','"+data5+"')");
            }
            reload();
        }
    }

}


void MainWindow::on_act_about_triggered()
{
    QMessageBox::information(this,"( درباره ما )","\t\tساخته شده توسط : \t\tپوریا بهاری\n\t\t09101658290 \n\t\twww.pouriya.bahari.82@gmail.com");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pb_cancel_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    selected = "-1";
}


void MainWindow::on_pb_save_clicked()
{
    QString date = ui->le_date->text();
    QString title = ui->le_title->text();
    QString exit = ui->le_exit->text();
    QString login = ui->le_login->text();
    QString Row = QString::number(row("select * from datas")+1);

    if ((exit=="" || exit=="0")&&(login=="" || login=="0"))
        QMessageBox::critical(this,"یک مقدار وارد کنید!","لطفا در قسمت برداشت یا واریز یک عدد وارد کنید");
    else
    {
        // QMessageBox::critical(this,"",selected);
        if (selected == "-1")
            QSqlQuery("insert into datas values('"+Row+"','"+date+"','"+title+"','"+exit+"','"+login+"')");
        else
            QSqlQuery("update datas set date='"+date+"',title='"+title+"',exit='"+exit+"',login='"+login+"' where ID = '"+selected+"'");
        reload();
        ui->stackedWidget->setCurrentIndex(0);
    }
}


void MainWindow::on_pb_date_clicked()
{
    ui->calendarWidget->setSelectedDate(today);
    ui->stackedWidget->setCurrentIndex(2);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_le_exit_textChanged()
{
    format(ui->le_exit);
}

void MainWindow::on_le_login_textChanged()
{
    format(ui->le_login);
}

void MainWindow::on_le_total_textChanged()
{
    format(ui->le_total);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::reload()
{
    QSqlQuery q;
    q.exec("select * from datas");
    QSqlQueryModel *m = new QSqlQueryModel;
    m->setQuery(std::move(q));
    m->setHeaderData(0,Qt::Horizontal,"ردیف");
    m->setHeaderData(1,Qt::Horizontal,"تاریخ");
    m->setHeaderData(2,Qt::Horizontal,"شرح");
    m->setHeaderData(3,Qt::Horizontal,"برداشت");
    m->setHeaderData(4,Qt::Horizontal,"واریز");
    ui->tableView->setModel(m);

    ui->tableView->setColumnWidth(0,15);
    ui->tableView->setColumnWidth(1,85);
    ui->tableView->setColumnWidth(2,120);

    selected = "-1";

    int totalExit=0;
    int totalLogin=0;
    QSqlQuery q2("select exit,login from datas");
    while (q2.next())
    {
        totalExit += q2.value(0).toString().remove(',').toInt();
        totalLogin += q2.value(1).toString().remove(',').toInt();
    }
    ui->le_total->setText(QString::number(totalExit-totalLogin));
}

void MainWindow::format(QLineEdit *le)
{
    QString before = le->text().remove(',');
    before.remove('.');

    int len = before.length(); //1,234,567 //1234567
    for (int i = len -3; i > 0; i -= 3)
        before.insert(i,',');

    le->setText(before);
}

int MainWindow::row(QString query)
{
    QSqlQuery q;
    q.exec(query);
    QSqlQueryModel *m = new QSqlQueryModel;
    m->setQuery(std::move(q));
    return m->rowCount();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
