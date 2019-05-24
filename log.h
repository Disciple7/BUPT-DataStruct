#pragma once
#include"server.h"
#include<QDateTime>

void logInit(void);//日志启动（如果以文件形式保存日志，就把返回值改为FILE* ，如果以容器形式保存，就写一个日志类，不要这个函数。
void logEvent();//启动日志
void logEvent(customer);//添加/修改旅客信息日志
void logEvent(QDate,QTime);//定时器刷新信号日志
void logEvent(Customer_ID customer_id,Shift_ID shift_id);//旅客完成一个航班时记录旅客和航班

string qstr2str(const QString qstr);//QString和string互相转码函数。防止乱码
QString str2qstr(const string str);//QString和string互相转码函数。防止乱码
