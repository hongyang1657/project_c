//
//  Tool.cpp
//  AM
//
//  Created by Trilobite on 2019/4/10.
//  Copyright © 2019 Trilobite. All rights reserved.
//

#include <typeinfo>
#include <math.h>
#include <time.h>
#include <strstream>
#include <sstream>
#include <dirent.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>

#include "Tool.h"
#include "fd_log.h"
#include "fd_strformat.h"

#define E 2.71828

const int CHN_NUM_CHAR_COUNT = 10;

const char *chnNumChar[CHN_NUM_CHAR_COUNT] = { "零", "一", "二", "三", "四", "五", "六", "七", "八", "九" };
const char *chnUnitChar[] = { "", "十", "百", "千" };
const char *chnUnitSection[] = { "", "万", "亿", "万亿" };

const int zh_len = 3;

//会有重复替换
std::string& replaceAll(std::string& str, const std::string& old_value, const std::string& new_value) {
    while(true) {
        std::string::size_type pos(0);
        if((pos=str.find(old_value)) != std::string::npos ) {
            str.replace(pos,old_value.length(),new_value);
        } else{
            break;
        }
    }
    return str;
}
//无重复替换
std::string& replaceAllN(std::string& str, const std::string& old_value, const std::string& new_value) {
    std::string::size_type pos(0);

    while(true) {    
        if((pos=str.find(old_value, pos)) != std::string::npos ) {
            str.replace(pos,old_value.length(),new_value);
            pos += new_value.length();
        } else{
            break;
        }
    }
    return str;
}

/**
 判断是否为中文

 @param p 输入字符
 @return 判断结果 0不是汉字， 1是中文汉字
 */
int isCh(char p) {
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

std::string getTimestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    std::string timestamp_str = std::to_string(tv.tv_sec) + std::to_string(tv.tv_usec/1000);

    return timestamp_str;
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

int enNumber(std::string numStr) {
    
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
    
    std::vector<std::string> elements = split(numStr, " ");
    
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

    elements.clear();
    std::vector<std::string>().swap(elements);
    
    return num;
}

int chNumber(std::string numStr) {
    
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

    elements.clear();
    num_elements.clear();
    num_units.clear();
    std::vector<std::string>().swap(elements);
    std::vector<int>().swap(num_elements);
    std::vector<int>().swap(num_units);
    
    return num;
}



void sectionToChinese(unsigned int section, std::string& chnStr) {
    chnStr.clear();
    std::string strIns;
    int unitPos = 0;
    bool zero = true;
    while(section > 0)
    {
        int v = section % 10;
        if(v == 0)
        {
            if( (section == 0) || !zero )
            {
                zero = true; /*需要补0，zero的作用是确保对连续的多个0，只补一个中文零*/
                chnStr.insert(0, chnNumChar[v]);
            }
        }
        else
        {
            zero = false; //至少有一个数字不是0
            strIns = chnNumChar[v]; //此位对应的中文数字
            strIns += chnUnitChar[unitPos]; //此位对应的中文权位
            chnStr.insert(0, strIns);
        }
        unitPos++; //移位
        section = section / 10;
    }
}

//num == 0需要特殊处理，直接返回"零"
void numberToChinese(unsigned int num, bool flag, std::string& chnStr) {
    chnStr.clear();
    int unitPos = 0;
    std::string strIns;
    bool needZero = false;
    
    flag = (flag && num/10 == 1);

    if(num == 0)
    {
        chnStr = chnNumChar[0];
        return;
    }

    while(num > 0)
    {
        unsigned int section = num % 10000;
        if(needZero)
        {
            chnStr.insert(0, chnNumChar[0]);
        }
        sectionToChinese(section, strIns);
        /*是否需要节权位？*/
        strIns += (section != 0) ? chnUnitSection[unitPos] : chnUnitSection[0];
        chnStr.insert(0, strIns);
        /*千位是0？需要在下一个section补零*/
        needZero = (section < 1000) && (section > 0);
        num = num / 10000;
        unitPos++;
    }
    
    if (flag) {
        chnStr.erase(0, chnStr.find_first_not_of("一"));
    }
}

int hexToDecimal(std::string str) {
    int count = str.length();
    int sum = 0;
    /*从十六进制个位开始，每位都转换成十进制*/
    for (int i = count - 1; i >= 0; i--) {      
        /*数字字符的转换*/
        if (str[i] >= '0' && str[i] <= '9') {
            sum += (str[i] - 48) * pow(16, count - i - 1);
        } else if (str[i] >= 'A' && str[i] <= 'F') {    //字母字符的转换
            sum += (str[i] - 55) * pow(16, count - i - 1);
        }
    }
    return sum;
}


std::string decimalToHex(int n){
    char hex[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    int i=0,m,num[2];
    std::string hex_str;
    while(n>0){
		num[i++]=n%16;
		n=n/16;
	}
	for(i=i-1;i>=0;i--){
		m=num[i];
        hex_str = hex_str + hex[m];
	}
    return hex_str;
}

std::string eraseWakeWord(std::string voicetxt, std::string wakeWord)
{
    std::vector<std::string> elements = subdump(wakeWord);

    for (int i; i < elements.size(); i++) {
        std::string element = elements[i];

        int pos = 0;
        while ((pos = voicetxt.find(element)) != -1) {
            voicetxt.erase(pos, 3);
        }
    }

    std::cout << "去除唤醒词处理后的字符串： " << voicetxt << std::endl;

    elements.clear();
    std::vector<std::string>().swap(elements);

    return voicetxt;
}

std::string intercept(std::string str, std::string begin_ch, std::string end_ch)
{
    size_t b_index = str.find_first_of(begin_ch);
    size_t e_index;
    if (b_index != str.npos) {
        std::string temp_str = str.substr(b_index + 1);
        e_index = temp_str.find_first_of(end_ch);
        str = temp_str.substr(0,e_index);
    }
    return str;
}

void getFileNames(std::string path,std::vector<std::string>& filenames)
{
    DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(path.c_str()))){
        FD_DEBUG_L("Folder doesn't Exist!");
        return;
    }
    while((ptr = readdir(pDir))!=0) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0){
            filenames.push_back(path + "/" + ptr->d_name);
    }
    }
    closedir(pDir);
}

