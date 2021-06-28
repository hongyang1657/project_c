#include "fd_gpio.h"
#include <pthread.h>

#define SYSFS_GPIO_EXPORT "/sys/class/gpio/export"

#define SYSFS_GPIO_UNEXPORT "/sys/class/gpio/unexport"

#define SYSFS_GPIO_RST_DIR_VAL "out"

#define SYSFS_GPIO_RST_DIR_VAL_IN "in"

#define SYSFS_GPIO_RST_VAL_H "1"

using namespace std;

FdGPIO* FdGPIO::fdGPIO;
FdGPIO* FdGPIO::getInstance(){
    if (fdGPIO == NULL)
    {
        fdGPIO = new FdGPIO();
    }
    
    return fdGPIO;
}

FdGPIO::FdGPIO(){

}
FdGPIO::~FdGPIO(){

}



//gpio控制线程
void * FdGPIO::PthreadGpio(void *arg){

    int n,interval;
    string strParame,strPin,strPin1,strPin2;
    strParame = (string)(*((string*)arg));

    // if(strParame!=""){
        
    // }
    if ((n = strParame.find(",")) != std::string::npos)
    {
        strPin = strParame.substr(0,n).c_str();
        interval = atoi(strParame.substr(n+1).c_str());
        if ((n = strPin.find("/")) != std::string::npos)
        {
            strPin1 = strPin.substr(0,n).c_str();
            strPin2 = strPin.substr(n+1).c_str();
        }else
        {
            strPin1 = strPin;
            cout << "only one pin = "<< strPin1<<endl;
        }
        
        
    }

    
    int ret = control(strPin1,"1");
    control(strPin2,"1");

    sleep(interval);
    
    ret = control(strPin1,"0");
    control(strPin2,"0");
    pthread_exit(NULL);
}

int FdGPIO::gpioScriptTask(string parame)
{ 
    pthread_t pthread_id;
    //Create a thread
    if (parame=="")
    {   
        cout << "parame = null" <<endl;
        return -1;
    }
    
    int rc = pthread_create( &pthread_id, NULL, &PthreadGpio, &parame );
    if (rc)
    {
        return -1;
    }
    
    pthread_join(pthread_id,NULL);
    
    return 0;
}


int FdGPIO::control(string pinVal,string rstVal)
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

int FdGPIO::readGpioPort(string pinVal)
{
    int fd,ret;
    char value[2];

    //打开端口/sys/class/gpio# echo pinVal > export
    fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    if (fd == -1) {
        perror("ERR: Radio hard reset pin open error");
        return -1;
    }

    // std::cout << "pinVal: " << pinVal << std::endl;
    ret = write(fd, pinVal.c_str(), sizeof(pinVal.c_str()));
    close(fd);

    //设置端口方向/sys/class/gpio/gpiopinVal# echo in > direction
    string dir;
    dir = "/sys/class/gpio/gpio" + pinVal + "/direction";
    fd = open(dir.c_str(), O_RDWR);
    if (fd == -1) {
        perror("ERR: Radio hard reset pin direction open error");
        return -1;
    }

    ret = write(fd, SYSFS_GPIO_RST_DIR_VAL_IN, sizeof(SYSFS_GPIO_RST_DIR_VAL_IN));
    close(fd);

    //读取端口电平
    string val;
    val = "/sys/class/gpio/gpio" + pinVal + "/value";
    fd = open(val.c_str(), O_RDWR);
    if (fd == -1) {
        perror("ERR: Radio hard reset pin value open error");
        return -1;
    }

    ret = read(fd, value, sizeof(value));
    close(fd);

    int levelValue = atoi(value);
    // std::cout << "read value = " << levelValue << endl;

    return levelValue;

}
