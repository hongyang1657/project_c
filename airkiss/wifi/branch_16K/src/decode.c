#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "kiss_fft.h"
#include "rscode.h"
#include "adt.h"
#include "Klog.h"
#define LOG_TAG "Decode"

#define QPRODUCT 15

#define STATE_SYNC1 0
#define STATE_SYNC2 1
#define STATE_DECODE 2
#define STATE_FRAMEEND 3
#define STATE_END	4
#define STATE_ERROR -1

/* 4 for 75% overlap, 2 for 50% overlap*/
#define OVERLAP_FACTOR 8
#define TRANSMIT_PER_SYMBOL 2
#define RECEIVE_PER_SYMBOL (OVERLAP_FACTOR*TRANSMIT_PER_SYMBOL)

#define sa_abs(a) ((a) > 0 ? (a) : (-(a)))

static int idx_sync[4] ={0xf, 0x0, 0xf, 0x0};
static int idx_sync_lag[4] ={0xf, 0x0, 0xf, 0x0};

//static int lag_sync = 0xf;

static unsigned char char_sync[2]={0x0f, 0x0f};
typedef struct
{
	/* maximum of probable string length in bytes */
	int max_strlen;
	/* error correcting coding flag */
	int error_correct;
	/* grouping of symbol number in symbols */
	int grouping_symbol_num;

	/* symbol length in samples */
	int symbol_length;
	/* sample rate of the encoded data*/
	int samplerate;
	/* freqrange select*/
	freq_type_t freqrange_select;
	/* maximum candidate freqidx in each symbol time */
	int max_candidate_num;
	/* threshold 1 in dB, threshold_1 lower than peak should not be a candidate */
	int threshold_1;
	/* threshold 2 in dB, threshold_2 larger than minmum should not be a candidate */
	int threshold_2;

}decode_init_prms_t;


typedef struct
{
	int order;
	short* history_buffer;
	short* coeff;

}firfilter_t;

typedef struct
{
	int* candidate_freqidx;
	int  candidate_num;
	int  candidate_idx;
}candidate_t;
typedef struct
{
	/* maximum of probable string length in bytes */
	int max_strlen;
	/* error correcting coding flag */
	int error_correct;
	/* grouping of symbol number in symbols */
	int grouping_symbol_num;
	/* symbol length in samples */
	int symbol_length;
	/* sync symbol number */
	int sync_symbol_num;
	/* error correct symbol number */
	int check_symbol_num;
	/* total symbol number per frame */
	int internal_symbol_num;
	/* pcm buffer length */
	int pcmbuf_length;
	/* sample rate of the encoded data*/
	int samplerate;
	/* freqrange select*/
	int freqrange_select;
	/* maximum candidate freqidx in each symbol time */
	int max_candidate_num;
	/* threshold 1 in dB, threshold_1 lower than peak should not be a candidate */
	int threshold_1;
	/* threshold 2 in dB, threshold_2 larger than minmum should not be a candidate */
	int threshold_2;
	/* freqency region low limit idx */
	int freq_idx_low;
	/* freqency region high limit idx */
	int freq_idx_high;
	/* freqency region high limit idx cover lag */
	int freq_idx_high_cover_lag;
	/* freqency region head sync idx */
	int freq_idx_lag_low, freq_idx_lag_high;
	/* total frequency bin number of selected frequency region */
	int freq_bin_num;
	int freq_bin_num_lag;
	/* difference of idx of adjoint frequency bin */
	int delta_freq_idx;
	/* fft size */
	int fft_size;
	/* max allowd candidate combin of freq serials */
	int max_allowed_ombin;
	/* frame count */
	int frame_count;
	/* Reed Solomon decoder */
	RS* rs;
	/* fft handle */
	kiss_fft_cfg fft_table;
	/* window */
	int* time_window;
	/* pcm buffer */
	kiss_fft_cpx* pcm_buf;
	/* temp buffer for fft */
	kiss_fft_cpx* temp_buf;
	/* frequency domain buffer */
	kiss_fft_cpx* fd_buf;
	/* power spectrum density buffer */
	unsigned int* psd_buf;
	/* candidate of all time segment */
	candidate_t** candidate_array;
	candidate_t** candidate_array_lag;
	/* candidate idx array */
	int** candidate_idx_array;
	/* shift idx */
	int shift_idx;
	/* decoder buffer */
	unsigned char* dec_buf;
	/* byte idx of dec_buf*/
	int idx;
	/* bit idx of dec_buf */
	int bidx;
	/* out char buffer */
	unsigned char* out_buf;
	/* freq idx after sorting */
	/* out buffer byte idx */
	int out_idx;
	/* freq idx after descending order sorting */
	int* sort_idx;
	/* status */
	int state;
	/* error byte count */
	int error_count;
	/* fir filter */
	firfilter_t* filter;
	/* sync tone define regions, for sync count and count total count */
    int lag_count;
	/* blablabla */
	int lag_symbol_num_internal;
    /* totally process freq range*/
	int process_freq_num;
	/* totally process freq start idx*/
    int process_start_idx;
	/* for debug, total play time*/
	int process_freq_num_lag;
	/* totally process freq start idx*/
    int process_start_idx_lag;
	int start_tone_combo;
	/* for debug, total play time*/
	unsigned int play_time;
	unsigned int fft_fetch_time;
}decoder_t;
static int sa_sin(int x)
{
	int x1, x2, x3, x5, s= 0;
	/* we first make sure that x in between [0, 2*pi) */
	x1 = x/(2*PI);
	x1 = x - (2*PI*x1);
	if(x1 < 0)
	{
		x1 += 2*PI;
	}
	if(x1 > PI)
	{
		x1 -= 2*PI;
	}
	if( x1 > PI/2)
	{
		x1 -= PI;
		s = 1;
	}else if (x1 < -PI/2)
	{
		x1 += PI;
		s = 1;
	}


	x2 = SAMUL(x1, x1);
	x3 = SAMUL(x1, x2);
	x5 = SAMUL(x2, x3);
	
	x2 =  (x1 + SAMUL(SINCOEF3, x3) + SAMUL(SINCOEF5, x5));
	if(s)
	{
		x2 = - x2;
	}
	x2 = SASAT(x2);
	return x2;


}
#define HAMMING_COEF1 17695
#define HAMMING_COEF2 15073
static int caculate_coeff_firfilter(firfilter_t* filter, int fs, int f0, filter_type_t type)
{
	int ALPHA = SAMULT(f0, 32768*2)/fs;
	int order = filter->order;
	int N = order/2;
	int i,ret=0;
	int w1, w2, w3;
	short* coeff = filter->coeff;
	/* we only support an even order now 
		because odd order of symmetric fir filter can not be an highpass filter
	*/
	if(order & 1)
	{
		kloge("we only support even order now!\n");
		return -1;
	}
	
	/* we use hamming windowing here to get an linear phase filter
		wn = 0.54-0.46cos(2*pi*n/L) 0<= n <= L
	*/
	switch(type)
	{
	case FILTER_TYPE_LOWPASS:
		coeff[N] = ALPHA;
		break;
	case FILTER_TYPE_HIGHPASS:
		coeff[N] = 32768-ALPHA;
		break;
	default:
		kloge("not yet support filter type : %d\n", type);
		return-1;
		
	}
	for(i = 1; i <= N; i++)
	{
		w1 = PI/2-SAMULT(2*PI,(i+N))/order;
		w1 = sa_sin(w1);
		w1 = HAMMING_COEF1 - SAMUL(w1,HAMMING_COEF2); 
		w2 = i*PI;
		w3 = SAMUL(w2,ALPHA);
		w3 = sa_sin(w3);
		w3 = SAMULT(w3,32768)/w2;
		w3 = SAMUL(w3,w1);
		switch(type)
		{
		case FILTER_TYPE_LOWPASS:
			coeff[i+N] = coeff[-i+N] = SAMUL(w3,w1);
			break;
		case FILTER_TYPE_HIGHPASS:
			coeff[i+N] = coeff[-i+N] = SAMUL(-w3,w1);
			break;
		default:
			ret = -1;
			kloge("not yet support filter type : %d\n", type);
			break;
		}
		 
	}
#if 0
	for(i = 0; i <= order; i++)
	{
		klogd("%d\n", coeff[i]);
	}
#endif
	return ret;
}
static void destroy_firfilter(firfilter_t* filter)
{
	if(filter)
	{
		if(filter->history_buffer)
		{
			sa_free(filter->history_buffer);
		}
		if(filter->coeff)
		{
			sa_free(filter->coeff);
		}
		sa_free(filter);
	}
}
static void reset_firfilter(firfilter_t* filter)
{
	sa_memset(filter->history_buffer,0,(sizeof(short)*filter->order));
}
static firfilter_t* init_firfilter(int order, int fs, int f0, filter_type_t type)
{
	firfilter_t* filter = sa_alloc(sizeof(firfilter_t));
	if(filter)
	{
		filter->order = order;
		//filter->coeff = coeff;
		filter->history_buffer = sa_alloc(sizeof(short)*order);
		if(filter->history_buffer == NULL)
		{
			destroy_firfilter(filter);
			filter = NULL;
		}
		filter->coeff = sa_alloc(sizeof(short)*(order+1));
		if(filter->coeff == NULL)
		{
			destroy_firfilter(filter);
			filter = NULL;
		}
		if (caculate_coeff_firfilter(filter, fs, f0, type) != 0)
		{
			destroy_firfilter(filter);
			filter = NULL;
		}
	}
	return filter;
}

