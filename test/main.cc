#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>

using namespace std;

union
{
    int i;
    int j;
} u;

void Func(char a[100])
{
    cout << a << endl;
    cout<<sizeof(a) << endl; // 4 字节而不是100 字节
}

int main()
{
    int a = 5,b = 7,c;
    c = a+++b;
    a++;
    cout << a << endl;
    cout << b << endl;
    cout << c << endl;
    return 0;
}