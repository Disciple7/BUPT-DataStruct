#include"server.h"
#include<iostream>

customer::customer(Customer_ID customer_id)
{
    data_pack.customer_ID=customer_id;
    data_pack.end_city_ID=0;
    data_pack.start_city_ID=0;
    data_pack.via_city_1_ID=0;
    data_pack.via_city_2_ID=0;
    data_pack.via_city_3_ID=0;
    if(DEV_MODE)
        cout << data_pack.customer_ID<<" has been constructed."<<endl;
}
customer::customer()
{
    data_pack.customer_ID=0;
    data_pack.end_city_ID=0;
    data_pack.start_city_ID=0;
    data_pack.via_city_1_ID=0;
    data_pack.via_city_2_ID=0;
    data_pack.via_city_3_ID=0;
}
customer::customer(const customer& customer_a)
{
    customer_shift.clear();
    for (vector<shift>::const_iterator a = customer_a.customer_shift.begin();a!=customer_a.customer_shift.end();a++)
	{
        customer_shift.push_back(*a);//这里不是压入指针，而是压入迭代器a（a是指针）指向的内容
	}
    data_pack = customer_a.data_pack;
    if(DEV_MODE)
        cout << data_pack.customer_ID<<" has been delivered.."<<endl;
}
customer::~customer()
{
	if(DEV_MODE)
        cout << data_pack.customer_ID<<" has been destructed."<<endl;
}
customer& customer::operator=(const customer& customer_a)
{
    customer_shift.clear();
    for (auto &a : customer_a.customer_shift)
	{
        customer_shift.push_back(a);
	}
    data_pack = customer_a.data_pack;
	return *this;
}

void customer::strategy1st(const vector<vector<shift>> shift_table)
{

}
void customer::strategy2nd(const vector<vector<shift>> shift_table)
{

}
void customer::strategy3rd(const vector<vector<shift>> shift_table)
{

}

city_name_dict::~city_name_dict()
{
    city_name_list.clear();
}
City_ID city_name_dict::get_city_id(QString city_name)
{
    int now_city_arg;
    for(now_city_arg=0;now_city_arg<city_name_list.size();now_city_arg++)
    {
        if(city_name==city_name_list[now_city_arg])
            break;
    }
    return now_city_arg;
}
QString city_name_dict::get_city_name(City_ID city_id)
{
    return city_name_list[city_id];
}
void city_name_dict::push_random_string(int length,int rand_seed)
{
    qsrand(rand_seed);//为随机值设定一个seed

    const char chrs[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int chrs_size = sizeof(chrs);

    char* ch = new char[length + 1];
    memset(ch, 0, length + 1);
    int randomx = 0;
    for (int i = 0; i < length; ++i)
    {
        randomx= rand() % (chrs_size - 1);
        ch[i] = chrs[randomx];
    }

    QString ret(ch);
    city_name_list.push_back(ret);
    delete[] ch;
}