static void process_firfilter(firfilter_t* filter, short* in, short* out, int len)
{
	int order = filter->order;
	short* coeff = filter->coeff;
	short* history_buffer = filter->history_buffer;
	int i,j;
	long long result;
	int loop1 = order < len ? order : len;
	for(i = 0; i < loop1; i++)
	{
		result = 0;
		for(j = 0; j < order -i; j++)
		{
			result += SAMULT(coeff[i+1+j],history_buffer[order-1-j]);
		}
		for(j = 0; j <= i; j++)
		{
			result += SAMULT(coeff[j], in[i-j]);
		}
		result = SASHR(result,15);
		out[i] = SASAT(result);
	}
	for(i = order; i < len; i++)
	{
		result = 0;
		for(j = 0; j <= order; j++)
		{
			result += SAMULT(coeff[j], in[i-j]);
		}
		result = SASHR(result,15);
		out[i] = SASAT(result);
	}

	for(i = 0; i < order-len; i++)
	{
		history_buffer[i] = history_buffer[i+len];
	}
	for(;i < order; i++)
	{
		history_buffer[i] = in[len-order+i];
	}

}
static void reset_dec_buf(decoder_t* decoder)
{
	sa_memset(decoder->dec_buf, 0, sizeof(char)*decoder->internal_symbol_num);
	decoder->idx = 0;
	decoder->bidx = 0;
}
static void load_default_prms(decoder_t* decoder)
{
	decoder->max_strlen = 256;
	decoder->error_correct = 1;
	decoder->grouping_symbol_num = 10;
	decoder->max_allowed_ombin = 1024;
	decoder->sync_symbol_num = 2;
	decoder->check_symbol_num = 8;
	decoder->symbol_length = 8*1024;
	decoder->samplerate = 48000;
	decoder->freqrange_select = FREQ_TYPE_MIDDLE;
	decoder->fft_size = decoder->symbol_length/TRANSMIT_PER_SYMBOL;
	
	decoder->max_candidate_num = 2;
	decoder->threshold_1 = 328; /* -26dB(0.01) in Q15 */
	decoder->threshold_2 = 3276800; /* 30dB(100) in Q15 */
	decoder->internal_symbol_num = (decoder->grouping_symbol_num+decoder->sync_symbol_num + decoder->check_symbol_num);
	decoder->pcmbuf_length = decoder->symbol_length/TRANSMIT_PER_SYMBOL/**decoder->internal_symbol_num*/;
	

}
static int set_prms(decoder_t* decoder, config_decoder_t* decoder_config)
{
	if(decoder_config != NULL)
	{
		if(( decoder_config->freq_type == FREQ_TYPE_LOW && decoder_config->sample_rate < 11025)
		   || ( decoder_config->freq_type == FREQ_TYPE_MIDDLE && decoder_config->sample_rate < 32000)
		   || ( decoder_config->freq_type == FREQ_TYPE_HIGH && decoder_config->sample_rate < 44100))
		{
			return -1;
		}
		decoder->max_strlen = decoder_config->max_strlen;
		decoder->freqrange_select =  decoder_config->freq_type;
		decoder->samplerate = decoder_config->sample_rate;
		decoder->error_correct = decoder_config->error_correct;
		switch(decoder->samplerate)
		{
		case 11025:
		case 16000:
            decoder->symbol_length = 2*1024*SYMB_LENGTH_FACTOR;
			break;
		case 22050:
		case 24000:
		case 32000:
			decoder->symbol_length = 4*1024*SYMB_LENGTH_FACTOR;
			break;
		case 44100:
		case 48000:
			decoder->symbol_length = 8*1024*SYMB_LENGTH_FACTOR;
			break;
		default:
			return -1;

		}
		
		if(decoder->error_correct)
		{
			decoder->check_symbol_num = decoder_config->error_correct_num*2;
			decoder->max_candidate_num = 2;
			decoder->max_allowed_ombin = 1024;
			decoder->threshold_1 = 1024; /* -26dB(0.01) in Q15 */
			decoder->threshold_2 = 3276800; /* 30dB(100) in Q15 */
		}else
		{
			decoder->max_candidate_num = 2;
			decoder->check_symbol_num = 0;
			decoder->max_allowed_ombin = 48;
			decoder->threshold_1 = 4096; /* -26dB(0.01) in Q15 */
			decoder->threshold_2 = 3276800; /* 30dB(100) in Q15 */
		}

		decoder->grouping_symbol_num = decoder_config->group_symbol_num+1;//10;
		//decoder->max_allowed_ombin = 1024;
		decoder->sync_symbol_num = 2;
		if(decoder->grouping_symbol_num+decoder->check_symbol_num > (255 - decoder->sync_symbol_num))
		{
			return -1;
		}


		decoder->fft_size = decoder->symbol_length/TRANSMIT_PER_SYMBOL;
		decoder->internal_symbol_num = (decoder->grouping_symbol_num+decoder->sync_symbol_num + decoder->check_symbol_num);
		decoder->pcmbuf_length = decoder->symbol_length/TRANSMIT_PER_SYMBOL/**decoder->internal_symbol_num*/;
	}else
	{
		load_default_prms(decoder);
	}
	decoder->freq_idx_low = freq_point[decoder->freqrange_select][0]*decoder->fft_size/decoder->samplerate;
	decoder->freq_idx_high = (freq_point[decoder->freqrange_select][15]*decoder->fft_size+decoder->samplerate/2)/decoder->samplerate;
	decoder->freq_idx_high_cover_lag = (freq_lag_top[decoder->freqrange_select]*decoder->fft_size+decoder->samplerate/2)/decoder->samplerate;
	decoder->delta_freq_idx = freq_delta[decoder->freqrange_select]*decoder->fft_size/decoder->samplerate;
	decoder->freq_bin_num = decoder->freq_idx_high - decoder->freq_idx_low + 1;
	decoder->freq_bin_num_lag = decoder->freq_idx_high_cover_lag - decoder->freq_idx_high;
	decoder->freq_idx_lag_low = freq_lag[decoder->freqrange_select]*decoder->fft_size/decoder->samplerate;
	decoder->freq_idx_lag_high = (freq_lag[decoder->freqrange_select] + 800)*decoder->fft_size/decoder->samplerate;
	decoder->lag_count = 0;
	decoder->lag_symbol_num_internal = SYNC_TONE_SYMB_LENGTH;
	return 0;
}
static void reset_sort_idx(decoder_t* decoder, int start, int range)
{
	int i;
	for(i = 0; i < range; i++)
	{
		decoder->sort_idx[i + start - decoder->process_start_idx] = i+start;
	}
}
static void reset_candidate(decoder_t* decoder)
{
	int i,k;
	for(k = 0; k < OVERLAP_FACTOR; k++)
	{
		for(i = 0; i < decoder->internal_symbol_num*TRANSMIT_PER_SYMBOL; i++)
		{
			sa_memset(decoder->candidate_array[k][i].candidate_freqidx, 0, sizeof(int)*decoder->max_candidate_num);
			decoder->candidate_array[k][i].candidate_num = 0;
			decoder->candidate_array[k][i].candidate_idx = 0;	
		}
		for(i = 0; i < decoder->lag_symbol_num_internal*TRANSMIT_PER_SYMBOL; i++)
		{
			sa_memset(decoder->candidate_array_lag[k][i].candidate_freqidx, 0, sizeof(int)*decoder->max_candidate_num);
			decoder->candidate_array_lag[k][i].candidate_num = 0;
			decoder->candidate_array_lag[k][i].candidate_idx = 0;	
		}
	}
	reset_sort_idx(decoder, decoder->process_start_idx, (decoder->process_freq_num + decoder->process_freq_num_lag));
}
static void frame_reset(decoder_t* decoder)
{
		decoder->error_count = 0;
		reset_dec_buf(decoder);
		decoder->frame_count = 0;
		decoder->state = STATE_SYNC2;
		reset_candidate(decoder);
}

