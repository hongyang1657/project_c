//
//  main.cpp
//  cppDemo
//
//  Created by PC-wzj on 2019/2/18.
//  Copyright © 2019 PC-wzj. All rights reserved.
//

#include <iostream>
#include "str_2_arr.h"

using namespace std;

int main(int argc, const char * argv[]) {
    
//    构建工具对象
    Tool tool;
    
//    工具添加字典
    tool.set_dictionay_for_file("/Users/apple/Desktop/Demo/cppDemo/char2index.txt");
    tool.isLog = true;

//    单字符查找
    cout<< tool.char_2_index("你") <<endl;
    
//    多字符查找
    string str = "你abと:你";
//    int *result_arr = tool.str_2_arr(str);
    int *result_arr = tool.str_2_arr(str, "/Users/apple/Desktop/Demo/cppDemo/1.txt");
    
    cout << "[";
    for (int i = 0; i < 200; i++) {
        cout << result_arr[i] << ", ";
    }
    cout << "]" << endl;
    
    delete result_arr;
    
    return 0;
}

