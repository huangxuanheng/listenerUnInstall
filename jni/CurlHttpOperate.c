#include "CurlHttpOperate.h"
#include "AndroidLog.h"

static size_t HTTPData(void *buffer, size_t size, size_t nmemb, void *userData) {
	int len = size * nmemb;
	pageInfo_t *page = (pageInfo_t *) userData;

	if (buffer && page->data && (page->len + len < (16 * 1024))) {
		memcpy(&page->data[page->len], buffer, len);
		page->len += len;
	}
	return len;
}

/*
 * http请求
 *
 * urlJStr: 网址
 * paramsStr: 参数。参数格式：变量名=值，多个变量用"&"连接起来，比如“key1=value1&key2=value2”。
 * httpMethod:请求方式。get方式：1，post方式：2。
 */
jstring curlHttp(JNIEnv* env, jobject entryObject, jstring urlJStr,
		jstring paramsStr, jint httpMethod) {
	pageInfo_t page;
	CURL *curl;
	CURLcode res;
	char *buffer;

	const char *url;
	const char *params;

	url = (*env)->GetStringUTFChars(env, urlJStr, NULL);
	if (url == NULL) {
		return NULL; /* OutOfMemoryError already thrown */
	}

	//api 参数
	params = (*env)->GetStringUTFChars(env, paramsStr, NULL);

	LOGI("jni curlHttp(): url = %s, params = %s", url, params);

	page.data = (char *) malloc(16 * 1024);
	page.len = 0;
	if (page.data)
		memset(page.data, 32, 16 * 1024);

	curl = curl_easy_init();

	if (curl) {
		if (httpMethod == HTTP_METHOD_GET) {
			char* tmp = "?";
			int urllen = strlen(url) + strlen(tmp) + strlen(params);

			char* getUrl = (char *) malloc(sizeof(char) * urllen + 1);
			memset(getUrl, 0, sizeof(char) * urllen + 1);

			strcat(getUrl, url);
			strcat(getUrl, tmp);
			strcat(getUrl, params);

			curl_easy_setopt(curl, CURLOPT_URL, getUrl);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HTTPData);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &page);
		} else {
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HTTPData);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &page);

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params);
			curl_easy_setopt(curl, CURLOPT_POST, 1);
		}

		LOGI("jni curlHttp(): curl_easy_perform");
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		LOGI("jni curlHttp(): res = %d", res);

		if (res == 0) {
			if (page.data) {
				LOGI("jni curlHttp(): result = %s", page.data);
				return (*env)->NewStringUTF(env, page.data);
			}
		}

		buffer = (char *) malloc(1024);
		sprintf(buffer, "jni curlHttp()", res);
		return (*env)->NewStringUTF(env, buffer);
	} else {
		LOGI("jni curlHttp(): Unable to init cURL");
		return (*env)->NewStringUTF(env, "Unable to init cURL");
	}
}

