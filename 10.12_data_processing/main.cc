#include <iostream>
#include <stdio.h>
#include "cJSON.h"
#include "Tool.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <cassert>
#include <set>

using namespace std;

string readTxt(string file)
{
    string content;
    ifstream infile;
    infile.open(file.data()); //将文件流对象与文件连接起来

    string s;
    while (getline(infile, s))
    {
        content = content + s;
    }
    infile.close(); //关闭文件输入流
    return content;
}

string getData(string filePath)
{
    string lineData;
    //读取文件
    //解析json，提取拼接元素
    cJSON *jsonRoot = cJSON_Parse(readTxt(filePath).c_str());
    cJSON *events_json = cJSON_GetObjectItem(jsonRoot, "events");
    cJSON *type_u2a_speech_json = cJSON_GetArrayItem(events_json, 0);
    cJSON *type_a2s_json = cJSON_GetArrayItem(events_json, 1);
    cJSON *temp_json = cJSON_GetObjectItem(type_u2a_speech_json, "example_speech_template");
    cJSON *prefix_json = cJSON_GetObjectItem(type_a2s_json, "intent_prefix");
    string temp = temp_json->valuestring;
    string prefix = prefix_json->valuestring;
    lineData = temp + "\t" + prefix + "\n";
    return lineData;
}

int main()
{
    ofstream outfile;
    outfile.open("data.txt", ios::binary | ios::in | ios::out);
    vector<string> name;
    string path = "./results_1";
    GetFileNames(path, name);
    cout << name.size() << endl;
    set<string> data_s;
    for (int i = 0; i < name.size(); i++)
    {
        //cout << name[i] << endl;
        
        data_s.insert(getData(name[i]));
        
    }

    for (set<string>::iterator it = data_s.begin();it!=data_s.end();it++)
    {
        /* code */
        cout << *it << endl;
        outfile << *it;
    }
    

    outfile.close();
    return 0;
}
