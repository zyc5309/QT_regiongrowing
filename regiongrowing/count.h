#ifndef COUNT_H
#define COUNT_H

#include <QDialog>
#include<QtCharts>
#if _MSC_VER >=1600 //VS2010版本号是1600，强制MSVC编译器采用UTF-8编码生成可执行文件
#pragma execution_character_set("utf-8")
#endif

extern QString srcDirPath;

namespace Ui {
class count;
}

class count : public QDialog
{
    Q_OBJECT

public:
    explicit count(QWidget *parent = 0);
    ~count();

private slots:
    void on_pushButton_return_clicked();
    void on_pushButton_choose_file_clicked();

    void on_pushButton_draw_clicked();


signals:
    void sendsignal();//这个函数用户向主界面通知关闭的消息
private:
    Ui::count *ui;
};

#endif // COUNT_H
