#include <stdio.h>
#include <malloc.h>
#include <iostream>
#include <unistd.h>
#include "fd_log.h"
using namespace std;  


int main()
{
    int i;
    for (int i = 0; i < 100; i++)
    {
        sleep(1);
        printf("sleep %d\n",i);
        checkCleanLogFile(CACHE_LOG_PATH);
    }
    
    return 0;
}
