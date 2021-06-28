#include "serialManager.h"
#include <cmath>
#include <signal.h>
#include <math.h>
#include "fd_log.h"
#include <math.h>

using namespace std;

const char *Serial_Dev1 = "/dev/ttyS0";
int serialRecBuff[REC_BUFF_LEN];

struct Motor_Data
{
    std::map<int, int> motor_position;
    std::map<int, int> motor_status;
} motor_data;


struct Motor_Config
{
    std::map<int, int> is_control;
    std::map<int, std::string> control_type;
    std::map<int, int> data1;
    std::map<int, int> data2;
};

struct Serial_IO_Config
{
    std::map<int, std::string> command_type;
    std::map<int, int> offset;
    std::map<int, double> multiple;
    std::map<int, int> motor_num;
    std::map<int, Motor_Config> motor;
} serial_io_config;

namespace serial_space
{
int ret;
int S_fd;
int n = 0;
struct termios oldstdio;
int fd;
char Rx_Data[REC_BUFF_LEN];
} // namespace serial_space
using namespace serial_space;


SerialManager *SerialManager::serialmanager;
SerialManager *SerialManager::getInstance()
{
    if (serialmanager == NULL)
    {
        serialmanager = new SerialManager();
        int error = serialmanager->initManager();
        printf("initManager error:%d\n", error);
        serialmanager->createSerialThread();
    }
    return serialmanager;
}

SerialManager::SerialManager()
{
}
SerialManager::~SerialManager()
{
}

int SerialManager::initManager()
{
    ret = loadIntentConfigJson("/data/found/model/serial_IO_config.json");
    if (ret != 0)
    {
        return -1;
    }


    //打开串口
    S_fd = open(Serial_Dev1, O_RDWR | O_NOCTTY); //O_NONBLOCK O_NDELAY 非阻塞
    if (-1 == S_fd)
        return -1;
    ret = set_opt(S_fd, 115200, 8, 'N', 1);
    if (ret == -1)
    {
        return -1;
    }
    FD_LOG("serial_IO_config加载完成");
    return 0;
}

//执行gpio脚本
int SerialManager::executeGpio(int intent)
{
    //ret = gpioManager->gpioScriptTask(serial_io_config.index2GpioMap[intent]);
    return ret;
}

//加载json配置文件
// int SerialManager::loadIntentConfigJson(std::string path){
//     std::cout<<"loadIntentConfigJson... "<<std::endl;
//     std::ifstream intent_file(path);
//     std::string line;
//     std::string intent_data = "";
//     // 有该文件
//     if(intent_file) {
//         while (getline (intent_file, line)) {
//             intent_data += line;
//         }
//     } else {
//         // 没有该文件
//         std::cout <<"<no such file path = >" << path << std::endl;
//         return -1;
//     }
//     intent_file.close();

//     cJSON *intent_json = cJSON_Parse(intent_data.c_str());

//     std::cout << "size = "<<cJSON_GetArraySize(intent_json)<< std::endl;
//     for(int i=0; i<cJSON_GetArraySize(intent_json); i++)   //遍历最外层json键值对
//     {
//         cJSON * item = cJSON_GetArrayItem(intent_json, i);
//         std::string key = item->string;

//         cJSON *config = cJSON_GetObjectItem(intent_json,item->string);

//         cJSON *control_type = cJSON_GetObjectItem(config,"control_type");
//         cJSON *offset = cJSON_GetObjectItem(config,"offset");
//         cJSON *serial = cJSON_GetObjectItem(config,"serial");
//         cJSON *multiple = cJSON_GetObjectItem(config,"multiple");
//         //cJSON *gpio = cJSON_GetObjectItem(config,"gpio");

//         if (control_type!=NULL&&control_type->type!=cJSON_NULL)
//         {
//             serial_io_config.serial_control_type[atoi(key.c_str())] = control_type->valuestring;
//         }
//         if (offset!=NULL&&offset->type!=cJSON_NULL)
//         {
//             serial_io_config.serial_offset[atoi(key.c_str())] = offset->valueint;
//         }
//         if (serial!=NULL&&serial->type!=cJSON_NULL)
//         {
//             serial_io_config.index2SerialMap[atoi(key.c_str())] = serial->valuestring;
//         }
//         if (multiple!=NULL&&multiple->type!=cJSON_NULL)
//         {
//             serial_io_config.serial_multiple[atoi(key.c_str())] = multiple->valuestring;
//         }

//         //index2GpioMap[atoi(key.c_str())] = gpio->valuestring;

//     }

