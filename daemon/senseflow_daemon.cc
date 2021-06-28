#include <stdio.h>                                                                                                                      
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void MyDaemon()
{
    //7.屏蔽umask
    umask(0);

    //1.创建一个子进程
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork");
    }
    else if(pid > 0)
    {
        //2.让父进程退出
        exit(0);
    }
    //3.子进程调用setsid，创建一个新的会话，
    pid_t ret = setsid();
    if(ret < 0)
    {
        perror("setsid");
    }
    //4.忽略掉SIGCHLD信号，忽略SIGHUP信号
    signal(SIGCHLD,SIG_IGN);
    signal(SIGHUP,SIG_IGN);

    //5.修改工作目录为“/”  
    if(chdir("/") < 0)
    {
        perror("chdir");
    }

    //6.重定向文件描述符
    int fd = open("/dev/null",O_RDWR);                                                                                                  
    dup2(fd,0);
    dup2(fd,1);

}

int main()
{
    MyDaemon();   //调用该函数之后就会使得main进程变为守护进程
    while(1)
    {
        sleep(1);
    }
    return 0;
}
