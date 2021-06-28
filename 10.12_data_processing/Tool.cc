//
//  Tool.c
//  generator
//
//  Created by Trilobite on 2020/7/9.
//  Copyright © 2020 Trilobite. All rights reserved.
//

#include "Tool.h"
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>

using namespace std;

//字符串分割
std::vector<std::string> split(const std::string& str, const std::string& pattern) {
    std::vector<std::string> ret;
    if(pattern.empty()) return ret;
    size_t start=0,index=str.find_first_of(pattern,0);
    while(index!=str.npos)
    {
        if(start!=index) {
            ret.push_back(str.substr(start,index-start));
        }
        start=index+1;
        index=str.find_first_of(pattern,start);
    }
    if(!str.substr(start).empty()) {
        ret.push_back(str.substr(start));
    }
    return ret;
}
std::vector<std::string> splits(const std::string& str, const std::string& pattern) {
    std::vector<std::string> ret;
    if(pattern.empty()) return ret;
    size_t start=0,index=str.find_first_of(pattern,0);
    while(index!=str.npos)
    {
        if(start!=index) {
            ret.push_back(str.substr(start,index-start));
        } else {
            ret.push_back("");
        }
        start=index+1;
        index=str.find_first_of(pattern,start);
    }
    if(!str.substr(start).empty()) {
        ret.push_back(str.substr(start));
    } else {
        ret.push_back("");
    }
    return ret;
}

long long getTimestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    return tv.tv_sec * 1000 + tv.tv_usec/1000;

//    return times_m[key];
}

std::vector <std::string> subdump(std::string str) {
    std::vector<std::string> characters;
    long num = str.size();
    int i = 0;
    while(i < num)
    {
        int size = 1;
        if (str[i] == ' ') {
            i++;
            continue;
        } else if(str[i] & 0x80) {
            char temp = str[i];
            temp <<= 1;
            do {
                temp <<= 1;
                ++size;
            } while(temp & 0x80);
        } else {
            size = 0;
            do {
                ++size;
            } while(str[i+size] != ' ' && !(str[i+size] & 0x80));
        }
        std::string subWord;
        subWord = str.substr(i, size);
        characters.push_back(subWord);
        i += size;
    }
    return characters;
}

void Subset(std::vector<std::string> &set_v, string a, int n, int len) {
 
    if (n == 0) {  // 递归终止条件
        set_v.push_back(a);
        return;
    }
    
    for (int j = 0; j <= 1; j++) { //每次需要做两种决策
 
        string temp = a.substr(0, len);
 
        if (j == 0) {
            temp[len - n] = '1';
        }else {
            temp[len - n] = '0';
        }
        Subset(set_v, temp, n - 1,len);
        
    }
    
}

string & replace_all(string& str,const string& old_value,const string& new_value) {
    while(true)   {
        string::size_type pos(0);
        if( (pos=str.find(old_value))!=string::npos )
            str.replace(pos,old_value.length(),new_value);
        else break;
    }
    return str;
}

void GetFileNames(string path,vector<string>& filenames)
{
    DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(path.c_str()))){
        cout<<"Folder doesn't Exist!"<<endl;
        return;
    }
    while((ptr = readdir(pDir))!=0) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0){
            filenames.push_back(path + "/" + ptr->d_name);
    }
    }
    closedir(pDir);
}
