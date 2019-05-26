#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "subwindow.h"
#include"server.h"
#include"log.h"
#include<iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Travel System");
    ui->Change_Button->setEnabled(false);//初始化删除按钮不可点击，当有旅客选中时才可点击
    connect(ui->Customer_Table_Widget,SIGNAL(itemSelectionChanged()),this,SLOT(set_Change_Button_Enabled()));
    //：启动程序的步骤。需要做如下工作：(在MainWindow的构造函数中）
    //日志模块记录启动事件
    //构造并加载航班表容器
    //构造旅客类容器
    //初始化计时器
    current_datetime=QDateTime::currentDateTime();
    this->LogEvent("Log has been initialized.\nProgram is running\n");
    timer.start(1000);//                                     正式版本timer应该为10000，这里用1000，更新快一点

    //初始化一些航班表
    shift_table_init(city_dict,shift_table,current_datetime);

    //航班表ID加载到窗口部件Shift_Table_Widget中（iterator checked）
    for(vector<vector<shift>>::iterator now_shift_line=shift_table.begin();now_shift_line!=shift_table.end();now_shift_line++)
    {
        for(vector<shift>::iterator now_shift=now_shift_line->begin();now_shift!=now_shift_line->end();now_shift++)
        {
            ui->Shift_Table_Widget->addItem(QString::number(now_shift->shift_ID)+"\t"+city_dict.get_city_name(now_shift->begin_city_ID)+"\t"+city_dict.get_city_name(now_shift->end_city_ID)+"\t"+now_shift->begin_Qdatetime.time().toString()+"\t"+now_shift->end_Qdatetime.time().toString()
                                            +"\t"+QString::number(now_shift->frequncy)+"day(s)\t"+now_shift->begin_Qdatetime.toString()+"\t"+now_shift->end_Qdatetime.toString());
        }
    }
    ui->statusBar->showMessage(current_datetime.toString());
    //子窗口创建，传递city_dict的指针，（子窗口的项目填充延迟到第一次打开时），连接子窗口用户数据到主窗口的事件
    subwindow_1 = new subwindow(this);
    subwindow_1->parent_city_dict=&city_dict;
    connect(subwindow_1,SIGNAL(new_customer_created_signal(customer)),this,SLOT(new_customer_created_slot(customer)));
    //系统时间初始化，定时器连接到触发事件
    connect(&timer,SIGNAL(timeout()),this,SLOT(QTimerEvent()));


    //初始化完毕
}

MainWindow::~MainWindow()
{
    delete subwindow_1;
    delete ui;
}

void MainWindow::on_Add_Button_clicked()
{
    //：状态转移任务：
    //暂停计时器
    //判断信号，如果是0号旅客则新建一个旅客，入栈，显示信息。
    //                  否则读取这个旅客的信息，显示信息。
    timer.stop();
    emit create_a_new_customer_signal();//发送一个信号，让subwindow的Customer_ID_Label显示为新建的Customer_ID并让ID计数器加一；
    subwindow_1->exec();
    timer.start();
}

void MainWindow::set_Change_Button_Enabled()
{
    ui->Change_Button->setEnabled(true);
}

void MainWindow::on_Change_Button_clicked()
{
    timer.stop();
    Customer_ID this_customer_ID=ui->Customer_Table_Widget->currentItem()->text().toInt();
    //（iterator checked）
    for(vector<customer>::iterator now_customer=customer_list.begin();now_customer!=customer_list.end();now_customer++)
    {
        if(this_customer_ID==now_customer->data_pack.customer_ID)
        {
            emit change_this_customer_signal(*now_customer);
            break;
        }
    }
    //发送一个信号，传入现有的customer数据，并设置subwindow的所有界面；
    subwindow_1->exec();
    timer.start();
}

