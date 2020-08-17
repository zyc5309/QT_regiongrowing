#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFileDialog>
#include<QSettings>
#include<QString>
#include<QPixmap>
#include<QImage>
#include<QMessageBox>
#include<QtSql/QSqlQuery>
#include<QtSql/QSqlDatabase>
#include<QDebug>
#include<QSqlError>
#include<QTextCodec>
#include"count.h"


#if _MSC_VER >=1600 //VS2010版本号是1600，强制MSVC编译器采用UTF-8编码生成可执行文件
#pragma execution_character_set("utf-8")
#endif

//regiongrowing参数
int Row;
int Column;
int Tolerance;
int Minsize;

int MIN;//第一次筛选 selectshape
int min;//第二次筛选 selectshape

QString filename;
int Number;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QCursor cursor;                      // 创建光标对象
    cursor.setShape(Qt::ArrowCursor); // 设置光标形状
    setCursor(cursor);

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
     query.exec("create table regiongrowing(errorname varchar(100) not null,errornum int not null)");//创建regiongrowing表
    }

    //原图片窗口
    winID =(Hlong)ui->label_old->winId();

    OpenWindow(0,0,ui->label_old->width(),ui->label_old->height(),winID,"visible","",&hv_WindowHandle);

    //处理后图片显示窗口
    winIDs =(Hlong)ui->label_new->winId();

    OpenWindow(0,0,ui->label_new->width(),ui->label_new->height(),winIDs,"visible","",&hv_WindowHandles);

    //图片初始放大倍数
    zoom_scale = 1;

    //使表格单元格自适应表格窗口大小
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)//鼠标单击事件
{
    if(event->button() == Qt::LeftButton)
    {
        QCursor cursor;
        cursor.setShape(Qt::BusyCursor);
        QApplication::setOverrideCursor(cursor);
        QPoint s(ui->centralWidget->pos()+ui->scrollArea->pos() + ui->label_old->pos());//控件在窗口内的坐标
        lastPoint = QPoint(event->pos().x() - s.x(), event->pos().y() - s.y());//鼠标在控件上的坐标
        mouseDown = true;
        if(ui->label_old->rect().contains(lastPoint))
        {
            firstPoint = lastPoint;
            m_dXO = firstPoint.x();
            m_dYO = firstPoint.y();
         }
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) // 鼠标双击事件
{
    if(event->button() == Qt::LeftButton){             // 如果是鼠标左键按下
        if(windowState() != Qt::WindowFullScreen)      // 如果现在不是全屏
            setWindowState(Qt::WindowFullScreen);      // 将窗口设置为全屏
        else setWindowState(Qt::WindowNoState);        // 否则恢复以前的大小
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint s(ui->centralWidget->pos() +ui->scrollArea->pos()+ ui->label_old->pos());//控件在窗口内的坐标
    lastPoint = QPoint(event->pos().x() - s.x(), event->pos().y() - s.y());//鼠标在控件上的坐标
    if(ui->label_old->rect().contains(lastPoint))
    {
        windowPoint = lastPoint;
    }
    else
    {
        windowPoint=QPoint(-1,-1);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)//鼠标释放事件
{
    Q_UNUSED(event);
    moveWnd(windowPoint, ho_Image, hv_WindowHandle);
    mouseDown = false;
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    short zDelta =short(event->delta());
    if (zDelta>0)           //图像放大
    {
        if (zoom_scale<6)      //最大放大6倍
        {
            zoom_scale = zoom_scale*1.05;
            displayImage(ho_Image, hv_WindowHandle);
        }
    }
    else if (zDelta<0)                   //图像缩小
    {
        if (zoom_scale>1)
        {
            zoom_scale = zoom_scale / 1.05;
            if (zoom_scale < 1)
            {
                zoom_scale = 1;
            }
            displayImage(ho_Image, hv_WindowHandle);
        }
    }
}

void MainWindow::displayImage(HImage srcImg,HTuple hv_Window)//显示放缩后的图像
{
    double dWidth = srcImg.Width().I();//原图片宽度
    double dHeight = srcImg.Height().I();//原图片高度
    double dWidth2 = dWidth / zoom_scale;//放缩后的图片宽度
    double dHeight2 = dHeight / zoom_scale;//放缩后的图片高度
    if (dHeight / 2 - dHeight2 / 2 >= m_dYOffset && dHeight / 2 + dHeight2 / 2 - m_dYOffset <= dHeight)
    {
        m_dDispImagePartRow0 = dHeight / 2 - dHeight2 / 2 - m_dYOffset;
        m_dDispImagePartRow1 = dHeight / 2 + dHeight2 / 2 - m_dYOffset;
    }
    else if (dHeight / 2 - dHeight2 / 2 <= m_dYOffset)
    {
        m_dYOffset = dHeight / 2 - dHeight2 / 2;
        m_dDispImagePartRow0 = dHeight / 2 - dHeight2 / 2 - m_dYOffset;
        m_dDispImagePartRow1 = dHeight / 2 + dHeight2 / 2 - m_dYOffset;
    }
    else if (dHeight / 2 + dHeight2 / 2 - m_dYOffset >= dHeight)
    {
        m_dYOffset = dHeight / 2 + dHeight2 / 2 - dHeight;
        m_dDispImagePartRow0 = dHeight / 2 - dHeight2 / 2 - m_dYOffset;
        m_dDispImagePartRow1 = dHeight / 2 + dHeight2 / 2 - m_dYOffset;
    }
    if (dWidth / 2 - dWidth2 / 2 >= m_dXOffset && dWidth / 2 + dWidth2 / 2 - m_dXOffset <= dWidth)
    {
        m_dDispImagePartCol0 = dWidth / 2 - dWidth2 / 2 - m_dXOffset;
        m_dDispImagePartCol1 = dWidth / 2 + dWidth2 / 2 - m_dXOffset;
    }
    else if (dWidth / 2 - dWidth2 / 2 <= m_dXOffset)
    {
        m_dXOffset = dWidth / 2 - dWidth2 / 2;
        m_dDispImagePartCol0 = dWidth / 2 - dWidth2 / 2 - m_dXOffset;
        m_dDispImagePartCol1 = dWidth / 2 + dWidth2 / 2 - m_dXOffset;
    }
    else if (dWidth / 2 + dWidth2 / 2 - m_dXOffset >= dWidth)
    {
        m_dXOffset = dWidth / 2 + dWidth2 / 2 - dWidth;
        m_dDispImagePartCol0 = dWidth / 2 - dWidth2 / 2 - m_dXOffset;
        m_dDispImagePartCol1 = dWidth / 2 + dWidth2 / 2 - m_dXOffset;
    }
    SetPart(hv_Window, m_dDispImagePartRow0, m_dDispImagePartCol0, m_dDispImagePartRow1-1, m_dDispImagePartCol1-1);
    DispObj(srcImg, hv_Window);
}

void MainWindow::moveWnd(QPoint point, HImage srcImg, HTuple hWindow)//鼠标移动事件
{

    QRect m_rPic=ui->label_old->rect();
    double wWidth = m_rPic.right() - m_rPic.left();
    double wHeight = m_rPic.bottom() - m_rPic.top();
    double dWidth = srcImg.Width().I();
    double dHeight = srcImg.Height().I();
    int xOffset = point.x() - m_dXO;
    int yOffset = point.y() - m_dYO;
    m_dXOffset = m_dXOffset + (point.x() - m_dXO)*dWidth / wWidth / zoom_scale;
    m_dYOffset = m_dYOffset + (point.y() - m_dYO)*dHeight / wHeight / zoom_scale;
    if (m_dDispImagePartRow0 >= yOffset *dHeight / wHeight / zoom_scale && m_dDispImagePartRow1 - yOffset *dHeight / wHeight / zoom_scale <= dHeight)
    {
        m_dDispImagePartRow0 = m_dDispImagePartRow0 - yOffset *dHeight / wHeight / zoom_scale;
        m_dDispImagePartRow1 = m_dDispImagePartRow1 - yOffset *dHeight / wHeight / zoom_scale;
    }
    else if (m_dDispImagePartRow0 <= yOffset *dHeight / wHeight / zoom_scale)
    {
        m_dDispImagePartRow1 = m_dDispImagePartRow1 - m_dDispImagePartRow0;
        m_dDispImagePartRow0 = m_dDispImagePartRow0 - m_dDispImagePartRow0;
    }
    else if (m_dDispImagePartRow1 - yOffset *dHeight / wHeight / zoom_scale >= dHeight)
    {
        m_dDispImagePartRow0 = m_dDispImagePartRow0 - m_dDispImagePartRow1 + dHeight;
        m_dDispImagePartRow1 = m_dDispImagePartRow1 - m_dDispImagePartRow1 + dHeight;
    }
    if (m_dDispImagePartCol0 >= xOffset *dWidth / wWidth / zoom_scale && m_dDispImagePartCol1 - xOffset *dWidth / wWidth / zoom_scale <= dWidth)
    {
        m_dDispImagePartCol0 = m_dDispImagePartCol0 - xOffset *dWidth / wWidth / zoom_scale;
        m_dDispImagePartCol1 = m_dDispImagePartCol1 - xOffset *dWidth / wWidth / zoom_scale;
    }
    else if (m_dDispImagePartCol0 <= xOffset *dWidth / wWidth / zoom_scale)
    {
        m_dDispImagePartCol1 = m_dDispImagePartCol1 - m_dDispImagePartCol0;
        m_dDispImagePartCol0 = m_dDispImagePartCol0 - m_dDispImagePartCol0;
    }
    else if (m_dDispImagePartCol1 - xOffset *dWidth / wWidth / zoom_scale >= dWidth)
    {
        m_dDispImagePartCol0 = m_dDispImagePartCol0 - m_dDispImagePartCol1 + dWidth;
        m_dDispImagePartCol1 = m_dDispImagePartCol1 - m_dDispImagePartCol1 + dWidth;
    }
    ClearWindow(hWindow);
    SetPart(hWindow, m_dDispImagePartRow0, m_dDispImagePartCol0, m_dDispImagePartRow1-1, m_dDispImagePartCol1-1);
    DispObj(srcImg, hWindow);
    QApplication::restoreOverrideCursor();
}

void MainWindow::on_pushButton_open_picture_clicked()//打开一张图片
{
    filename = QFileDialog::getOpenFileName();

    if(filename.isEmpty())
    {
        return;
    }
    else
    {
        ReadImage(&ho_Image, HTuple(filename.toStdString().c_str()));

        GetImageSize(ho_Image, &hv_Width, &hv_Height);//读取图片尺寸

        Hlong winID =(Hlong)ui->label_old->winId();

        OpenWindow(0,0,ui->label_old->width(),ui->label_old->height(),winID,"visible","",&hv_WindowHandle);//打开窗口，尺寸为label的大小

        DispObj(ho_Image,hv_WindowHandle);//在窗口上显示图片


        //读取ini文件里面的参数值
        QString iniFilePath = "D:/QT5.8/QTsave/halcon_regiongrowing/QtPad.ini"; //路径

        QSettings *configIniread = new QSettings(iniFilePath,QSettings::IniFormat);

        Row = configIniread->value("regiongrowing/Row").toInt();

        Column = configIniread->value("regiongrowing/Column").toInt();

        Tolerance = configIniread->value("regiongrowing/Tolerance").toInt();

        Minsize = configIniread->value("regiongrowing/Minsize").toInt();

        MIN = configIniread->value("select_shape1/MIN").toInt();

        min = configIniread->value("select_shape2/min").toInt();
        
        
        //将参数显示在textEdit上
        ui->textEdit_regiongrowing_row->setText(QString().setNum(Row));

        ui->textEdit_regiongrowing_colunm->setText(QString().setNum(Column));

        ui->textEdit_regiongrowing_tolerance->setText(QString().setNum(Tolerance));

        ui->textEdit_regiongrowing_minsize->setText(QString().setNum(Minsize));

        ui->textEdit_selectshape_MIN->setText(QString().setNum(MIN));

        ui->textEdit_selectshape_min->setText(QString().setNum(min));
    }

}

void MainWindow::on_pushButton_handle_picture_clicked()
{
    //将参数值存入ini文件中
    QSettings *configIni = new QSettings("D:/QT5.8/QTsave/halcon_regiongrowing/QtPad.ini",QSettings::IniFormat);

    configIni->setValue("regiongrowing/Row",ui->textEdit_regiongrowing_row->toPlainText());

    configIni->setValue("regiongrowing/Column",ui->textEdit_regiongrowing_colunm->toPlainText());

    configIni->setValue("regiongrowing/Tolerance",ui->textEdit_regiongrowing_tolerance->toPlainText());

    configIni->setValue("regiongrowing/Minsize",ui->textEdit_regiongrowing_minsize->toPlainText());

    configIni->setValue("select_shape1/MIN",ui->textEdit_selectshape_MIN->toPlainText());

    configIni->setValue("select_shape2/min",ui->textEdit_selectshape_min->toPlainText());

    delete configIni;


    //读出ini文件参数值进行图像处理
    QString iniFilePath = "D:/QT5.8/QTsave/halcon_regiongrowing/QtPad.ini"; //路径

    QSettings *configIniread = new QSettings(iniFilePath,QSettings::IniFormat);

    Row = configIniread->value("regiongrowing/Row").toInt();

    Column = configIniread->value("regiongrowing/Column").toInt();

    Tolerance = configIniread->value("regiongrowing/Tolerance").toInt();

    Minsize = configIniread->value("regiongrowing/Minsize").toInt();

    MIN = configIniread->value("select_shape1/MIN").toInt();

    min = configIniread->value("select_shape2/min").toInt();

    ui->textEdit_regiongrowing_row->setText(QString().setNum(Row));

    ui->textEdit_regiongrowing_colunm->setText(QString().setNum(Column));

    ui->textEdit_regiongrowing_tolerance->setText(QString().setNum(Tolerance));

    ui->textEdit_regiongrowing_minsize->setText(QString().setNum(Minsize));

    ui->textEdit_selectshape_MIN->setText(QString().setNum(MIN));

    ui->textEdit_selectshape_min->setText(QString().setNum(min));

    //图像处理
    ClearWindow(hv_WindowHandles);

    SetDraw(hv_WindowHandles,"margin");

    SetColored(hv_WindowHandles,12);

    Threshold(ho_Image, &ho_Region, 1, 255);

    DispObj(ho_Region,hv_WindowHandles);

    Regiongrowing(ho_Image, &ho_Regions, Row, Column, Tolerance, Minsize);//区域增长法

    SelectShape(ho_Regions, &ho_SelectedRegions1, "area", "and", MIN, 99999);//第一次筛选

    Difference(ho_Region, ho_SelectedRegions1, &ho_RegionDifference);

    Connection(ho_RegionDifference, &ho_ConnectedRegions);

    SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", min, 9999);//第二次筛选

    Hlong winIDs =(Hlong)ui->label_new->winId();

    OpenWindow(0,0,ui->label_new->width(),ui->label_new->height(),winIDs,"visible","",&hv_WindowHandles);

    SetDraw(hv_WindowHandles,"margin");

    SetColored(hv_WindowHandles,12);

    DispObj(ho_Image, hv_WindowHandles);

    DispObj(ho_SelectedRegions, hv_WindowHandles);

    SmallestRectangle1(ho_SelectedRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);//最小内接矩形

    GenRectangle1(&ho_Rectangle, hv_Row1, hv_Column1, hv_Row2, hv_Column2);

    SetColor(hv_WindowHandles,"blue");

    DispObj(ho_Image,hv_WindowHandles);

    DispObj(ho_Rectangle,hv_WindowHandles);

    CountObj(ho_Rectangle, &hv_Number);//统计缺陷个数

    SetTposition(hv_WindowHandles, 0, 0);//设置光标

    SetColor(hv_WindowHandles,"red");

    WriteString(hv_WindowHandles,"检测出的缺陷个数为:"+hv_Number);//显示缺陷个数

    HTuple hTuple = hv_Number;

    Number = hTuple[0].I();//将缺陷个数赋给Number，由HTuple类型转换为int类型

  }

void MainWindow::on_pushButton_close_clicked()
{
    close();
}


void MainWindow::on_pushButton_save_filepath_clicked()//将被处理的图片路径和处理后的缺陷个数存入数据库中
{
    QSqlQuery query;
    QString name = filename;
    QString num = QString::number(Number);
    QString str = QString("INSERT INTO regiongrowing(errorname,errornum) VALUES('%1','%2')").arg(name).arg(num);
    query.prepare(str);
        if(!query.exec()){
            qDebug()<<"query error :"<<query.lastError();
        }
        else{
            qDebug()<<"insert data success!";
        }//插入路径和缺陷个数


    if(query.exec("select *from regiongrowing"))//将数据库内容显示在tableWidget上
        {
            //删除所有行
            ui->tableWidget->clearContents();
            ui->tableWidget->setRowCount(0);

            while(query.next())//显示在tableWidget上
            {
            int rowCount = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(rowCount);
            QTableWidgetItem *columnItem0 = new QTableWidgetItem(
            query.value(0).toString());
            QTableWidgetItem *columnItem1 = new QTableWidgetItem(
            query.value(1).toString());
            ui->tableWidget->setItem(rowCount,0, columnItem0);
            ui->tableWidget->setItem(rowCount,1, columnItem1);
        }
     }


}

void MainWindow::on_pushButton_count_clicked()//跳转到子窗口
{
    this->hide();//显示子窗口，同时将主窗口隐藏
    count *c = new count(this);
    connect(c,SIGNAL(sendsignal()),this,SLOT(reshow()));//当点击子界面时，调用主界面的reshow()函数
    c -> show();

}

//显示主窗口
void MainWindow::reshow(){
    this->show();
}