//     cJSON_Delete(intent_json);
//     std::cout<<"loadIntentConfigJson success "<<std::endl;
//     return 0;
// }

//加载json配置文件
int SerialManager::loadIntentConfigJson(std::string path)
{
    std::cout << "loadIntentConfigJson... " << std::endl;

    std::ifstream intent_file(path);
    std::string line;
    std::string intent_data = "";

    // 有该文件
    if (intent_file)
    {
        while (getline(intent_file, line))
        {
            intent_data += line;
        }
    }
    else
    {
        // 没有该文件
        std::cout << "<no such file path = >" << path << std::endl;
        return -1;
    }
    intent_file.close();

    cJSON *intent_json = cJSON_Parse(intent_data.c_str());

    std::cout << "size = " << cJSON_GetArraySize(intent_json) << std::endl;
    for (int i = 0; i < cJSON_GetArraySize(intent_json); i++) //遍历最外层json键值对
    {
        //cout << "################i = " << i << endl;
        cJSON *item = cJSON_GetArrayItem(intent_json, i);
        std::string key = item->string;

        cJSON *config = cJSON_GetObjectItem(intent_json, item->string);

        cJSON *command_type = cJSON_GetObjectItem(config, "command_type");
        cJSON *multiple = cJSON_GetObjectItem(config, "multiple");
        cJSON *offset = cJSON_GetObjectItem(config, "offset");
        cJSON *motor_num = cJSON_GetObjectItem(config, "motor_num");
        cJSON *motor = cJSON_GetObjectItem(config, "motor");

        if (motor != NULL && motor->type != cJSON_NULL)
        {
            cJSON *motor_item = motor->child;
            Motor_Config motor_config;
            for (int j = 0; j < cJSON_GetArraySize(motor); j++)
            {
                //cout << "################j = " << j << endl;
                cJSON *isControl = cJSON_GetObjectItem(motor_item, "isControl");
                cJSON *control_type = cJSON_GetObjectItem(motor_item, "control_type");
                cJSON *data1 = cJSON_GetObjectItem(motor_item, "data1");
                cJSON *data2 = cJSON_GetObjectItem(motor_item, "data2");

                if (isControl != NULL && isControl->type != cJSON_NULL)
                {
                    //cout << "########isControl######## " << endl;
                    motor_config.is_control[j] = isControl->valueint;
                }
                if (control_type != NULL && control_type->type != cJSON_NULL)
                {
                    //cout << "########control_type######## " << endl;
                    motor_config.control_type[j] = control_type->valuestring;
                }
                if (data1 != NULL && data1->type != cJSON_NULL)
                {
                    //cout << "########data1######## " << endl;
                    motor_config.data1[j] = data1->valueint;
                }
                if (data2 != NULL && data2->type != cJSON_NULL)
                {
                    //cout << "########data2######## " << endl;
                    motor_config.data2[j] = data2->valueint;
                }
                motor_item = motor_item->next;
            }
            serial_io_config.motor[atoi(key.c_str())] = motor_config;
        }

        if (command_type != NULL && command_type->type != cJSON_NULL)
        {
            //cout << "########command_type######## " << endl;
            serial_io_config.command_type[atoi(key.c_str())] = command_type->valuestring;
        }
        if (offset != NULL && offset->type != cJSON_NULL)
        {
            //cout << "########offset######## " << endl;
            serial_io_config.offset[atoi(key.c_str())] = offset->valueint;
        }
        if (multiple != NULL && multiple->type != cJSON_NULL)
        {
            //cout << "########multiple######## " << endl;
            serial_io_config.multiple[atoi(key.c_str())] = multiple->valuedouble;
        }
        if (motor_num != NULL && motor_num->type != cJSON_NULL)
        {
            //cout << "########motor_num######## " << endl;
            serial_io_config.motor_num[atoi(key.c_str())] = motor_num->valueint;
        }
    }

    cJSON_Delete(intent_json);
    std::cout << "loadIntentConfigJson success " << std::endl;
    return 0;
}

//---------------------------------
void *SerialManager::Pthread_Serial(void *arg)
{

    while (1)
    {
        ret = read(S_fd, Rx_Data, (size_t)REC_BUFF_LEN);
        if (ret > 0)
        {
            printf("read ret = %d , Rx_data = %s\n", ret, Rx_Data);
            char2Int(Rx_Data); //char[] to int[]
        }
    }
    std::cout << "----------------serial thread exit----------------------" << std::endl;
    pthread_exit(NULL);
}

