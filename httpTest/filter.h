#pragma once
 
#include <iostream>
#include <vector>
using namespace std;
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
 
class filter{
public:
    //该函数用来设置正则表达式匹配
    void SetReg(string reg){
        _reg = reg;
    }
 
    //该函数将匹配的结果连成一个字符串放在一起
    string filterUrlString(string url){
        string::const_iterator begin = url.begin();
        string::const_iterator end = url.end();
 
        string results;
        while(regex_search(begin,end,_what,_reg)){
 
            string result(_what[0].first,_what[0].second);
            results += result;
            
            begin = _what[0].second;
        }       
 
        return results;
    }
 
    //该函数将匹配的结果放到vector中
    vector<string> filterUrlVector(string url){
        string::const_iterator begin = url.begin();
        string::const_iterator end = url.end();
 
        vector<string> results;
        while(regex_search(begin,end,_what,_reg)){
 
            string result(_what[0].first,_what[0].second);
            results.push_back(result);
            
            begin = _what[0].second;
        }       
 
        return results;
    }
 
private:
    boost::regex _reg;
    boost::smatch _what;
};