#ifndef SUBWINDOW_H
#define SUBWINDOW_H
#include"server.h"
#include <QDialog>
#include<QMainWindow>

namespace Ui {
class subwindow;
}

class subwindow : public QDialog
{
    Q_OBJECT

public:
    explicit subwindow(QMainWindow *parent = nullptr);
    ~subwindow();

public:
    QMainWindow *parent_window;
    Customer_ID customer_ID_count=0;
    city_name_dict *parent_city_dict;

signals:
    void new_customer_created_signal(customer);//旅客类已创建信号（发送回创建好的旅客类）

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void create_a_new_customer_slot();//新旅客类创建信号槽
    void change_this_customer_slot(customer);//旅客类更改信号槽


private:
    Ui::subwindow *ui;
};

#endif // subwindow_H