//通过process方法轮询获取串口消息
void SerialManager::getSerialInput()
{
    ret = read(S_fd, Rx_Data, (size_t)REC_BUFF_LEN);
    if (ret > 0)
    {
        printf("read ret = %d , Rx_data = %s\n", ret, Rx_Data);
        char2Int(Rx_Data); //char[] to int[]
    }
    return;
}

void SerialManager::char2Int(char recbuf[REC_BUFF_LEN])
{
    for (int i = 0; i < REC_BUFF_LEN; i++)
    {
        serialRecBuff[i] = (unsigned int)(unsigned char)recbuf[i];
    }
    recSerialIntent();
    return;
}

//heartbeat
int SerialManager::sendHeartbeat()
{

    size_t w_len = 5;

    char w_buf[5];

    w_buf[0] = 0xFA;
    w_buf[1] = 0xFD;
    w_buf[2] = 0x05;
    w_buf[3] = 0x05;
    w_buf[4] = 0xFB;
    //printf("ayah serial length = %d\n",sizeof(w_buf));
    return write(S_fd, w_buf, w_len);
}

//爱芽标准串口协议
int SerialManager::sendSerialByAYAHAgreement(int intent)
{
    size_t w_len = 8;
    char w_buf[8];
    int s_type, s_data, n;
    std::string str_serial;
    int s1, s2;

    //str_serial = serial_io_config.index2SerialMap[intent];
    if (str_serial == "")
    {
        return -1;
    }
    if (str_serial.length() == 4)
    {
        s1 = hex2dec(str_serial.substr(2).c_str());
        s2 = hex2dec(str_serial.substr(0, 2).c_str());
    }
    else
    {
        return -1;
    }

    w_buf[0] = 0xA5;
    w_buf[1] = 0xFA;
    w_buf[2] = 0x00;
    w_buf[3] = 0x81;
    w_buf[4] = s2;
    w_buf[5] = s1;
    w_buf[6] = (s1 + s2 + 129) % 256; //sum
    w_buf[7] = 0xFB;
    return write(S_fd, w_buf, w_len);
}

//亿微按摩椅串口协议
int SerialManager::sendSerialByYiweiAgreement(int intent)
{
    size_t w_len = 6;
    char w_buf[6];
    int s_type, s_data, n;
    std::string str_serial;
    int s1, s2;

    //str_serial = serial_io_config.index2SerialMap[intent];
    if (str_serial == "")
    {
        return -1;
    }
    if (str_serial.length() == 4)
    {
        s1 = hex2dec(str_serial.substr(2).c_str());
        s2 = hex2dec(str_serial.substr(0, 2).c_str());
    }
    else
    {
        return -1;
    }

    w_buf[0] = 0x59;
    w_buf[1] = 0x59;
    w_buf[2] = 0x06;
    w_buf[3] = s2;
    w_buf[4] = s1;
    w_buf[5] = (s1 + s2 + 89 + 89 + 6) % 256; //sum
    return write(S_fd, w_buf, w_len);
}

// //智源电机控制板协议
// //head1 head2 length source channel data1 data2… datan checksum end
// int SerialManager::sendSerialByZhiyuanAgreement(int intent,int slot){
//     int offset = serial_io_config.serial_offset[intent];
//     vector<int> v_data;
//     vector<int>::iterator it;
//     std::string str_serial,byte_data;
//     int length,j=0,k=0,dec_data,data_sum=0,check_sum,position;

//     str_serial = serial_io_config.index2SerialMap[intent];
//     if(str_serial==""){
//         return -1;
//     }
//     length = 5 + str_serial.length()/2;

//     v_data.push_back(0xFA);
//     v_data.push_back(0xFD);
//     v_data.push_back(length);

//     for(int i=0;i<str_serial.length()/2;i++){

// 	    byte_data = str_serial.substr(j,2);
// 	    //std::cout<< byte_data <<std::endl;

//         if ("abs"!=serial_io_config.serial_control_type[intent])    //移动到非绝对位置
//         {
//             if (i==2)
//             {
//                 position = motor_data.motor_1_position;
//                 std::cout<<"i=2,position="<<position<<std::endl;
//                 dec_data = judgeAggrement(intent,slot,position,offset);
//             }else if (i==4)
//             {
//                 position = motor_data.motor_2_position;
//                 dec_data = judgeAggrement(intent,slot,position,offset);
//             }else
//             {
//                 dec_data = hex2dec(byte_data);
//             }

//         }else
//         {
//             dec_data = hex2dec(byte_data);
//         }

