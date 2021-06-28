#pragma once 
 
#include <iostream>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
 
using namespace std;
 
//ibcurl主要提供了两种发送http请求的方式，分别是Easy interface方式和multi interface方式
//前者是采用阻塞的方式发送单条数据，后者采用组合的方式可以一次性发送多条数据
 
//Easy interface方式最基本的用法如下
//1、在主线程中调用curl_global_init(CURL_GLOBAL_ALL)初始化
//2、调用curl_easy_init获取一个句柄；
//3、调用curl_easy_setopt函数设置此次传输的一些基本参数，如url地址、http头、cookie信息、发送超时时间等，其中，CURLOPT_URL是必设的选项；
//4、设置完成后，调用curl_easy_perform函数发送数据；
//5、数据发送完毕后，调用curl_easy_cleanup清空句柄；
//6、调用curl_global_cleanup()做清理工作。
class HttpCurl
{
public:
    HttpCurl()
        :conn(NULL)
    {}
 
    ~HttpCurl(){
        //调用该函数清空句柄
        curl_easy_cleanup(conn);                                            
    }
 
    bool HttpCurlInit(string& context){
        CURLcode code;
 
        //1、在主线程中调用curl_global_init(CURL_GLOBAL_ALL)初始化
        code = curl_global_init(CURL_GLOBAL_ALL);
        if(CURLE_OK != code){
            cout << "Failed to global init default" << "\n";
            return false;
        }                   
        //2、调用curl_easy_init获取一个句柄；
        conn = curl_easy_init();
        if(NULL == conn){
            cout << "Failed to create CURL" << "\n";
            return false;
        }
 
        //3、调用curl_easy_setopt函数设置此次传输的一些基本参数
        
        //CURLOPT_WRITEFUNCTION 第三个参数表明的函数将在libcurl接收到数据后被调用，因此函数多做数据保存的功能
        //第三个参数回调函数原型为：size_t function( void *ptr, size_t size, size_t nmemb, void *stream ); 
        code = curl_easy_setopt(conn,CURLOPT_WRITEFUNCTION,HttpCurl::Write);
        if(CURLE_OK != code){
            cout << "Failed to set write" << "\n";
            return false;
        }
        // CURLOPT_WRITEDATA 用于表明CURLOPT_WRITEFUNCTION函数中的第三个参数stream的来源
        // 因此调用完该函数以后，收到的数据就被放到了context中
        code = curl_easy_setopt(conn,CURLOPT_WRITEDATA,&context);
        if(CURLE_OK != code){
            cout << "Failed to set write data" << "\n";
            return false;
        }
        return true;
    }
    
    bool SetUrl(string& url){
        CURLcode code;
        //CURLOPT_URL 用于设置访问的url
        code = curl_easy_setopt(conn,CURLOPT_URL,url.c_str());
        if(CURLE_OK != code){
            cout << "Failed to set URL" << "\n";
            return false;                                                                                                                        
        }
        return true;
    }
                                             
    bool GetHttpResponse(){
        CURLcode code;
        // curl_easy_perform函数完成curl_easy_setopt指定的所有选项
        code = curl_easy_perform(conn);
        if(CURLE_OK != code){
            cout << "Failed to get response" << "\n";
            return false;                                                                                                                                                
        }
        return true;                                                                                                                        
    }
                                                 
    static size_t Write(void* data,size_t size,size_t nmemb,string& context){
       long sizes = size*nmemb;
       string temp((char*)data,sizes);
       context += temp;
       return sizes;
    }
 
private:
    CURL* conn;
};