#include <iostream>
#include <stdio.h>
#include "Tool.h"
#include <vector>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <cassert>
#include <set>

using namespace std;

#define TARGET "%设备名-"

int i=0;
ofstream outfile("data.txt");
    

void readTxt(string file)
{
    

    ifstream infile(file.c_str());
    //infile.open(file.data(),ios::in); //将文件流对象与文件连接起来


    //提取-之后，%之前的设备名
    int a = file.find(TARGET);
    string a_str(TARGET);
    a = a + a_str.length();
    int b = file.find("%.txt");
    string str = file.substr(a,b-a);
    cout << str <<endl;

    string s;
    
    while (getline(infile, s))
    {
        s.replace(s.find("0"),1,std::to_string(i));
        replace_all(s, "\n", "");
        replace_all(str, "\n", "");
        replace_all(s, "\r", "");
        replace_all(str, "\r", "");
        cout<< s  <<endl;
        outfile << s;
        outfile << "\t";
        outfile << str;
        outfile << endl;
        i++;
    }
    infile.close(); //关闭文件输入流
    return;
}

void getFileContent(string filepath)
{
    //只取文件名为：设备名-xxx 或者 设定值-xxx 的文件（带下划线的一律不要）
    if (filepath.find("_") == filepath.npos)
    {
        if (filepath.find(TARGET) != filepath.npos)
        {
            /* code */
            //cout << filepath <<endl;
            readTxt(filepath);
        }
    }
    //cout << filepath.find("_") <<endl;
    return;
}

int main()
{
    vector<string> name;
    string path = "./svds";
    GetFileNames(path, name);
    for (int i = 0; i < name.size(); i++)
    {
        getFileContent(name[i]);
        //cout << name[i] <<endl;
    }
    
    outfile.close();
    return 0;
}