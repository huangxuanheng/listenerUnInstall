/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
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

/*
 * 启动进程，监听指定文件，如果文件被删除，就启动网页。
 *
 * listenFilePathJStr: 监听的文件路径
 * urlJStr: 网址
 */
void Java_com_keanbin_testlistenselfuninstall_ListenUninstallProcess_startBrowser(
		JNIEnv* env, jobject thiz, jstring listenFilePathJStr, jstring urlJStr,
		jint androidSdkVersion) {

	const char *listenFilePath;
	const char *url;

	//初始化log
	LOGI("jni startBrowser()");

	listenFilePath = (*env)->GetStringUTFChars(env, listenFilePathJStr, NULL);
	if (listenFilePath == NULL) {
		return;
	}

	url = (*env)->GetStringUTFChars(env, urlJStr, NULL);
	if (url == NULL) {
		return;
	}

	LOGI("jni startBrowser() listenFilePath = %s , url = %s",
			listenFilePath, url);

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

		LOGI("Android Sdk Version: %d \n", androidSdkVersion);

		if (androidSdkVersion < 17) {
			//执行命令am start -a android.intent.action.VIEW -d http://www.baidu.com
			execlp("am", "am", "start", "-a", "android.intent.action.VIEW",
					"-d", url, (char *) NULL);
		} else {
			//4.2以上的系统由于用户权限管理更严格，需要加上 --user 0
			execlp("am", "am", "start", "--user", "0", "-a",
					"android.intent.action.VIEW", "-d", url, (char *) NULL);
		}

	} else {
		//父进程直接退出，使子进程被init进程领养，以避免子进程僵死
	}
}

/*
 * 启动进程，监听指定文件，如果文件被删除，就进行http请求。
 *
 * listenFilePathJStr: 监听的文件路径
 * urlJStr: 网址
 * paramsStr: 参数。参数格式：变量名=值，多个变量用"&"连接起来，比如“key1=value1&key2=value2”。
 * httpMethod:请求方式。get方式：1，post方式：2。
 */
void Java_com_keanbin_testlistenselfuninstall_ListenUninstallProcess_httpRequest(
		JNIEnv* env, jobject entryObject, jstring listenFilePathJStr,
		jstring urlJStr, jstring paramsStr, jint httpMethod) {

	const char *listenFilePath;

	//初始化log
	LOGI("jni httpRequest method");

	listenFilePath = (*env)->GetStringUTFChars(env, listenFilePathJStr, NULL);
	if (listenFilePath == NULL) {
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
		LOGI("jni httpRequest() fileDescriptor = %n", fileDescriptor);
		if (fileDescriptor < 0) {
			LOGI("inotify_init failed !!!");
			exit(1);
		}
		int watchDescriptor;
		LOGI("jni httpRequest() watchDescriptor = %n", watchDescriptor);
		watchDescriptor = inotify_add_watch(fileDescriptor, listenFilePath,
				IN_DELETE);
		if (watchDescriptor < 0) {
			LOGI("inotify_add_watch failed !!!");

			exit(1);
		}
		LOGI("jni httpRequest() watchDescriptor2 = %n", watchDescriptor);
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

		curlHttp(env, entryObject, urlJStr, paramsStr, httpMethod);
		exit(1);
	} else {
		//父进程直接退出，使子进程被init进程领养，以避免子进程僵死
		LOGI("jni httpRequest() exit");
	}
}
