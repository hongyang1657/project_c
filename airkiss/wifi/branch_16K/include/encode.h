#ifndef _ENCODER_NEW_H_
#define _ENCODER_NEW_H_


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
}config_encoder_t;


#define		RET_ENC_NORMAL 0
#define 	RET_ENC_END 1
#define		RET_ENC_ERROR -1


void encoder_new_destroy(void* handle);
void* encoder_new_create(config_encoder_t* config);
int encoder_new_getoutsize(void* handle);
int encoder_new_getpcm(void* handle, short* outpcm);
int encoder_new_setinput(void* handle, unsigned char* input);
#endif