/*************************************************************************
	> File Name: keshe.cpp
	> Author: 
	> Mail: 
	> Created Time: 2018年01月02日 星期二 13时01分36秒
 ************************************************************************/

#include<iostream>
#include<mysql.h>
#include<vector>
#include<string.h>
#include<string>
#include<cstdlib>
#include<unistd.h>
#include<time.h>
#include<queue>
#include<algorithm>
const char *line = "\t\t**********************************************\t\n";
#define HOST "127.0.0.1"
#define USER "root"
#define PASSWORD "173874"
#define DB "keshe"
using namespace std;
MYSQL *mysql;
void menu_word(void);//菜单文字
void menu_act(void);//菜单操作
void get_info(void);//获取信息
void add_line(void);//添加路线
void delete_line(void);//删除路线
void watch_info(void);//查看裤线
void watch_all(void);
void watch_start(void);
void watch_end(void);
void find_info(void);//查找指定要求路线
void min_time(void);//最少时间
void min_cost(void);//最省钱
void min_times(void);//中转次数最少
void my_err(string str,int line);//错误处理
int choose_route(struct tm one,struct tm two);//比较one是否小于two的时间
string standard_time(double  time);//将秒转换成年天小时
void get_time(string wrong_time,struct tm * correct_time);//用sscancf将字符串转化出来
void cout_err(const char*);
#define db_query(data) db_query_real(data, __LINE__ )
//define展开 可以自动获取行数
void db_query_real(string data,int line);

template<typename T>
void inline input_sth(const char* prompts, T &to)
{
    cout << "\t\t"<<prompts;
    cin >> to;
}

struct Edge {
    string from;
    string to;
    string tool;
    string start_time;
    string end_time;
    int power;
    string num;//车次
};
struct Path { 
    string vertex;//顶点
    int power; 
    vector<Edge> path;
    struct tm before_time;
}; 
 
struct PathComparer { //大的在前，就交换，保证从小到达
    bool operator()(Path &a, Path &b) 
    { 
        return a.power > b.power; 
    } 
};
vector<Edge> findFrom(string from,int flag);

Path dijkstra(string from, string to,int flag);//dijkstra算法
Path bfs(string from,string to);//求中转次数最少

int main()
{
    mysql = mysql_init(NULL);//数据库初始化
    if(mysql_real_connect(mysql,HOST,USER,PASSWORD,DB,0,NULL,0)==NULL)
    {
        my_err("mysql connect",__LINE__);
    }
    mysql_query(mysql, "SET NAMES UTF8");
    menu_word();
    menu_act();
}

void menu_word(void)
{
    system("clear");
    cout <<line;
    cout<<"\t\t\t\t欢迎来到查询票务系统\t\t"<<endl;
    cout<<"\n\n";
    cout<<"\t\t\t\t1、管理航线信息"<<endl;
    cout<<"\t\t\t\t2、查询车次"<<endl;
    cout<<"\t\t\t\t3、根据需求查询线路"<<endl;
    cout<<"\t\t\t\t4、退出系统"<<endl;
    cout << line;
    cout <<"\t\t请输入你的选项:";
}

void menu_act(void)
{
    string choice;
    cin >> choice;
    while(true)
    {
        if(choice=="1")
            get_info();
        else if(choice=="2")
            watch_info();
        else if(choice=="3")
            find_info();
        else if(choice=="4")
            break;
        else 
        {
            cout_err("输入有误,请重新输入");
        }

        menu_word();
        cin >> choice;
    }
    system("clear");
    cout << line;
    cout <<"\t\t\t\t欢迎下次使用"<<endl;
    cout <<line;

}