static void init_blackmanwindow(int* window, int L)
{
	/* 0.42-0.5*cos(2*pi*n/(L-1))+0.08*cos(4*pi*n/(L-1))*/
	int w1, w2, w3,i;
	for(i = 0; i < L; i++)
	{
		w1 = PI/2-SAMULT(2*PI,i)/(L-1);
		w2 = PI/2-2*w1;
		if(w2 < (-2*PI))
		{
			w2 += 2*PI;
		}
		w1 = sa_sin(w1);
		w2 = sa_sin(w2);
		w3 = BLACK_COEF1 - SAMUL(BLACK_COEF2, w1) + SAMUL(BLACK_COEF3, w2);
		window[i] = w3;
	}
}
void* decoder_create(config_decoder_t* decode_config )
{

	int i,k/*, symsize, gfpoly, fcr, prim, nroots, pad*/;
	decoder_t* decoder;
#ifdef MEMORYLEAK_DIAGNOSE
	sa_memsdiagnoseinit();
#endif
    klogd("ADT SA Deocde Init!");
	decoder = sa_alloc(sizeof(decoder_t));
	if(decoder)
	{
		/*load_default_prms(decoder);
		decoder->max_strlen = decode_config->max_strlen;
		decoder->freqrange_select = decode_config->freq_type;
		decoder->freq_idx_low = freq_point[decoder->freqrange_select][0]*decoder->fft_size/decoder->samplerate;
		decoder->freq_idx_high = (freq_point[decoder->freqrange_select][15]*decoder->fft_size+decoder->samplerate/2)/decoder->samplerate;
		decoder->delta_freq_idx = freq_delta[decoder->freqrange_select]*decoder->fft_size/decoder->samplerate;
		decoder->freq_bin_num = decoder->freq_idx_high - decoder->freq_idx_low + 1;
		*/
		if(set_prms(decoder, decode_config) != 0)
		{
			decoder_destroy(decoder);
			return NULL;
		}

		decoder->process_freq_num  = decoder->freq_bin_num;//(decoder->freq_idx_low > decoder->freq_bin_num ? 2*decoder->freq_bin_num : (decoder->freq_bin_num + decoder->freq_idx_low));
		decoder->process_start_idx = decoder->freq_idx_low;//// - decoder->freq_bin_num > 0)?(decoder->freq_idx_low - decoder->freq_bin_num):0;
		decoder->process_freq_num_lag  = decoder->freq_bin_num_lag;
		decoder->process_start_idx_lag = decoder->freq_idx_high + 1;

		decoder->fft_fetch_time    = decoder->fft_size*1000/(OVERLAP_FACTOR*decoder->samplerate);
		decoder->time_window = sa_alloc(sizeof(int)*decoder->fft_size);
		decoder->pcm_buf = sa_alloc(sizeof(kiss_fft_cpx)*decoder->pcmbuf_length);
		decoder->temp_buf = sa_alloc(sizeof(kiss_fft_cpx)*decoder->fft_size);
		decoder->fd_buf = sa_alloc(sizeof(kiss_fft_cpx)*decoder->fft_size);
		decoder->psd_buf = sa_alloc(sizeof(unsigned int)*(decoder->process_freq_num + decoder->process_freq_num_lag));
		decoder->candidate_array = sa_alloc(sizeof(candidate_t*)*OVERLAP_FACTOR);
		decoder->candidate_array_lag = sa_alloc(sizeof(candidate_t*)*OVERLAP_FACTOR);
		if(decoder->error_correct)
		{
			decoder->rs = init_rs_char(8, 285, 1, 1, decoder->check_symbol_num, 255-decoder->internal_symbol_num/*symsize, gfpoly, fcr, prim, nroots, pad*/);
		}
		decoder->fft_table = kiss_fft_alloc(decoder->fft_size, 0, NULL, NULL);
		decoder->sort_idx = sa_alloc(sizeof(int)*(decoder->process_freq_num + decoder->process_freq_num_lag));
		decoder->candidate_idx_array = sa_alloc(sizeof(int*)*decoder->max_allowed_ombin);
		decoder->dec_buf = sa_alloc(sizeof(unsigned char)*decoder->internal_symbol_num);
		decoder->out_buf = sa_alloc(sizeof(unsigned char)*(decoder->max_strlen+1));
		if(!decoder->pcm_buf || !decoder->fd_buf || !decoder->psd_buf || !decoder->sort_idx
			|| !decoder->candidate_array || !decoder->candidate_array_lag ||(decoder->error_correct && !decoder->rs) || !decoder->fft_table || !decoder->candidate_idx_array
			|| !decoder->dec_buf || !decoder->out_buf || !decoder->time_window || !decoder->temp_buf)
		{
			decoder_destroy((void*) decoder);
			return NULL;
		}
		init_blackmanwindow(decoder->time_window, decoder->fft_size);
		for(k = 0; k < OVERLAP_FACTOR; k++)
		{
			decoder->candidate_array[k] = sa_alloc(decoder->internal_symbol_num*TRANSMIT_PER_SYMBOL*sizeof(candidate_t));
			if(decoder->candidate_array[k] == NULL)
			{
				decoder_destroy((void*)decoder);
				return NULL;
			}
			sa_memset(decoder->candidate_array[k], 0, decoder->internal_symbol_num*TRANSMIT_PER_SYMBOL*sizeof(candidate_t));
		
			for(i = 0; i < decoder->internal_symbol_num*TRANSMIT_PER_SYMBOL; i++)
			{
				decoder->candidate_array[k][i].candidate_freqidx = sa_alloc(sizeof(int)*decoder->max_candidate_num);
				if(!decoder->candidate_array[k][i].candidate_freqidx)
				{
					decoder_destroy((void*) decoder);
					return NULL;
				}
			}
		}
		for(k = 0; k < OVERLAP_FACTOR; k++)
		{
			decoder->candidate_array_lag[k] = sa_alloc(decoder->lag_symbol_num_internal*TRANSMIT_PER_SYMBOL*sizeof(candidate_t));
			if(decoder->candidate_array_lag[k] == NULL)
			{
				decoder_destroy((void*)decoder);
				return NULL;
			}
			sa_memset(decoder->candidate_array_lag[k], 0, decoder->lag_symbol_num_internal*TRANSMIT_PER_SYMBOL*sizeof(candidate_t));
		
			for(i = 0; i < decoder->lag_symbol_num_internal*TRANSMIT_PER_SYMBOL; i++)
			{
				decoder->candidate_array_lag[k][i].candidate_freqidx = sa_alloc(sizeof(int)*decoder->max_candidate_num);
				if(!decoder->candidate_array_lag[k][i].candidate_freqidx)
				{
					decoder_destroy((void*) decoder);
					return NULL;
				}
			}
		}
		decoder->shift_idx = 0;

		for(i = 0; i < decoder->max_allowed_ombin; i++)
		{
			decoder->candidate_idx_array[i] = sa_alloc(sizeof(int)*TRANSMIT_PER_SYMBOL*decoder->internal_symbol_num);
			if(!decoder->candidate_idx_array[i])
			{
				decoder_destroy((void*) decoder);
				return NULL;
			}
		}

		decoder->filter = init_firfilter(32, decoder->samplerate, freq_cutoff[decoder->freqrange_select], filter_type[decoder->freqrange_select]);//init_firfilter(32, coefftable);
		if(decoder->filter == NULL)
		{
			decoder_destroy((void*) decoder);
			return NULL;	
		}

		decoder->out_idx = 0;
		decoder->error_count = 0;
		reset_dec_buf(decoder);
		decoder->frame_count = 0;
		idx_sync[0] = decoder->freq_idx_high;
		idx_sync[1] = decoder->freq_idx_low;
		idx_sync[2] = decoder->freq_idx_high;
		idx_sync[3] = decoder->freq_idx_low;
		idx_sync_lag[0] = decoder->freq_idx_lag_high;
		idx_sync_lag[1] = decoder->freq_idx_lag_low;
		idx_sync_lag[2] = decoder->freq_idx_lag_high;
		idx_sync_lag[3] = decoder->freq_idx_lag_low;
		decoder->state = STATE_SYNC1;
	}
	return (void*)decoder;
}

