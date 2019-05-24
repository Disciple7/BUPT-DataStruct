#include"log.h"
#include"server.h"
#include<iostream>

void logInit(void)//日志启动（如果以文件形式保存日志，就把返回值改为FILE* ，如果以容器形式保存，就写一个日志类，不要这个函数。现在是单纯的cout版本
{
    cout<<"Log has been initialized."<<endl;
}
void logEvent()//启动日志
{
    cout<<"Program is running."<<endl;
}
void logEvent(customer customer_a)//添加/修改旅客信息日志
{
    cout<<"Customer "<<customer_a.data_pack.customer_ID<<" has finished all the journey. Infomation has been cleared."<<endl;
}
void logEvent(Customer_ID customer_id,Shift_ID shift_id)//旅客完成一个航班时记录旅客和航班
{
    cout<<"Customer "<<customer_id<<" has finished a shift(shift id: "<<shift_id<<" )."<<endl;
}
void logEvent(QDate current_date,QTime current_time)//定时器刷新信号日志
{
    cout<<"Clock flushing... "<<qstr2str(current_date.toString())+" "+current_time.toString().toStdString()<<endl;
}
QString str2qstr(const string str)//QString和string互相转码函数
{
    return QString::fromLocal8Bit(str.data());
}

string qstr2str(const QString qstr)
{
    QByteArray cdata = qstr.toLocal8Bit();
    return string(cdata);
}
