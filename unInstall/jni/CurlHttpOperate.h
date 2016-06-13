#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include "curl_config.h"
#include "curl/curl.h"

enum {
	HTTP_METHOD_GET = 1, HTTP_METHOD_POST
} HttpMethod;

typedef struct pageInfo_t {
	char *data;
	int len;
} pageInfo_t;

/*
 * http请求
 *
 * urlJStr: 网址
 * paramsStr: 参数。参数格式：变量名=值，多个变量用"&"连接起来，比如“key1=value1&key2=value2”。
 * httpMethod:请求方式。get方式：1，post方式：2。
 */
extern jstring curlHttp(JNIEnv* env,
		jobject entryObject, jstring urlJStr, jstring paramsStr,
		jint httpMethod);

