#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include<QMouseEvent>
#include"count.h"
#include<qstring.h>
#ifndef __APPLE__
#  include "HalconCpp.h"
#else
#  ifndef HC_LARGE_IMAGES
#    include <HALCONCpp/HalconCpp.h>
#  else
#    include <HALCONCppxl/HalconCpp.h>
#  endif
#endif

#if _MSC_VER >=1600 //VS2010版本号是1600，强制MSVC编译器采用UTF-8编码生成可执行文件
#pragma execution_character_set("utf-8")
#endif

extern int Row;
extern int Column;
extern int Tolerance;
extern int Minsize;
extern int MIN;
extern int min;
extern QString filename;
extern int Number;


using namespace HalconCpp;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    // Local iconic variables
    HObject  ho_Image, ho_Region, ho_Regions, ho_SelectedRegions1;
    HObject  ho_RegionDifference, ho_ConnectedRegions, ho_SelectedRegions;
    HObject  ho_Rectangle;

    // Local control variables
    HTuple  hv_Width, hv_Height, hv_WindowHandle,hv_WindowHandles;
    HTuple  hv_Row1, hv_Column1, hv_Row2, hv_Column2, hv_Number;

    Hlong winID,winIDs;

    QPoint lastPoint;//鼠标在控件中的位置
    bool mouseDown;//鼠标按下标志位
    float zoom_scale;//放大倍数
    QPoint windowPoint,firstPoint;//图像移动点和初始点
    int m_dDispImagePartRow0, m_dDispImagePartCol0, m_dDispImagePartRow1, m_dDispImagePartCol1;//显示图像的区域
    int m_dYOffset, m_dXOffset, m_dXO, m_dYO;//图像偏移，与初始位置
    void mousePressEvent(QMouseEvent *event);//鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event);//鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event);//鼠标松开事件
    void wheelEvent(QWheelEvent *event);//鼠标滚轮事件
    void displayImage(HImage srcImg,HTuple hv_Window);//显示图像
    void moveWnd(QPoint point, HImage srcImg, HTuple hWindow);//移动显示区域
    void mouseDoubleClickEvent(QMouseEvent *event);//鼠标双击事件

private slots:
    void on_pushButton_open_picture_clicked();

    void on_pushButton_handle_picture_clicked();

    void on_pushButton_close_clicked();

    void on_pushButton_save_filepath_clicked();

    void on_pushButton_count_clicked();

    void reshow();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
