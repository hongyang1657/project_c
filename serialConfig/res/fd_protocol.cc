#include "fd_protocol.h"
#include "fd_log.h"
#include "Tool.h"
#include "fd_config.h"

using namespace std;

fd_protocol::fd_protocol()
{
}

fd_protocol::~fd_protocol()
{
    if (S_fd > 0)
        close(S_fd);
}

int fd_protocol::init_protocol(int recv_len, int nSpeed, int nBits, char nEvent, int nStop)
{
    //设置串口读取数据长度
    RecvBuffLen = recv_len;

    //打开串口
    S_fd = open(FD_PROTOCOL_SERIAL_DEV1, O_RDWR | O_NOCTTY); //O_NONBLOCK O_NDELAY 非阻塞
    if (-1 == S_fd)
    {
        perror("!!!open serial failed!!! ");
        return -1;
    }

    ret = set_opt(S_fd, nSpeed, nBits, nEvent, nStop);
    if (ret == -1)
    {
        perror("!!!set serial failed!!! ");
        return -1;
    }

    createSerialThread();

    return 0;
}
int fd_protocol::set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
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

int fd_protocol::serial_send(const char *data)
{
    int string_num;
    string_num = strlen(data);
    FD_LOG_L("string_num = " + to_string(string_num));
    return write(S_fd, data, string_num);
}

int fd_protocol::createSerialThread()
{
    pthread_t pthread_id;
    //Create a thread
    pthread_create(&pthread_id, NULL, &Pthread_Serial, this);
    usleep(1000);

    return 0;
}

void fd_protocol::setDataRecvListener(void (*recvData)(char*,int))
{
    recvSerial = recvData;
}

void *fd_protocol::Pthread_Serial(void *__this)
{
    fd_protocol *_this = (fd_protocol *)__this;

    // /*由于系统时间容易出错，所以需要在开始时同步初始时间*/
    // gettimeofday(&_this->tv1, NULL);
    // _this->pollingSencond = _this->tv1.tv_sec * 1000 + _this->tv1.tv_usec / 1000;
    // _this->pollingTimes = _this->pollingSencond;

    while (1)
    {

        char Rx_Data[_this->RecvBuffLen];
        _this->ret = read(_this->S_fd, Rx_Data, (size_t)_this->RecvBuffLen);
        if (_this->ret > 0)
        {
            if (_this->recvSerial!=NULL)
            {
                _this->recvSerial(Rx_Data,_this->RecvBuffLen);
            }
            
        }
        usleep(10 * 1000);
    }
    std::cout << "----------------serial thread exit----------------------" << std::endl;
    pthread_exit(NULL);
}