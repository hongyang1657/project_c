//
//  Tool.cpp
//  AM
//
//  Created by Trilobite on 2019/4/10.
//  Copyright © 2019 Trilobite. All rights reserved.
//

#include "Tool.h"
#include <typeinfo>
#include <math.h>
#include <time.h>
#include <strstream>
#include <sstream>
#include <iostream>

#define E 2.71828


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

int includeChinese(std::string str) {
    char temp;
    int i = 0;
    
    while (i < str.size()) {
        temp = str[i];
        if (temp & 0x80) {
            return 1;
        }
        i++;
    }
    
    return 0;
}

//字符串分割
std::vector<std::string> split(const std::string& str, const std::string& pattern) {
    std::vector<std::string> ret;
    if(pattern.empty()) return ret;
    size_t start=0,index=str.find_first_of(pattern,0);
    while(index!=str.npos)
    {
        if(start!=index)
            ret.push_back(str.substr(start,index-start));
        start=index+1;
        index=str.find_first_of(pattern,start);
    }
    if(!str.substr(start).empty())
        ret.push_back(str.substr(start));
    return ret;
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

std::string clearHeadTailQuotes(std::string &str) {
    if (str.empty()) {
        return str;
    }
    str.erase(0,str.find_first_not_of("\""));//去首引号
    str.erase(str.find_last_not_of("\"") + 1);//去尾引号
    return str;
}

float softMax(float *values, int num) {
    float max = 0.0;
    float sum = 0.0;
    for(int i = 0; i < num; i++){
        float thisTurn = pow(E*1.0,values[i]);
        if(max<thisTurn)
        {
            max = thisTurn;
        }
        sum += thisTurn;
        // printf("max = %lf sum = %lf softMax = %lf\n", max, sum, max/(sum*1.0));
    }
    
    return max/(sum*1.0);
}

float softMax(float *values, float value, int num) {
    // std::cout << num << std::endl;
    float sum = 0.0;
    for(int i = 0; i < num; i++){
        float thisTurn = pow(E*1.0,values[i]);
        sum += thisTurn;
        // printf("max = %lf sum = %lf softMax = %lf\n", max, sum, max/(sum*1.0));
    }
    
    return value/(sum*1.0);
}

std::string getTime()
{
    time_t t = time(NULL);
    
    std::string result;
    std::strstream s;
    s <<  t << "000";
    s >> result;

    return result;

}

long getCurrentTime() {
    time_t t = time(NULL);
    
    std::string result;
    std::strstream s;
    s <<  t;
    s >> result;

    return atol(result.c_str());
}

void getDateAndTime(){
    time_t now = time(0);// 基于当前系统的当前日期/时间
    tm *ltm = localtime(&now);
	char iyear[50],imonth[50],iday[50],ihour[50],imin[50],isec[50];
    sprintf(iyear, "%d",1900 + ltm->tm_year );
    sprintf(imonth, "%02d", 1 + ltm->tm_mon );
    sprintf(iday, "%02d", ltm->tm_mday );
    sprintf(ihour, "%02d", ltm->tm_hour );
    sprintf(imin, "%02d",  ltm->tm_min);
    sprintf(isec, "%02d",  ltm->tm_sec);

    std::cout << iyear <<"-"<<imonth<<"-"<<iday <<" "<<ihour<<":"<<imin<<":"<<isec<<std::endl;
}

int EN_number(std::string numStr) {
    
    std::string base_str[28] = {"zero","one","two","three","four","five","six","seven","eight","nine","ten","eleven","twelve","thirteen","fourteen","fifteen","sixteen","sevente","eighteen","nineteen","twenty","thirty","forty","fifty","sixty","seventy","eighty","ninety"};
    int base_num[28] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 30, 40, 50, 60, 70, 80, 90};
    
    std::map<std::string, int> base_map;
    std::map<std::string, int> units_map;

    int num = 0;
    
    for (int i = 0; i < 28; i++) {
        base_map[base_str[i]] = base_num[i];
    }
    
    units_map["hundred"] = 100;
    units_map["thousand"] = 1000;
    units_map["million"] = 1000000;
    
    std::vector<std::string>elements = split(numStr, " ");
    
    int temp_num = 0;
    int carry = 1;
    int symbol = 1;
    
    for (long i = elements.size()-1; i >= 0; i--) {
        std::string element = elements[i];
        if (base_map.find(element) != base_map.end()) {
            temp_num += (base_map[element] * carry);
        } else if(units_map.find(element) != units_map.end()) {
            if (units_map[element] > carry) {
                num += temp_num;
                temp_num = 0;
                carry = units_map[element];
            } else {
                carry *= units_map[element];
            }
        } else if (element == "minus" || element == "Minus") {
            symbol *= -1;
        }
    }
    num += temp_num;
    num *= symbol;
    
    return num;
}

int ZH_number(std::string numStr) {
    
    //设置运算基数及单位基数
    std::string base_str[11] = {"零","一","二","两","三","四","五","六","七","八","九"};
    int base_num[11] = {0, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9};
    std::string units_str[6] = {"个", "十", "百", "千", "万", "亿"};
    int units_num[6] = {1, 10, 100, 1000, 10000, 100000000};
    
    std::map<std::string, int> base_map;
    std::map<std::string, int> units_map;
    
    for (int i = 0; i < 11; i++) {
        base_map[base_str[i]] = base_num[i];
    }
    
    for (int i = 0; i < 6; i ++) {
        units_map[units_str[i]] = units_num[i];
    }
    
    std::vector<std::string>elements = subdump(numStr);
    std::vector<int>num_elements;
    std::vector<int>num_units;
    
    int num = 0;
    int symbol = 1;
    for (int i = 0; i < elements.size(); i ++) {
        std::string element = elements[i];
        
        if (base_map.find(element) != base_map.end()) {
            //匹配到中文数字
            num_elements.push_back(base_map[element]);
            num_units.push_back(1);
        } else if(units_map.find(element) != units_map.end()) {
            
            //首位匹配到单位
            if (!num_elements.size()) {
                num_elements.push_back(1);
                num_units.push_back(1);
            }
            
            //匹配到中文单位
            int unit = units_map[element];
            
            for (long j = num_units.size()-1; j >= 0; j--) {
                if (num_units[j] > unit) {
                    break;
                } else {
                    num_units[j] *= unit;
                }
            }
        } else if (element == "负") {
            //匹配到符号
            symbol *= -1;
        }
        
    }
    
    for (int i = 0; i < num_elements.size(); i++) {
        num += (num_elements[i] * num_units[i]);
    }
    num *= symbol;
    
    return num;
}
