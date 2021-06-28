#ifndef KLOG_H
#define KLOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CTOOL_ARM_ADS     0
#define CTOOL_ANDROIDEABI 1
#define CTOOL_GNUEABI     2
#define CTOOL_GNUEABIHF   3

#define KLOG_LEVEL_HIGH    0
#define KLOG_LEVEL_DEBUG   1
#define KLOG_LEVEL_VERBOS  2

#define VERSION "SA ADT Version 1.2 gama Branch Hfreq-16K"

#if defined (CTOOL) && defined (LLVL)

#if (CTOOL == CTOOL_ARM_ADS)

	#if (LLVL == KLOG_LEVEL_HIGH)
		#define klogv(fmt, arg...) printf("Verbos: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
		#define klogd(fmt, arg...) printf("Debug: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
		#define kloge(fmt, arg...) printf("Error: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
	#elif (LLVL == KLOG_LEVEL_DEBUG)
		#define klogv(fmt, arg...)
		#define klogd(fmt, arg...) printf("Debug: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
		#define kloge(fmt, arg...) printf("Error: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
	#elif (LLVL == KLOG_LEVEL_VERBOS)
		#define klogv(fmt, arg...)
		#define klogd(fmt, arg...)
		#define kloge(fmt, arg...) printf("Error: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
	#else
		#error "you must resign a log level!"
	#endif

#elif (CTOOL == CTOOL_ANDROIDEABI)
    #include <cutils/log.h>

	#define LOG_LEVEL_ERROR     ANDROID_LOG_ERROR
    #define LOG_LEVEL_WARNING   ANDROID_LOG_WARN
    #define LOG_LEVEL_INFO      ANDROID_LOG_INFO
    #define LOG_LEVEL_VERBOSE   ANDROID_LOG_VERBOSE
    #define LOG_LEVEL_DEBUG     ANDROID_LOG_DEBUG

    #define AWLOG(level, fmt, arg...)  \
        LOG_PRI(level, LOG_TAG, "%s - <%s:%u>: "fmt, VERSION, __FUNCTION__, __LINE__, ##arg)
        
	#if (LLVL == KLOG_LEVEL_HIGH)
		#define klogv(fmt, arg...) AWLOG(LOG_LEVEL_VERBOSE, fmt, ##arg)
		#define klogd(fmt, arg...) AWLOG(LOG_LEVEL_DEBUG, fmt, ##arg)
		#define kloge(fmt, arg...) AWLOG(LOG_LEVEL_ERROR, fmt, ##arg)
	#elif (LLVL == KLOG_LEVEL_DEBUG)
		#define klogv(fmt, arg...)
		#define klogd(fmt, arg...) AWLOG(LOG_LEVEL_DEBUG, fmt, ##arg)
		#define kloge(fmt, arg...) AWLOG(LOG_LEVEL_ERROR, fmt, ##arg)
	#elif (LLVL == KLOG_LEVEL_VERBOS)
		#define klogv(fmt, arg...)
		#define klogd(fmt, arg...)
		#define kloge(fmt, arg...) AWLOG(LOG_LEVEL_ERROR, fmt, ##arg)
	#else
		#error "you must resign a log level!"
	#endif

#elif (CTOOL == CTOOL_GNUEABI)

	#if (LLVL == KLOG_LEVEL_HIGH)
		#define klogv(fmt, arg...) printf("Verbos: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
		#define klogd(fmt, arg...) printf("Debug: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
		#define kloge(fmt, arg...) printf("Error: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
	#elif (LLVL == KLOG_LEVEL_DEBUG)
		#define klogv(fmt, arg...)
		#define klogd(fmt, arg...) printf("Debug: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
		#define kloge(fmt, arg...) printf("Error: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
	#elif (LLVL == KLOG_LEVEL_VERBOS)
		#define klogv(fmt, arg...)
		#define klogd(fmt, arg...)
		#define kloge(fmt, arg...) printf("Error: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
	#else
		#error "you must resign a log level!"
	#endif

#elif (CTOOL == CTOOL_GNUEABIHF)

	#if (LLVL == KLOG_LEVEL_HIGH)
		#define klogv(fmt, arg...) printf("Verbos: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
		#define klogd(fmt, arg...) printf("Debug: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
		#define kloge(fmt, arg...) printf("Error: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
	#elif (LLVL == KLOG_LEVEL_DEBUG)
		#define klogv(fmt, arg...)
		#define klogd(fmt, arg...) printf("Debug: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
		#define kloge(fmt, arg...) printf("Error: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
	#elif (LLVL == KLOG_LEVEL_VERBOS)
		#define klogv(fmt, arg...)
		#define klogd(fmt, arg...)
		#define kloge(fmt, arg...) printf("Error: %s - <%s:%d>, "fmt"\n",VERSION ,LOG_TAG, __LINE__, ##arg)
	#else
		#error "you must resign a log level!"
	#endif

#else
	#error "unsupported compile tool!"
#endif

#else
	#error "tool chain or log level not defined!"
#endif
#endif