//         v_data.push_back(dec_data);
//         cout << "dec_data = "<< dec_data <<endl;
//         data_sum = data_sum+dec_data;
//         j = j+2;
//     }

//     check_sum = (length+data_sum)%256;

//     v_data.push_back(check_sum);     //checksum
//     v_data.push_back(0xFB);

//     char w_buf[v_data.size()];

//     std::cout<<"send_serial_command:";
//     for(it=v_data.begin();it!=v_data.end();it++){
//         w_buf[k] = *it;
//         std::cout << (int)w_buf[k]<< " ";
//         k++;
//     }
//     std::cout<<std::endl;

//     std::cout<<"motor_data = "<<motor_data.motor_1_position<<","
//                 <<motor_data.motor_1_status<<","
//                 <<motor_data.motor_2_position<<","
//                 <<motor_data.motor_2_status<<std::endl;

//     return write(S_fd,w_buf,v_data.size());
// }

//智源电机控制板协议
//head1 head2 length source channel data1 data2… datan checksum end
int SerialManager::sendSerialByZhiyuanAgreement(int intent, int slot)
{   
    if (serial_io_config.motor_num[intent]==NULL||serial_io_config.motor_num[intent]<1)
    {
        std::cout<<"没有对应的串口指令"<<std::endl;
        return -1;
    }
    
    int offset = serial_io_config.offset[intent];
    vector<int> v_data;
    vector<int>::iterator it;
    int length, source, channel = 0, k = 0, check_sum = 0;
    length = serial_io_config.motor_num[intent] * 2;
    length = 7 + length;

    v_data.push_back(0xFA);
    v_data.push_back(0xFD);
    v_data.push_back(length);

    if (serial_io_config.command_type[intent] == "record")
    {
        source = 0x40;
        v_data.push_back(source);
    }
    else
    {
        source = 0x80;
        v_data.push_back(source);
    }

    for (int i = 0; i < serial_io_config.motor_num[intent]; i++)
    {
        if (serial_io_config.motor[intent].is_control[i])
        {
            channel = channel + pow(2, i);
        }
    }
    v_data.push_back(channel);
    for (int i = 0; i < serial_io_config.motor_num[intent]; i++)
    {
        
        int data1 = judgeAggrement(i, intent, slot, motor_data.motor_position[i], offset);
        if (data1<0)
        {
            return -1;
        }
        
        int data2 = serial_io_config.motor[intent].data2[i];
        v_data.push_back(data1);
        v_data.push_back(data2);
        check_sum = check_sum + data1 + data2;
    }

    check_sum = (length + source + channel + check_sum) % 256;
    v_data.push_back(check_sum); //checksum
    v_data.push_back(0xFB);

    char w_buf[v_data.size()];

    std::cout << "send_serial_command:";
    for (it = v_data.begin(); it != v_data.end(); it++)
    {
        w_buf[k] = *it;
        std::cout << (int)w_buf[k] << " ";
        k++;
    }
    std::cout << std::endl;

    return write(S_fd, w_buf, v_data.size());
}

int SerialManager::serial_send(const char *data)
{
    int string_num;
    string_num = strlen(data);
    printf("string_num = %d\n", string_num);
    return write(S_fd, data, string_num);
}

//receiver 回调函数，回调接收到的串口指令  智源
void SerialManager::recSerialIntent()
{
    for (int i = 0; i < REC_BUFF_LEN; i++)
    {
        printf("serialRecBuff[%d] = %d\n", i, serialRecBuff[i]);
        if (serialRecBuff[i] == 0xFA && serialRecBuff[i + 1] == 0xFD)
        {
            int length = serialRecBuff[i + 2];
            if (serialRecBuff[i + length - 1] == 0xFB)
            {
                int checksum = 0;
                for (int j = i; j < i + length; j++)
                {

                    std::cout << serialRecBuff[j] << " ";
                    checksum = checksum + serialRecBuff[j];
                }
                std::cout << std::endl;
                //检查加和校验
                checksum = checksum - 0xFA - 0xFD - serialRecBuff[i + length - 2] - 0xFB;

                if (checksum % 255 == serialRecBuff[i + length - 2])
                {
                    std::cout << "checksum success" << std::endl;
                }

                for (int k = 0; k < (length - 7) / 2; k++)
                {
                    motor_data.motor_position[k] = serialRecBuff[i + 5 + (2 * k)];
                    motor_data.motor_status[k] = serialRecBuff[i + 6 + (2 * k)];
                }
            }
            memset(serialRecBuff, 0, sizeof(serialRecBuff));
            break;
        }
        else
        {
        }
    }
}

