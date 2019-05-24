#include "subwindow.h"
#include "ui_subwindow.h"
#include"mainwindow.h"
#include"server.h"

subwindow::subwindow(QMainWindow *parent) :
    QDialog(parent),
    ui(new Ui::subwindow)
{
    ui->setupUi(this);
    parent_window=parent;
    //这里需要初始化每个comboBox的列表！然而，由于不能交叉包含头文件，所以不能在创建时传递mainwindow的指针
    //把comboBox的填充延迟到第一次打开窗口时
    connect(parent,SIGNAL(create_a_new_customer_signal()),this,SLOT(create_a_new_customer_slot()));
    connect(parent,SIGNAL(change_this_customer_signal(customer)),this,SLOT(change_this_customer_slot(customer)));
}

subwindow::~subwindow()
{
    delete ui;
}

void subwindow::on_buttonBox_accepted()
{
    //日志函数记录添加/修改事件
    //根据旅客要求调用算法模块。
    //启动计时器
    customer new_customer(ui->Customer_ID_Label->text().toInt());
    new_customer.data_pack.end_city_ID=ui->Destination_ComboBox->currentIndex()+1;//0号城市不存在，特别注意这一点
    new_customer.data_pack.start_city_ID=ui->Location_ComboBox->currentIndex()+1;//
    new_customer.data_pack.strategy_ID=ui->Strategy_ComboBox->currentIndex();
    if(ui->Via_1_ComboBox->currentText()!="(Blank)")
        new_customer.data_pack.via_city_1_ID=ui->Via_1_ComboBox->currentIndex();
    else
        new_customer.data_pack.via_city_1_ID=0;
    if(ui->Via_2_ComboBox->currentIndex()!=0)
        new_customer.data_pack.via_city_2_ID=ui->Via_2_ComboBox->currentIndex();
    else
        new_customer.data_pack.via_city_2_ID=0;
    if(ui->Via_3_ComboBox->currentIndex()!=0)
        new_customer.data_pack.via_city_3_ID=ui->Via_3_ComboBox->currentIndex();
    else
        new_customer.data_pack.via_city_3_ID=0;
    if(new_customer.data_pack.strategy_ID==2)//只有选择最后一个策略时才会录入限制时间
        new_customer.data_pack.limit_time=ui->Limit_Time_spinBox->value();
    emit new_customer_created_signal(new_customer);
    hide();
}

void subwindow::on_buttonBox_rejected()
{
    //启动计时器
    hide();
}

void subwindow::create_a_new_customer_slot()
{
    if(ui->Destination_ComboBox->count()==0)
    {
        for(int i=1;i<11;i++)
        {
            ui->Destination_ComboBox->addItem(parent_city_dict->get_city_name(i));
            ui->Location_ComboBox->addItem(parent_city_dict->get_city_name(i));
            ui->Via_1_ComboBox->addItem(parent_city_dict->get_city_name(i));
            ui->Via_2_ComboBox->addItem(parent_city_dict->get_city_name(i));
            ui->Via_3_ComboBox->addItem(parent_city_dict->get_city_name(i));
        }
    }
    customer_ID_count++;
    ui->Customer_ID_Label->setNum(customer_ID_count);
    ui->Destination_ComboBox->setCurrentIndex(0);
    ui->Location_ComboBox->setCurrentIndex(0);
    ui->Strategy_ComboBox->setCurrentIndex(0);
    ui->Via_1_ComboBox->setCurrentIndex(0);
    ui->Via_2_ComboBox->setCurrentIndex(0);
    ui->Via_3_ComboBox->setCurrentIndex(0);
    ui->Limit_Time_spinBox->setValue(0);
}

void subwindow::change_this_customer_slot(customer this_customer)
{
    ui->Customer_ID_Label->setNum(this_customer.data_pack.customer_ID);
    ui->Destination_ComboBox->setCurrentText(QString::number(this_customer.data_pack.start_city_ID));//这些涉及城市名字的以后都要改
    ui->Location_ComboBox->setCurrentText(QString::number(this_customer.data_pack.end_city_ID));
    ui->Strategy_ComboBox->setCurrentIndex(this_customer.data_pack.strategy_ID);//Strategy读取的是序号，不像其他ID一样
    if(this_customer.data_pack.via_city_1_ID!=0)
        ui->Via_1_ComboBox->setCurrentText(QString::number(this_customer.data_pack.via_city_1_ID));
    ui->Customer_Plan_List->clear();
    for(vector<shift>::iterator now_plan=this_customer.customer_shift.begin();now_plan!=this_customer.customer_shift.end();now_plan++)//显示当前旅客计划表
    {
        ui->Customer_Plan_List->addItem(QString::number(now_plan->shift_ID)+"\n"+parent_city_dict->get_city_name(now_plan->begin_city_ID)+"\n"+parent_city_dict->get_city_name(now_plan->end_city_ID));
    }
}
