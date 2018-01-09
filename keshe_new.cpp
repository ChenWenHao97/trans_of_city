#include <iostream>
#include <mysql.h>
#include <vector>
#include <string.h>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <time.h>
#include <map>
#include <queue>
#include <algorithm>
#include <functional>

using namespace std;

#define HOST "127.0.0.1"
#define USER "root"
#define PASSWORD "173874"
#define DB "keshe"

struct Edge {
    string from;
    string to;
    string tool;
    string start_time;
    string end_time;
    int power;
    string num; //车次
};

struct Path {
    string vertex; //顶点
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

#define db_query(data) db_query_real(data, __LINE__)
//define展开 可以自动获取行数
void db_query_real(string data, int line);
void get_time(string wrong_time, struct tm *correct_time); //用sscancf将字符串转化出来
string standard_time(double time);                         //将秒转换成年天小时
void my_err(string str, int line);                         //错误处理
void min_times(void);                                      //中转次数最少
void min_cost(void);                                       //最省钱
void min_time(void);                                       //最少时间num
void mins(const char *prompts, int flag);
int choose_route(struct tm one, struct tm two); //比较one是否小于two的时间
Path dijkstra(string from, string to, int flag);
vector<Edge> findFrom(string from, int flag); //与找出起点相关的路线
void watch_end(void);
void watch_start(void);
void watch_all(void);
void watch_any(function<string()> query_string_getter);
void delete_line(void); //删除路线
void add_line(void);    //添加路线
void find_info(void);   //查找指定要求路线
void watch_info(void);  //查询路线
void get_info(void);    //获取信息
void menu(void);        //菜单操作
void choose_linker(string break_string, map<string, function<void()>> jump, function<void()> word);

const char *line = "\t\t**********************************************\t\n";

MYSQL *mysql;

string to_string(string &v)
{
    return v;
}

inline string build_query(string oq)
{
    return oq;
}

template <typename T1, typename... T2>
string build_query(string oq, T1 p, T2... arg)
{
    string n = oq.replace(oq.find("?"), 1, to_string(p));
    return build_query(n, arg...);
}

template <typename T>
void inline input_sth(const char *prompts, T &to)
{
    cout << "\t\t" << prompts << ": ";
    cin >> to;
}

class {} clear;
struct tab {
    int n;
    tab(int n) : n(n) {}
};

struct tip {
    string pt;
    tip(string pt) : pt(pt) {}
};

ostream &operator<<(ostream &os, decltype(clear) &sc)
{
    system("clear");
    return os;
}

ostream &operator<<(ostream &os, tab t)
{
    for (int i = 0; i < t.n; i++)
        os << "\t";
    return os;
}

ostream &operator<<(ostream &os, tip p)
{
    os << tab(3) << p.pt << flush;
    sleep(1);
    return os;
}

void choose_linker(string break_string, map<string, function<void()>> jump, function<void()> word)
{
    for (string choice; word(), cin >> choice;)
    {
        if (choice == break_string)
            break;
        if (jump.find(choice) != jump.end())
            jump[choice]();
        else
            cout << tip("输入错误，请重新输入");
    }
}

int main()
{
    mysql = mysql_init(NULL); //数据库初始化
    if (mysql_real_connect(mysql, HOST, USER, PASSWORD, DB, 0, NULL, 0) == NULL)
        my_err("mysql connect", __LINE__);
    mysql_query(mysql, "SET NAMES UTF8");

    menu();
}

void menu(void)
{
    choose_linker("4", {{"1", get_info}, {"2", watch_info}, {"3", find_info}}, []() {
        cout << clear << line
             << tab(4) << "欢迎来到查询票务系统" << endl
             << endl
             << endl
             << tab(4) << "1、管理航线信息" << endl
             << tab(4) << "2、查询车次" << endl
             << tab(4) << "3、根据需求查询线路" << endl
             << tab(4) << "4、退出系统" << endl
             << line << tab(2) << "请输入你的选项:";
    });

    cout << clear << line << tab(4) << "欢迎下次使用" << endl
         << line;
}

void get_info(void)
{
    choose_linker("3", {{"1", add_line}, {"2", delete_line}}, []() {
        cout << clear
             << tab(3) << "1、添加路线" << endl
             << tab(3) << "2、删除路线" << endl
             << tab(3) << "3、返回上一层" << endl
             << endl
             << tab(2) << "请输入你的选项:";
    });
}

void watch_info(void) //查询路线
{
    choose_linker("4", {{"1", watch_all}, {"2", watch_start}, {"3", watch_end}}, []() {
        cout << clear
             << tab(3) << "1、查询全部路线" << endl
             << tab(3) << "2、查询特定起点的路线" << endl
             << tab(3) << "3、查询特定终点的路线" << endl
             << tab(3) << "4、返回上一层" << endl
             << tab(2) << "请输入你的选项: ";
    });
}

void find_info(void)
{
    choose_linker("4", {{"1", min_time}, {"2", min_cost}, {"3", min_times}}, []() {
        cout << clear
             << tab(2) << "请输入你的选项" << endl
             << tab(3) << "1、时间最短的路径" << endl
             << tab(3) << "2、最省费用的路径" << endl
             << tab(3) << "3、中转次数最少的路径" << endl
             << tab(3) << "4、返回上一层" << endl
             << tab(2) << "请输入你的选项:";
    });
}

void add_line(void)
{
    Edge in;
    input_sth("起点", in.from);
    input_sth("终点", in.to);
    input_sth("交通工具", in.tool);
    input_sth("起始时间", in.start_time);
    input_sth("终止时间", in.end_time);
    input_sth("费用", in.power);
    input_sth("车次", in.num);

    string insert_data = build_query("insert into data values(\"?\", \"?\", \"?\", \"?\", \"?\", \"?\", \"?\");",
                                     in.from, in.to, in.tool, in.start_time, in.end_time, to_string(in.power), in.num);

    db_query(insert_data);
    cout << tip("添加成功！");
}

void delete_line(void)
{
    Edge in;
    input_sth("请输入删除路线的起点", in.from);
    input_sth("请输入删除路线终点", in.to);
    input_sth("请输入删除路线的车次", in.num);

    string insert_data = build_query("delete from data where start=\"?\" and end=\"?\" and num=\"?\");",
                                     in.from, in.to, in.num);

    db_query(insert_data);
    cout << tip("删除成功！");
}

void watch_any(function<string()> query_string_getter)
{
    for (string choice; choice != "yes"; cin >> choice)
    {
        string result = query_string_getter();
        db_query(result);
        MYSQL_RES *res = mysql_store_result(mysql);
        MYSQL_ROW row;

        int count;
        for (count = 1; row = mysql_fetch_row(res); count++)
        {
            cout << count << "、"
                 << "从" << row[0] << " 到" << row[1] << " 乘坐" << row[2]
                 << " 起始时间：" << row[3] << " 终止时间：" << row[4]
                 << "费用:" << row[5] << " 车次:" << row[6] << endl;
        }

        if (count < 2)
            cout << endl
                 << tab(2) << "没有相关路线!" << endl;

        cout << tab(2) << "是否返回（yes/no)：" << endl
             << tab(3);
    }
}

void watch_all(void)
{
    watch_any([]() { return "select * from data"; });
}

void watch_start(void)
{
    watch_any([]() {
        string start;
        input_sth("请输入查询的起点:", start);
        return build_query("select * from data where start=\"?\";", start);
    });
}

void watch_end(void)
{
    watch_any([]() {
        string end;
        input_sth("请输入查询的终点", end);
        return build_query("select * from data where end=\"?\";", end);
    });
}

vector<Edge> findFrom(string from, int flag) //与找出起点相关的路线
{
    vector<Edge> find_result;
    string search_data = build_query("select * from data where start=\"?\";", from);
    db_query(search_data);
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row;
    while (row = mysql_fetch_row(res))
    {
        struct tm one = {0}, two = {0}; //计算起止时间差
        get_time(row[3], &one);         //从格式中提取时间
        get_time(row[4], &two);

        int power = 1;
        power = (flag == 1) ? int(difftime(mktime(&two), mktime(&one))) : power;
        power = (flag == 2) ? stoi(row[5]) : power;

        find_result.push_back({row[0], row[1], row[2], row[3], row[4], power, row[6]}); //将结果放入vector里
    }
    mysql_free_result(res);
    return find_result;
}

Path dijkstra(string from, string to, int flag)
{
    priority_queue<Path, vector<Path>, PathComparer> Q; // 创建优先队列
    struct tm before_time = {0};                        //之前结束时间
    struct tm temp = {0};

    Q.push({from, 0, {}, before_time});
    while (!Q.empty())
    {
        auto u = Q.top();
        Q.pop();
        if (u.vertex == to)
            return u;

        before_time = u.before_time;

        auto find_result = findFrom(u.vertex, flag);
        for (auto i : find_result) //输出路线
        {
            get_time(i.start_time, &temp);
            if (choose_route(before_time, temp))
            {
                get_time(i.end_time, &temp);
                u.path.push_back({i.from, i.to, i.tool, i.start_time, i.end_time, i.power, i.num});
                Q.push({i.to, u.power + i.power, u.path, temp});
                u.path.pop_back();
            }
        }
    }

    return {"", -1, {}, temp};
}

//保证之前的结束时间小于现在的开始时间
int choose_route(struct tm one, struct tm two)
{
    int aval[] = {one.tm_year, one.tm_mon, one.tm_mday, one.tm_hour, one.tm_min, one.tm_sec},
        bval[] = {two.tm_year, two.tm_mon, two.tm_mday, two.tm_hour, two.tm_min, two.tm_sec};

    for (int i = 0; i < 6; i++)
        if (aval[i] != bval[i])
            return aval[i] < bval[i];

    return 1;
}

void mins(const char *prompts, int flag)
{
    for (bool flag = true; flag;)
    {
        string get_in;
        cout << clear;

        string start, end;
        input_sth("请输入起点:", start);
        input_sth("请输入终点:", end);

        Path result = dijkstra(start, end, flag); // dijkstra(start, end, 1);
        if (result.path.empty())
        {
            cout << endl
                 << tab(2) << "没有从 " << start << " 到 " << end << " 的路径！" << endl;
        }
        else
        {
            cout << endl
                 << tab(2) << "最少时间的路程是:" << endl
                 << tab(2) << "① 详细路线:" << endl;

            struct tm before = {0}, after = {0};

            for (int i = 0; i < result.path.size(); i++)
            {
                cout << tab(2) << i + 1 << "、从" << result.path[i].from << " 到 " << result.path[i].to
                     << " 乘坐 " << result.path[i].tool << " 起始时间：" << result.path[i].start_time
                     << " 终止时间：" << result.path[i].end_time << endl;
            }
            get_time(result.path.front().start_time.c_str(), &before);
            get_time(result.path.back().end_time.c_str(), &after);
            string total_time = standard_time(difftime(mktime(&after), mktime(&before)));
            cout << tab(2) << "② 所花费总时间:" << total_time << endl;
        }

        for (;;)
        {
            cout << endl
                 << tab(3) << "是否继续查询(yes/no)" << endl
                 << tab(3);
            cin >> get_in;
            if (get_in == "no")
                flag = false;
            else if (get_in != "yes")
            {
                cout << tip("输入错误，请重新输入！");
                continue;
            }
            break;
        }
    }
}

void min_time(void) //最少时间num
{
    mins("最少时间", 1);
}

void min_cost(void) //最省钱
{
    mins("最少花费", 2);
}

void min_times(void) //中转次数最少
{
    mins("最少中转", 3);
}

void my_err(string str, int line)
{
    cerr << "Error on line " << line << ": ";
    perror(str.c_str());
}

string standard_time(double time) //将秒转换成年天小时
{
    string day = "0", hour = "0", min = "0", secs = "0";
    int t = int(time);
    while (t / 3600 / 24 >= 1) //天
    {
        day = to_string(t / 3600 / 24);
        t = t % 3600;
    }
    while (t / 3600 >= 1 && t / 3600 < 24) //小时
    {
        hour = to_string(t / 3600);
        t = t % 3600;
    }
    while (t > 1 && t < 60) //分钟
    {
        min = to_string(t);
        t = t % 60;
    }
    secs = to_string(t); //秒

    return day + "天" + hour + "小时" + min + "分钟" + secs + "秒";
}

void get_time(string wrong_time, struct tm *correct_time) //用sscancf将字符串转化出来
{
    sscanf(wrong_time.c_str(), "%d-%d-%d-%d-%d-%d", &(correct_time->tm_year), &(correct_time->tm_mon), &(correct_time->tm_mday),
           &(correct_time->tm_hour), &(correct_time->tm_min), &(correct_time->tm_sec));
}

void db_query_real(string data, int line)
{
    if (mysql_real_query(mysql, data.c_str(), data.size()) != 0)
        cerr << "MySQL Error on line " << line << ": " << mysql_error(mysql) << endl;
}