//采用轮询的方式获取串口接收到的数据
// int SerialManager::getRecSerialIntent()
// {

//     for (int i = 0; i < REC_BUFF_LEN; i++)
//     {
//         //printf("serialRecBuff[%d] = %d\n",i,serialRecBuff[i]);
//         if (serialRecBuff[i] == 0xA5 && serialRecBuff[i + 1] == 0xFA && serialRecBuff[i + 2] == 0 && serialRecBuff[i + 7] == 0xFB)
//         {
//             int intent = serialRecBuff[i + 4];
//             memset(serialRecBuff, 0, sizeof(serialRecBuff));
//             return intent;
//         }
//     }

//     return -1;
// }

int SerialManager::createSerialThread()
{
    pthread_t pthread_id;
    //Create a thread
    pthread_create(&pthread_id, NULL, &Pthread_Serial, NULL);
    usleep(1000);

    if (-1 == S_fd)
    {
        printf("error: cannot open serial dev\r\n");
        return -1;
    }
    return 0;
}

int SerialManager::set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio, oldtio;
    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("error:SetupSerial 3\n");
        return -1;
    }
    bzero(&newtio, sizeof(newtio));
    //使能串口接收
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;
    newtio.c_lflag &= ~ICANON; //原始模式

    //设置串口数据位
    switch (nBits)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }
    //设置奇偶校验位
    switch (nEvent)
    {
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB;
        break;
    }
    //设置串口波特率
    switch (nSpeed)
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 38400:
        cfsetispeed(&newtio, B38400);
        cfsetospeed(&newtio, B38400);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 460800:
        cfsetispeed(&newtio, B460800);
        cfsetospeed(&newtio, B460800);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    //设置停止位
    if (nStop == 1)
        newtio.c_cflag &= ~CSTOPB;
    else if (nStop == 2)
        newtio.c_cflag |= CSTOPB;
    newtio.c_cc[VTIME] = 1;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd, TCIFLUSH);

    if (tcsetattr(fd, TCSANOW, &newtio) != 0)
    {
        perror("com set error\n");
        return -1;
    }
    return 0;
}

int SerialManager::hex2dec(std::string s)
{
    int count = s.length();
    int sum = 0;
    for (int i = count - 1; i >= 0; i--) //从十六进制个位开始，每位都转换成十进制
    {
        if (s[i] >= '0' && s[i] <= '9') //数字字符的转换
        {
            sum += (s[i] - 48) * pow(16, count - i - 1);
        }
        else if (s[i] >= 'A' && s[i] <= 'F') //字母字符的转换
        {
            sum += (s[i] - 55) * pow(16, count - i - 1);
        }
    }
    return sum;
}

int cache_intent,cache_slot;
int SerialManager::judgeAggrement(int i, int intent, int slot, int position, int offset)
{
    int dec_data;
    int slot_offset = slot * serial_io_config.multiple[intent];
    if ("up" == serial_io_config.motor[intent].control_type[i])
    {
        cache_intent = intent;
        cache_slot = slot;
        if (slot != 0)
        {

            dec_data = (position + slot_offset) < 255 ? (position + slot_offset) : 255;
            std::cout << "position+slot*multi = " << dec_data << std::endl;
        }
        else
        {
            dec_data = (position + offset) < 255 ? (position + offset) : 255;
        }
    }
    else if ("down" == serial_io_config.motor[intent].control_type[i])
    {
        cache_intent = intent;
        cache_slot = slot;
        if (slot != 0)
        {

            dec_data = (position - slot_offset) > 0 ? (position - slot_offset) : 0;
            std::cout << "position-slot*multi = " << dec_data << std::endl;
        }
        else
        {
            dec_data = (position - offset) > 0 ? (position - offset) : 0;
        }
    }
    //暂停
    else if ("suspend" == serial_io_config.motor[intent].control_type[i])
    {   
        printf("暂停");
        dec_data = position;
    }
    //继续
    else if ("continue" == serial_io_config.motor[intent].control_type[i])
    {
        printf("继续:cache_i = %d,cache_s = %d",cache_intent,cache_slot);
        sendSerialByZhiyuanAgreement(cache_intent,cache_slot);
        return -1;
    }
    else   //abs
    {
        cache_intent = intent;
        cache_slot = slot;
        if (slot!=0)
        {
            dec_data = (slot_offset) < 255 ? (slot_offset) : 255;
        }else
        {
            dec_data = serial_io_config.motor[intent].data1[i];
        }
        
    }
    return dec_data;
}
