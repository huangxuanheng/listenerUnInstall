#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>

#include "AndroidLog.h"
#include "CurlHttpOperate.h"

/* 宏定义begin */
//清0宏
#define MEM_ZERO(pDest, destSize) memset(pDest, 0, destSize)

//LOG宏定义
#define LOG_INFO(tag, msg) __android_log_write(ANDROID_LOG_INFO, tag, msg)
#define LOG_DEBUG(tag, msg) __android_log_write(ANDROID_LOG_DEBUG, tag, msg)
#define LOG_WARN(tag, msg) __android_log_write(ANDROID_LOG_WARN, tag, msg)
#define LOG_ERROR(tag, msg) __android_log_write(ANDROID_LOG_ERROR, tag, msg)

//#define FEEDBACK_URL "http://www.baidu.com"
//#define LISTEN_FILE_PATH "/data/data/com.keanbin.app"

/* 内全局变量begin */
static jboolean b_IS_COPY = JNI_TRUE;

/**
 * @params flag 是浏览器还是http ，flag=0 是浏览器，flag=1 是http请求
 */
void listenerUnInstall(JNIEnv *env, jclass obj, jstring listenFilePathJStr, jstring urlJStr,jstring paramsStr, jint httpMethod,jint sdkVersion,jboolean isUnInstalBrowser,int flag){
	//初始化log
		LOGI("jni httpRequest method");
		const char *listenFilePath;
		listenFilePath = (*env)->GetStringUTFChars(env, listenFilePathJStr, NULL);
		if (listenFilePath == NULL) {
			return;
		}
		const char *url;
		url = (*env)->GetStringUTFChars(env, urlJStr, NULL);
		if (url == NULL) {
			return;
		}

		LOGI("jni httpRequest() listenFilePath = %s", listenFilePath);
		//fork子进程，以执行轮询任务
		pid_t pid = fork();

		if (pid < 0) {
			//出错log
			LOGI("fork failed !!!");
		} else if (pid == 0) {
			//子进程注册"/data/data/pym.test.uninstalledobserver"目录监听器
			int fileDescriptor = inotify_init();

			if (fileDescriptor < 0) {
				LOGI("inotify_init failed !!!");
				exit(1);
			}
			int watchDescriptor;

			watchDescriptor = inotify_add_watch(fileDescriptor, listenFilePath,
					IN_DELETE);
			if (watchDescriptor < 0) {
				LOGI("inotify_add_watch failed !!!");

				exit(1);
			}

			//分配缓存，以便读取event，缓存大小=一个struct inotify_event的大小，这样一次处理一个event
			void *p_buf = malloc(sizeof(struct inotify_event));
			if (p_buf == NULL) {
				LOGI("malloc failed !!!");
				exit(1);
			}

			//开始监听
			LOGI("start observer");
			size_t readBytes = read(fileDescriptor, p_buf,
					sizeof(struct inotify_event));

			//read会阻塞进程，走到这里说明收到目录被删除的事件，注销监听器
			free(p_buf);
			inotify_rm_watch(fileDescriptor, IN_DELETE);

			//目录不存在log
			LOGI("uninstalled");
			char* action="android.intent.action.VIEW";
			char* browser="com.android.browser/.BrowserActivity";
			if(flag==0){
				//浏览器
				if (sdkVersion >= 17) {
									// Android4.2系统之后支持多用户操作，所以得指定用户d
					if(isUnInstalBrowser){
						LOGI("packageDir--->%s\n",browser);
						execlp("am", "am", "start", "--user", "0", "-a",action, "-d", url,"-n",browser,(char*) NULL);
					}else{
						LOGI("packageDir--->%s\n",action);
						execlp("am", "am", "start", "--user", "0", "-a",action, "-d", url,(char*) NULL);
					}

				} else {
					// Android4.2以前的版本无需指定用户
					if(isUnInstalBrowser){
						LOGI("packageDir--->%s\n",browser);
						execlp("am", "am", "start", "-a",action,"-d", url,"-n",browser, (char*) NULL);
					}else{
						LOGI("packageDir--->%s\n",action);
						execlp("am", "am", "start", "-a", action,"-d", url, (char*) NULL);
					}

				}
			}else if(flag==1){
				//http
				LOGI("packageDir--->%s\n",browser);
				curlHttp(env, obj, urlJStr,paramsStr,httpMethod);
			}

			exit(1);
		} else {
			//父进程直接退出，使子进程被init进程领养，以避免子进程僵死
			LOGI("jni httpRequest() exit");
		}
}

JNIEXPORT void JNICALL Java_com_ishow_app_uninstall_UnInstallManager_unInstallStartBrowser
  (JNIEnv *env, jobject obj, jstring packageDir,jint sdkVersion,jstring url, jboolean isUnInstalBrowser){
	jstring paramsStr;
	listenerUnInstall(env,obj,packageDir,url,paramsStr,0,sdkVersion,isUnInstalBrowser,0);
}


JNIEXPORT void JNICALL Java_com_ishow_app_uninstall_UnInstallManager_unInstallHttps
  (JNIEnv *env, jclass obj, jstring packageDir, jstring url,jstring paramsStr, jint httpMethod){
	jboolean a;
	listenerUnInstall(env,obj,packageDir,url,paramsStr,httpMethod,0,a,1);
}
