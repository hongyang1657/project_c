// system header files
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <sys/file.h>
#include <fcntl.h> 
#include <errno.h> 
#include <pthread.h>
#include <iostream>
using namespace std; 

// user-defined header files
#include "likely.h" 
#include "serialport.h" 

const int SLEEP_TIME_INTERVAL = 2;
const int RETRY_INIT_INTERVAL = 5;
int wait_flag = noflag;

SerialPort::Port_INFO *SerialPort::readyPort(int id, Port_INFO *pPort) 
{
	pthread_mutex_init(&pPort->mt,NULL); 
	/*Ubuntu�����
	 *sudo gpasswd --add zhaojq dialout
	 *logout
	 *����open /dev/ttyS0Ȩ��
	*/
	sprintf(pPort->name,"/dev/ttyS%d",id);
	/*A8������*/
	//sprintf(pPort->name,"/dev/s3c2410_serial%d",id); 
	
	pPort->fd = open(pPort->name, O_RDWR | O_NOCTTY | O_NDELAY); 
	if (unlikely(pPort->fd < 0)) {
		fprintf(stderr, "Open Serial [%s] Error...  [FAIL]\n", pPort->name); 
		free(pPort); 
		return NULL; 
	}
	return pPort; 
} 

int SerialPort::cleanPort(Port_INFO *pPort) 
{ 
	if(likely(pPort->fd > 0)) { 
		close(pPort->fd); 
		pPort->fd = -1; 
		free(pPort); 
		pPort = NULL; 
	}
	return 0; 
} 

/*c_cflag:����ģʽ��־,ָ���ն�Ӳ��������Ϣ
 *	CLOCAL:����,����Ե��ƽ������·״̬,�����豸�Ǳ�������
 *	CREAD:����,�����ý���װ��,���Խ����ַ�
*/
int SerialPort::setPortSpeed(Port_INFO *pPort, int port_speed) 
{ 
	/*tcgetattr:���ڻ�ȡ�ն���صĲ���*/
	if(unlikely(tcgetattr(pPort->fd,&pPort->ntm) != 0)) { 
		fprintf(stderr, "Set Serial [%s] Speed Error...  [FAIL]\n",pPort->name);
		return -1; 
	}

	int speed = 0;
	switch(port_speed) 
	{ 
		case 300: 
			speed = B300; 
			break; 
		case 1200: 
			speed = B1200; 
			break; 
		case 2400: 
			speed = B2400; 
			break; 
		case 4800: 
			speed = B4800; 
			break; 
		case 9600: 
			speed = B9600; 
			break; 
		case 19200: 
			speed = B19200; 
			break; 
		case 38400: 
			speed = B38400; 
			break; 
		case 115200: 
			speed = B115200; 
			break;
		default: 
			fprintf(stderr, "Unsupported Port Speed...  [FAIL]\n");
			return -1; 
	} 
	/*�������벨����*/
	cfsetispeed(&pPort->ntm, speed);
	/*�������������*/
	cfsetospeed(&pPort->ntm, speed);
	pPort->ntm.c_cflag |= (CLOCAL | CREAD);	//Enable the receiver and set local mode
	/*tcsetattr:���������ն���صĲ���
	 *	TCSANOW:�������ݴ�����Ͼ������ı�����
	 *	TCSADRAIN:�ȴ��������ݴ�����ϲŸı�����
	 *	TCSAFLUSH:�����������������Ÿı�����
	*/
	tcsetattr(pPort->fd,TCSANOW,&pPort->ntm);
	fprintf(stdout, "Set SerialPort Speed:[%d]...  [OK]\n", port_speed); 
	return 0; 
} 

/*
 *c_cflag:
 *	CSIZE:�ֽڴ�С����,ָ�����ͺͽ��յ�ÿ���ֽڵ�λ��(���Ȳ�������żУ��λ).ȡֵ��ΧΪCS5,CS6,CS7,CS8
 *	PARENB:������ż�ԵĲ����ͼ��.����,�������ַ�������żλ,�������ַ�ִ����ż��У��
 *	PARODD:����,������������ַ�����ż�Զ�����;����Ϊż
 *	CSTOPB:��������ֹͣλ;����Ϊһ��ֹͣλ
 *	CRTSCTS:ʹ��RTS/CTS������
 *c_iflag:����ģʽ��־,�����ն����뷽ʽ
 *	INPCK:����,��������żУ��������;����������
 *c_oflag:���ģʽ��־,�����ն������ʽ
 *	OPOST:����,��ִ��ʵ�ֶ�����������
 *c_lflag:����ģʽ��־,�����ն˱༭����
 *	ICANON:ʹ�ñ�׼����ģʽ
 *	ECHO:����,�������ַ����͵��ն��豸
 *	ECHOE:�ɼ�������
 *	ISIG:����,������INTR,OUIT,SUSPʱ,������Ӧ�ź�
*/
int SerialPort::setPortParity(Port_INFO *pPort,int databits,int parity,int stopbits) 
{ 
	if(unlikely(tcgetattr(pPort->fd,&pPort->ntm) != 0)) { 
		fprintf(stderr, "Set Serial [%s] Parity Error...  [FAIL]\n",pPort->name);
		return -1; 
	}
	pPort->ntm.c_cflag &= ~CSIZE;
	/*��������λ*/
	switch (databits) 
	{ 
		case 7: 
			pPort->ntm.c_cflag |= CS7; 
			break; 
		case 8: 
			pPort->ntm.c_cflag |= CS8; 
			break; 
		default: 
			fprintf(stderr, "Unsupported Port Data bits...  [FAIL]\n");
			return -1; 
	} 
	/*������żУ��λ*/
	switch (parity) 
	{ 
		/*No parity ����żУ��λ*/
		case 'n': 
		case 'N': 
			pPort->ntm.c_cflag &= ~PARENB;	/* Clear parity enable */ 
			pPort->ntm.c_iflag &= ~INPCK; 	/* Enable parity checking */ 
			break;
		
		/*Odd parity ��У��*/ 
		case 'o': 
		case 'O':
			pPort->ntm.c_cflag |= (PARODD | PARENB);
			pPort->ntm.c_iflag |= INPCK; 	/* Disnable parity checking */ 
			break;
		/*Even parity żУ��*/	 
		case 'e': 
		case 'E': 
			pPort->ntm.c_cflag |= PARENB; 	/* Enable parity */ 
			pPort->ntm.c_cflag &= ~PARODD;
			pPort->ntm.c_iflag |= INPCK; 
			break; 
		case 'S': 
		case 's': /*as no parity*/ 
			pPort->ntm.c_cflag &= ~PARENB; 
			pPort->ntm.c_cflag &= ~CSTOPB; 
			break; 
		default: 
			fprintf(stderr, "Unsupported Port Parity...  [FAIL]\n");
			return -1; 
	} 
	/*����ֹͣλ*/
	switch (stopbits) 
	{ 
		case 1: 
			pPort->ntm.c_cflag &= ~CSTOPB; 
			break; 
		case 2: 
			pPort->ntm.c_cflag |= CSTOPB; 
			break; 
		default: 
			fprintf(stderr, "Unsupported Port Stop bits...  [FAIL]\n");
			return -1; 
	}
	pPort->ntm.c_cflag &= ~CRTSCTS;	// disable hardware flow control
	pPort->ntm.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
	pPort->ntm.c_oflag  &= ~OPOST;
	/*tcflush:����ն�δ��ɵ�����/�����������
	 *	TCIFLUSH:ˢ���������
	 *	TCOFLUSH:ˢ���������
	 *	TCIOFLUSH:ˢ������/�������
	*/
	tcflush(pPort->fd, TCIFLUSH); 
	/*�ȴ�ʱ��(�ٺ���)*/
	pPort->ntm.c_cc[VTIME] = 5;	
	/*�ȴ�����С�ֽ���*/
	pPort->ntm.c_cc[VMIN] = 1;
	tcsetattr(pPort->fd,TCSANOW,&pPort->ntm);
	fprintf(stdout, "Set SerialPort Data bits:[%d], Parity:[%c], Stop bits:[%d]...  [OK]\n", databits, parity, stopbits);
	return 0; 
} 

int SerialPort::InitPort(Port_INFO *pPort, int COM_id)
{	
	int i=0;
	while(RETRY_INIT_INTERVAL - i) {
		if(COM_id < 0) COM_id = COM_PORT;
		pPort = readyPort(COM_id, pPort); 
		if(unlikely(pPort == NULL)) {
			sleep(SLEEP_TIME_INTERVAL); i++; continue;
		}
		fprintf(stdout, "Ready Serial [%s]...  [OK]\n",pPort->name); 

		lockPort(pPort);
		bzero(&pPort->ntm, sizeof(pPort->ntm)); 
		if(unlikely(setPortSpeed(pPort, PORT_SPEED) < 0)) {
			sleep(SLEEP_TIME_INTERVAL); i++; continue;
		}

		if(unlikely(setPortParity(pPort, PORT_DATABITS, PORT_PARITY, PORT_STOPBITS) < 0)) {
			sleep(SLEEP_TIME_INTERVAL); i++; continue;
		}
		unlockPort(pPort);
		return 0;
	}
	return -1;
}

void signal_handler_IO (int status)  
{
	wait_flag = noflag;
}

int SerialPort::recvnPort(SerialPort::Port_INFO *pPort,char *pbuf,int size) 
{ 
	int ret,left;
	char *ptmp;

	ret = 0;
	left = size;
	ptmp = pbuf;

	while(left > 0) {
		pthread_mutex_lock(&pPort->mt); 
		ret = read(pPort->fd,ptmp,left); 
		printf("ret = %d\n",ret);
		pthread_mutex_unlock(&pPort->mt); 
		if(likely(ret > 0)) {
			left -= ret; 
			ptmp += ret; 
		} else if(ret <= 0) {
			tcflush(pPort->fd, TCIOFLUSH);
			continue;
		} 
	}
	return size - left; 
}

int SerialPort::signal_recvnPort(Port_INFO *pPort,char *pbuf,int size)
{
	int nread = 0;
	struct sigaction saio;
	saio.sa_handler = signal_handler_IO;  
	sigemptyset (&saio.sa_mask);  
	saio.sa_flags = 0;  
	saio.sa_restorer = NULL;
	/*
	 *Ӧ�ò������첽֪ͨ:
	 *���豸��дʱ,�豸������������һ���źŸ��ں�,��֪�ں������ݿɶ�,������������֮ǰ,�������������
	*/
	//���ö��źŵĴ����������,SIGIO:�ļ�������׼������,���Կ�ʼ��������/�������
	sigaction(SIGIO, &saio, NULL);  
	//ָ��һ��������Ϊ�ļ���"����"
	fcntl(pPort->fd, F_SETOWN, getpid ());  
	//���豸�ļ������FASYNC��־
	int f_flags = fcntl(pPort->fd, F_GETFL);
	fcntl(pPort->fd, F_SETFL, f_flags|FASYNC); 

	while (wait_flag == noflag) { 
		memset (pbuf, 0, size);  
		nread = recvnPort(pPort, pbuf, size); 
		wait_flag = flag; /*wait for new input */  
	}
	return nread;
}

int SerialPort::sendnPort(SerialPort::Port_INFO *pPort,char *pbuf,int size) 
{
	int ret,left; 
	char *ptmp; 

	ret = 0; 
	left = size; 
	ptmp = pbuf;

	while(left > 0) { 
		pthread_mutex_lock(&pPort->mt);
		ret = write(pPort->fd,ptmp,left);
		pthread_mutex_unlock(&pPort->mt); 
		if(likely(ret > 0)) { 
			left -= ret; 
			ptmp += ret; 
		} else if(ret <= 0) {
			tcflush(pPort->fd, TCIOFLUSH);
			continue;
		} 
	}
	return size - left; 
} 

int SerialPort::lockPort(Port_INFO *pPort) 
{ 
	if(unlikely(pPort->fd < 0)) { 
		return 1; 
	}
	/*LOCK_EX:������������*/
	return flock(pPort->fd,LOCK_EX); 
}

int SerialPort::unlockPort(Port_INFO *pPort) 
{ 
	if(unlikely(pPort->fd < 0)) { 
		return 1; 
	}
	/*LOCK_UN������ļ�����*/
	return flock(pPort->fd,LOCK_UN); 
}
