#pragma once
#include"server.h"
#include<QDateTime>

void logInit(void);//��־������������ļ���ʽ������־���Ͱѷ���ֵ��ΪFILE* �������������ʽ���棬��дһ����־�࣬��Ҫ���������
void logEvent();//������־
void logEvent(customer);//���/�޸��ÿ���Ϣ��־
void logEvent(QDate,QTime);//��ʱ��ˢ���ź���־
void logEvent(Customer_ID customer_id,Shift_ID shift_id);//�ÿ����һ������ʱ��¼�ÿͺͺ���

string qstr2str(const QString qstr);//QString��string����ת�뺯������ֹ����
QString str2qstr(const string str);//QString��string����ת�뺯������ֹ����
