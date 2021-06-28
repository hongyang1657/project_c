#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>
#include <unistd.h>
#include <fcntl.h>   //define O_WRONLY and O_RDONLY  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <pthread.h>


#define SYSFS_GPIO_EXPORT "/sys/class/gpio/export"

#define SYSFS_GPIO_UNEXPORT "/sys/class/gpio/unexport"

#define SYSFS_GPIO_RST_DIR_VAL "out"

#define SYSFS_GPIO_RST_VAL_H "1"

using namespace std;




int control(string pinVal,string rstVal)
{
    int fd,ret;
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

    //设置端口方向/sys/class/gpio/gpio96# echo out > direction
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
    
    ret = write(fd, rstVal.c_str(), sizeof(rstVal.c_str()));
    // printf("ERR3 = %d\n",ret);
    close(fd);

    //关闭端口 /sys/class/gpio/unexport
    // fd = open(SYSFS_GPIO_UNEXPORT, O_WRONLY );
    // if (fd == -1)
    // {
    //     printf("ERR: Radio hard reset pin open error.\n");
    //     return EXIT_FAILURE;
    // }

    // //printf("pinVal.c_str = %s\n",pinVal.c_str());
    // ret = write(fd, pinVal.c_str(), sizeof(pinVal.c_str()));
    // //printf("ERR4 = %d\n",ret);
    // close(fd);
    return 0;
}

int flag = 0;
int main(int argc, char* argv[]) 
{ 
  // while (true){
  //   if (flag==0)
  //   {
  //     control("98","1");
  //     printf("val = 1\n");
  //     sleep(3);
  //     flag = 1;
  //   }else if (flag==1)
  //   {
  //     control("98","0");
  //     printf("val = 0\n");
  //     sleep(3);
  //     flag = 0;
  //   }
    
  // }

  control("98",argv[1]);

  return 0;
}  