void MainWindow::QTimerEvent()
{
    //：状态转移任务：
    //日志函数记录刷新事件
    //所有旅客的所有计划推进1小时
    //将所有旅客的信息与定时器做比较，删除旅客旅程中结束时间小于定时器时间的旅程
    //判断是不是有旅客已经完成旅程了，是则传给日志函数，然后删除这个旅客
    //将所有航班的信息与定时器做比较，把航班开始时间小于定时器时间的航班加上一个周期
    //绘制地图（选）
    //**以下迭代器中的算法仅为模板，其他部分设计完成后需要重写。**
    current_datetime=current_datetime.addSecs(3600);//加上3600秒（一小时）
    LogEvent("Clock flushing... "+current_datetime.toString());
    //旅客信息类容器的刷新
    for(vector<customer>::iterator now_customer=customer_list.begin();now_customer!=customer_list.end();)
    {
        for(vector<shift>::iterator now_plan = now_customer->customer_plan.begin();now_plan!=now_customer->customer_plan.end();)
        {//该循环刷新某个旅客的计划
            if(now_customer->data_pack.event_flag==0&&now_plan->begin_Qdatetime<=current_datetime)
            {
                now_customer->data_pack.start_city_ID=now_plan->end_city_ID;//修改旅客的出发地点为航班到达地点，毕竟在车上不能中途下去
                LogEvent("Customer "+QString::number(now_customer->data_pack.customer_ID)+" , Shift "+QString::number(now_plan->shift_ID)+" has begun.");
                now_customer->data_pack.event_flag=1;//不要让Log复读......读了一次就别读了。只有当完成这个航班时才可能再次发送Log
            }
            if(now_plan->end_Qdatetime<=current_datetime)//如果正乘坐的航班结束时间晚于当前时间，把这个航班从旅客计划表上删除
            {
                this->LogEvent("Customer "+QString::number(now_customer->data_pack.customer_ID)+" has finished a shift(shift id: "+QString::number(now_plan->shift_ID)+" ).");
                now_plan=now_customer->customer_plan.erase(now_plan);
                now_customer->data_pack.event_flag=0;
            }
            else
                now_plan++;
        }
        if(now_customer->customer_plan.size()==0)//如果旅客的计划上已经没有航班，把这个旅客从旅客容器中删除
       {
            LogEvent("Customer "+QString::number(now_customer->data_pack.customer_ID)+" has finished all the journey. Infomation has been cleared.");//日志记录
            QString delete_customer_ID=QString::number(now_customer->data_pack.customer_ID);
            for(int item_count=0;item_count<ui->Customer_Table_Widget->count();item_count++)//该循环从窗口中删除这个旅客的ID
            {
                if(delete_customer_ID==ui->Customer_Table_Widget->item(item_count)->text())
                    delete ui->Customer_Table_Widget->takeItem(item_count);
            }
            now_customer=customer_list.erase(now_customer);//迭代器失效，重新获取迭代器
       }
        else
            now_customer++;
    }
    //刷新航班表数据
    for(vector<vector<shift>>::iterator now_shift_line=shift_table.begin();now_shift_line!=shift_table.end();now_shift_line++)
    {
        for(vector<shift>::iterator now_shift=now_shift_line->begin();now_shift!=now_shift_line->end();)
        {
            if(now_shift->begin_Qdatetime<current_datetime)
            {
                now_shift->begin_Qdatetime=now_shift->begin_Qdatetime.addDays(now_shift->frequncy);
                now_shift->end_Qdatetime=now_shift->end_Qdatetime.addDays(now_shift->frequncy);
                int i=0;
                for(;i<ui->Shift_Table_Widget->count();i++)
                {
                    if(now_shift->shift_ID==ui->Shift_Table_Widget->item(i)->text().split("\t")[0].toInt())
                    {
                        delete ui->Shift_Table_Widget->takeItem(i);
                        break;
                    }
                }
                ui->Shift_Table_Widget->addItem(QString::number(now_shift->shift_ID)+"\t"
                                                +city_dict.get_city_name(now_shift->begin_city_ID)+"\t"+city_dict.get_city_name(now_shift->end_city_ID)+"\t"
                                                +now_shift->begin_Qdatetime.time().toString()+"\t"+now_shift->end_Qdatetime.time().toString()
                                                +"\t"+QString::number(now_shift->frequncy)+"day(s)\t"
                                                +now_shift->begin_Qdatetime.toString()+"\t"+now_shift->end_Qdatetime.toString());
            }
            else
            {
                now_shift++;
            }
        }
    }
    if(ui->Customer_Table_Widget->count()==0)//当旅客列表都被删除时，Change按钮恢复不可点击状态
        ui->Change_Button->setEnabled(false);
    ui->statusBar->showMessage(current_datetime.toString());//刷新状态栏上的日期和时间;
}

void MainWindow::new_customer_created_slot(customer new_customer)
{

    Customer_ID delete_customer_ID=new_customer.data_pack.customer_ID;
    bool delete_customer_ID_flag=false;
    for(vector<customer>::iterator now_customer=customer_list.begin();now_customer!=customer_list.end();)
    {
        if(now_customer->data_pack.customer_ID==delete_customer_ID)//假如存在对应的旧customer信息，就把它删掉
        {
            now_customer=customer_list.erase(now_customer);
            delete_customer_ID_flag=true;
        }
        else
            now_customer++;
    }//检索并删除旧的customer信息（如果有的话）

    //调用策略函数，设置该乘客的行程。
    int strategy_flag=new_customer.strategy(shift_table,current_datetime);
    if(strategy_flag==-1)
        LogEvent("Customer : "+QString::number(new_customer.data_pack.customer_ID)+" raised an unsatisfiable request.\nThe customer info will be deleted soon.");

    customer_list.push_back(new_customer);//无论是否已存在这个customer，都把它加入列表
    if(delete_customer_ID_flag==false)//如果上面的删除没有删除掉customer_ID，则说明是一个新的ID，加入到窗口中。
    {
        QString new_customer_ID = QString::number(new_customer.data_pack.customer_ID);
        ui->Customer_Table_Widget->addItem(new_customer_ID);
    }
}


