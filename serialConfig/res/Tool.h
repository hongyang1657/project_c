//
//  Tool.h
//  AM
//
//  Created by Trilobite on 2019/4/10.
//  Copyright © 2019 Trilobite. All rights reserved.
//

#ifndef Tool_h
#define Tool_h

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <sys/time.h>
#include <sys/stat.h>

/**
 * 字符串替换，使用一个字符串替换掉原字符串中的子字符串
 * 
 * @param str 原字符串
 * @param old_value 需要替换掉的原子字符串
 * @param new_value 要更换的字符串
 * 
 * @return string 替换后的字符串
 * 
 * */
std::string& replaceAll(std::string& str, const std::string& old_value, const std::string& new_value);
std::string& replaceAllN(std::string& str, const std::string& old_value, const std::string& new_value);

/**
 判断是否为中文
 
 @param p 输入字符
 @return 判断结果 0不是汉字， 1是中文汉字
 */
int isCh(char p);

int includeChinese(std::string str);

/**
 字符串去掉首位引号

 @param str 目标字符串
 @return 结果
 */
std::string clearHeadTailQuotes(std::string &str);

/**
 字符串分割

 @param str 原串
 @param pattern 分割工具字符串
 @return 分割结果列表
 */
std::vector<std::string> split(const std::string& str, const std::string& pattern);

/**
 字符分离（可分离中英文）

 @param str 目标字符串
 @return 结果列表
 */
std::vector <std::string> subdump(std::string str);

float softMax(float *values, int num);
float softMax(float *values, float value, int num);

/**
 获取时间戳

 @return 时间戳
 */
std::string getTime();

std::string getTimestamp();

/**
 * 获取当前时间戳
 * @return 当前时间戳（精确到秒）
 **/
long getCurrentTime();

/**
 * 英文数字字符串转阿拉伯数字
 * 
 * @param numStr 英文数字字符串
 * @return 对应的阿拉伯数字
 **/
int enNumber(std::string numStr);

/**
 * 中文数字字符串转阿拉伯数字
 * 
 * @param numStr 中文数字字符串
 * @return 对应的阿拉伯数字
 **/
int chNumber(std::string numStr);

/**
 * 阿拉伯数字转中文数字
 * 
 * @param num
 * @param flag
 * @param chnStr
 * */
void sectionToChinese(unsigned int section, std::string& chnStr);
void numberToChinese(unsigned int num, bool flag, std::string& chnStr);

/**
 * 打印当前时间
 * */
void getDateAndTime();

/**
 * 字符串转16进制int
 * 
 * @param str 16进制字符串数据
 * @return int 对应的16进制int数据
 **/
int hexToDecimal(std::string str);

std::string decimalToHex(int num);

/**
 * oneshot补丁
 * @param voicetxt  asr原串
 * @param wakeWord  唤醒词
 * @return 去除唤醒词后的string
 * */
std::string eraseWakeWord(std::string voicetxt, std::string wakeWord);

/**
 * 字符串截取
 * @param str 原串
 * @param begin_ch 开始截取的字符
 * @param end_ch 截止截取的字符
 * */
std::string intercept(std::string str, std::string begin_ch, std::string end_ch);

/**
 * 字符串相似度匹配
 * 
 * str1， str2，匹配相似度的目标字符串
 * 字符串相似度
 * */
float fdSimilarity(std::string str1, std::string str2);

/**
 * 获取文件大小和内容
 * @param file_name      文件名
 * @param file_size      文件大小
 * @return file_buf      文件内容
 * */
char* getFileBuffer(const char *fname, int *size);

/**
 * 扫描制定目录下的所有文件
 * 
 * path 目标目录
 * 
 * filenames 目录下所有文件地址
 * */
void getFileNames(std::string path, std::vector<std::string>& filenames);

double fdGetFileSize(std::string path);

bool isNum(std::string str);

void fdSetSysDate(std::string timestampStr);

#endif /* Tool_h */
