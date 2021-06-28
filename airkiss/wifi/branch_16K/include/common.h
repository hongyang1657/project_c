#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#define MEMORYLEAK_DIAGNOSE
#ifndef LP_64
typedef long long int64_t;
#else
#include <stdint.h>
#endif

#define SYMB_LENGTH_FACTOR    1
#define SYNC_TONE_SYMB_LENGTH 2
#define LOW_FREQ_SIMPLE_NOISE_SHAPING 0
#define QCONST32(x,bits) ((int)(.5+(x)*(((int)1)<<(bits))))

#define PI QCONST32(3.1415926, 15)
#define HAM_COEF1 QCONST32(0.54, 15)
#define HAM_COEF2 QCONST32(0.46, 15)

#define BLACK_COEF1 QCONST32(0.42,15)
#define BLACK_COEF2 QCONST32(0.5,15)
#define BLACK_COEF3 QCONST32(0.08,15)

#define HAN_COEF1 QCONST32(0.5,15)
#define HAN_COEF2 QCONST32(0.5,15)

#define SINCOEF1 QCONST32(1,15)
#define SINCOEF3 QCONST32(-1./6,15)
#define SINCOEF5 QCONST32(1./120,15)

#define SAMUL(a, b) (int)(((((int64_t)(a))*((int64_t)(b))) + (1<<14) )>> 15)
#define SAMULT(a, b) (((int64_t)(a))*((int64_t)(b)))
#define SAMULT16(a, b) (((int)(a))*((int)(b)))
#define SASHR(a, b)	((a)>>(b))

#define SASAT(a) (((a)>32767)? 32767 :(((a)<-32768)?-32768:(a)))
#define SAABS(a) (((a) > 0) ? a : (-(a)))

#define SYMBITS 4

#define FREQNUM (1<<SYMBITS)

#ifndef MEMORYLEAK_DIAGNOSE
#define sa_alloc(size) calloc(1, size)
#define sa_free(ptr) free(ptr)

#else
void* sa_alloc(size_t size);
void sa_free(void* ptr);
void sa_memsdiagnose();
void sa_memsdiagnoseinit();
#endif
#define sa_memset(ptr, v, size) memset(ptr, v, size)
#define sa_memcpy(dst, src, size) memcpy(dst, src, size)
#ifdef _DEBUG
#define saprintf(...) klogd(__VA_ARGS__)
#else
#define saprintf(...) 
#endif

/* frequency mapping (HZ), log scale linear */
typedef enum
{
	FILTER_TYPE_LOWPASS,
	FILTER_TYPE_HIGHPASS,
	FILTER_TYPE_BANDPASS,
	FILTER_TYPE_BANDSTOP
}filter_type_t;
extern int freq_point[3][FREQNUM];
extern int freq_delta[3];
extern int freq_lag[3];
extern int freq_lag_top[3];
extern int freq_cutoff[3];
extern filter_type_t filter_type[3];
#endif
