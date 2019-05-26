#include"server.h"
#include<iostream>

QDateTime MAX_DATETIME=QDateTime::fromString("2035-01-01 00:00:00","yyyy-MM-dd hh:mm:ss");//时间上定义的无穷远

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

int add_city(int size,vector<int>& weight,vector<bool> is_checked)
{
    int min_weight_city=0;
    int min_weight=INT_MAX;
    int i;
    for(i=1;i<size;i++)
    {
           if(is_checked[i]==false&&weight[i]<=min_weight)
           {
               min_weight=weight[i];
               min_weight_city=i;
           }
    }
    return min_weight_city;
}
int add_city(int size,vector<QDateTime>& weight,vector<bool> is_checked)
{
    int min_weight_city=1;
    QDateTime min_weight=MAX_DATETIME;
    int i;
    for(i=1;i<size;i++)
    {
           if(is_checked[i]==false&&weight[i]<=min_weight)
           {
               min_weight=weight[i];
               min_weight_city=i;
           }
    }
    return min_weight_city;
}

void flush_shift_plan(QDateTime& now_datetime,vector<vector<shift>>& shift_table_for_plan)//这个函数其实还可以在QTimeEvent中再用一次的......但是写的比那个晚，就算了。
{

    for(vector<vector<shift>>::iterator now_shift_line=shift_table_for_plan.begin();now_shift_line!=shift_table_for_plan.end();now_shift_line++)
    {
        for(vector<shift>::iterator now_shift=now_shift_line->begin();now_shift!=now_shift_line->end();)
        {
            if(now_shift->begin_Qdatetime<now_datetime)
            {
                now_shift->begin_Qdatetime=now_shift->begin_Qdatetime.addDays(now_shift->frequncy);
                now_shift->end_Qdatetime=now_shift->end_Qdatetime.addDays(now_shift->frequncy);
            }
            else
            {
                now_shift++;
            }
        }
    }
}

