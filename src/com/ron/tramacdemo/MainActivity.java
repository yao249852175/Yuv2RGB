package com.ron.tramacdemo;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity
{
	private MyView myView;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		myView = new MyView(this);
		setContentView(myView);
	}
	
}
