#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTimer>
#include"server.h"
#include<QObject>
#include<QDateTime>
#include"subwindow.h"
#include<fstream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public:
    vector<customer> customer_list;//旅客信息容器
    vector<vector<shift>> shift_table;//航班表容器
    city_name_dict city_dict;//city_dict是一个类，里面有一个city_list，下标（City_ID）和其中的名字（QString）一一对应

public:
    subwindow* subwindow_1;

public:
    QDateTime current_datetime;

public:
    void LogEvent(QString);

signals:
    void create_a_new_customer_signal();
    void change_this_customer_signal(customer);


private slots:
    void on_Add_Button_clicked();
    void QTimerEvent();
    void new_customer_created_slot(customer);
    void on_Change_Button_clicked();
    void set_Change_Button_Enabled();

private:
    Ui::MainWindow *ui;
    QTimer timer;
};

void shift_table_init(city_name_dict& city_dict,vector<vector<shift>>& shift_table,QDateTime current_datetime);
#endif // MAINWINDOW_H
