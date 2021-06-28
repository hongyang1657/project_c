#include "spider.h"
//#include "../HTTP_server/cgi/operate.hpp"   //这个文件是我自己写的一个数据库交互库，下面在用到的时候会说明
#include "filter.h"
#include <map>
 
 
//将新闻信息存放到一个结构体中
struct NewsInfo{
    string _topic;
    string _title;
    string _author;
    string _content;
 
    NewsInfo(string topic,string title,string author,string content)
        :_topic(topic),
         _title(title),
         _author(author),
         _content(content)
    {}
};
 
//替换结果中的标签部分，取出文字部分
//该函数只使用于在文字一边<>成对出现的情况
//比如<h1>xxx</h1>
//不适用与这种情况<meta property="og:title" content="xxx" />
void Replace(string& src){
    while(src.find('<') != string::npos){
        boost::regex expr("<.*?>");
        src = boost::regex_replace(src,expr,"");
    }
}
 
//获取网页的html源码
bool GetHtml(HttpCurl curl,string& context,string url){
    curl.HttpCurlInit(context);
    curl.SetUrl(url);
    //调用完GetHttpResponse函数以后，响应的body部分就在context中了
    return curl.GetHttpResponse();
}
 
//获取相应主题的新闻链接
map<string,string> GetTopicLinks(string context,filter ft,MYSQL* conn){
    //下面这个函数用来取出我的数据库中的所有主题
    vector<string> topics = OperateTopic::AllTopic(conn);
 
    //我们要匹配到新浪首页对应标签栏的部分的内容
    //这个部分不是固定的，是根据你要爬的网页自己找出共性，自己分析的
    ft.SetReg("<div class=\"nav-mod-1.*?</div>");
    string result = ft.filterUrlString(context);
 
    //再拿到能点击进入链接部分（这是点击不同主题进入的链接,需要有主题的标题）
    ft.SetReg("<a href.*?</a>");
    vector<string> hrefs = ft.filterUrlVector(result);
    map<string,string> links;
 
    //最后拿到http链接
    //这里因为结尾最后都有一个",所以到时候返回的时候这个"得删去
    ft.SetReg("http.*?\"");
    for(auto i:topics){
        //只查找href中有对应主题的几个链接
        for(auto j:hrefs){
            //下面的函数用来判断在href中是否有主题名称
            boost::iterator_range<string::iterator> r = boost::algorithm::find_first(j,i);
            string link;
            if(!r.empty()){
                link = ft.filterUrlString(j);
                links[i] = link.substr(0,link.size() - 1);
            }
        }
    }
    return links;
}
 
//该函数用于拿到对应主题下的新闻链接
vector<pair<string,string>> GetNewsLinks(map<string,string> topicLinks,filter ft){
    HttpCurl curl;
    vector<pair<string,string>> v; 
    for(auto i : topicLinks){
        string context;
        if(!GetHtml(curl,context,i.second))
            continue;
        string flag;
        //现在已经拿到了点进每个主题下的html
        
 
        //根据每个主题下的内容不同，设置对应的flag
        //这个也是新浪对应主题下的新闻链接的格式
        //因为每个主题的格式各不相同，因此也是需要你去对应的新闻下查出来有新闻链接部分的共性内容
        //下面这8个主题是我数据库中的8个主题
        if(i.first == "文化"){
            ft.SetReg("div class=\"action\".*?</div>");;
            flag = "cul"; 
        }else if(i.first == "体育"){
            ft.SetReg("<div style=\"display:none!important;\">.*?</div>");
            flag = "sports";
        }else if(i.first == "NBA"){
            ft.SetReg("<ul class=\"list layout-mb-b\">.*?</ul>");
            flag = "sports";
        }else if(i.first == "国际"){
            ft.SetReg("<div class=\"news-item  \">.*?</div>");
            flag = "news";
        }else if(i.first == "股票"){
            ft.SetReg("<ul class=\"list04\">.*?</ul>");
            flag = "finance";
        }else if(i.first == "娱乐"){
            ft.SetReg("<ul class=\"seo_data_list\">.*?</ul>");
            flag = "ent";
        }else if(i.first == "教育"){
            ft.SetReg("<ul class=\"seo_data_list\">.*?</ul>");
            flag = "edu";
        }else if(i.first == "科技"){
            ft.SetReg("<ul class=\"seo_data_list\">.*?</ul>");
            flag = "tech";
        }
        
        string result = ft.filterUrlString(context);
        string tmp = "http://" + flag + ".*?shtml";
        ft.SetReg(tmp);
        vector<string> links = ft.filterUrlVector(result);
        //有些网页会有些错误，比如说下面这种
        //http://tech.sina.cn/digi/nb/2018-07-09/detail-ihezpzwt8158157.d.html" target="_blank">新老冰箱保鲜效果大比拼 结果让人大吃一惊a>li>
        //<li><a href="https://tech.sina.com.cn/i/2018-07-09/doc-ihezpzwt8148659.shtml
        //但没关系，到时候在GetHtml这个函数中就不会继续了
        for(auto item:links)
            v.push_back(make_pair(i.first,item));
    }
 
    return v;
}
 
