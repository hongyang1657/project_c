#include "common.h"
#include "Klog.h"
#define LOG_TAG "Common"

int freq_point[3][FREQNUM]=
{
	{2104, 2213, 2328, 2449, 2577, 2711, 2852, 3000, 3156, 3320, 3493, 3674, 3865, 4066, 4278, 4500},
	{8662, 8827, 8996, 9167, 9342, 9520, 9702, 9887, 10075, 10268, 10463, 10663, 10866, 11074, 11285, 11500,},
    {16673, 16848, 17025, 17204, 17384, 17567, 17751, 17937, 18126, 18316, 18508, 18702, 18899, 19097, 19297, 19500}
};

int freq_delta[3] ={109, 165, 175};
int freq_lag[3] = {4700, 7734, 15336};
int freq_lag_top[3] = {5500, 7734, 15336};
int freq_cutoff[3] = {8000,7000,14000};

filter_type_t filter_type[3] = {FILTER_TYPE_LOWPASS, FILTER_TYPE_HIGHPASS, FILTER_TYPE_HIGHPASS};

#ifdef  MEMORYLEAK_DIAGNOSE
#define ADT_HEAP_SIZE (8*1024*1024)
#define ADT_MAX_ALLOC_ITEM (2*2048)
unsigned char memoryblock[ADT_HEAP_SIZE];
unsigned int memorytable[ADT_MAX_ALLOC_ITEM][2];
unsigned int size_cur = 0;
unsigned int idx_table = 0;
unsigned int size_occupy = 0;
unsigned int max_size_occupy = 0;
void* sa_alloc(size_t size)
{
	void* ptr =  (void*)(memoryblock+size_cur);
	memset(ptr, 0, size);
	memorytable[idx_table][1] = (unsigned int)ptr;
	memorytable[idx_table][2] = size;
	
	if(size_cur + size > ADT_HEAP_SIZE)
	{
		kloge("heap overflow %d is larger than heapsize: %d !\n",size_cur, ADT_HEAP_SIZE);
		return NULL;
	
	}
	size_cur += size;
	size_occupy += size;
	max_size_occupy = (max_size_occupy < size_occupy) ? size_occupy : max_size_occupy;
	idx_table++;
	return ptr;

}
void sa_free(void* ptr)
{
	unsigned int i;
	for(i = 0; i < idx_table; i++)
	{
		
		if(memorytable[i][1] == (unsigned int) ptr)
		{
			if(memorytable[i][2] == 0)
			{
				kloge("illegal free of %d th alloc, addr: %#X\n",i, memorytable[i][1]);
				return ;
			}
			size_occupy -= memorytable[i][2];
			max_size_occupy = (max_size_occupy < size_occupy) ? size_occupy : max_size_occupy;
			memorytable[i][2] = 0;
			return;
		}
	}
	klogd("illegal free %#X\n", (unsigned int)ptr);
}
void sa_memsdiagnose()
{
	unsigned int i;
	klogv("total allocate %d times, total allocated size %d Bytes, max occupy size %d Bytes !\n", idx_table, size_cur, max_size_occupy);
	for(i = 0; i < idx_table; i++)
	{
		if(memorytable[i][2] != 0)
		{
			kloge("memory leak of %d th alloc, addr: %#X, size: %d\n", i, memorytable[i][1],memorytable[i][2]);
		}
	}
	
}
void sa_memsdiagnoseinit()
{
	memset(memorytable, 0, sizeof(unsigned int)*ADT_MAX_ALLOC_ITEM*2);
	size_cur = 0;
	idx_table = 0;
}

#endif
