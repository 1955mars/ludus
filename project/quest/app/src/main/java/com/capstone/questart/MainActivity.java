package com.capstone.questart;

public class MainActivity extends android.app.NativeActivity {
	static {
		System.loadLibrary("vrapi");
		System.loadLibrary("questart");
	}
}
