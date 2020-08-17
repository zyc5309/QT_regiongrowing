#include "count.h"
#include "ui_count.h"
#include<QDebug>
#include<QFileDialog>
#include<QDateTime>
#include<QDir>
#include<QFileInfoList>
#include<QtSql/QSqlQuery>
#include<QtSql/QSqlDatabase>
#include<QSqlError>
#include<QMessageBox>
#include<QTextCodec>
#include<QPainter>
#include<QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include<QPainter>
#include<QtCharts/QBarSet>

QString srcDirPath;//文件夹路径

count::count(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::count)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    // 设置编码格式
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    //Set Character-Set
    db.exec("set NAMES UTF8");

    db.setHostName("localhost");
    db.setDatabaseName("bulges");
    db.setPort(3306);
    db.setUserName("root");
    db.setPassword("4-Tu*;+w5tBd");
    QSqlQuery query=QSqlQuery(db);
    if(!db.open())
    {
     QMessageBox::warning(this,"warning","数据库打开失败");
    }else
    {
     qDebug()<<"数据库打开成功"<<endl;
     query.exec("create table count_dayworking(workingname varchar(100) not null,workingtime int not null)");
    }
}

count::~count()
{
    delete ui;
}

void count::on_pushButton_return_clicked()//返回主窗口
{
    emit sendsignal();
        this->close();
}

void count::on_pushButton_choose_file_clicked()//选取要操作的文件夹路径并显示在textEdit_path中
{
    srcDirPath = QFileDialog::getExistingDirectory(
                   this, "choose src Directory",
                    "/");

        if (srcDirPath.isEmpty())
        {
            return;
        }
        else
        {
            ui->textEdit_path->setText(srcDirPath);
        }
}

void count::on_pushButton_draw_clicked()
{
    int hour_1 = 0;
    int hour_2 = 0;
    int hour_3 = 0;
    int hour_4 = 0;
    int hour_5 = 0;
    int hour_6 = 0;
    int hour_7 = 0;
    int hour_8 = 0;
    int hour_9 = 0;
    int hour_10 = 0;
    int hour_11 = 0;
    int hour_12 = 0;
    int hour_13 = 0;
    int hour_14 = 0;
    int hour_15 = 0;
    int hour_16 = 0;
    int hour_17 = 0;
    int hour_18 = 0;
    int hour_19 = 0;
    int hour_20 = 0;
    int hour_21 = 0;
    int hour_22 = 0;
    int hour_23 = 0;
    int hour_0 = 0;
    QDir *dir=new QDir(srcDirPath);
    QStringList filter;
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
    QSqlQuery query;
    QString name;
    QString time;
    for(int i = 0;i<fileInfo->count(); i++)//将文件夹内的文件名和最后修改时间依次读入数据库中
       {
           QFileInfo info(fileInfo->at(i).filePath());
           QDateTime dt = info.lastModified();
           QString modify_time = tr("%1").arg(dt.toString("hh"));
           time = modify_time;
           name = fileInfo->at(i).fileName();
           QString str = QString("INSERT INTO count_dayworking(workingname,workingtime) VALUES('%1','%2')").arg(name).arg(time);
           query.prepare(str);
           query.exec();
       }
    if(!query.exec()){
        qDebug()<<"query error :"<<query.lastError();
    }
    else{
        qDebug()<<"insert data success!";
    }


    for(int i = 0;i<fileInfo->count(); i++)//从数据库读出最后修改时间并统计
    {
        QString str = QString("select workingtime from count_dayworking where workingname = '%1'").arg(fileInfo->at(i).fileName());
        query.exec(str);
        query.first();
        qDebug()<<query.value(0).toInt();
        if(query.value(0).toInt() == 0)
        {hour_0+=1;}
        else if(query.value(0).toInt() == 1)
        {hour_1+=1;}
        else if(query.value(0).toInt() == 2)
        {hour_2+=1;}
        else if(query.value(0).toInt() == 3)
        {hour_3+=1;}
        else if(query.value(0).toInt() == 4)
        {hour_4+=1;}
        else if(query.value(0).toInt() == 5)
        {hour_5+=1;}
        else if(query.value(0).toInt() == 6)
        {hour_6+=1;}
        else if(query.value(0).toInt() == 7)
        {hour_7+=1;}
        else if(query.value(0).toInt() == 8)
        {hour_8+=1;}
        else if(query.value(0).toInt() == 9)
        {hour_9+=1;}
        else if(query.value(0).toInt() == 10)
        {hour_10+=1;}
        else if(query.value(0).toInt() == 11)
        {hour_11+=1;}
        else if(query.value(0).toInt() == 12)
        {hour_12+=1;}
        else if(query.value(0).toInt() == 13)
        {hour_13+=1;}
        else if(query.value(0).toInt() == 14)
        {hour_14+=1;}
        else if(query.value(0).toInt() == 15)
        {hour_15+=1;}
        else if(query.value(0).toInt() == 16)
        {hour_16+=1;}
        else if(query.value(0).toInt() == 17)
        {hour_17+=1;}
        else if(query.value(0).toInt() == 18)
        {hour_18+=1;}
        else if(query.value(0).toInt() == 19)
        {hour_19+=1;}
        else if(query.value(0).toInt() == 20)
        {hour_20+=1;}
        else if(query.value(0).toInt() == 21)
        {hour_21+=1;}
        else if(query.value(0).toInt() == 22)
        {hour_22+=1;}
        else
        {hour_23+=1;}
    }


    QBarSet *set = new QBarSet("在这一时间段的文件数目");
    *set <<hour_0<<hour_1<<hour_2<<hour_3<<hour_4<<hour_5<<hour_6<<
           hour_7<<hour_8<<hour_9<<hour_10<<hour_11<<hour_12<<hour_13<<
           hour_14<<hour_15<<hour_16<<hour_17<<hour_18<<hour_19<<hour_20<<
           hour_21<<hour_22<<hour_23;
    QBarSeries *series = new QBarSeries();
    series->append(set);//为序列添加数据
    QChart *chart = new QChart();
    chart->addSeries(series);//序列添加到图表
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories <<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<"11"<<"12"<<"13"<<"14"<<"15"<<"16"<<"17"<<"18"<<"19"<<"20"<<"21"<<"22"<<"23";
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);//横坐标的坐标值
    chart->createDefaultAxes();//创建默认的左侧的坐标轴（根据 QBarSet 设置的值）
    chart->setAxisX(axis, series);//设置坐标轴
    QChartView *chartView = new QChartView(chart,ui->widget_table);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->resize(721,551);
    chartView->show();

    chart->legend()->setVisible(true);//显示图例
    chart->legend()->setAlignment(Qt::AlignBottom);//图例文字在下方

    query.prepare("truncate count_dayworking");
    query.exec();
}


