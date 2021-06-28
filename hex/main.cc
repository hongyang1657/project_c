#include <iostream>  
#include <string>  
using namespace std;  
  
std::string DecIntToHexStr(long long num)  
{  
    std::string str;  
    long long Temp = num / 16;  
    int left = num % 16;  
    if (Temp > 0)  
        str += DecIntToHexStr(Temp); 
    else if(Temp==0)
        str += "0"; 
    if (left < 10)  
        str += (left + '0');  
    else  
        str += ('A' + left - 10);  

    if (str.length()==3)
    {
        str = str.substr(1);
    }
    
    return str;  
}
  
string DecStrToHexStr(string str)  
{  
    long long Dec = 0;  
    for (int i = 0; i < str.size(); ++i)  
        Dec = Dec * 10 + str[i] - '0';  
    return DecIntToHexStr(Dec);  
}  
int main()  
{  
    string DecStr;  
    while (cin >> DecStr)  
    {  
        cout << DecStrToHexStr(DecStr);  
    }  
    return 0;  
}  
