/*************************************************************************
	> File Name: test_time.cpp
	> Author: 
	> Mail: 
	> Created Time: 2018年01月02日 星期二 19时41分23秒
 ************************************************************************/

#include<iostream>
using namespace std;
#include <time.h>       /* time_t, struct tm, difftime, time, mktime */

int main ()
{
  time_t timer;
  struct tm y2k = {0};
  struct tm y1k ={0};
  double seconds;

  y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
  y2k.tm_year = 2017; y2k.tm_mon = 12; y2k.tm_mday = 31;
  
  y1k.tm_hour = 0;   y1k.tm_min = 0; y1k.tm_sec = 0;
  y1k.tm_year = 2018; y1k.tm_mon = 1; y1k.tm_mday = 1;

  seconds = difftime(mktime(&y1k),mktime(&y2k));

  cout << seconds/3600;

  return 0;
}
