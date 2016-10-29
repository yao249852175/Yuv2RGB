package com.ron.tramacdemo;

public class MyVideo 
{
	public	MyVideo()
	{
		
	}
	
	public native boolean initDecoder(String input,String ouput,DecoderCallBack cb);
	
	public native void beginDecoder();
	
	public native void endDecoder();
	
	public native void setCB(DecoderCallBack deBack);
}
