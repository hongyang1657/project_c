#include <stdio.h>
#include <iostream>
//2. sink方式

class ISerialRecListener
{
public:  
    virtual void OnSerialReceiver(const int intent) = 0;
};

class CMyDownloader
{
public:  
    CMyDownloader (ISerialRecListener *pSink)
        :m_pSink(pSink)
    {

    }

    void DownloadFile(const int intent)
    {
        std::cout<<"downloading..."<<intent<<""<<std::endl;
        if(m_pSink!=NULL)
        {
            m_pSink->OnSerialReceiver(intent);
        }
    }

private:
    ISerialRecListener *m_pSink;
};


class CMyFile:public ISerialRecListener
{
public:  
    void download(int intent)
    {
        CMyDownloader downloader(this);
        downloader.DownloadFile(intent);
    }

    virtual void OnSerialReceiver(const int intent) 
    {
        std::cout<<"OnSerialReceiver..."<<intent<<"   status:"<<std::endl;
    }
};

int main()
{
    CMyFile *file = new CMyFile();
    file->download(1);


    return 0;
}