#include <iostream>

int main(){
    httplib::Client cli("https://www.baidu.com");
    auto res = cli.Get("/hi");

    res->status;
    res->body;
    return 0;
}