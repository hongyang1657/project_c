#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>

#define SYSFS_GPIO_EXPORT "/sys/class/gpio/export"

#define SYSFS_GPIO_UNEXPORT "/sys/class/gpio/unexport"

#define SYSFS_GPIO_RST_DIR_VAL "in"

#define SYSFS_GPIO_RST_VAL_H "1"

using namespace std;

const int door_signal_model = 0;   //为0表示常态为低电平;1表示常态为高电平
int signal_flag = 0;

int readGpioPort(string pinVal);

//gpio控制线程
void * Pthread_gpio(void *arg){

    while (true)
    {
        int ret = readGpioPort(string("66"));
        sleep(1);
        
    }
    
    
    pthread_exit(NULL);
}

// int writeGpio(string pinVal){
//     int fd,ret;
//     char value[2];
//     //打开端口/sys/class/gpio# echo 96 > export
//     fd = open(SYSFS_GPIO_EXPORT, O_WRONLY  );
//     if (fd == -1)
//     {
//         printf("ERR: Radio hard reset pin open error.\n");
//         return EXIT_FAILURE;
//     }

//     // printf("pinVal.c_str = %s\n",pinVal.c_str());
//     ret = write(fd, pinVal.c_str(), sizeof(pinVal.c_str()));
//     // printf("ERR1 = %d\n",ret);
//     close(fd);

//     //设置端口方向/sys/class/gpio/gpio96# echo in > direction
//     string dir;
//     dir = "/sys/class/gpio/gpio" + pinVal + "/direction";
//     // printf("dir.c_str = %s\n",dir.c_str());
//     fd = open(dir.c_str(), O_RDWR );
//     if (fd == -1)
//     {
//         printf("ERR: Radio hard reset pin direction open error.\n");
//         return EXIT_FAILURE;
//     }

//     ret = write(fd, SYSFS_GPIO_RST_DIR_VAL, sizeof(SYSFS_GPIO_RST_DIR_VAL));
//     // printf("ERR2 = %d\n",ret);
//     close(fd);


//     string val;
//     val = "/sys/class/gpio/gpio"+pinVal+"/value";
//     // printf("val.c_str = %s\n",val.c_str());
//     fd = open(val.c_str(), O_RDWR);

//     if (fd == -1)
//     {
//         printf("ERR: Radio hard reset pin value open error.\n");
//         return EXIT_FAILURE;
//     }
//     string rstVal = to_string(door_signal_model);

//     ret = write(fd, rstVal.c_str(), sizeof(rstVal.c_str()));
//     // printf("ERR3 = %d\n",ret);
//     close(fd);
// }

//return 1001表示开门； 1000表示关门
int readGpioPort(string pinVal)
{
    int fd,ret;
    char value[2];
    //打开端口/sys/class/gpio# echo 96 > export
    fd = open(SYSFS_GPIO_EXPORT, O_WRONLY  );
    if (fd == -1)
    {
        printf("ERR: Radio hard reset pin open error.\n");
        return EXIT_FAILURE;
    }

    // printf("pinVal.c_str = %s\n",pinVal.c_str());
    ret = write(fd, pinVal.c_str(), sizeof(pinVal.c_str()));
    // printf("ERR1 = %d\n",ret);
    close(fd);

    //设置端口方向/sys/class/gpio/gpio96# echo in > direction
    string dir;
    dir = "/sys/class/gpio/gpio" + pinVal + "/direction";
    // printf("dir.c_str = %s\n",dir.c_str());
    fd = open(dir.c_str(), O_RDWR );
    if (fd == -1)
    {
        printf("ERR: Radio hard reset pin direction open error.\n");
        return EXIT_FAILURE;
    }

    ret = write(fd, SYSFS_GPIO_RST_DIR_VAL, sizeof(SYSFS_GPIO_RST_DIR_VAL));
    // printf("ERR2 = %d\n",ret);
    close(fd);


    string val;
    val = "/sys/class/gpio/gpio"+pinVal+"/value";
    // printf("val.c_str = %s\n",val.c_str());
    fd = open(val.c_str(), O_RDWR);

    if (fd == -1)
    {
        printf("ERR: Radio hard reset pin value open error.\n");
        return EXIT_FAILURE;
    }
    
    ret = read(fd,value,sizeof(value));
    
    int levelValue = atoi(value);
    printf("read value = %d\n",levelValue);

    if (levelValue!=door_signal_model&&signal_flag==1)
    {
        printf("收到开门信号\n");
        signal_flag = 0;
        //TODO 电平恢复
        close(fd);
        return 1001;
    }else if(levelValue==door_signal_model&&signal_flag==0)
    {
        printf("收到关门信号\n");
        signal_flag = 1;
        //TODO 电平恢复
        close(fd);
        return 1000;
    }

    close(fd);
    return 0;
}

int main(){
    //开启线程
    pthread_t pthread_id;
    int rc = pthread_create( &pthread_id, NULL, &Pthread_gpio, NULL );
    if (rc)
    {
        return -1;
    }
    pthread_join(pthread_id,NULL);
    cin.get();
    return 0;
}