void shift_table_init(city_name_dict& city_dict,vector<vector<shift>>& shift_table,QDateTime current_datetime)
{
    //读取城市名称列表
    fstream city_name_file;
    if(DEV_MODE)
        city_name_file.open("D:\\coding\\Qt\\Travel_System\\city_name.csv",ios::in|ios::out);
    else
        city_name_file.open("city_name.csv");
    if(!city_name_file.good())
    {
        cerr<<"Open Name File Failure"<<endl;
        abort();
    }
    string city_name_string;
    city_name_file>>city_name_string;
    QStringList Qcity_name=QString::fromStdString(city_name_string).split(',');
    int i;//i是循环计数变量
    int CITY_NUM=Qcity_name.size()-1;//文件中有一个Blank，因此CITY_NUM是11个。
    for(i=0;i<CITY_NUM+1;i++)
        city_dict.city_name_list.push_back(Qcity_name[i]);

    //初始化航班表数据结构
    for(i=0;i<CITY_NUM+1;i++)//压11个城市，0号城市(Blank)，没有航班。
    {
        vector<shift> shift_line;
        shift_table.push_back(shift_line);
    }

    //读取城市航班表
    fstream shift_file;
    if(DEV_MODE)
        shift_file.open("D:\\coding\\Qt\\Travel_System\\shift.csv",ios::in|ios::out);
    else
        shift_file.open("shift.csv");
    if(!shift_file.good())
    {
        cerr<<"Open Shift File Failure"<<endl;
        abort();
    }
    while(shift_file.good()&&shift_file.peek()!=EOF)
    {
        string tmp_string;//读取文件一行
        shift_file>>tmp_string;
        QStringList Qtmp_string_list=QString::fromStdString(tmp_string).split(',');//切割成List
        if(Qtmp_string_list.size()!=7)
            break;
        shift new_shift;
        new_shift.begin_city_ID=Qtmp_string_list[0].toInt();
        new_shift.end_city_ID=Qtmp_string_list[1].toInt();
        new_shift.shift_ID=Qtmp_string_list[2].toInt();
        new_shift.money_cost=Qtmp_string_list[3].toInt();
        QTime tmp_time(Qtmp_string_list[4].toInt(),0,0,0);//日期是现在的日期，但时间是输入的时间
        current_datetime.setTime(tmp_time);
        new_shift.begin_Qdatetime=current_datetime;
        tmp_time.setHMS(Qtmp_string_list[5].toInt(),0,0,0);
        current_datetime.setTime(tmp_time);
        new_shift.end_Qdatetime=current_datetime;
        if(new_shift.begin_Qdatetime<current_datetime)
        {
            new_shift.begin_Qdatetime=new_shift.begin_Qdatetime.addDays(1);
            new_shift.end_Qdatetime=new_shift.end_Qdatetime.addDays(1);
        }
        if(new_shift.begin_Qdatetime>new_shift.end_Qdatetime)
        {
            new_shift.end_Qdatetime=new_shift.end_Qdatetime.addDays(1);
            new_shift.time_cost_hours=24;
        }
        else
            new_shift.time_cost_hours=0;
        new_shift.frequncy=1;
        new_shift.transport_ID=Qtmp_string_list[6].toStdString();
        new_shift.time_cost_hours+=Qtmp_string_list[5].toInt()-Qtmp_string_list[4].toInt();
        shift_table[new_shift.begin_city_ID].push_back(new_shift);
    }
}
void MainWindow::LogEvent(QString event_Qstring)
{
    if(ui->Log_listWidget->count()>50)
    {
        ui->Log_listWidget->clear();
        ui->Log_listWidget->addItem("Log has been cleared automatically due to too many messages.");
    }
    ui->Log_listWidget->addItem(event_Qstring);

    fstream log_file;
    if(DEV_MODE)
        log_file.open("D:\\coding\\Qt\\Travel_System\\log_file.txt",ios::in|ios::out|ios::app);
    else
        log_file.open("log_file.txt",ios::in|ios::out|ios::app);
    log_file<<event_Qstring.toStdString()<<endl;
    log_file.close();
}
