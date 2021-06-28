#include "fd_gpio.h"
#include "fd_protocol.h"
#include "fd_config.h"
#include "Tool.h"
#include "fd_log.h"

void revSerial(char * data,int length);

int main(){
    int serialLength = -1;

    FdConfig::_GetInstance();
    FdGPIO::getInstance();
    fd_protocol::_GetInstance()->setDataRecvListener(revSerial);
    serialLength = FdConfig::_GetInstance()->getConfigInt("serialConfig", "serialLength");
    if (serialLength)
    {
        fd_protocol::_GetInstance()->init_protocol(serialLength, 9600, 8, 'N', 1);
    }else
    {
        std::cout << "请配置serialConfig参数" << std::endl;
    }
    fd_protocol::_GetInstance()->serial_send("abcdefg");
    

    pthread_exit(NULL);
    return 0;
}

//
void revSerial(char *data,int length){
    std::string recv_serial;
    int pin,value;
    for (size_t i = 0; i < length; i++)
    {
        //cout << (int)data[i] << endl;
        recv_serial = recv_serial + decimalToHex((int)data[i]);
    }
    cout << "res:" <<recv_serial<< std::endl;
    pin = FdConfig::_GetInstance()->pin_map[recv_serial];
    value = FdConfig::_GetInstance()->value_map[recv_serial];
    if (pin)
    {
        FdGPIO::getInstance()->control(to_string(pin),to_string(value));
    }
    
}