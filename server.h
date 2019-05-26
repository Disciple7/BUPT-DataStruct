#ifndef SERVER_H
#define SERVER_H
#include<vector>
#include<QString>
#include<QDateTime>
#define DEV_MODE 1

using namespace std;


typedef string Transport_ID;
typedef int Customer_ID;
typedef int City_ID;
typedef int Shift_ID;
typedef int Strategy_ID;
typedef int Event_Flag;

struct shift
{
	Shift_ID shift_ID;
	Transport_ID transport_ID;
    City_ID begin_city_ID;
	City_ID end_city_ID;
    int time_cost_hours;//时间花费和终止时间都写了，可以都保留，也可以选一个
    int money_cost;
    int frequncy;
    QDateTime begin_Qdatetime;
    QDateTime end_Qdatetime;
};

struct Data_Pack
{
    Event_Flag event_flag;
	Customer_ID customer_ID;
	City_ID start_city_ID;
	City_ID end_city_ID;
	City_ID via_city_1_ID;
	City_ID via_city_2_ID;
	City_ID via_city_3_ID;
    Strategy_ID strategy_ID;
    QDateTime limit_datetime;
};
class customer
{
public://如果需要的话这个也可以改成public
    vector<shift> customer_plan;
public://写private太麻烦了，干脆全部public了，省的还要写set和get函数一大堆。
    Data_Pack data_pack;
public:
    int strategy(const vector<vector<shift>> shift_table,QDateTime now_datetime);//传入航班表，函数的作用是计算该旅客的计划，并按时间顺序压到vector中。

public:
    customer();
    customer(Customer_ID);
	~customer();
    customer(const customer& customer_a);
	customer& operator=(const customer& customer_a);
};

class city_name_dict
{
public:
    vector<QString> city_name_list;
public:
    ~city_name_dict();
    QString get_city_name(City_ID city_id);
    City_ID get_city_id(QString city_name);
};
int add_city(int size,vector<QDateTime>& weight,vector<bool> is_checked);//策略2用的，寻找一个最短时间的城市
int add_city(int size,vector<int>& weight,vector<bool> is_checked);//策略1用的，寻找一个最小花费的城市。
void flush_shift_table(QDateTime& now_datetime,vector<vector<shift>>& shift_table_for_plan);//每次加入一个航班后，都要看这个航班的结束时间（写到now_datetime里了）是不是早于某些航班的开始时间。如果是则刷新这些航班（日期加1）
void shift_plan_copy(const vector<shift>& from_shift_plan,vector<shift>& to_shift_plan);
void flush_shift_plan(vector<shift>& shift_plan);

#endif //SERVER_H
