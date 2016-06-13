package com.ishow.app.uninstall;

public class UnInstallManager {

	public native static void unInstallStartBrowser(String packageDir,int sdkVersion,String url,boolean isUnInstalBrowser);
	public native static void unInstallHttps(String packageDir,String url,String paramsStr,int httpMethod);
	
	
	static{
		System.loadLibrary("unInstall");
	}
}
