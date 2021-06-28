#include "fd_log.h"

#include <fstream>
#include <iostream>
using namespace std;

std::string logTime() {
    // time_t t = time(NULL);
    // char ch[64] = {0};
    // strftime(ch, sizeof(ch)-1, "%H:%M:%S", localtime(&t));
    // std::string time_str = ch;
    // return time_str;
    timeb t;
    ftime(&t);
    char tmp[16];
    strftime(tmp,sizeof(tmp),"%Y%m%d%H%M%S",localtime(&t.time));
    char tm[20];
    sprintf(tm, "%s%d", tmp, t.millitm);
    
    string time = tm;
    time = time.substr(8, 2) + ":" + time.substr(10, 2) +":"+ time.substr(12, 2) +":"+ time.substr(14, 3);
    return time;
}

void LOG_FILE(std::string title, std::string log) {
    std::ofstream file(LOG_FILE_PATH, std::ios::app);
    file << "------------------------------------------------------";
    file << std::endl;
    file << title << std::endl;
    file << log << std::endl;
    file.close();
}

double getFileSize(char *path)
{
    double filesize = -1;
    struct stat fileInfo;
    if (stat(path, &fileInfo) < 0)
    {
        // cout << "log文件不存在" <<filesize << endl;
        return -1;
    }
    else
    {
        filesize = fileInfo.st_size;
        // cout << "log文件大小" <<filesize << endl;
        return filesize;
    }
}

//复制缓存文件内容到日志文件
void copyCache2logFile(char *sourcefile,char *targetfile){
    string command = "cat ";
    command = command+sourcefile+">"+targetfile;
    
    system((char*)command.c_str());
}

//检查日志文件大小
int checkCleanLogFile(char *path)
{

    if (getFileSize(path) < 500 * 1024)
    {
        // cout << "文件小于500k" << endl;
        return 0;
    }
    else
    {
        cout << "文件大于500k" << endl;
        //缓存文件复制到日志文件
        copyCache2logFile(path,ALL_LOG_PATH);
        string fstr("\n----------------clean------------------\n");
        try
        {
            fstream fout(path, ios::out | ios::trunc);
            fout << fstr << endl;
            fout.close();
        }
        catch (exception &e)
        {
            cout << "\n--------error happened---------\n" << e.what() << endl;
            return -1;
        }
    }

    return 0;
}
