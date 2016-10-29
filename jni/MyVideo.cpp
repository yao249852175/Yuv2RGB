
#include "com_ron_tramacdemo_MyVideo.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf("(>_<) " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf("(^_^) " format "\n", ##__VA_ARGS__)
#endif



//Output FFmpeg's av_log()
/**
 * ffmpeg的 日志回调借口
 */
void custom_log(void *ptr, int level, const char* fmt, va_list vl){
	FILE *fp=fopen("/storage/emulated/0/av_log.txt","a+");
	if(fp){
		vfprintf(fp,fmt,vl);
		fflush(fp);
		fclose(fp);
	}
}

jbyteArray rgbArray;

int isClose;

JNIEXPORT void JNICALL Java_com_ron_tramacdemo_MyVideo_setCB
(JNIEnv *env, jobject job, jobject jb)
{

}

JNIEXPORT jboolean JNICALL Java_com_ron_tramacdemo_MyVideo_initDecoder
  (JNIEnv * env, jobject job, jstring input, jstring output,jobject jcb)
{
	isClose = 0;
	decode(env,job,input,output,jcb);
}

/*
 * Class:     com_ron_tramacdemo_MyVideo
 * Method:    beginDecoder
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_ron_tramacdemo_MyVideo_beginDecoder
  (JNIEnv *env, jobject jobj)
{

}

/*
 * Class:     com_ron_tramacdemo_MyVideo
 * Method:    endDecoder
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_ron_tramacdemo_MyVideo_endDecoder
  (JNIEnv *env, jobject job)
{
	isClose = 1;
}

/**
 * 图像处理，并合并图像处理的结果
 */
void imgAlgorithm(unsigned char *rgb, int width, int height,int length)
{
//   unsigned char rgb24[length*3/2];
//   LOGE("beign rgb565_to_rgb24");
//   rgb565_to_rgb24(rgb,rgb24,width,height);
//   LOGE("end rgb565_to_rgb24");
//  int result = 0;
	LOGE("begin Arith_Parking");
  Arith_Parking(rgb,width,height);

  LOGE("ubSuccessDetectFlag = %d, ubSuccessTrackFlag = %d\n", ubSuccessDetectFlag, ubSuccessTrackFlag);
  LOGE("algorithm,L:%d,R:%d,T:%d,B:%d,H:%d,W:%d,CX:%d,CY:%d",
		  g_trackRect.left,g_trackRect.right,g_trackRect.top,
		  g_trackRect.bottom,g_trackRect.h,g_trackRect.w,
		  g_trackRect.cener_x,g_trackRect.cener_y);

  make_rectangle(rgb,width,height,g_trackRect);

}

