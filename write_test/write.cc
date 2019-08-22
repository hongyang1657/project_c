#include <iostream>
 
#include <fstream>
#include <iomanip>
using namespace std;

float result;

int main(){
    // char str_index[100]; 
     string max_index = "asdasdasdad";
     int a = 1;
    // FILE* fpIntent;
    // fpIntent = fopen("log.txt","wb");
    // fwrite(str_index,sizeof(str_index),1,fpIntent);
    // fclose(fpIntent);

    std::ofstream fileW("test.log");
if (fileW.is_open())
{
fileW<<max_index;
fileW<<"---def\n";
fileW.close();
}

//在文件末尾追加字符串
    result = 0.9956584;
    ofstream SaveFile("file1.txt", ios::app);
    SaveFile << a;
    
    SaveFile << max_index << "\t" << setprecision(4) << result; 
    SaveFile.close();
}