#ifndef __SHAREMEMORY_H__
#define __SHAREMEMORY_H__
 
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
 
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <iostream>
#include <string.h>
#include <string>
 
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define FLAG IPC_CREAT|IPC_EXCL|SHM_R|SHM_W 		//共享内存创建标志常量符号
 
class CShareMemory
{
public:
	CShareMemory(const char *name, u32 size);
	~CShareMemory();
	int PutToShareMem(u8 *buffer, u32 length);
	int GetFromShareMem(u8 *buffer, u32 length);
private:
	void *CreateShareMemory(const char *name, u32 size);
	
	void SemLock ( void )
	{
		sem_wait ( m_sem );
	};
	
	void SemUnLock ( void )
	{
		sem_post ( m_sem );
	};
	
	sem_t *m_sem;
	u8 *m_shareMemory;
	u32 m_memSize;	
};
 
#endif