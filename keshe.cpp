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
void find_info(void);//查找指定要求路线
void min_time(void);//最少时间
void min_cost(void);//最省钱
void min_times(void);//中转次数最少
void my_err(const char *string,int line);//错误处理
int choose_route(struct tm one,struct tm two);
string standard_time(double  time);
void get_time(const char * wrong_time,struct tm * correct_time);

struct node{//要写入数据库的结构
    char start[100];
    char end[100];
    char tool[100];
    char start_time[100];
    char end_time[100];
    char cost[100];
    char num[100];
};
struct Edge {
    char from[100];
    char to[100];
    char tool[100];
    char start_time[100];
    char end_time[100];
    int power;
    char num[100];//车次
    //string route;
    Edge(char *from, char *to, char *tool, char* start_time,char* end_time,int power,char* num): power(power){
        strcpy(this->from, from);
        strcpy(this->to, to);
        strcpy(this->tool, tool);
        strcpy(this->start_time,start_time);
        strcpy(this->end_time,end_time);
        strcpy(this->num,num);
    }
};
struct detail_route{//输出详细结果的机构体
    string from;
    string to;
    string tool;
    string start_time;
    string end_time;
    int cost;
    string num;
};
struct Path { 
    char vertex[100];
    int power; 
    vector<detail_route> path;
    struct tm before_time;
    Path(const char *vertex, int power, vector<detail_route> path,struct tm before_time): power(power), path(path) {
        memcpy(&(this->before_time), &before_time, sizeof(struct tm));
        strcpy(this->vertex, vertex);
    }
}; 
 
struct PathComparer { //大的在前，就交换，保证从小到达
    bool operator()(Path &a, Path &b) 
    { 
        return a.power > b.power; 
    } 
};
vector<Edge> findFrom(char *from,int flag);

Path dijkstra(char* from, char* to,int flag);//dijkstra算法
Path bfs(char *from,char *to);//求中转次数最少

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
}
void menu_act(void)
{
    char choice[100];
    cout<<"\t\t\t";
    cin >> choice;
    int flag = 1;
    while(flag)
    {
        if(strcmp(choice,"1")==0)
            get_info();
        else if(strcmp(choice,"2")==0)
            watch_info();
        else if(strcmp(choice,"3")==0)
            find_info();
        else if(strcmp(choice,"4")==0)
            break;
        else 
        {
            cout <<"\t\t\t输入有误,请重新输入"<<endl;
            sleep(1);
        }

        menu_word();
        cout <<"\t\t\t";
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
        char choice[100];
        cout<<"\t\t";
        cin >> choice;
        if(strcmp(choice,"1")==0)
            add_line();
        else if(strcmp(choice,"2")==0)
            delete_line();
        else if(strcmp(choice,"3")==0)
            break;
        else 
        {
            cout<<"输入有误,请重新输入"<<endl;
            sleep(1);
        }
    }
}
void add_line(void)
{
    node in;
    cout <<"\t\t起点:";
    cin >> in.start;
    cout<<"\t\t终点:";
    cin>>in.end;
    cout <<"\t\t交通工具:";
    cin>>in.tool;
    cout <<"\t\t起始时间:";
    cin>>in.start_time;
    cout <<"\t\t终止时间:";
    cin>>in.end_time;
    cout <<"\t\t费用:";
    cin>>in.cost;
    cout <<"\t\t车次";
    cin >>in.num;
    char insert_data[400];
    memset(insert_data,0,sizeof(insert_data));
    sprintf(insert_data,"insert into data values(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");",
            in.start,in.end,in.tool,in.start_time,in.end_time,in.cost,in.num);
    //cout << insert_data;
    if(mysql_real_query(mysql,insert_data,strlen(insert_data))!=0)
    {
        my_err("insert error",__LINE__);
       // printf("\033[32m%s\033[0m\n", mysql_error(mysql));
    }
    cout <<"\t\t\t添加成功！"<<endl;
    sleep(1);
}

