#ifndef _DECODER_NEW_H_
#define _DECODER_NEW_H_

#define RET_DEC_ERROR -1
#define RET_DEC_NORMAL 0
#define RET_DEC_NOTREADY 1
#define RET_DEC_END 2

typedef enum
{
	/*µÍÆµ£¬ 2K~5K */
	FREQ_TYPE_LOW =0,
	/*ÖÐÆµ, 8K~12K*/
	FREQ_TYPE_MIDDLE,
	/*¸ßÆµ£¬ 16K~20K*/
	FREQ_TYPE_HIGH

}freq_type_t;
typedef struct
{
	int max_strlen;
	freq_type_t freq_type;
}config_decoder_t;


void* decode_new_create(config_decoder_t* decode_config );
int decode_new_getbsize(void* handle);
int decode_new_fedpcm(void* handle, short* pcm);
int decode_new_getstr(void* handle, unsigned char* str);
void decode_new_destroy(void* handle);
#endif