void get_info(void)
{
    while(1)
    {
        system("clear");
        cout<<"\t\t\t1、添加路线"<<endl;
        cout<<"\t\t\t2、删除路线"<<endl;
        cout<<"\t\t\t3、返回上一层"<<endl;
        cout<<"\n";
        cout <<"\t\t请输入你的选项:";
        string choice;
        cin >> choice;
        if(choice=="1")
            add_line();
        else if(choice=="2")
            delete_line();
        else if(choice=="3")
            break;
        else 
        {
            cout_err("输入有误,请重新输入");
        }
    }
}
void add_line(void)
{
    //detail_route in;
    Edge in;
    input_sth("起点:",in.from);
    input_sth("终点:",in.to);
    input_sth("交通工具:",in.tool);
    input_sth("起始时间:",in.start_time);
    input_sth("终止时间:",in.end_time);
    input_sth("费用:",in.power);
    input_sth("车次",in.num);
    string insert_data = "insert into data values(\"";
    insert_data += in.from + "\",\"" + in.to + "\",\"" + in.tool + "\",\"" + in.start_time + "\",\"" +
                   in.end_time + "\",\"" + to_string(in.power) + "\",\"" + in.num + "\""+");";         
    db_query(insert_data);
    cout_err("添加成功！");
}

void delete_line(void)
{
    Edge in;
    input_sth("请输入删除路线的起点:",in.from);
    input_sth("请输入删除路线终点:",in.to);
    input_sth("请输入删除路线的车次:",in.num);
    
    string insert_data;
    insert_data = "delete from data where start=";
    insert_data +="\""+in.from+"\" and end=\"" + in.to + "\" and num=\"" + in.num + "\";";
    db_query(insert_data);
    cout_err("删除成功！");
}

void watch_info(void)//查询路线
{
   while(1)
   {
    system("clear");
    cout << "\t\t\t1、查询全部路线"<<endl;
    cout << "\t\t\t2、查询特定起点的路线"<<endl;
    cout << "\t\t\t3、查询特定终点的路线"<<endl;
    cout << "\t\t\t4、返回上一层"<<endl;
    string choice;
    input_sth("请输入你的选项:",choice);
    cout << "choice=" << choice << endl;
    if(choice == "1")
        watch_all();
    else if(choice == "2")
        watch_start();
    else if(choice =="3")
        watch_end();
    else if(choice =="4")
        break;
   }
}
void watch_all(void)
{
    while(1)
    {
        system("clear");
        string result = "select * from data";
        db_query(result);
        MYSQL_RES* res=mysql_store_result(mysql);
        MYSQL_ROW row;
        int count = 1;
        while(row=mysql_fetch_row(res))
        {
            cout <<count<<"、";
            cout <<"从"<<row[0]<<" 到"<<row[1]<<" 乘坐"<<
                row[2]<<" 起始时间："<<row[3]<<" 终止时间："<<row[4]<<"费用:"<<row[5]<<" 车次:"<<row[6]<<endl;
            count++;
        }
        cout <<"\t\t是否返回（yes/no)：";
        string choice;
        cout <<"\n\t\t\t";
        cin >> choice;
        if(choice == "yes")
            break;
    }
}
void watch_start(void)
{
    while(1)
    {
        system("clear");
        string start;
        cout <<"\t\t请输入查询的起点:"<<endl;
        cout <<"\t\t";
        cin >> start;
        string result= "select * from data where start=\"";
        result += start+"\";";

        db_query(result);
        MYSQL_RES* res=mysql_store_result(mysql);
        MYSQL_ROW row;
        int count = 1;
        while(row=mysql_fetch_row(res))
        {
            cout <<count<<"、";
            cout <<"从"<<row[0]<<" 到"<<row[1]<<" 乘坐"<<
                row[2]<<" 起始时间："<<row[3]<<" 终止时间："<<row[4]<<"费用:"<<row[5]<<" 车次:"<<row[6]<<endl;
            count++;
        }
        if(count < 2)
            cout <<"\n\t\t没有相关路线!"<<endl;

        cout <<"\t\t是否返回（yes/no)：";
        string choice;
        cout <<"\n\t\t\t";
        cin >> choice;
        if(choice == "yes")
            break;
        else 
            cout_err("输入错误!");
    }
}
void watch_end(void)
{
    while(1)
    {
        system("clear");
        string end;
        cout <<"\t\t请输入查询的终点:"<<endl;
        cout <<"\t\t";
        cin >> end;
        string result= "select * from data where end=\"";
        result += end+"\";";
        db_query(result);
        MYSQL_RES* res=mysql_store_result(mysql);
        MYSQL_ROW row;
        int count = 1;
        while(row=mysql_fetch_row(res))
        {
            cout <<count<<"、";
            cout <<"从"<<row[0]<<" 到"<<row[1]<<" 乘坐"<<
                row[2]<<" 起始时间："<<row[3]<<" 终止时间："<<row[4]<<"费用:"<<row[5]<<" 车次:"<<row[6]<<endl;
            count++;
        }
        if(count < 2)
            cout <<"\n\t\t没有相关路线!"<<endl;
        cout <<"\t\t是否返回（yes/no)：";
        string choice;
        cout <<"\n\t\t\t";
        cin >> choice;
        if(choice == "yes")
            break;
    }
}

