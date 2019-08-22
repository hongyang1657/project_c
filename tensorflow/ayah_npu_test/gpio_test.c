#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

 
#define SYSFS_GPIO_EXPORT           "/sys/class/gpio/export"  
 
#define SYSFS_GPIO_RST_PIN_VAL      "96"   
 
#define SYSFS_GPIO_RST_DIR          "/sys/class/gpio/gpio96/direction"
 
#define SYSFS_GPIO_RST_DIR_VAL      "OUT"  
 
#define SYSFS_GPIO_RST_VAL          "/sys/class/gpio/gpio96/value"
 
#define SYSFS_GPIO_RST_VAL_H        "1"
 
#define SYSFS_GPIO_RST_VAL_L        "0"



int main() 
 
{ 
 
    int fd; 
 
        
 
         //打开端口/sys/class/gpio# echo 96 > export
 
         fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
 
         if(fd == -1)
 
         {
 
                   printf("ERR: Radio hard reset pin open error.\n");
 
                   return EXIT_FAILURE;
 
         }
 
         write(fd, SYSFS_GPIO_RST_PIN_VAL ,sizeof(SYSFS_GPIO_RST_PIN_VAL)); 
 
         close(fd); 
 
 
 
         //设置端口方向/sys/class/gpio/gpio96# echo out > direction
 
         fd = open(SYSFS_GPIO_RST_DIR, O_WRONLY);
 
         if(fd == -1)
 
         {
 
                   printf("ERR: Radio hard reset pin direction open error.\n");
 
                   return EXIT_FAILURE;
 
         }
 
         write(fd, SYSFS_GPIO_RST_DIR_VAL, sizeof(SYSFS_GPIO_RST_DIR_VAL)); 
 
         close(fd); 
 
 
 
         //输出复位信号: 拉高>100ns
 
         fd = open(SYSFS_GPIO_RST_VAL, O_RDWR);
 
         if(fd == -1)
 
         {
 
                   printf("ERR: Radio hard reset pin value open error.\n");
 
                   return EXIT_FAILURE;
 
         }       
 
         while(1)
 
         {
 
                   write(fd, SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
 
                   usleep(1000000);
 
                   write(fd, SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
 
                   usleep(1000000);
 
         }
 
         close(fd);
 
 
 
         printf("INFO: Radio hard reset pin value open error.\n");
 
         return 0;
 
 
 
}