
//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "arcsoft_face_sdk.h"
#include "amcomdef.h"
#include "asvloffscreen.h"
#include "merror.h"
#include <direct.h>
#include <iostream>  
#include <stdarg.h>
#include <string>
#include "opencv2/opencv.hpp"
#include <windows.h>
#include <time.h>


#pragma comment(lib, "libarcsoft_face_engine.lib")

using namespace std;

#define APPID "9Xr7QtQi1igfFzmgkaRe5wjbxymVd4J17GNwFcT4mGJF"
#define SDKKEY "3betTutDEH1TaT8A39eFXymCNPYkP3HtGNRPTGz47pq8"


#define SafeFree(p) { if ((p)) free(p); (p) = NULL; }
#define SafeArrayDelete(p) { if ((p)) delete [] (p); (p) = NULL; } 
#define SafeDelete(p) { if ((p)) delete (p); (p) = NULL; } 

string gen[5] = { "male","female" };

//裁剪图片
void CutIplImage(IplImage* src, IplImage* dst, int x, int y)
{
	CvSize size = cvSize(dst->width, dst->height);//区域大小
	cvSetImageROI(src, cvRect(x, y, size.width, size.height));//设置源图像ROI
	cvCopy(src, dst); //复制图像
	cvResetImageROI(src);//源图像用完后，清空ROI
}

//时间戳转换为日期格式
void timestampToTime(char* timeStamp, char* dateTime, int dateTimeSize)
{
	time_t tTimeStamp = atoll(timeStamp);
	struct tm* pTm = gmtime(&tTimeStamp);
	strftime(dateTime, dateTimeSize, "%Y-%m-%d %H:%M:%S", pTm);
}

