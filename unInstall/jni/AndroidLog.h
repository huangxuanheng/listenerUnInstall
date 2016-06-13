#include <android/log.h>


#define DEBUG 1

#define LOG_TAG "com.keanbin.framework"

#define LOGI(fmt, args...) if(DEBUG){__android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args);}
#define LOGD(fmt, args...) if(DEBUG){__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args);}
#define LOGE(fmt, args...) if(DEBUG){__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args);}

//Example:
//LOGI("keanbin", "test log!!!!")
//LOGI("keanbin", "the string is: %s \n",buff);