void decoder_reset(void* handle)
{
	decoder_t* decoder = (decoder_t*)handle;
	klogd("ADT SA Deocde Reset!");
	sa_memset(decoder->pcm_buf,0,sizeof(kiss_fft_cpx)*decoder->pcmbuf_length);
	decoder->shift_idx = 0;
	decoder->out_idx = 0;
	decoder->error_count = 0;
	reset_dec_buf(decoder);
	decoder->frame_count = 0;
	decoder->lag_count   = 0;
	decoder->state = STATE_SYNC1;
	reset_firfilter(decoder->filter);

}
int decoder_getbsize(void* handle)
{
	decoder_t* decoder = (decoder_t*)handle;
	return decoder->fft_size/OVERLAP_FACTOR;

}

static void sorting_psd(decoder_t* decoder, int start, int range)
{
	int i, j, temp;
	register int* psd = (int*)(&decoder->psd_buf[start - decoder->process_start_idx]);
	register int* sort_idx = (int*)(&decoder->sort_idx[start - decoder->process_start_idx]);
#if LOW_FREQ_SIMPLE_NOISE_SHAPING
	if(start != decoder->process_start_idx)
	{
		for(i = 0; i < range - 1; i++)
		{
			if(i < range/2)
				psd[i] = sa_abs(psd[i] - psd[range - 1 -i]);
			else
				psd[i] = 0;
		}
	}
#endif
	for(i = 0; i < range - 1; i++)
	{
		for(j = i+1; j < range; j++)
		{
			if(psd[j] > psd[i])
			{
				temp = psd[j];
				psd[j] = psd[i];
				psd[i] = temp;
				temp = sort_idx[j];
				sort_idx[j] = sort_idx[i];
				sort_idx[i] = temp;

			}
		}

	}
    //klogd("shift_idx : %d, sort_idx : %d", decoder->shift_idx, sort_idx[0]);
}
static int is_valid_idx(decoder_t* decoder, int idx, int symbol_i, int len)
{
	int i, v;
	for(i = 0; i < len; i++)
	{
		v = decoder->candidate_array[decoder->shift_idx][symbol_i].candidate_freqidx[i];
		if( sa_abs(idx-v) <= decoder->delta_freq_idx)
		{
			return 0;
		}
	}
	return 1;
}
/*
static void show_candidate(decoder_t* decoder)
{
	int symbol_i = decoder->internal_symbol_num*TRANSMIT_PER_SYMBOL;

    int j;
	FILE * text = NULL;
	char name[128] = {0};
	char *prefix = "E:\\CK-DS5-WORK\\tmp\\print_result";
	char *postfix ="txt";

	sprintf(name, "%s_%dms-%dms.%s", prefix, decoder->play_time - decoder->fft_fetch_time, decoder->play_time, postfix);
    text = fopen(name, "wb");
	
    fprintf(text,"|========== print start ===========\n");

    {
		int* psd  = (int*)&decoder->psd_buf[decoder->process_start_idx_lag - decoder->process_start_idx];
		int* sort_idx = &decoder->sort_idx[decoder->process_start_idx_lag - decoder->process_start_idx];
		for(j = 0; j < decoder->process_freq_num_lag; j++)
		{
			fprintf(text, "psd[%d] : %d, sort_idx : %d\n", j, psd[j], sort_idx[j]);
		}
    }
	fprintf(text,"|========== print end   ===========\n");
	fclose(text);
}*/