void find_info(void)
{
    system("clear");
    int choice;
    while(1)
    {
        system("clear");
        cout <<"\t\t请输入与你的选项"<<endl;
        cout<<"\t\t\t1、时间最短的路径"<<endl;
        cout<<"\t\t\t2、最省费用的路径"<<endl;
        cout<<"\t\t\t3、中转次数最少的路径"<<endl;
        cout<<"\t\t\t4、返回上一层"<<endl;
        cout <<"\t\t";
        cout <<"请输入你的选项:";
        cin >>choice;
        if(choice == 1)
            min_time();
        else if(choice ==2)
            min_cost();
        else if(choice ==3)
            min_times();
        else if(choice ==4)
            break;
        else 
        {
            cout_err("输入错误，请重新输入");
        }
    }
}

vector<Edge> findFrom(string from,int flag)//与找出起点相关的路线
{
    vector<Edge> find_result;
    string search_data;
    search_data = "select * from data where start=\"";
    search_data += from+"\";";
    db_query(search_data);
    MYSQL_RES* res=mysql_store_result(mysql);
    MYSQL_ROW row;
    struct tm one = {0};//计算起止时间差
    struct tm two = {0};
    while(row=mysql_fetch_row(res))
    {
        //从格式中提取时间
        get_time(row[3],&one);
        get_time(row[4],&two);
     
        string from,to,tool,start_time,end_time,num;
        from = row[0];to = row[1];tool=row[2];
        start_time = row[3];end_time = row[4];
        num = row[6];
        int cost;
        sscanf(row[5], "%d", &cost);
        int power;
        if(flag ==1)
             power = int(difftime(mktime(&two),mktime(&one)));
        else if(flag ==2)
            power = cost;
        else 
            power =1;

        Edge t = {from,to,tool, start_time,end_time,power,num};//,route};
        find_result.push_back(t);//将结果放入vector里
    }
    mysql_free_result(res);

    return find_result;
}

void min_time(void)//最少时间num  
{
    int flag =1;
    while(1)
    {
        system("clear");
        string start;
        string end; 
        input_sth("请输入起点:",start);
        input_sth("请输入终点:",end);
        Path result = dijkstra(start,end,1);
        if (result.power < 0)
        {
            cout << "\n\t\t 没有从 " << start << " 到 " << end << " 的路径！" << endl;
        }
        else
        {
            cout << "\n\t\t最少时间的路程是:"<<endl;
            cout <<"\t\t① 详细路线:"<<endl;
            int count = 1;
            struct tm  before = {0};
            struct tm after ={0};
            for(auto i:result.path)
            {
                cout<<"\t\t";
                cout <<count<<"、";
                cout <<string("从")+i.from<<string(" 到")+i.to<<string(" 乘坐")+i.tool<<string(" 起始时间：")+i.start_time<<string(" 终止时间：")+i.end_time<<endl;
                count++;
            }
            get_time(result.path.front().start_time.c_str(),&before);
            get_time(result.path.back().end_time.c_str(),&after);
            string total_time = standard_time(difftime(mktime(&after),mktime(&before)));
            cout <<"\t\t② 所花费总时间:"<<total_time<<endl;

        }
        cout <<"\n\t\t\t是否继续查询(yes/no)"<<endl;
        string get_in;
        cout <<"\t\t\t";
        cin >> get_in;
        if(get_in == "no")
            break;
        
        else if(get_in !="yes")
        {
            while(1)
            {
                cout << "\t\t输入有误，请重新输入"<<endl;
                cout <<"\t\t";
                cin >> get_in;
                if(get_in=="no")
                {
                    flag = 0;
                    break;
                }
                else if(get_in =="yes")
                    break;
            }
        }
        if(flag ==0)
             break;

    }
}