void delete_line(void)
{
    node in;
    cout <<"\t\t请输入删除路线的起点";
    cin >>in.start;
    cout<<"\t\t请输入删除路线终点";
    cin >>in.end;
    cout<<"\t\t请输入删除路线的车次";
    cin >>in.num;
    
    
    char insert_data[400];
    memset(insert_data,0,sizeof(insert_data));
    sprintf(insert_data,"delete from data where start=\"%s\" and end=\"%s\" and num=\"%s\";",
            in.start,in.end,in.num);
    //cout << insert_data;
    if(mysql_real_query(mysql,insert_data,strlen(insert_data))!=0)
    {
        my_err("insert error",__LINE__);
        //printf("\033[32m%s\033[0m\n", mysql_error(mysql));
    }
    cout <<"\t\t\t删除成功！"<<endl;
    sleep(1);

}

void watch_info(void)//查询路线
{
    while(1)
    {
        system("clear");
        char result[400];
        strcpy(result,"select * from data");
        if(mysql_real_query(mysql,result,strlen(result))!=0)
        {
            my_err("watch_info",__LINE__);
            printf("\033[32m%s\033[0m\n", mysql_error(mysql));
        }
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
        string chooice;
        cout <<"\n\t\t\t";
        cin >> chooice;
        if(chooice == "yes")
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
            cout << "\t\t\t输入错误，请重新输入"<<endl;
            sleep(1);
            system("clear");

        }
    }
}

vector<Edge> findFrom(char *from,int flag)//与找出起点相关的路线
{
    vector<Edge> find_result;
    char search_data[400];
    sprintf(search_data,"select * from data where start=\"%s\";",
            from);//只要从起点找就行

    if(mysql_real_query(mysql,search_data,strlen(search_data))!=0)
    {
        my_err("insert error",__LINE__);
        printf("\033[32m%s\033[0m\n", mysql_error(mysql));
    }
    MYSQL_RES* res=mysql_store_result(mysql);
    MYSQL_ROW row;
    struct tm one = {0};//计算起止时间差
    struct tm two = {0};
    while(row=mysql_fetch_row(res))
    {
        //从格式中提取时间
        get_time(row[3],&one);
        get_time(row[4],&two);
        int cost;
        char from[100], to[100], tool[100],start_time[100],end_time[100],num[100];
        strcpy(from, row[0]); strcpy(to, row[1]); strcpy(tool, row[2]);
        strcpy(start_time,row[3]);strcpy(end_time,row[4]);
        strcpy(num,row[6]);
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

void min_time(void)//最少时间
{
    int flag =1;
    while(1)
    {
        system("clear");
        cout << "\t\t\t请输入起点和终点"<<endl;
        char start[100];
        char end[100];
        cout <<"\n\t\t\t";
        cin >> start>>end;
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
            cout <<"\t\t②所花费总时间:"<<total_time<<endl;

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

Path dijkstra(char* from, char* to,int flag) 
{ 
   priority_queue<Path, vector<Path>, PathComparer> Q; // 创建优先队列
   struct tm before_time = {0};//之前结束时间
   struct tm temp = {0};

   Q.push({from, 0, {}, before_time}); 
   while (!Q.empty()) 
   { 
       auto u = Q.top(); Q.pop(); 
       if (strcmp(u.vertex,to)==0) 
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
Path bfs(char *from,char *to)
{
    queue<Path> Q; // 创建队列
    struct tm before_time = {0};//之前结束时间
    struct tm temp = {0};
 
    Q.push({from, 0, {}, before_time}); 
    while (!Q.empty()) 
    { 
        auto u = Q.front(); Q.pop(); 
        if (strcmp(u.vertex,to)==0) 
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
        cout << "\t\t\t请输入起点和终点"<<endl;
        char start[100];
        char end[100];
        cout <<"\n\t\t\t";
        cin >> start>>end;
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
                    <<string(" 起始时间：")+i.start_time<<string(" 终止时间：")+i.end_time<<"费用是:"<<i.cost<<endl;
                count++;
                total_cost+=i.cost;
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
         cout << "\t\t\t请输入起点和终点"<<endl;
         char start[100];
         char end[100];
         cout <<"\n\t\t\t";
         cin >> start>>end;
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
                 cost +=i.cost;
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
void my_err(const char*string,int line)
{
    fprintf(stderr,"line:%d",line);
    perror(string);
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
void get_time(const char * wrong_time,struct tm *correct_time)//用sscancf将字符串转化出来
{
    sscanf(wrong_time,"%d-%d-%d-%d-%d-%d",&(correct_time->tm_year), &(correct_time->tm_mon),&(correct_time->tm_mday),
             &(correct_time->tm_hour),&(correct_time->tm_min),&(correct_time->tm_sec));
}