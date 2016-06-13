package com.example.uninstall;


public class UnInstallListener {

	public native void unInstall(String data,int sdkVersion);
	
	public native void silentInstall(String appPath);
	
	static{
		System.loadLibrary("unInstall");
	}
	public void setMobileInfo(String data){
		System.out.println("http://192.168.0.12:8088/IShowOrgInterface/api/startup/indexInfo?memberId=2481");
		System.out.println(data);
		/*HttpUtils utils=new HttpUtils();
    	String url="http://192.168.0.12:8088/IShowOrgInterface/api/startup/indexInfo?memberId=2481";
		utils.send(HttpMethod.GET, url, new RequestCallBack<String>() {

			@Override
			public void onFailure(HttpException arg0, String arg1) {
				System.out.println(arg0);
//				Toast.makeText(getApplicationContext(), arg0.toString(), 0).show();
				Log.i("LogcatScannerService",arg0.toString());
			}

			@Override
			public void onSuccess(ResponseInfo<String> arg0) {
//				Toast.makeText(getApplicationContext(), arg0.result, 0).show();
				System.out.println(arg0.result);
			}
		});*/
	}
}
