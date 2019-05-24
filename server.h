#ifndef SERVER_H
#define SERVER_H
#include<vector>
#include<QString>
#include<QDateTime>
#define DEV_MODE 1

using namespace std;


typedef int Transport_ID;
typedef int Customer_ID;
typedef int City_ID;
typedef int Shift_ID;
typedef int Strategy_ID;

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
	Customer_ID customer_ID;
	City_ID start_city_ID;
	City_ID end_city_ID;
	City_ID via_city_1_ID;
	City_ID via_city_2_ID;
	City_ID via_city_3_ID;
    Strategy_ID strategy_ID;
    int limit_time=0;
};
class customer
{
public://如果需要的话这个也可以改成public
    vector<shift> customer_shift;
public://写private太麻烦了，干脆全部public了，省的还要写set和get函数一大堆。
    Data_Pack data_pack;
public:
	void strategy1st(const vector<vector<shift>> shift_table);//传入航班表，函数的作用是计算该旅客的计划，并按时间顺序压到vector中。
	void strategy2nd(const vector<vector<shift>> shift_table);//传一个二维vector可能会发生意想不到的结果（vector的底层也是指针），考虑使用全局变量作为航班表。
	void strategy3rd(const vector<vector<shift>> shift_table);
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
    void push_random_string(int length,int rand_seed);
};

#endif //SERVER_H
