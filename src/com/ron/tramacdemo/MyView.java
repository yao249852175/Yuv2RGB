package com.ron.tramacdemo;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MyView extends SurfaceView implements SurfaceHolder.Callback,DecoderCallBack{

		static
		{
	    	System.loadLibrary("avutil-54");
	    	System.loadLibrary("swresample-1");
	    	System.loadLibrary("avcodec-56");
	    	System.loadLibrary("avformat-56");
	    	System.loadLibrary("swscale-3");
	    	System.loadLibrary("postproc-53");
	    	System.loadLibrary("avfilter-5");
	    	System.loadLibrary("avdevice-56");
	    	System.loadLibrary("mydecoder");
	    }

	
	private	MyVideo myVideo;
	private SurfaceHolder surfaceHolder;
	private Matrix matrix;
	private boolean isRun;
	
	private Thread thread;
	
	private Paint paint;
	
	
	public MyView(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
		init();
	}
	
	public MyView(Context context) {
		super(context);
		
		init();
	}

	public void init()
	{
		surfaceHolder = this.getHolder();
		surfaceHolder.addCallback(this);
		paint = new Paint();
		myVideo = new MyVideo();
		matrix = new Matrix();
		matrix.postScale(3f,3f);
	}
	
	
	public void onDestroy()
	{
		myVideo.endDecoder();
	}
	
	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		isRun = true;
		thread = new Thread(runnable);
		thread.start();
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
			isRun = false;
			onDestroy();
		
	}
	
	/**
	 * <p>数据线程</p>
	 */
	private Runnable runnable = new Runnable() {
		
		@Override
		public void run() 
		{
			Log.e("ron", "开始启动");
			String input   = "rtsp://192.168.11.10/0";
			String output  = "/sdcard/ron/my.yuv";
			
			myVideo.initDecoder(input,output,MyView.this);
			myVideo.beginDecoder();
		}
	};
	
		private Bitmap bmp;
		
		private boolean b = false;
		private int a =0;
		@Override
		public void onDecoder(byte[] rgb, int width, int height)
		{
			Log.e("decoderCallBack", "回调了" + rgb.length);
			
			if(!b && a++ == 100)
			{
				try {
					writeFile(rgb);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				b = true;
			}
			/* if (bmp == null) {
		            bmp = Bitmap.createBitmap(width, height, Config.ARGB_8888);
		        }
		        bmp.copyPixelsFromBuffer(ByteBuffer.wrap(rgb));*/
			  bmp = createMyBitmap(rgb, width, height);
			  bmp = Bitmap.createBitmap(bmp,0,0,bmp.getWidth(),bmp.getHeight(),matrix,true);
			Canvas canvas = surfaceHolder.lockCanvas();
			if(canvas == null) return;
			if(!bmp.isRecycled()) 
			{
				canvas.drawBitmap(bmp, 0, 0, paint);
			}
			surfaceHolder.unlockCanvasAndPost(canvas);
		}
		
		private void writeFile(byte[] rgb) throws IOException
		{
			File file = new File("/sdcard/ron.jpg");
			if(!file.exists())
			{
				file.createNewFile();
			}else
			{
				file.delete();
				file.createNewFile();
			}
			FileOutputStream outputStream  =new FileOutputStream(file);
			outputStream.write(rgb);
			outputStream.flush();
			outputStream.close();
			
		}
		
		
		 public Bitmap createMyBitmap(byte[] data, int width, int height){      
	        int []colors = convertByteToColor(data);  
	        if (colors == null){  
	            return null;  
	        }  
	              
	        Bitmap bmp = null;  
	  
	        try {  
	            bmp = Bitmap.createBitmap(colors, 0, width, width, height,   
	                    Bitmap.Config.ARGB_8888);  
	        } catch (Exception e) {  
	            // TODO: handle exception  
	      
	            return null;  
	        }  
	                          
	        return bmp;  
	    }  
	  
	      
	    /* 
	     * 将RGB数组转化为像素数组 
	     */  
	    private  int[] convertByteToColor(byte[] data){  
	        int size = data.length;  
	        if (size == 0){  
	            return null;  
	        }  
	          
	          
	        // 理论上data的长度应该是3的倍数，这里做个兼容  
	        int arg = 0;  
	        if (size % 3 != 0){  
	            arg = 1;  
	        }  
	          
	        int []color = new int[size / 3 + arg];  
	        int red, green, blue;  
	          
	          
	        if (arg == 0){                                  //  正好是3的倍数  
	            for(int i = 0; i < color.length; ++i){  
	          
	                color[i] = (data[i * 3] << 16 & 0x00FF0000) |   
	                           (data[i * 3 + 1] << 8 & 0x0000FF00 ) |   
	                           (data[i * 3 + 2] & 0x000000FF ) |   
	                            0xFF000000;  
	            }  
	        }else{                                      // 不是3的倍数  
	            for(int i = 0; i < color.length - 1; ++i){  
	                color[i] = (data[i * 3] << 16 & 0x00FF0000) |   
	                   (data[i * 3 + 1] << 8 & 0x0000FF00 ) |   
	                   (data[i * 3 + 2] & 0x000000FF ) |   
	                    0xFF000000;  
	            }  
	              
	            color[color.length - 1] = 0xFF000000;                   // 最后一个像素用黑色填充  
	        }  
	      
	        return color;  
	    }  
	      

}
