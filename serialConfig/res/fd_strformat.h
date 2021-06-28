//
//  fd_Strformat.h
//  字符串处理
//
//  Created by Trilobite on 2019/6/5.
//  Copyright © 2019 Trilobite. All rights reserved.
//

#ifndef fd_Strformat_h
#define fd_Strformat_h

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>

namespace FdStrformat {
    // char* ==> string
    std::string strformat(unsigned char * charstr, int len);

    // 字符串转大写
    std::string stringupper(std::string str);

    // 单字符转大写
    char charuper(char Char);

    // 单字符转小写
    char charlower(char Char);

    // 数字字符串转数值
    long int strTolong(std::string str);

    std::string strAppend(std::string str1, std::string str2);
}

#endif /* fd_Strformat_h */
