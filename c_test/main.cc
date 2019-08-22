#include <stdio.h>

#include <string>
#include <vector>
#include <iostream>
#include <ctime>
using namespace std;



int main(){

	time_t now = time(0);// 基于当前系统的当前日期/时间
    tm *ltm = localtime(&now);
	char iyear[50],imonth[50],iday[50],ihour[50],imin[50],isec[50];
    sprintf(iyear, "%d",1900 + ltm->tm_year );
    sprintf(imonth, "%02d", 1 + ltm->tm_mon );
    sprintf(iday, "%02d", ltm->tm_mday );
    sprintf(ihour, "%02d", ltm->tm_hour );
    sprintf(imin, "%02d",  ltm->tm_min);
    sprintf(isec, "%02d",  ltm->tm_sec);
 

	printf("gpio ret = %s\n",iyear);
	return 0;
}