static int update_candidate(decoder_t* decoder, int range, int symbol_i)
{
	int j;

	int* psd  = (int*)&decoder->psd_buf[0];
    int* sort_idx = &decoder->sort_idx[0];
	
	int low_limit2 = (int)(((long long)decoder->threshold_2*psd[range-1]) >> QPRODUCT);
	int low_limit1 = (int)(((long long)decoder->threshold_1*psd[0]) >> QPRODUCT);

	/* shift */

	for(j = 0; j < symbol_i; j++)
	{	
		sa_memcpy(decoder->candidate_array[decoder->shift_idx][j].candidate_freqidx,decoder->candidate_array[decoder->shift_idx][j+1].candidate_freqidx,sizeof(int)*decoder->candidate_array[decoder->shift_idx][j+1].candidate_num);
		decoder->candidate_array[decoder->shift_idx][j].candidate_num = decoder->candidate_array[decoder->shift_idx][j+1].candidate_num;
		decoder->candidate_array[decoder->shift_idx][j].candidate_idx = decoder->candidate_array[decoder->shift_idx][j+1].candidate_idx;
	}


	decoder->candidate_array[decoder->shift_idx][symbol_i].candidate_idx = 0;
	decoder->candidate_array[decoder->shift_idx][symbol_i].candidate_num = 0;
	
	if(psd[0] > low_limit2)
	{
		decoder->candidate_array[decoder->shift_idx][symbol_i].candidate_freqidx[decoder->candidate_array[decoder->shift_idx][symbol_i].candidate_num++]
									= sort_idx[0];
									
		for(j = 1; j < range&& decoder->candidate_array[decoder->shift_idx][symbol_i].candidate_num < decoder->max_candidate_num; j++)
		{
			if(psd[j] > low_limit1 && psd[j] > low_limit2 )
			{
				if(is_valid_idx(decoder, sort_idx[j], symbol_i, decoder->candidate_array[decoder->shift_idx][symbol_i].candidate_num))
				{
					decoder->candidate_array[decoder->shift_idx][symbol_i].candidate_freqidx[decoder->candidate_array[decoder->shift_idx][symbol_i].candidate_num++]
									= sort_idx[j];
				}
			}else
			{
				break;
			}
		}	
		return 0;
	}
	
	return -1;
}

