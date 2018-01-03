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
using namespace std;

struct node{//要写入数据库的结构
    char start[100];
    char end[100];
    char tool[100];
    char start_time[100];
    char end_time[100];
    char cost[100];
};
struct Edge {
    char from[100];
    char to[100];
    char tool[100];
    int time;
    int cost;
    Edge(char *from, char *to, char *tool, int time, int cost): time(time), cost(cost) {
        strcpy(this->from, from);
        strcpy(this->to, to);
        strcpy(this->tool, tool);
    }
};

struct Path { 
    char vertex[100];
    int power; 
    string path; 
    Path(char *vertex, int power, string path): power(power), path(path) {
        strcpy(this->vertex, vertex);
    }
}; 
 
struct PathComparer { 
    bool operator()(Path &a, Path &b) 
    { 
        return a.power < b.power; 
    } 
};
vector<Edge> findFrom(char *from);

Path dijkstra(char* from, char* to);

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
    cout<<"\t\t\t\t1、录入航线信息"<<endl;
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
    cout <<"请依次输入起点，终点，交通工具，起始时间，终止时间，费用："<<endl;
    cin >> in.start>>in.end>>in.tool>>in.start_time>>in.end_time>>in.cost;
    
    char insert_data[400];
    memset(insert_data,0,sizeof(insert_data));
    sprintf(insert_data,"insert into data values(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");",
            in.start,in.end,in.tool,in.start_time,in.end_time,in.cost);
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
    cout <<"请删除路线的输入起点，终点"<<endl;
    cin >> in.start>>in.end;
    
    char insert_data[400];
    memset(insert_data,0,sizeof(insert_data));
    sprintf(insert_data,"delete from data where start=\"%s\" and end=\"%s\";",
            in.start,in.end);
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
    ///////
}
void find_info(void)
{
    system("clear");
    int choice;
    while(1)
    {
        system("clear");
        cout <<"请输入与你的选项"<<endl;
        cout<<"\t\t\t1、时间最短的路径"<<endl;
        cout<<"\t\t\t2、最省费用的路径"<<endl;
        cout<<"\t\t\t3、中转次数最少的路径"<<endl;
        cout<<"\t\t\t4、返回上一层"<<endl;
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
            cout << "输入错误，请重新输入"<<endl;
            sleep(1);
        }
    }
}


vector<Edge> findFrom(char *from)
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
        sscanf(row[3],"%d-%d-%d-%d-%d-%d",
                &one.tm_year,&one.tm_mon,&one.tm_mday,&one.tm_hour,&one.tm_min,&one.tm_sec);
        sscanf(row[4],"%d-%d-%d-%d-%d-%d",
                &two.tm_year,&two.tm_mon,&two.tm_mday,&two.tm_hour,&two.tm_min,&two.tm_sec);
        int cost;
        char a[100], b[100], c[100];
        strcpy(a, row[0]); strcpy(b, row[1]); strcpy(c, row[2]);
        sscanf(row[5], "%d", &cost);
        Edge t = {a,b,c , (int)difftime(mktime(&one),mktime(&two)),cost};
        find_result.push_back(t);
    }
    mysql_free_result(res);

    return find_result;
}

void min_time(void)//最少时间
{
    system("clear");
    cout << "\t\t\t请输入起点和终点"<<endl;
    char start[100];
    char end[100];

    cin >> start>>end;
    Path result = dijkstra(start,end);
    cout << "最少时间的路程是:";
    cout << result.path<<endl;
    sleep(1);
}
Path dijkstra(char* from, char* to) 
{ 
   priority_queue<Path, vector<Path>, PathComparer> Q; // 创建优先队列
   Q.push({from, 0, from}); 

   while (!Q.empty()) 
   { 
       auto u = Q.top(); Q.pop(); 
       if (strcmp(u.vertex,to)==0) 
       { 
           return u; 
       } 

       auto find_result = findFrom(u.vertex); 
       sort(find_result.begin(), find_result.end(), [](const Edge &a, const Edge &b) 
       { 
           return a.time < b.time; 
       }); 
        
       for (auto i: find_result) 
       { 
           Q.push({i.to, u.power + i.time, u.path +"->"+i.to}); 
       } 
   } 
   return {0, 0, ""}; 
} 
void min_cost(void)//最省钱
{

}
void min_times(void)//中转次数最少
{

}
void my_err(const char*string,int line)
{
    fprintf(stderr,"line:%d",line);
    perror(string);
}