double fdGetFileSize(std::string path)
{
    double filesize = -1;
    struct stat fileInfo;
    if (stat(path.c_str(), &fileInfo) < 0)
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

char* getFileBuffer(const char *fname, int *size)
{
    FILE * fd = fopen(fname, "rb");
    if (fd == 0)
        return 0;
    struct stat st;
    char *file_buf = 0;
    if (fstat(fileno(fd), &st) < 0) {
        fclose(fd);
        return file_buf;
    }

    file_buf = (char *)malloc(st.st_size + 1);
    if (file_buf != NULL) {
        if (fread(file_buf, st.st_size, 1, fd) < 1) {
            fclose(fd);
            return 0;
        }
        file_buf[st.st_size] = 0;
    }

    if (size)
        *size = st.st_size;

    fclose(fd);
    return file_buf;
}

bool isNum(std::string str) {
    std::stringstream sin(str);
    double d;
    char c;
    if(!(sin >> d)) {
        return false;
    }
    if (sin >> c) {
        return false;
    }
    return true;
}


void fdSetSysDate(std::string timestampStr)
{
    
    std::cout << timestampStr << std::endl;

    if (!isNum(timestampStr)) {
        return;
    }

    long int timestamp = FdStrformat::strTolong(timestampStr);
    long int _timestamp = FdStrformat::strTolong(getTimestamp());

    if (_timestamp >= timestamp) {
        return;
    }

    //timestamp/=1000;

    // std::cout << __FUNCTION__ << " " << __LINE__ << ": "<< timestampStr << std::endl;
    // std::cout << __FUNCTION__ << " " << __LINE__ << ": "<< timestamp << std::endl;

    // std::cout << __FUNCTION__ << " " << __LINE__ << ": "<< timestamp << std::endl;
    
    tm *ltm = localtime(&timestamp);

    int year = ltm->tm_year + 1900;
    int mon = ltm->tm_mon + 1;

    // std::cout << __FUNCTION__ << " " << __LINE__ << ": year = " << year << std::endl;
    // std::cout << __FUNCTION__ << " " << __LINE__ << ": mon = " << mon << std::endl;
    // std::cout << __FUNCTION__ << " " << __LINE__ << ": day = " << ltm->tm_mday << std::endl;
    // std::cout << __FUNCTION__ << " " << __LINE__ << ": hour = " << ltm->tm_hour << std::endl;
    // std::cout << __FUNCTION__ << " " << __LINE__ << ": min = " << ltm->tm_min << std::endl;
    // std::cout << __FUNCTION__ << " " << __LINE__ << ": sec = " << ltm->tm_sec << std::endl;

   std::stringstream dateStream;
   dateStream << ltm->tm_year + 1900 << "."
   << ltm->tm_mon + 1 << "."
   << ltm->tm_mday << "-"
   << ltm->tm_hour << ":"
   << ltm->tm_min << ":"
   << ltm->tm_sec;

   std::string command = "date -s " + dateStream.str();
    
   std::cout << command << std::endl;
    
   system(command.c_str());
}

