#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <time.h>

using namespace std;

int main(){
    ofstream os;
    os.open("out.txt");
    os << "111sdfsdgdfs\tdfewfwfrewefqdfdsf111\n111wefweffdf111\n111wdqwedfwef" <<endl;
    os.close();

    ifstream is;
    string content;
    string total;
    is.open("out.txt");
    
    while (!is.eof())
    {
        getline(is,content);
        //cout << content << endl;
        total.append(content);
    }
    cout << total << endl;
    is.close();
    return 0;
}