static int update_lag_candidate(decoder_t* decoder, int range, int symbol_i)
{
	int j;

	int* psd  = (int*)&decoder->psd_buf[0];
    int* sort_idx = &decoder->sort_idx[0];
	
	int low_limit2 = (int)(((long long)decoder->threshold_2*psd[range-1]) >> QPRODUCT);
	int low_limit1 = (int)(((long long)decoder->threshold_1*psd[0]) >> QPRODUCT);

	/* shift */

	for(j = 0; j < symbol_i; j++)
	{	
		sa_memcpy(decoder->candidate_array_lag[decoder->shift_idx][j].candidate_freqidx,decoder->candidate_array_lag[decoder->shift_idx][j+1].candidate_freqidx,sizeof(int)*decoder->candidate_array_lag[decoder->shift_idx][j+1].candidate_num);
		decoder->candidate_array_lag[decoder->shift_idx][j].candidate_num = decoder->candidate_array_lag[decoder->shift_idx][j+1].candidate_num;
		decoder->candidate_array_lag[decoder->shift_idx][j].candidate_idx = decoder->candidate_array_lag[decoder->shift_idx][j+1].candidate_idx;
	}


	decoder->candidate_array_lag[decoder->shift_idx][symbol_i].candidate_idx = 0;
	decoder->candidate_array_lag[decoder->shift_idx][symbol_i].candidate_num = 0;
	
	if(psd[0] > low_limit2)
	{
		decoder->candidate_array_lag[decoder->shift_idx][symbol_i].candidate_freqidx[decoder->candidate_array_lag[decoder->shift_idx][symbol_i].candidate_num++]
									= sort_idx[0];
									
		for(j = 1; j < range&& decoder->candidate_array_lag[decoder->shift_idx][symbol_i].candidate_num < decoder->max_candidate_num; j++)
		{
			if(psd[j] > low_limit1 && psd[j] > low_limit2 )
			{
				if(is_valid_idx(decoder, sort_idx[j], symbol_i, decoder->candidate_array_lag[decoder->shift_idx][symbol_i].candidate_num))
				{
					decoder->candidate_array_lag[decoder->shift_idx][symbol_i].candidate_freqidx[decoder->candidate_array_lag[decoder->shift_idx][symbol_i].candidate_num++]
									= sort_idx[j];
				}
			}else
			{
				break;
			}
		}
		return 0;
	}
	
	return -1;
}


static int storebits(decoder_t* decoder, unsigned char c, int bits)
{
	int bidx, idx;
	bidx = decoder->bidx;
	idx = decoder->idx;

	while(bits > 0)
	{
		if(idx >= decoder->internal_symbol_num)
		{
			return -1;
		}
		if(8-bidx > bits)
		{
			decoder->dec_buf[idx] |= c<<bidx;
			bidx += bits;
			bits = 0;
		}else
		{
			decoder->dec_buf[idx++] |= c<<bidx;
			
			bits -= (8-bidx);
			c >>= (8-bidx);
			bidx = 0;
		}
	}
	decoder->bidx = bidx;
	decoder->idx = idx;
	return 0;
}

static int search_NULL(char* str, int len)
{
	int i;
	for(i = len-1; i >= 0; i--)
	{
		if(str[i] == '\0')
		{
			return 1;
		}
	}
	return 0;
}

static int search_sync(decoder_t* decoder)
{
	int i, j;
	candidate_t* candidate_array = decoder->candidate_array[decoder->shift_idx];
	for(i = 0; i < decoder->sync_symbol_num*TRANSMIT_PER_SYMBOL; i++)
	{
		for(j = 0; j < candidate_array[i].candidate_num; j++)
		{
			if(sa_abs(candidate_array[i].candidate_freqidx[j] - idx_sync[i]) < decoder->delta_freq_idx)
			{
				candidate_array[i].candidate_idx = j;
				break;
			}
		}
		if(j == candidate_array[i].candidate_num)
		{
			return -1;
		}
		
	}
	return 0;
}

static int search_sync_lag(decoder_t* decoder)
{
	int i, j;
	{
		candidate_t* candidate_array = decoder->candidate_array_lag[decoder->shift_idx];
		for(i = 0; i < decoder->lag_symbol_num_internal*TRANSMIT_PER_SYMBOL; i++)
		{
			for(j = 0; j < candidate_array[i].candidate_num; j++)
			{
				if(sa_abs(candidate_array[i].candidate_freqidx[j] - idx_sync_lag[i]) < decoder->delta_freq_idx)
				{
					candidate_array[i].candidate_idx = j;
					break;
				}
			}
			if(j == candidate_array[i].candidate_num)
			{
				return -1;
			}		
		}
	}
	return 0;
}

