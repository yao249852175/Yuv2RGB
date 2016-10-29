/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_ron_tramacdemo_MyVideo */

#ifndef _Included_com_ron_tramacdemo_MyVideo
#define _Included_com_ron_tramacdemo_MyVideo
#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <time.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/log.h"
#include "libavutil/imgutils.h"
#include "ParkingInterface.h"
#include "Arith_GlobalVar.h"
#include "Arith_TrackObject.h"
/*
 * Class:     com_ron_tramacdemo_MyVideo
 * Method:    initDecoder
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_ron_tramacdemo_MyVideo_initDecoder
  (JNIEnv *, jobject, jstring, jstring,jobject);

/*
 * Class:     com_ron_tramacdemo_MyVideo
 * Method:    beginDecoder
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_ron_tramacdemo_MyVideo_beginDecoder
  (JNIEnv *, jobject);

/*
 * Class:     com_ron_tramacdemo_MyVideo
 * Method:    endDecoder
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_ron_tramacdemo_MyVideo_endDecoder
  (JNIEnv *, jobject);

/*
 * Class:     com_ron_tramacdemo_MyVideo
 * Method:    setCB
 * Signature: (Lcom/ron/tramacdemo/DecoderCallBack;)V
 */
JNIEXPORT void JNICALL Java_com_ron_tramacdemo_MyVideo_setCB
  (JNIEnv *, jobject, jobject);


int decode
 (JNIEnv *env, jobject obj, jstring input_jstr, jstring output_jstr,jobject jcb);

/**
 * 图像跟踪算法，并把 算法得到的框 和  rgb合并
 */
void imgAlgorithm(unsigned char *rgb, int width, int height,int length);

void rgb565_to_rgb24(unsigned char *rgb16,unsigned char * rgb24,int width,int height);

int ScaleImg(AVCodecContext *pCodecCtx,AVFrame *src_picture,AVFrame *dst_picture,int nDstH ,int nDstW );

#ifdef __cplusplus
}
#endif
#endif