//获取新闻信息
NewsInfo* GetNewsInfo(pair<string,string> item,filter ft){
    HttpCurl curl;
    string context;
    if(!GetHtml(curl,context,item.second)){
        cout << item.first << ":" << item.second << ":" << "link error";
        return NULL;
    }
 
    //取出新闻标题
    string title;
    //基本上的新闻的标题都是下面两个格式之一
    vector<string> reg = {"<h1 class=\"main-title\">.*?</h1>","<meta property=\"og:title\" content=\".*?\""};
    for(auto i : reg){
        ft.SetReg(i);
        title = ft.filterUrlString(context);
        if(title.empty())
            continue; 
        else{
            if(i == "<h1 class=\"main-title\">.*?</h1>")
                Replace(title);
            else{
                //这两个函数用于替换中文标题两边的所有内容为空
                boost::algorithm::replace_all(title,"<meta property=\"og:title\" content=\"","");
                boost::algorithm::replace_all(title,"\"","");
            }
            break;
        }
    }
    if(title.empty()){
        //认为没有标题，则放弃这个新闻，不要了
        cout << item.first << ":" << item.second << ":" << "no title" << endl;
        return NULL;
    }
    
    //取出新闻作者
    //基本上的新闻的作者都是下面两个格式之一
    reg = {"<p class=\"article-editor\">.*?</p>","article:author\" content=\".*?\""};
    string author;
    for(auto i : reg){
        ft.SetReg(i);
        author = ft.filterUrlString(context);
        if(author.empty())
            continue; 
        else{
            if(i == "<p class=\"article-editor\">.*?</p>")
                Replace(author);
            else{
                boost::algorithm::replace_all(author,"article:author\" content=\"","");
                boost::algorithm::replace_all(author,"\"","");
            }
            break;
        }
    }
    if(author.empty()){
        //认为没有作者，则放弃这个新闻，不要了
        cout << item.first << ":" << item.second << ":" << "no author" << endl;
        return NULL;
    }
 
    //取出新闻内容题
    //基本上的新闻的内容都是下面两个格式之一
    reg = {"<span class=\"img_descr\">.*?<div","<div class=\"article\".*?</div>"};
    string result;
    for(auto i : reg){
        ft.SetReg(i);
        result = ft.filterUrlString(context);
        if(result.empty())
            continue; 
        else
            break;
    }
    if(result.empty()){
        //认为没有内容，则放弃这个新闻，不要了
        cout << item.first << ":" << item.second << ":" << "no content" << endl;;
        return NULL;
    }
 
    //<p></p>包含着的部分就是新闻正文部分
    ft.SetReg("<p.*?</p>");
    vector<string> paragraphs = ft.filterUrlVector(result);
    if(paragraphs.empty()){
        //认为没有内容，则放弃这个新闻，不要了
        cout << item.first << ":" << item.second << ":" << "no content" << endl;;
        return NULL;
    }
 
    string content = "";
    for(size_t i = 0; i < paragraphs.size();++i){
        Replace(paragraphs[i]);
        content += paragraphs[i] + '\n';//在每一个段落末尾加上一个\n换行
    }
 
    NewsInfo* info = new NewsInfo(item.first,title,author,content);
    return info;
}
 
int main(){
    string context;
    HttpCurl curl;
    //找新浪是因为新浪的网页编码是utf8
    string url = "http://news.sina.com.cn/";
 
    if(!GetHtml(curl,context,url)){
        cout << "The link is error!" << endl;
        return 0;
    }
    
    //下面两个函数用于连接数据库
    //具体爬下来的数据怎么处置，由你自己定，只是我这里是放到数据库里罢了
    MYSQL* conn;
    DataBaseInit(conn);
 
    filter ft;
    map<string,string> topicLinks = GetTopicLinks(context,ft,conn);
 
    vector<pair<string,string>> newsLinks = GetNewsLinks(topicLinks,ft);
    int count = newsLinks.size();
    //这个count表示我们拿到的所有链接数
    cout << count  << endl;
    
    for(auto i:newsLinks){
        NewsInfo* info = GetNewsInfo(i,ft);
        if(info == NULL){
            //返回空表示给的链接是错误的
            count--;
            continue;
        }
        
        //将数据放到数据库中
        int topicid = OperateTopic::IdOfTopic(conn,i.first);
        OperateNews::AddNews(conn,topicid,info->_title,info->_author,info->_content);
    }
 
    //这个count表示我们实际获取到的匹配到的新闻数
    cout << count << endl;
 
    return 0;
}