int ScaleImg(AVCodecContext *pCodecCtx,AVFrame *src_picture,AVFrame *dst_picture,int nDstH ,int nDstW ){
int i ;
int nSrcStride[3];
int nDstStride[3];
int nSrcH = pCodecCtx->height;
int nSrcW = pCodecCtx->width;
struct SwsContext* m_pSwsContext;


uint8_t *pSrcBuff[3] = {src_picture->data[0],src_picture->data[1], src_picture->data[2]};


nSrcStride[0] = nSrcW ;
nSrcStride[1] = nSrcW/2 ;
nSrcStride[2] = nSrcW/2 ;




dst_picture->linesize[0] = nDstW;
dst_picture->linesize[1] = nDstW / 2;
dst_picture->linesize[2] = nDstW / 2;


printf("nSrcW%d\n",nSrcW);


m_pSwsContext = sws_getContext(nSrcW, nSrcH, PIX_FMT_YUV420P,
nDstW, nDstH, PIX_FMT_YUV420P,
SWS_BICUBIC,
NULL, NULL, NULL);


if (NULL == m_pSwsContext)
{
printf("ffmpeg get context error!\n");
return  -1;
}


sws_scale(m_pSwsContext, src_picture->data,src_picture->linesize, 0, pCodecCtx->height,dst_picture->data,dst_picture->linesize);


printf("line0:%d line1:%d line2:%d\n",dst_picture->linesize[0] ,dst_picture->linesize[1] ,dst_picture->linesize[2]);
sws_freeContext(m_pSwsContext);


return 1 ;
}

 int decode
  (JNIEnv *env, jobject obj, jstring input_jstr, jstring output_jstr,jobject jcb)
{
	 LOGE("begin decode 10000");
	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame,*pFrameYUV;
	uint8_t *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;
	int frame_cnt;
	clock_t time_start, time_finish;
	double  time_duration = 0.0;
	AVPicture					mVideoPicture;
//	char input_str[500]={0};//输入文件的地址
	char output_str[500]={0};// 输出文件的地址地址
	char info[1000]={0};

	const char *input_str = env->GetStringUTFChars(input_jstr, 0);
	//把java的地址转换为 c的 str
//	sprintf(input_str,"%s",(*env)->GetStringUTFChars(env,input_jstr, NULL));
//	sprintf(output_str,"%s",(*env)->GetStringUTFChars(env,output_jstr, NULL));


	 	 jclass native_clazz = env->GetObjectClass(jcb);
	 	jmethodID jCallBackID = env->GetMethodID(native_clazz, "onDecoder", "([BII)V");


	//FFmpeg av_log() callback
	av_log_set_callback(custom_log);//设置 ffmpeg的日志打印的回调函数
	//注册所有的
	av_register_all();

	//访问网络需要注册
	avformat_network_init();
	//格式化 AVFarmmat
	pFormatCtx = avformat_alloc_context();

	 //元数据
	AVDictionary *opts = 0;
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);

	LOGE("begin decode 20000");
	/**
	 * 代码中的英文注释写的已经比较详细了，在这里拿中文简单叙述一下。
		ps：函数调用成功之后处理过的AVFormatContext结构体。
		file：打开的视音频流的URL。
		fmt：强制指定AVFormatContext中AVInputFormat的。这个参数一般情况下可以设置为NULL，这样FFmpeg可以自动检测AVInputFormat。
		dictionay：附加的一些选项，一般情况下可以设置为NULL。
		函数执行成功的话，其返回值大于等于0。
	 */
	if(avformat_open_input(&pFormatCtx,input_str,NULL,&opts)!=0)
	{
		LOGE("Couldn't open input stream.\n");
		return -1;
	}

	/**
	 * 把 AVFarmatContext里面的 stream流 全部找出来
	 */
	if(avformat_find_stream_info(pFormatCtx,NULL)<0)
	{
		LOGE("Couldn't find stream information.\n");
		return -1;
	}
	videoindex=-1;

	/**
	 * 查找Video的流 (nb_streams 包含 音频流，字幕流)
	 */
	for(i=0; i<pFormatCtx->nb_streams; i++)
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
			videoindex=i;
			break;
		}
	if(videoindex==-1){
		LOGE("Couldn't find a video stream.\n");
		return -1;
	}

	//根据 视频流 拿到 解析的上下文 AVCodecContext
	pCodecCtx=pFormatCtx->streams[videoindex]->codec;

	//通过code ID查找一个已经注册的音视频解码器
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);

	if(pCodec==NULL){
		LOGE("Couldn't find Codec.\n");
		return -1;
	}

	//初始化 pCode 解析器，打开并初始化解码器
	if(avcodec_open2(pCodecCtx, pCodec,NULL)<0)
	{
		LOGE("Couldn't open codec.\n");
		return -1;
	}
	int count = 0;
		int height = 270;
		int width = 480;
	/**
	 * AVFrame结构体一般用于存储原始数据（即非压缩数据，例如对视频来说是YUV，RGB，
	 * 对音频来说是PCM），此外还包含了一些相关的信息。比如说，解码的时候存储了宏块类型表，
	 * QP表，运动矢量表等数据。编码的时候也存储了相关的数据。因此在使用FFMPEG进行码流分析的时候，AVFrame是一个很重要的结构体
	 */
	pFrame=av_frame_alloc();
	pFrameYUV=av_frame_alloc();

	int avpictureValid = avpicture_alloc(&mVideoPicture, PIX_FMT_RGB24, //PIX_FMT_RGB565,gmq switch 565 to 24
			width,height);

	/**
	 * 解析1帧 的长度
	 */
	/*size_t imgeSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
			pCodecCtx->width, pCodecCtx->height,1);// 1标示的是 只有1个视频流*/
	/**
		 * 创建 帧的输出数组
		 */
		/*out_buffer=(unsigned char *)av_malloc(avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height));

//	out_buffer=new uint8_t[avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height)];
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);*/



	//目前这个函数的的用途比较模糊，猜测是 把 pFrame->data初始化， lineszie是 data的大小
