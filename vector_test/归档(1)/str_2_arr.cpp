//
//  str_2_arr.cpp
//  cppDemo
//
//  Created by PC-wzj on 2019/2/18.
//  Copyright © 2019 PC-wzj. All rights reserved.
//

#include "str_2_arr.h"

using namespace std;

const int num = 200;
const int zh_len = 3;


/**
 判断是否为中文

 @param p 输入字符
 @return 判断结果 0不是汉字， 1是中文汉字
 */
int is_zh_ch(char p) {
    if(~(p >> 8) == 0) {
        return 1;
    }
    return 0;
}

vector <string> subdump(string str) {
    //先把str里的汉字和英文分开
    vector <string> dump;
    if(typeid(str)==typeid(string) && str.length()>0) {
        
        unsigned long len = str.length();
        string tmp="";
        int i=0;
        
        while(i<len) {
            //中文字符
            if (is_zh_ch(str.at(i))) {
                dump.push_back(str.substr(i,zh_len));
                i=i+zh_len;
            } else {
                //英文字符
                dump.push_back(str.substr(i,1));
                i=i+1;
            }
        }
    }
    
    return dump;
}

void Tool::set_dictionay_for_file(std::string path) {
    
    ifstream dic_file(path);
    string line;
    
    // 有该文件
    if(dic_file) {
        if (this->isLog) cout << "<读取字典数据：>" << endl;
        while (getline (dic_file, line)) {
            //单行键值分离
            unsigned long index = line.rfind(":", line.length()-0);
            
            string key = line.substr(0, index); //取键
            
            string value = line.substr(index+1, line.length()); //取值
            int int_value = atoi(value.c_str());
            
            if (this->isLog) cout << key << ":" << value << endl;
            
            this->dic[key] = int_value;
        }
    } else {
         // 没有该文件
        cout <<"<no such file>" << endl;
    }
    
    dic_file.close();
    
}

/**
 字典对照
 
 @param str 需对照原串
 @return 字典对照结果（下表数组）
 */
int * Tool::str_2_arr(string str) {
    
//    中英文字符分离
    vector <string> dump = subdump(str);
    
    if (this->isLog) {
        cout << "<输入数据拆分结果：>" << endl;
        for (string sub : dump) {
            cout << sub << endl;
        }
    }
    
//    超长度截取
    while (dump.size() > (num-1)) {
        dump.erase(dump.begin());
    }
    
    int str_len = (int)dump.size()+1;
    int *result_arr = new int[num];  //结果集
    
//    补前置空缺
    for (int i = 0; i < num-str_len; i++) {
        result_arr[i] = 1;
    }
    
//    插入用户标签
    result_arr[num-str_len] = 2;
    
//    查字典记录结果
    if (this->isLog) cout << "<字典查询结果：>" << endl;
    
    for (int i = 0; i < dump.size(); i++) {
        
        string char_str = dump[i];
        int result = this->char_2_index(char_str);
        
        if (this->isLog) cout << char_str << " = " << result << endl;
        
        result_arr[num-str_len+i+1] = result != -1 ? result : 0;
    }
    
    return result_arr;
}

int * Tool::str_2_arr(string str, string path) {
    //查询结果
    int *result_arr = this->str_2_arr(str);
    
    //定义输出文件
    ofstream result_file;
    result_file.open(path, ios::trunc);
    
    //写入字典结果
    for (int i = 0; i < num; i++) {
        result_file << result_arr[i] ;
        if (i != num-1) result_file << endl;
    }
    
    result_file.close();
    
    return result_arr;
}

int Tool::char_2_index(string str) {
    //字典存在字符
    if (this->dic.find(str) != this->dic.end()) {
        return this->dic[str];
    } else {
    //字典无所查字符
        return -1;
    }
}
