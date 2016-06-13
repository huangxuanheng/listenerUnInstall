package com.example.uninstall;

import java.io.File;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.ActionBarActivity;
import android.view.View;

import com.ishow.app.uninstall.UnInstallManager;


public class MainActivity extends ActionBarActivity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
	/*	Intent intent = new Intent(this, SDCardListenSer.class);  
        startService(intent);  */
        
//		UnInstallListener  listener=new UnInstallListener();
//		UnInstallManager.unInstallStartBrowser(this.getFilesDir().getAbsolutePath(),android.os.Build.VERSION.SDK_INT,"http://write.blog.csdn.net/postlist",true);
	
		UnInstallManager.unInstallHttps(this.getFilesDir().getAbsolutePath(), "http://192.168.0.136:8080/ishow/home/banner?name=123","", 1);
//		System.out.println("VERSION"+android.os.Build.VERSION.SDK_INT);
//		down();
	}

	public void change(View view){
	}

}