//	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
//		AV_PIX_FMT_YUV420P,pCodecCtx->width, pCodecCtx->height,1);

	//创建 一个 AVPacket 对象，AVPacket是压缩的数据包
	packet=(AVPacket *)av_malloc(sizeof(AVPacket));



	//初始化 img_convert_ctx 对象
	/**
	 * // 初始化sws_scale
			struct SwsContext *sws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat,
											  int dstW, int dstH, enum AVPixelFormat dstFormat,
											  int flags,
											  SwsFilter *srcFilter, SwsFilter *dstFilter, const double *param);
			参数int srcW, int srcH, enum AVPixelFormat srcFormat定义输入图像信息（?、高、颜色空间）
			参数int dstW, int dstH, enum AVPixelFormat dstFormat定义输出图像信息。
			参数int flags选择缩放算法（只有当输入输出图像大小不同时有效）
			参数SwsFilter *srcFilter, SwsFilter *dstFilter分别定义输入/输出图像滤波器信息，如果不做前后图像滤波，输入NULL
			参数const double *param定义特定缩放算法需要的参数(?)，默认为NULL
			函数返回SwsContext结构体，定义了基本变换信息。
			如果是对一个序列的所有帧做相同的处理，函数sws_getContext只需要调用一次就可以了。
			sws_getContext(w, h, YV12, w, h, NV12, 0, NULL, NULL, NULL);      // YV12->NV12 色彩空间转换
			sws_getContext(w, h, YV12, w/2, h/2, YV12, 0, NULL, NULL, NULL);  // YV12图像缩小到原图1/4
			sws_getContext(w, h, YV12, 2w, 2h, YN12, 0, NULL, NULL, NULL);    // YV12图像放大到原图4倍，并转换为NV12结构
	 */
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
	width,height, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);//SWS_BICUBIC

	/*mVideoSwsContext = sws_getContext(mVideoWidth, mVideoHeight,
				stream->codec->pix_fmt, mVideoWidth, mVideoHeight, PIX_FMT_RGB565, //PIX_FMT_RGB565,gmq switch 565 to 24
				SWS_BILINEAR, NULL, NULL, NULL);*/


  sprintf(info,   "[Input     ]%s\n", input_str);
  sprintf(info, "%s[Output    ]%s\n",info,output_str);
  sprintf(info, "%s[Format    ]%s\n",info, pFormatCtx->iformat->name);
  sprintf(info, "%s[Codec     ]%s\n",info, pCodecCtx->codec->name);
  sprintf(info, "%s[Resolution]%dx%d\n",info, pCodecCtx->width,pCodecCtx->height);



	frame_cnt=0;
	time_start = clock();

	LOGE("begin decode 30000");

	/**
	 * av_read_frame()使用方法在注释中写得很详细，用中文简单描述一下它的两个参数：
		s：输入的AVFormatContext
		pkt：输出的AVPacket
		如果返回0则说明读取正常
	 */



	while(av_read_frame(pFormatCtx, packet)>=0)
	{
		if(isClose) break;
		if( packet->stream_index == videoindex )
		{
/*
			*/

			/**
			 * ffmpeg中的avcodec_decode_video2()的作用是解码一帧视频数据。
			 * 输入一个压缩编码的结构体AVPacket，输出一个解码后的结构体AVFrame。
			 * 该函数的声明位于libavcodec\avcodec.h，如下所示。
			 * @param[in,out] got_picture_ptr Zero if no frame could be decompressed, otherwise, it is nonzero.
			 */
			ret = avcodec_decode_video2(pCodecCtx,pFrameYUV , &got_picture, packet);

			if(count ++ % 5 != 0) continue;

						if(count > 1000000) count = 1;
			if(ret < 0){
				LOGE("Decode Error.\n");
				return -1;
			}
			if(got_picture)
			{
//				ScaleImg(pCodecCtx,pFrame,pFrameYUV,height ,width);
				/*if(pFrame->pict_type != AV_PICTURE_TYPE_I)
					continue;*/

				//把 从 流里面拿的 frame转换为 我们指定的 frame
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrameYUV->data, pFrameYUV->linesize, 0,pFrameYUV->height,
						mVideoPicture.data, mVideoPicture.linesize);

//				y_size=pCodecCtx->width*pCodecCtx->height;
				y_size = mVideoPicture.linesize[0] *height;
				imgAlgorithm(mVideoPicture.data[0],width,height,y_size);
//				LOGE("begin decode 30009,%d",y_size);
				if(rgbArray == NULL )
				{
					rgbArray = env->NewByteArray(y_size );
				}
//				LOGE("begin decode 30100");
				env->SetByteArrayRegion(rgbArray,0, y_size,(jbyte *) mVideoPicture.data[0]);
//				LOGE("begin decode 30101");
				env->CallVoidMethod(jcb,jCallBackID,rgbArray,width,height);
//				LOGE("begin decode 30102");

				//Output info

//				LOGI("Frame Index: %5d. Type:%s",frame_cnt,pictype_str);
				frame_cnt++;
			}
		}
		av_free_packet(packet);
	}
	//flush decoder
	//FIX: Flush Frames remained in Codec
	//感觉 下面的是 无用的
/*	while (1) {
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
		if (ret < 0)
			break;
		if (!got_picture)
			break;
		sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
			pFrameYUV->data, pFrameYUV->linesize);
		int y_size=pCodecCtx->width*pCodecCtx->height;
		fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y
		fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
		fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
		//Output info
		char pictype_str[10]={0};
		switch(pFrame->pict_type)
		{
			case AV_PICTURE_TYPE_I:sprintf(pictype_str,"I");break;
		  case AV_PICTURE_TYPE_P:sprintf(pictype_str,"P");break;
			case AV_PICTURE_TYPE_B:sprintf(pictype_str,"B");break;
			default:sprintf(pictype_str,"Other");break;
		}
		LOGI("Frame Index2: %5d. Type:%s",frame_cnt,pictype_str);
		frame_cnt++;
	}*/

	time_finish = clock();
	time_duration=(double)(time_finish - time_start);

	sprintf(info, "%s[Time      ]%fms\n",info,time_duration);
	sprintf(info, "%s[Count     ]%d\n",info,frame_cnt);

	sws_freeContext(img_convert_ctx);

	avpicture_free(&mVideoPicture);
	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}

#ifdef __cplusplus
}
#endif