static unsigned char freqidx2char(decoder_t* decoder, int freq_idx)
{
	int freq = freq_idx*decoder->samplerate/decoder->fft_size;
	int i;
	for(i = 0; i < 15; i++)
	{
		if(sa_abs(freq - freq_point[decoder->freqrange_select][i]) < sa_abs(freq - freq_point[decoder->freqrange_select][i+1]))
		{
			return i;
		}
	}
	return 15;
}
static void compute_psd(kiss_fft_cpx* Xf, unsigned int* power, int length, int idx_start, int base_start)
{
	int i;
	
	for(i = 0; i < length ; i++)
	{
		power[i+idx_start - base_start] = SAMULT16(Xf[i+idx_start].r, Xf[i+idx_start].r) + SAMULT16(Xf[i+idx_start].i, Xf[i+idx_start].i);
	}

}
static void windowing_pcm(kiss_fft_cpx* pcm, kiss_fft_cpx* out, int* window, int L)
{
	int i;
	for(i = 0; i < L; i++)
	{
		out[i].r = SAMUL(window[i], pcm[i].r);
		out[i].i = SAMUL(window[i], pcm[i].i);
	}
}
int decoder_fedpcm(void* handle, short* pcm)
{
	int i, j, k, idx_total, idx_total1;
	int ret = 0;
	int sync_score = 0;
	decoder_t* decoder = (decoder_t*)handle;
	int max_amp, count_leadingzeros;

	/* filter input to keep the precision of the fixed point number 
	   we use the temp_buf, because it can be used uninitialized later 
	*/
	short* filt_tmpbuf = (short*)decoder->temp_buf;
	process_firfilter(decoder->filter, pcm, filt_tmpbuf,decoder->fft_size/OVERLAP_FACTOR);

	decoder->play_time += decoder->fft_fetch_time;//milisec

	/*shift buffer */
	for(i = 0; i < decoder->pcmbuf_length - decoder->fft_size/OVERLAP_FACTOR; i++)
	{
		decoder->pcm_buf[i].r = decoder->pcm_buf[i+decoder->fft_size/OVERLAP_FACTOR].r;
		decoder->pcm_buf[i].i = decoder->pcm_buf[i+decoder->fft_size/OVERLAP_FACTOR].i;
	}
	
	for(i = 0; i < decoder->fft_size/OVERLAP_FACTOR; i++)
	{
		decoder->pcm_buf[i+decoder->pcmbuf_length - decoder->fft_size/OVERLAP_FACTOR].r
																= filt_tmpbuf[i]/*pcm[i]*/;
		decoder->pcm_buf[i+decoder->pcmbuf_length - decoder->fft_size/OVERLAP_FACTOR].i
																= 0;
	}

	/* normalize to avoid precision dropping */
	max_amp = 0;
	for(i = 0; i < decoder->fft_size; i++)
	{
		int abs_amp = SAABS(decoder->pcm_buf[i].r);
		max_amp = (max_amp > abs_amp) ? max_amp : abs_amp;
	}
	/*count leading zeros of max_amp */
	count_leadingzeros = 0;
	while((max_amp & 0x8000 ) == 0 && count_leadingzeros <= 75)
	{
		count_leadingzeros++;
		max_amp <<= 1;
	}
	if(count_leadingzeros > 1)
	{
		count_leadingzeros -= 1;
		for(i = 0; i < decoder->fft_size; i++)
		{
			decoder->pcm_buf[i].r <<= count_leadingzeros;
		}
	}

    if(decoder->state == STATE_SYNC1)
    {
#ifdef ADD_SYNC_TONE
        reset_sort_idx(decoder, decoder->process_start_idx, decoder->process_freq_num_lag + decoder->process_freq_num);
        /* windowing */
        windowing_pcm(decoder->pcm_buf, decoder->temp_buf, decoder->time_window, decoder->fft_size);
        /* fft */
        kiss_fft(decoder->fft_table, decoder->temp_buf, decoder->fd_buf);
        /* caculate the psd */
        compute_psd(decoder->fd_buf, decoder->psd_buf, decoder->process_freq_num_lag + decoder->process_freq_num, decoder->process_start_idx, decoder->process_start_idx);
        /* sorting psd, TODO: init sort_idx */
        sorting_psd(decoder, decoder->process_start_idx, decoder->process_freq_num_lag + decoder->process_freq_num);
        decoder->lag_count++;
        decoder->shift_idx = (decoder->shift_idx + 1) % (OVERLAP_FACTOR);
		ret = update_lag_candidate(decoder, decoder->process_freq_num_lag + decoder->process_freq_num, decoder->lag_symbol_num_internal*TRANSMIT_PER_SYMBOL - 1);
        if(ret < 0)
        {	
            /* no candidate in current sync tone symbol interval, so we just return to get more data */
            return RET_DEC_NORMAL;
        }
        if(decoder->lag_count < decoder->lag_symbol_num_internal*RECEIVE_PER_SYMBOL)
        {
            return RET_DEC_NORMAL;
        }
        if(search_sync_lag(decoder) < 0)
        {
            return RET_DEC_NORMAL;
        }
        reset_candidate(decoder);
        decoder->state = STATE_SYNC2;
        klogd("find sync tone... decoding start!! lag_count : %d, sync_score : %d, during (%d ms - %d ms)", decoder->lag_count, sync_score, decoder->play_time - decoder->fft_fetch_time, decoder->play_time);
        return RET_DEC_NORMAL;
#else
        decoder->state = STATE_SYNC2;
#endif
    }

	reset_sort_idx(decoder, decoder->process_start_idx, (decoder->process_freq_num + decoder->process_freq_num_lag));
	/* windowing */
	windowing_pcm(decoder->pcm_buf, decoder->temp_buf, decoder->time_window, decoder->fft_size);
	/* fft */
	kiss_fft(decoder->fft_table, decoder->temp_buf, decoder->fd_buf);
	/* caculate the psd */
	compute_psd(decoder->fd_buf, decoder->psd_buf, (decoder->process_freq_num + decoder->process_freq_num_lag), decoder->process_start_idx , decoder->process_start_idx);
	/* sorting psd, TODO: init sort_idx */
	sorting_psd(decoder, decoder->process_start_idx, decoder->process_freq_num_lag + decoder->process_freq_num);

	decoder->frame_count++;
	decoder->shift_idx = (decoder->shift_idx + 1) % OVERLAP_FACTOR;
	
	ret = update_candidate(decoder, decoder->process_freq_num_lag + decoder->process_freq_num, decoder->internal_symbol_num*TRANSMIT_PER_SYMBOL - 1);
	update_lag_candidate(decoder, decoder->process_freq_num_lag + decoder->process_freq_num, decoder->lag_symbol_num_internal*TRANSMIT_PER_SYMBOL - 1);
	if(!search_sync_lag(decoder))
	{
		decoder->out_idx = 0;
		decoder->error_count = 0;
		reset_dec_buf(decoder);
		reset_candidate(decoder);
		decoder->state = STATE_SYNC2;
		sa_memset(decoder->out_buf,0x00, (sizeof(unsigned char)*(decoder->max_strlen+1)));
		klogd("find some start tone during decoding..., sync_score : %d, during (%d ms - %d ms)", sync_score, decoder->play_time - decoder->fft_fetch_time, decoder->play_time);
		return RET_DEC_NORMAL;
	}

	if(ret < 0)
	{	
		/* no candidate in current sync tone symbol interval, so we just return to get more data */
		return RET_DEC_NORMAL;
	}

	if(decoder->frame_count < decoder->internal_symbol_num*RECEIVE_PER_SYMBOL)
	{
		return RET_DEC_NORMAL;
	}
	//show_candidate(decoder);
	if(decoder->state == STATE_SYNC2 && search_sync(decoder) != 0)
	{
		return RET_DEC_NORMAL;
	}
	decoder->state = STATE_DECODE;
	/* TODO init candidate_idx_array*/
	idx_total = 1;
	for(i  = decoder->sync_symbol_num*TRANSMIT_PER_SYMBOL; i < decoder->internal_symbol_num*TRANSMIT_PER_SYMBOL; i++)
	{
		idx_total1 =  idx_total* decoder->candidate_array[decoder->shift_idx][i].candidate_num;
		if(idx_total1  > decoder->max_allowed_ombin)
		{
			break;
		}
		for(k = 0; k < idx_total; k++)
		{
			decoder->candidate_idx_array[k][i] = decoder->candidate_array[decoder->shift_idx][i].candidate_freqidx[0];
		}
		for(j = 1; j < decoder->candidate_array[decoder->shift_idx][i].candidate_num; j++)
		{
			for(k = 0; k < idx_total; k++)
			{
				memcpy(decoder->candidate_idx_array[k+j*idx_total],decoder->candidate_idx_array[k], sizeof(int)*(i-1));
				decoder->candidate_idx_array[k+j*idx_total][i] = decoder->candidate_array[decoder->shift_idx][i].candidate_freqidx[j];
			}
			 
		}
		idx_total = idx_total1;
		
	}

	if(idx_total1 > decoder->max_allowed_ombin)
	{
		idx_total = decoder->error_correct ? 1:0;
		for(i  = decoder->sync_symbol_num*TRANSMIT_PER_SYMBOL; i < decoder->internal_symbol_num*TRANSMIT_PER_SYMBOL; i++)
		{
			decoder->candidate_idx_array[0][i] = decoder->candidate_array[decoder->shift_idx][i].candidate_freqidx[0];
		}

	}
	for(k = 0; k < idx_total; k++)
	{
		for(i = 0; i < decoder->sync_symbol_num*TRANSMIT_PER_SYMBOL; i++)
		{
			decoder->candidate_idx_array[k][i] = idx_sync[i];
		}
	}

	for(k = 0; k < idx_total; k++)
	{
		int ret1;
		reset_dec_buf(decoder);
		for(i = 0; i < decoder->internal_symbol_num*TRANSMIT_PER_SYMBOL; i++)
		{
			unsigned char c_value = freqidx2char(decoder, decoder->candidate_idx_array[k][i]);
			storebits(decoder, c_value, 4);
		}
		if(decoder->error_correct)
		{
			ret1 = decode_rs_char(decoder->rs, decoder->dec_buf, NULL, 0);
		}else
		{
			ret1 = 0;
		}
		if(ret1 >= 0 && ret1 <= decoder->check_symbol_num/TRANSMIT_PER_SYMBOL && memcmp(decoder->dec_buf,char_sync,sizeof(char_sync))==0)
		{
			memcpy(&decoder->out_buf[decoder->out_idx], &decoder->dec_buf[decoder->sync_symbol_num], decoder->grouping_symbol_num*sizeof(unsigned char));
				
			decoder->error_count += ret1;	
			if(search_NULL((char*)&decoder->dec_buf[decoder->sync_symbol_num], decoder->grouping_symbol_num) != 0)
			{
					
				decoder->state = STATE_END;
				return RET_DEC_END;
					
			}else
			{
				decoder->state = STATE_FRAMEEND;
				decoder->out_idx += decoder->grouping_symbol_num;
				frame_reset(decoder);
			}		
			return RET_DEC_NORMAL;
		}
	}
	//decoder->state = STATE_ERROR;
	return RET_DEC_NORMAL;
	
}
int decoder_getstr(void* handle, unsigned char* str)
{
	decoder_t* decoder = (decoder_t*)handle;
	if(decoder->state != STATE_END)
	{
		return RET_DEC_NOTREADY;
	}
	strcpy((char*)str, (const char *)decoder->out_buf);
	return RET_DEC_NORMAL;

}
void decoder_destroy(void* handle)
{
	decoder_t* decoder = (decoder_t*)handle;
	klogd("ADT SA Deocde Destroy!");
	if(decoder)
	{
		int i,k;
		if(decoder->pcm_buf)
			sa_free(decoder->pcm_buf);
		if(decoder->fd_buf)
			sa_free(decoder->fd_buf);
		if(decoder->psd_buf)
			sa_free(decoder->psd_buf);
		if(decoder->candidate_array)
		{
			for(k = 0; k < OVERLAP_FACTOR; k++)
			{
				if(decoder->candidate_array[k])
				{
					for(i = 0; i < decoder->internal_symbol_num*TRANSMIT_PER_SYMBOL; i++)
					{
						if(decoder->candidate_array[k][i].candidate_freqidx)
							sa_free(decoder->candidate_array[k][i].candidate_freqidx);
					}
					sa_free(decoder->candidate_array[k]);
				}
			}
			sa_free(decoder->candidate_array);
		}
		if(decoder->candidate_array_lag)
		{
			for(k = 0; k < OVERLAP_FACTOR; k++)
			{
				if(decoder->candidate_array_lag[k])
				{
					for(i = 0; i < decoder->lag_symbol_num_internal*TRANSMIT_PER_SYMBOL; i++)
					{
						if(decoder->candidate_array_lag[k][i].candidate_freqidx)
							sa_free(decoder->candidate_array_lag[k][i].candidate_freqidx);
					}
					sa_free(decoder->candidate_array_lag[k]);
				}
			}
			sa_free(decoder->candidate_array_lag);
		}
		if(decoder->error_correct&&decoder->rs)
			free_rs_char(decoder->rs);
		if(decoder->fft_table)
			kiss_fft_free(decoder->fft_table);
		if(decoder->sort_idx)
			sa_free(decoder->sort_idx);
		if(decoder->candidate_idx_array)
		{
			for(i = 0; i < decoder->max_allowed_ombin; i++)
			{
				
				if(decoder->candidate_idx_array[i])
				{
					sa_free(decoder->candidate_idx_array[i]);
					
				}
			}
			sa_free(decoder->candidate_idx_array);
		}
		if(decoder->dec_buf)
			sa_free(decoder->dec_buf);
		if(decoder->out_buf)
			sa_free(decoder->out_buf);
		if(decoder->time_window)
			sa_free(decoder->time_window);
		if(decoder->temp_buf)
			sa_free(decoder->temp_buf);

		if(decoder->filter)
			destroy_firfilter(decoder->filter);
		sa_free(decoder);
	}
#ifdef MEMORYLEAK_DIAGNOSE
	sa_memsdiagnose();
#endif

}
