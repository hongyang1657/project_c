//
//  fd_Strformat.cc
//  AM
//
//  Created by Trilobite on 2019/6/5.
//  Copyright © 2019 Trilobite. All rights reserved.
//

#include "fd_strformat.h"

std::string FdStrformat::strformat(unsigned char * charstr, int len)
{
    std::string str = "";
    
    for (int i = 0; i < len; i++) {
        char temp[4] = {0};
        sprintf(temp, "%02x", charstr[i]);
        str += temp;
    }
    return str;
}

std::string FdStrformat::stringupper(std::string str)
{
    std::string temp = "";
    for (int i = 0; i < str.length(); i++) {
        temp += charuper(str[i]);
    }
    return temp;
}

char FdStrformat::charuper(char Char)
{
    if (Char >= 'a' && Char <= 'z') {
        Char -= 32;
    }
    return Char;
}

char FdStrformat::charlower(char Char)
{
    if (Char >= 'A' && Char <= 'Z') {
        Char += 32;
    }
    return Char;
}

long int FdStrformat::strTolong(std::string str)
{
    long int num = 0;
    bool negative = false;
    
    for (int i = 0; i < str.length()-3; i++) {
        char item = str[i];
        if (item == '-')
        {
            negative = !negative;
            continue;
        }
        else if(item == '+')
        {
            continue;
        }
        else if ( (item-'0') >= 0 && (item-'0') <= 9)
        {
            num *= 10;
            num += (item-'0');
        }
    }
    
    if(negative) num *= -1;
    
    return num;
}

std::string FdStrformat::strAppend(std::string str1, std::string str2) {
    return str1+str2;
}