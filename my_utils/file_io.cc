#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int writeFile(){
    
}

int main(){
    string s("12345678你好啊asd123");
    string::size_type pos(0);

    pos = s.find("好");

    cout << pos <<endl;

    cout << s.size() <<endl;

    cout<< s.find("123") <<endl;

    cout<< s.rfind("123") <<endl;

    cout << s.find_first_of("你啊") <<endl;

}