
#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <exception>
using namespace std;

#define ALL_LOG_PATH "/data/found/log/log.txt"

double getFileSize()
{
    double filesize = -1;
    struct stat fileInfo;
    if (stat(ALL_LOG_PATH, &fileInfo) < 0)
    {
        cout << filesize << endl;
        return -1;
    }
    else
    {
        filesize = fileInfo.st_size;
        cout << filesize << endl;
        return filesize;
    }
}

int main()
{

    if (getFileSize() < 500 * 1024)
    {
        cout << "文件小于500k" << endl;
    }
    else
    {
        cout << "文件大于500k" << endl;
        string fstr("\n----------------clean------------------\n");
        try
        {
            fstream fout(ALL_LOG_PATH, ios::out | ios::trunc);
            fout << fstr << endl;
            fout.close();
        }
        catch (exception &e)
        {
            cout << "\n--------error happened---------\n" << e.what() << endl;
        }
    }

    return 0;
}