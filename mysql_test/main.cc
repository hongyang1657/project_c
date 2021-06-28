#include <iostream>
#include <stdio.h>
#include "MyDB.h"

//g++ $(mysql_config --cflags) main.cc MyDB.cc -o main $(mysql_config --libs)
using namespace std;

int main(){
    MyDB db;
    //链接数据库
    db.initDB("localhost","root","fitme","TEST");
    //添加数据
    db.exeSQL("INSERT people values(2,'nana',27);");
    db.exeSQL("SELECT * from people;");
    return 0;
}