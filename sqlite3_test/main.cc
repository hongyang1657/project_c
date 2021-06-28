#include <iostream>
#include <string>
#include "sqlite3.h"
#include <mutex>
#include <unistd.h>
#include <stdio.h>

using namespace std;

mutex fd_database_mutex;

sqlite3 *db;

int initDatabase()
{
    char *errMsg = 0;
    int rc;
    string sql = "";

    //创建数据库
    rc = sqlite3_open("/home/hongy/C_C++/sqlite3_test/fd_database.db", &db);

    if (rc)
    {
        printf("数据库打开失败\n");
    }

    return 0;
}

std::string checkoutSettingValueType(std::string setting_value)
{
    string type = "";
    if (db == NULL)
        return type;

    //锁住，防止侧漏
    fd_database_mutex.lock();

    string sql = "SELECT type FROM setting_value WHERE value = '" + setting_value + "'";
    sqlite3_stmt *stmt = NULL;

    int res = sqlite3_prepare(db, sql.c_str(), -1, &stmt, NULL);

    if (res == SQLITE_OK)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            string type = (char *)sqlite3_column_text(stmt, 0);
            cout << "type = " << type << endl;
            if (!type.empty())
            {
                //锁不住啦，漏啦
                fd_database_mutex.unlock();
                //清除sql对象
                sqlite3_finalize(stmt);
                return type;
            }
        }
    }
    //锁不住啦，漏啦
    fd_database_mutex.unlock();
    //清除sql对象
    sqlite3_finalize(stmt);
    return type;
}

int main(){
    initDatabase();
    while (true)
    {
        usleep(10000);
        string type; 
        type = checkoutSettingValueType("消毒");
        cout << type << endl;
    }
    
    return 0;
}