extern "C" bool __declspec(dllexport) img_matching(char* i1, char* i2);
extern "C" int __declspec(dllexport) img_age(char* i2);
	bool img_matching(char* i1, char* i2)
	{
		MRESULT res = MOK;
		ASF_ActiveFileInfo activeFileInfo = { 0 };
		res = ASFGetActiveFileInfo(&activeFileInfo);
		if (res != MOK)
		{
			printf("ASFGetActiveFileInfo fail: %d\n", res);
		}
		else
		{
			char startDateTime[32];
			timestampToTime(activeFileInfo.startTime, startDateTime, 32);
			printf("Time: %s\n", startDateTime);
			/*
			char endDateTime[32];
			timestampToTime(activeFileInfo.endTime, endDateTime, 32);
			printf("endTime: %s\n", endDateTime);
			*/
		}
		/*
		const ASF_VERSION version = ASFGetVersion();
		printf("\nVersion:%s\n", version.Version);
		printf("BuildDate:%s\n", version.BuildDate);
		printf("CopyRight:%s\n", version.CopyRight);
		*/
		//api的一些版本介绍跟授权
		//printf("\n************* Face matching *****************\n");

		//激活接口,首次激活需联网
		res = ASFOnlineActivation(APPID, SDKKEY);
		if (MOK != res && MERR_ASF_ALREADY_ACTIVATED != res)
			printf("ASFActivation fail: %d\n", res);
		/*
		else
			printf("ASFActivation sucess: %d\n", res);
		*/
		//初始化接口
		MHandle handle = NULL;
		MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE |
			ASF_LIVENESS | ASF_IR_LIVENESS;
		res = ASFInitEngine(ASF_DETECT_MODE_IMAGE, ASF_OP_0_ONLY, 30, 5, mask, &handle);
		if (res != MOK)
			printf("ASFInitEngine fail: %d\n", res);
		/*
		else
			printf("ASFInitEngine sucess: %d\n", res);
		*/

		// 人脸检测
		const char* c1 = i1, * c2 = i2;
		IplImage* img1 = cvLoadImage(c1);
		IplImage* img2 = cvLoadImage(c2);

		if (img1 && img2)
		{
			printf("image loading success\n");
			//第一张图片特征提取
			ASF_MultiFaceInfo detectedFaces1 = { 0 };
			ASF_SingleFaceInfo SingleDetectedFaces1 = { 0 };
			ASF_FaceFeature feature1 = { 0 };
			ASF_FaceFeature copyfeature1 = { 0 };
			IplImage* cutImg1 = cvCreateImage(cvSize(img1->width - img1->width % 4, img1->height), IPL_DEPTH_8U, img1->nChannels);
			CutIplImage(img1, cutImg1, 0, 0);

			ASVLOFFSCREEN offscreen1 = { 0 };
			offscreen1.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
			offscreen1.i32Width = cutImg1->width;
			offscreen1.i32Height = cutImg1->height;
			offscreen1.pi32Pitch[0] = cutImg1->widthStep;
			offscreen1.ppu8Plane[0] = (MUInt8*)cutImg1->imageData;

			res = ASFDetectFacesEx(handle, &offscreen1, &detectedFaces1);
			if (MOK == res && detectedFaces1.faceNum > 0)
			{
				SingleDetectedFaces1.faceRect.left = detectedFaces1.faceRect[0].left;
				SingleDetectedFaces1.faceRect.top = detectedFaces1.faceRect[0].top;
				SingleDetectedFaces1.faceRect.right = detectedFaces1.faceRect[0].right;
				SingleDetectedFaces1.faceRect.bottom = detectedFaces1.faceRect[0].bottom;
				SingleDetectedFaces1.faceOrient = detectedFaces1.faceOrient[0];

				res = ASFFaceFeatureExtractEx(handle, &offscreen1, &SingleDetectedFaces1, &feature1);
				if (res == MOK)
				{
					//拷贝feature
					copyfeature1.featureSize = feature1.featureSize;
					copyfeature1.feature = (MByte*)malloc(feature1.featureSize);
					memset(copyfeature1.feature, 0, feature1.featureSize);
					memcpy(copyfeature1.feature, feature1.feature, feature1.featureSize);
				}
				else
					printf("ASFFaceFeatureExtract 1 fail: %d\n", res);
			}
			else
				printf("ASFDetectFaces 1 fail: %d\n", res);


			//第二张图片提取特征
			ASF_MultiFaceInfo	detectedFaces2 = { 0 };
			ASF_SingleFaceInfo SingleDetectedFaces2 = { 0 };
			ASF_FaceFeature feature2 = { 0 };
			IplImage* cutImg2 = cvCreateImage(cvSize(img2->width - img2->width % 4, img2->height), IPL_DEPTH_8U, img2->nChannels);
			CutIplImage(img2, cutImg2, 0, 0);

			ASVLOFFSCREEN offscreen2 = { 0 };
			offscreen2.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
			offscreen2.i32Width = cutImg2->width;
			offscreen2.i32Height = cutImg2->height;
			offscreen2.pi32Pitch[0] = cutImg2->widthStep;
			offscreen2.ppu8Plane[0] = (MUInt8*)cutImg2->imageData;

			res = ASFDetectFacesEx(handle, &offscreen2, &detectedFaces2);
			if (MOK == res && detectedFaces2.faceNum > 0)
			{
				SingleDetectedFaces2.faceRect.left = detectedFaces2.faceRect[0].left;
				SingleDetectedFaces2.faceRect.top = detectedFaces2.faceRect[0].top;
				SingleDetectedFaces2.faceRect.right = detectedFaces2.faceRect[0].right;
				SingleDetectedFaces2.faceRect.bottom = detectedFaces2.faceRect[0].bottom;
				SingleDetectedFaces2.faceOrient = detectedFaces2.faceOrient[0];

				res = ASFFaceFeatureExtractEx(handle, &offscreen2, &SingleDetectedFaces2, &feature2);
				if (MOK != res)
					printf("ASFFaceFeatureExtractEx 2 fail: %d\n", res);
			}
			else
				printf("ASFDetectFacesEx 2 fail: %d\n", res);


			// 单人脸特征比对
			MFloat confidenceLevel;
			res = ASFFaceFeatureCompare(handle, &copyfeature1, &feature2, &confidenceLevel);
			if (res != MOK)
				printf("ASFFaceFeatureCompare fail: %d\n", res);
			else
				printf("ASFFaceFeatureCompare sucess\nconfidence level is: %lf\n", confidenceLevel);

			SafeFree(copyfeature1.feature);		//释放内存
			cvReleaseImage(&cutImg1);
			cvReleaseImage(&cutImg2);

			//判断是否匹配
			if (confidenceLevel > 0.8) return true;
			else return false;
			/*
			//设置活体置信度 SDK内部默认值为 IR：0.7  RGB：0.5（无特殊需要，可以不设置）
			ASF_LivenessThreshold threshold = { 0 };
			threshold.thresholdmodel_BGR = 0.5;
			threshold.thresholdmodel_IR = 0.7;
			res = ASFSetLivenessParam(handle, &threshold);
			if (res != MOK)
				printf("ASFSetLivenessParam fail: %d\n", res);
			else
				printf("RGB Threshold: %f  IR Threshold: %f\n", threshold.thresholdmodel_BGR, threshold.thresholdmodel_IR);

			printf("\n*************** the infomation of second face ***************\n");

			// RGB图像检测，获取第二张图片的人脸信息
			MInt32 processMask = ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS;
			res = ASFProcessEx(handle, &offscreen2, &detectedFaces2, processMask);
			if (res != MOK)
				printf("ASFSProcessEx fail: %d\n", res);
			else
				printf("ASFProcessEx sucess: %d\n", res);

			// 获取年龄
			ASF_AgeInfo ageInfo = { 0 };
			res = ASFGetAge(handle, &ageInfo);
			if (res != MOK)
				printf("ASFGetAge fail: %d\n", res);
			else
				printf("Age: %d\n", ageInfo.ageArray[0]);

			// 获取性别
			ASF_GenderInfo genderInfo = { 0 };
			res = ASFGetGender(handle, &genderInfo);
			if (res != MOK)
				printf("ASFGetGender fail: %d\n", res);
			else
				printf("Gender: %d\n", genderInfo.genderArray[0]);

			// 获取3D角度
			/*
			ASF_Face3DAngle angleInfo = { 0 };
			res = ASFGetFace3DAngle(handle, &angleInfo);
			if (res != MOK)
				printf("ASFGetFace3DAngle fail: %d\n", res);
			else
				printf("3DAngle roll: %f   yaw: %f   pitch: %f\n", angleInfo.roll[0], angleInfo.yaw[0], angleInfo.pitch[0]);
			//获取RGB活体信息
			ASF_LivenessInfo rgbLivenessInfo = { 0 };
			res = ASFGetLivenessScore(handle, &rgbLivenessInfo);
			if (res != MOK)
				printf("ASFGetLivenessScore fail: %d\n", res);
			else
				printf("RGB Liveness: %d\n", rgbLivenessInfo.isLive[0]);

			
			printf("\n********** IR Process *************\n");

			ASF_MultiFaceInfo	detectedFaces3 = { 0 };
			IplImage* cutImg3 = cvCreateImage(cvSize(img3->width - img3->width % 4, img3->height), IPL_DEPTH_8U, img3->nChannels);
			CutIplImage(img3, cutImg3, 0, 0);

			//opencv读图时会将灰度图读成RGB图，需要转换成GRAY图进行IR活体检测
			cv::Mat grayMat;
			cv::Mat copyCutImg3(cutImg3, false);	//IplImage转Mat 设为ture为深拷贝
			cv::cvtColor(copyCutImg3, grayMat, CV_BGR2GRAY);

			ASVLOFFSCREEN offscreen3 = { 0 };
			offscreen3.u32PixelArrayFormat = ASVL_PAF_GRAY;
			offscreen3.i32Width = grayMat.cols;
			offscreen3.i32Height = grayMat.rows;
			offscreen3.pi32Pitch[0] = grayMat.step;
			offscreen3.ppu8Plane[0] = grayMat.data;

			res = ASFDetectFacesEx(handle, &offscreen3, &detectedFaces3);
			if (res != MOK && detectedFaces3.faceNum > 1)
				printf("ASFDetectFacesEx fail: %d\n", res);
			else
				printf("Face num: %d\n", detectedFaces3.faceNum);

			//IR图像活体检测
			MInt32 processIRMask = ASF_IR_LIVENESS;
			res = ASFProcessEx_IR(handle, &offscreen3, &detectedFaces3, processIRMask);
			if (res != MOK)
				printf("ASFProcessEx_IR fail: %d\n", res);
			else
				printf("ASFProcessEx_IR sucess: %d\n", res);

			//获取IR活体信息
			ASF_LivenessInfo irLivenessInfo = { 0 };
			res = ASFGetLivenessScore_IR(handle, &irLivenessInfo);
			if (res != MOK)
				printf("ASFGetLivenessScore_IR fail: %d\n", res);
			else
				printf("IR Liveness: %d\n", irLivenessInfo.isLive[0]);
			*/
		}
		else
		{
			printf("loading fail,please check the path\n");
		}
		cvReleaseImage(&img1);
		cvReleaseImage(&img2);
		//cvReleaseImage(&img3);

		//反初始化
		res = ASFUninitEngine(handle);
		if (res != MOK)
			printf("ALUninitEngine fail: %d\n", res);

	}
	int img_age(char* i2)
	{
		MRESULT res = MOK;
		ASF_ActiveFileInfo activeFileInfo = { 0 };
		res = ASFGetActiveFileInfo(&activeFileInfo);
		if (res != MOK)
		{
			printf("ASFGetActiveFileInfo fail: %d\n", res);
		}
		else
		{
			char startDateTime[32];
			timestampToTime(activeFileInfo.startTime, startDateTime, 32);
			printf("Time: %s\n", startDateTime);
			/*
			char endDateTime[32];
			timestampToTime(activeFileInfo.endTime, endDateTime, 32);
			printf("endTime: %s\n", endDateTime);
			*/
		}
		/*
		const ASF_VERSION version = ASFGetVersion();
		printf("\nVersion:%s\n", version.Version);
		printf("BuildDate:%s\n", version.BuildDate);
		printf("CopyRight:%s\n", version.CopyRight);
		*/
		//激活接口,首次激活需联网
		res = ASFOnlineActivation(APPID, SDKKEY);
		if (MOK != res && MERR_ASF_ALREADY_ACTIVATED != res)
			printf("ASFActivation fail: %d\n", res);
		/*
		else
			printf("ASFActivation sucess: %d\n", res);
		*/
		//初始化接口
		MHandle handle = NULL;
		MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE |
			ASF_LIVENESS | ASF_IR_LIVENESS;
		res = ASFInitEngine(ASF_DETECT_MODE_IMAGE, ASF_OP_0_ONLY, 30, 5, mask, &handle);
		if (res != MOK)
			printf("ASFInitEngine fail: %d\n", res);
		/*
		else
			printf("ASFInitEngine sucess: %d\n", res);
		*/

		// 人脸检测
		const char* c2 = i2;
		IplImage* img2 = cvLoadImage(c2);

		if (img2)
		{
			printf("image loading success\n");
			//第一张图片特征提取
			/*
			ASF_MultiFaceInfo detectedFaces1 = { 0 };
			ASF_SingleFaceInfo SingleDetectedFaces1 = { 0 };
			ASF_FaceFeature feature1 = { 0 };
			ASF_FaceFeature copyfeature1 = { 0 };
			IplImage* cutImg1 = cvCreateImage(cvSize(img1->width - img1->width % 4, img1->height), IPL_DEPTH_8U, img1->nChannels);
			CutIplImage(img1, cutImg1, 0, 0);

			ASVLOFFSCREEN offscreen1 = { 0 };
			offscreen1.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
			offscreen1.i32Width = cutImg1->width;
			offscreen1.i32Height = cutImg1->height;
			offscreen1.pi32Pitch[0] = cutImg1->widthStep;
			offscreen1.ppu8Plane[0] = (MUInt8*)cutImg1->imageData;

			res = ASFDetectFacesEx(handle, &offscreen1, &detectedFaces1);
			if (MOK == res && detectedFaces1.faceNum > 0)
			{
				SingleDetectedFaces1.faceRect.left = detectedFaces1.faceRect[0].left;
				SingleDetectedFaces1.faceRect.top = detectedFaces1.faceRect[0].top;
				SingleDetectedFaces1.faceRect.right = detectedFaces1.faceRect[0].right;
				SingleDetectedFaces1.faceRect.bottom = detectedFaces1.faceRect[0].bottom;
				SingleDetectedFaces1.faceOrient = detectedFaces1.faceOrient[0];

				res = ASFFaceFeatureExtractEx(handle, &offscreen1, &SingleDetectedFaces1, &feature1);
				if (res == MOK)
				{
					//拷贝feature
					copyfeature1.featureSize = feature1.featureSize;
					copyfeature1.feature = (MByte*)malloc(feature1.featureSize);
					memset(copyfeature1.feature, 0, feature1.featureSize);
					memcpy(copyfeature1.feature, feature1.feature, feature1.featureSize);
				}
				else
					printf("ASFFaceFeatureExtract 1 fail: %d\n", res);
			}
			else
				printf("ASFDetectFaces 1 fail: %d\n", res);

			*/
			//第二张图片提取特征
			ASF_MultiFaceInfo	detectedFaces2 = { 0 };
			ASF_SingleFaceInfo SingleDetectedFaces2 = { 0 };
			ASF_FaceFeature feature2 = { 0 };
			IplImage* cutImg2 = cvCreateImage(cvSize(img2->width - img2->width % 4, img2->height), IPL_DEPTH_8U, img2->nChannels);
			CutIplImage(img2, cutImg2, 0, 0);

			ASVLOFFSCREEN offscreen2 = { 0 };
			offscreen2.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
			offscreen2.i32Width = cutImg2->width;
			offscreen2.i32Height = cutImg2->height;
			offscreen2.pi32Pitch[0] = cutImg2->widthStep;
			offscreen2.ppu8Plane[0] = (MUInt8*)cutImg2->imageData;

			res = ASFDetectFacesEx(handle, &offscreen2, &detectedFaces2);
			if (MOK == res && detectedFaces2.faceNum > 0)
			{
				SingleDetectedFaces2.faceRect.left = detectedFaces2.faceRect[0].left;
				SingleDetectedFaces2.faceRect.top = detectedFaces2.faceRect[0].top;
				SingleDetectedFaces2.faceRect.right = detectedFaces2.faceRect[0].right;
				SingleDetectedFaces2.faceRect.bottom = detectedFaces2.faceRect[0].bottom;
				SingleDetectedFaces2.faceOrient = detectedFaces2.faceOrient[0];

				res = ASFFaceFeatureExtractEx(handle, &offscreen2, &SingleDetectedFaces2, &feature2);
				if (MOK != res)
					printf("ASFFaceFeatureExtractEx 2 fail: %d\n", res);
			}
			else
				printf("ASFDetectFacesEx 2 fail: %d\n", res);

			// RGB图像检测，获取第二张图片的人脸信息
			MInt32 processMask = ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE | ASF_LIVENESS;
			res = ASFProcessEx(handle, &offscreen2, &detectedFaces2, processMask);
			if (res != MOK)
				printf("ASFSProcessEx fail: %d\n", res);
			else
				printf("ASFProcessEx sucess: %d\n", res);

			// 获取年龄
			ASF_AgeInfo ageInfo = { 0 };
			res = ASFGetAge(handle, &ageInfo);
			if (res != MOK)
				printf("ASFGetAge fail: %d\n", res);
			else
				printf("Age: %d\n", ageInfo.ageArray[0]);

			cvReleaseImage(&cutImg2);

			return ageInfo.ageArray[0];

			// 获取性别
			/*
			ASF_GenderInfo genderInfo = { 0 };
			res = ASFGetGender(handle, &genderInfo);
			if (res != MOK)
				printf("ASFGetGender fail: %d\n", res);
			else
				printf("Gender: %d\n", genderInfo.genderArray[0]);
			*/
			// 获取3D角度
			/*
			ASF_Face3DAngle angleInfo = { 0 };
			res = ASFGetFace3DAngle(handle, &angleInfo);
			if (res != MOK)
				printf("ASFGetFace3DAngle fail: %d\n", res);
			else
				printf("3DAngle roll: %f   yaw: %f   pitch: %f\n", angleInfo.roll[0], angleInfo.yaw[0], angleInfo.pitch[0]);
			*/

		}
		else
		{
			printf("loading fail,please check the path\n");
		}
		cvReleaseImage(&img2);

		//反初始化
		res = ASFUninitEngine(handle);
		if (res != MOK)
			printf("ALUninitEngine fail: %d\n", res);
}