Path dijkstra(string  from, string to,int flag) 
{ 
   priority_queue<Path, vector<Path>, PathComparer> Q; // 创建优先队列
   struct tm before_time = {0};//之前结束时间
   struct tm temp = {0};

   Q.push({from, 0, {}, before_time}); 
   while (!Q.empty()) 
   { 
       auto u = Q.top(); Q.pop(); 
       if (u.vertex == to) 
       {
           return u; 
       }

       before_time = u.before_time;

       auto find_result = findFrom(u.vertex,flag); 
       sort(find_result.begin(), find_result.end(), [](const Edge &a, const Edge &b) //排序
       { 
           return a.power < b.power;
       }); 
       
       for (auto i: find_result) //输出路线
       {
            get_time(i.start_time,&temp);
            if(choose_route(before_time, temp)) 
            {
                get_time(i.end_time,&temp);
                u.path.push_back({i.from,i.to,i.tool,i.start_time,i.end_time,i.power,i.num});
                Q.push({i.to, u.power + i.power, u.path, temp});
                u.path.pop_back();
            }
       }
    }

    return {"", -1, {}, temp};
}
Path bfs(string from,string to)
{
    queue<Path> Q; // 创建队列
    struct tm before_time = {0};//之前结束时间
    struct tm temp = {0};
 
    Q.push({from, 0, {}, before_time}); 
    while (!Q.empty()) 
    { 
        auto u = Q.front(); Q.pop(); 
        if (u.vertex==to) 
        {
            return u; 
        }
 
        before_time = u.before_time;
 
        auto find_result = findFrom(u.vertex,3); 
        
        for (auto i: find_result) //输出路线
        {
             get_time(i.start_time,&temp);
             if(choose_route(before_time, temp)) 
             {
                 get_time(i.end_time,&temp);
                 u.path.push_back({i.from,i.to,i.tool,i.start_time,i.end_time,i.power,i.num});
                 Q.push({i.to, u.power + 1, u.path, temp});
                 u.path.pop_back();
             }
        }
     }
 
    return {"", -1, {}, temp};
}
int choose_route(struct tm one,struct tm two)
//保证之前的结束时间小于现在的开始时间
{
    if(one.tm_year!=two.tm_year)
        return one.tm_year < two.tm_year;
    if(one.tm_mon!=two.tm_mon)
        return one.tm_mon < two.tm_mon;
    if(one.tm_mday!=two.tm_mday)
        return one.tm_mday < two.tm_mday;
    if(one.tm_hour!=two.tm_hour)
        return one.tm_hour < two.tm_hour;
    if(one.tm_min!=two.tm_min)
        return one.tm_min < two.tm_min;
    if(one.tm_sec!=two.tm_sec)
        return one.tm_sec < two.tm_sec;
    return 1;
}
void min_cost(void)//最省钱
{
    int flag =1;
    while(1)
    {
        system("clear");
        string start;
        string end; 
        input_sth("请输入起点:",start);
        input_sth("请输入终点:",end);
        Path result = dijkstra(start,end,2);
        if (result.power < 0)
        {
            cout << "\n\t\t 没有从 " << start << " 到 " << end << " 的路径！" << endl;
        }
        else
        {
            cout << "\n\t\t最少花费的路程是:"<<endl;
            cout <<"\t\t① 详细路线:"<<endl;
            int count = 1;
            struct tm  before = {0};
            struct tm after ={0};
            int total_cost=0;
            for(auto i:result.path)
            {
                cout<<"\t\t";
                cout <<count<<"、";
                cout <<string("从")+i.from<<string(" 到")+i.to<<string(" 乘坐")+i.tool
                    <<string(" 起始时间：")+i.start_time<<string(" 终止时间：")+i.end_time<<"费用是:"<<i.power<<endl;
                count++;
                total_cost+=i.power;
            }
            cout <<"\t\t② 所花费总费用:"<<total_cost<<endl;

        }
        cout <<"\n\t\t是否继续查询(yes/no)"<<endl;
        string get_in;
        cout <<"\t\t";
        cin >> get_in;
        if(get_in == "no")
            break;
        
        else if(get_in !="yes")
        {
            while(1)
            {
                cout << "\t\t\t输入有误，请重新输入"<<endl;
                cout <<"\t\t\t";
                cin >> get_in;
                if(get_in=="no")
                {
                    flag = 0;
                    break;
                }
                else if(get_in =="yes")
                    break;
            }
        }
    
        if(flag ==0)
             break;

    }
        
}
void min_times(void)//中转次数最少
{
     int flag =1;
     while(1)
     {
         system("clear");
         string start;
         string end; 
         input_sth("请输入起点:",start);
         input_sth("请输入终点:",end);
         Path result = bfs(start,end);
         if (result.power < 0)
         {
             cout << "\n\t\t 没有从 " << start << " 到 " << end << " 的路径！" << endl;
         }
         else
         {
             cout << "\n\t最少中转的路程是:"<<endl;
             cout <<"\t① 详细路线:"<<endl;
             int count = 1;
            int cost =0;
             for(auto i:result.path)
             {
                 cout<<"\t\t";
                 cout <<count<<"、";
                 cout <<string("从")+i.from<<string(" 到")+i.to<<string(" 乘坐")+i.tool<<string(" 起始时间：")+i.start_time<<string(" 终止时间：")+i.end_time<<endl;
                 cost +=i.power;
                 count++;
             }
             cout <<"\n\t② 中转次数:"<<cost<<endl;
 
         }
         cout <<"\n\t\t\t是否继续查询(yes/no)"<<endl;
         string get_in;
         cout <<"\t\t\t";
         cin >> get_in;
         if(get_in == "no")
             break;
         
         else if(get_in !="yes")
         {
             while(1)
             {
                 cout << "\t\t\t输入有误，请重新输入"<<endl;
                 cout <<"\t\t\t";
                 cin >> get_in;
                 if(get_in=="no")
                 {
                     flag = 0;
                     break;
                 }
                 else if(get_in =="yes")
                     break;
             }
         }
     
         if(flag ==0)
              break;
    }
}
void my_err(string str,int line)
{
    fprintf(stderr,"line:%d",line);
    perror(str.c_str());
}
string standard_time(double  time)//将秒转换成年天小时
{
    string day="0",hour="0",min="0",secs="0";
    int t = int(time);
    while(t/3600/24 >= 1)//天
    {
        day = to_string(t/3600/24);
        t = t%3600;
    }
    while(t/3600 >= 1 && t/3600 < 24 )//小时
    {
        hour = to_string(t/3600);
        t = t%3600 ;
    }
    while(t>1 && t< 60)//分钟
    {
        min = to_string(t);
        t = t%60;
    }
        secs = to_string(t);//秒

    return day+"天"+hour+"小时"+min+"分钟"+secs+"秒";
}
void get_time(string wrong_time,struct tm *correct_time)//用sscancf将字符串转化出来
{
    sscanf(wrong_time.c_str(),"%d-%d-%d-%d-%d-%d",&(correct_time->tm_year), &(correct_time->tm_mon),&(correct_time->tm_mday),
             &(correct_time->tm_hour),&(correct_time->tm_min),&(correct_time->tm_sec));
}
void cout_err(const char* str)
{
    cout << "\t\t\t";
    cout<<str<<endl;
    sleep(1);
}
void db_query_real(string data,int line)
{
    if(mysql_real_query(mysql,data.c_str(),data.size())!=0)
    {
        my_err("query error",line);
       printf("\033[32m%s\033[0m\n", mysql_error(mysql));
    }
}