int customer::strategy(vector<vector<shift>> shift_table,QDateTime true_datetime)
{
    vector<City_ID> city_passed_by;//途径多个城市时使用。把途径看作一系列起点-终点的城市序列。对每对起点-终点都进行一次操作。
    city_passed_by.push_back(data_pack.start_city_ID);
    if(data_pack.via_city_1_ID!=0)
    {
        city_passed_by.push_back(data_pack.via_city_1_ID);
        if(data_pack.via_city_2_ID!=0)
        {
            city_passed_by.push_back(data_pack.via_city_2_ID);
            if(data_pack.via_city_3_ID!=0)
                city_passed_by.push_back(data_pack.via_city_3_ID);
        }
    }
    city_passed_by.push_back(data_pack.end_city_ID);

    vector<vector<shift>> shift_table_for_plan;
    for(int i=0;i<shift_table.size();i++)//拷贝一份时刻表，用于计划，因为做计划时需要把所有的计划表更新。
    {
        vector<shift> new_shift_line;
        shift_table_for_plan.push_back(new_shift_line);
        for(int j=0;j<shift_table[i].size();j++)
        {
            shift_table_for_plan[i].push_back(shift_table[i][j]);
        }
    }
    QDateTime destination_QDatetime=true_datetime;//由于加入了途径城市，不能再用即时时间比较，而要用到达地的时间比较下一轮航班

    for(int journey=0;journey<city_passed_by.size()-1;journey++)
    {
        City_ID location=city_passed_by[journey];
        City_ID destination=city_passed_by[journey+1];
        switch (data_pack.strategy_ID)
        {
        case 0://策略1：最少费用算法。需要：航班费用，航班起点，航班终点。使用dij算法
        {
            //初始化
            vector<bool> is_checked;//最短路径结点的集合
            for(int i=0;i<shift_table.size();i++)
                is_checked.push_back(false);
            vector<int> weight;//路径权重集合
            for(int i=0;i<shift_table.size();i++)
                weight.push_back(INT_MAX);
            vector<vector<shift>> shortest_shift_link;//从start_city_ID开始的路径集合
            shortest_shift_link.resize(shift_table.size());
            for(int i=0;i<shift_table.size();i++)
            {
                vector<shift> shift_link;
                shortest_shift_link[i]=shift_link;
            }
            //开始运行
            int epoch_n;
            for(epoch_n=0;epoch_n<shift_table.size();epoch_n++)//每一轮加入一个点，一共进行n轮。（当然，还有个（Blank）城市......）
            {
                int now_city;
                if(epoch_n==0)//第一轮把起点加入，从起点开始运行算法
                {
                    is_checked[location]=true;
                    weight[location]=0;
                    now_city=location;
                }
                else//寻找权重最小且未加入最短路径的城市
                {
                    now_city=add_city(shift_table.size(),weight,is_checked);
                    is_checked[now_city]=true;
                }
                //遍历这个城市now_city的航班表，若shift_table_for_plan[now_city][j]的cost+weight[now_city]（从起点到now_city的cost）小于weight[j]（从起点到j的权）
                //则把shift_link[j]清空，把shift_link[now_city]压进去，再压入now_city到j的这列航班。
                int shift_num;
                for(shift_num=0;shift_num<shift_table_for_plan[now_city].size();shift_num++)//shift_num是一个序号，代表shift_table中从now_city出发的所有航班。shift_table[now_city][j].end_city_ID可以获取这个航班的终点
                {
                    City_ID target_city=shift_table_for_plan[now_city][shift_num].end_city_ID;
                    if(weight[target_city]>weight[now_city]+shift_table_for_plan[now_city][shift_num].money_cost)
                    {
                        weight[target_city]=weight[now_city]+shift_table_for_plan[now_city][shift_num].money_cost;//更改权重
                        shortest_shift_link[target_city].clear();
                        for(int tmp_shift_num=0;tmp_shift_num<shortest_shift_link[now_city].size();tmp_shift_num++)//tmp_shift_num还是一个计数变量，用于把起点到now_city的计划表拷贝到起点到target_city中
                            shortest_shift_link[target_city].push_back(shortest_shift_link[now_city][tmp_shift_num]);
                        shortest_shift_link[target_city].push_back(shift_table_for_plan[now_city][shift_num]);//最后再压入从now_city到target_city的路线。暂时的最短路线就变为起点->now_city->target_city
                    }
                }
            }
            //最后把起点->终点的最佳路径压到用户的路径表中。
            for(int copy_shift_num=0;copy_shift_num<shortest_shift_link[destination].size();copy_shift_num++)
            {
                if(copy_shift_num!=0&&shortest_shift_link[destination][copy_shift_num-1].begin_Qdatetime<shortest_shift_link[destination][copy_shift_num].end_Qdatetime)
                {
                    shortest_shift_link[destination][copy_shift_num].begin_Qdatetime=shortest_shift_link[destination][copy_shift_num].begin_Qdatetime.addDays(1);//加一天。
                    shortest_shift_link[destination][copy_shift_num].end_Qdatetime=shortest_shift_link[destination][copy_shift_num].end_Qdatetime.addDays(1);
                }
                else if(copy_shift_num==0&&shortest_shift_link[destination][copy_shift_num].begin_Qdatetime<destination_QDatetime)
                {
                    shortest_shift_link[destination][copy_shift_num].begin_Qdatetime=shortest_shift_link[destination][copy_shift_num].begin_Qdatetime.addDays(1);//加一天。
                    shortest_shift_link[destination][copy_shift_num].end_Qdatetime=shortest_shift_link[destination][copy_shift_num].end_Qdatetime.addDays(1);
                }
                customer_shift.push_back(shortest_shift_link[destination][copy_shift_num]);
            }
            destination_QDatetime=customer_shift.back().end_Qdatetime;//下一轮的开始时间更新为已经选择航班的最晚时间
            break;
        }
        case 2://策略3：限制时间最少费用算法。在策略2的基础上修改。如果策略2结果不满足则计划置空。
        case 1://策略2：最短时间算法。需要：航班开始时间，航班终止时间，航班起点，航班终点。
        {//使用dij算法，每次选择一条路径后都要更新一次所有航班的时间。
            //在全局变量中设置了一个无穷远的时间MAX_DATETIME，在server.cpp（这个文件）中
            vector<bool> is_checked;//最短路径结点的集合
            for(int i=0;i<shift_table.size();i++)
                is_checked.push_back(false);
            vector<QDateTime> weight;//路径权重集合。这里的权重与策略1不一样，是最晚时间。
            for(int i=0;i<shift_table.size();i++)
                weight.push_back(MAX_DATETIME);
            vector<vector<shift>> shortest_shift_link;//从start_city_ID开始的路径集合
            shortest_shift_link.resize(shift_table.size());
            for(int i=0;i<shift_table.size();i++)
            {
                vector<shift> shift_link;
                shortest_shift_link[i]=shift_link;
            }
            //开始运行
            int epoch_n;
            for(epoch_n=0;epoch_n<shift_table.size();epoch_n++)//每一轮加入一个点，一共进行n轮。（当然，还有个（Blank）城市......）
            {
                int now_city;
                if(epoch_n==0)//第一轮把起点加入，从起点开始运行算法
                {
                    is_checked[location]=true;
                    weight[location]=destination_QDatetime;//没有途径是是true_datetime。有途径以后，进行一次起点-终点的计算以后需要更新当地时间。
                    now_city=location;
                }
                else//寻找权重最小且未加入最短路径的城市
                {
                    now_city=add_city(shift_table.size(),weight,is_checked);
                    is_checked[now_city]=true;
                    flush_shift_plan(weight[now_city],shift_table_for_plan);
                }
                //遍历这个城市now_city的航班表，若shift_table_for_plan[now_city][j]的cost+weight[now_city]（从起点到now_city的cost）小于weight[j]（从起点到j的权）
                //则把shift_link[j]清空，把shift_link[now_city]压进去，再压入now_city到j的这列航班。
                int shift_num;
                for(shift_num=0;shift_num<shift_table_for_plan[now_city].size();shift_num++)//shift_num是一个序号，代表shift_table中从now_city出发的所有航班。shift_table[now_city][j].end_city_ID可以获取这个航班的终点
                {
                    City_ID target_city=shift_table_for_plan[now_city][shift_num].end_city_ID;
                    if(weight[now_city]<shift_table_for_plan[now_city][shift_num].begin_Qdatetime&&weight[target_city]>shift_table_for_plan[now_city][shift_num].end_Qdatetime)//多了一个条件：到达now_city的时间还要早于该班发起的时间才行。
                    {
                        weight[target_city]=shift_table_for_plan[now_city][shift_num].end_Qdatetime;//更改权重
                        shortest_shift_link[target_city].clear();
                        for(int tmp_shift_num=0;tmp_shift_num<shortest_shift_link[now_city].size();tmp_shift_num++)//tmp_shift_num还是一个计数变量，用于把起点到now_city的计划表拷贝到起点到target_city中
                            shortest_shift_link[target_city].push_back(shortest_shift_link[now_city][tmp_shift_num]);
                        shortest_shift_link[target_city].push_back(shift_table_for_plan[now_city][shift_num]);//最后再压入从now_city到target_city的路线。暂时的最短路线就变为起点->now_city->target_city
                        //与策略1不同的地方在于，每次选择一条行程后，都要刷新行程时间。刷新时间的操作在选好当前城市之后，防止到达该城市的时候航班表已经过期。
                    }
                }
            }
            //最后把起点->终点的最佳路径压到用户的路径表中。
            for(int copy_shift_num=0;copy_shift_num<shortest_shift_link[destination].size();copy_shift_num++)
                customer_shift.push_back(shortest_shift_link[destination][copy_shift_num]);
            if(data_pack.strategy_ID==3)//策略3在策略2基础上修改。
            {
                destination_QDatetime=customer_shift.back().end_Qdatetime;
                break;//这个是策略3的返回出口
            }
            else
            {
                destination_QDatetime=customer_shift.back().end_Qdatetime;
                flush_shift_plan(destination_QDatetime,shift_table_for_plan);
                break;//这个是策略2的返回出口
            }
        }
        }
    }
    return  1;
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
