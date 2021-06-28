#include "fd_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <string.h>
using namespace std;

std::string log_time() {
    time_t t = time(NULL);
    char ch[64] = {0};
    strftime(ch, sizeof(ch)-1, "%H:%M:%S", localtime(&t));
    return ch;
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
        cout << "log文件不存在" <<filesize << endl;
        return -1;
    }
    else
    {
        filesize = fileInfo.st_size;
        cout << "log文件大小" <<filesize << endl;
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

    if (getFileSize(path) < 100 * 1024)
    {
        //cout << "文件小于100k" << endl;
        return 0;
    }
    else
    {
        cout << "文件大于100k" << endl;
        //缓存文件复制到日志文件
        copyCache2logFile(path,ALL_LOG_PATH);
        string fstr("\n----------------clean------------------\n");
        try
        {
            fstream fout(path, ios::out | ios::trunc);
            fout << fstr << endl;
            fout.close();
            cout << "\n--------覆盖log---------\n" << endl;
        }
        catch (exception &e)
        {
            cout << "\n--------error happened---------\n" << e.what() << endl;
            return -1;
        }
    }

    return 0;
}

//遍历文件夹中的文件
void traverseLogFile(){
    DIR * dir;
    struct dirent * ptr;
    char file_list[100][40];
    int i=0;
    char srcFile1[1][100];
    string rootdirPath = "/home/hongy/C_C++/memoryTest/";
    string x,dirPath;
    dir = opendir((char *)rootdirPath.c_str()); //打开一个目录
    while((ptr = readdir(dir)) != NULL) //循环读取目录数据
    {
        printf("d_name : %s\n", ptr->d_name); //输出文件名
        x=ptr->d_name;
        dirPath = rootdirPath + x;
        //printf("d_name : %s\n", dirPath.c_str()); //输出文件绝对路径
//        x = dirPath.c_str();
        strcpy(srcFile1[i],dirPath.c_str()); //存储到数组

        if ( ++i>=100 ) break;
    }
    closedir(dir);//关闭目录指针
}

