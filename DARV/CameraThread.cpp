#include "stdafx.h"
#include "CameraThread.h"
#include "CameraImageLoader.h"

CameraThread::CameraThread(CameraImageLoader* loader)
{
	_loader = loader;
	_capture = 0;
	_hasStoped = false;
}

CameraThread::~CameraThread()
{
	if(_capture != 0)
		cvReleaseCapture( &_capture);
}

void CameraThread::SetCamCapture(CvCapture*capture, QString camID)
{
	_capture = capture;
	_camID = camID;
}

void CameraThread::run()
{
	if(_capture != 0)
	{
		_camImage = cvQueryFrame(_capture);
		if(!_hasStoped)
			emit _loader->CameraImageUpdated(_camImage,_camID, false);
	}
}


ImageFileThread::ImageFileThread(CameraImageLoader* loader, QString path, QString camID)
{
	_loader = loader;
	_camID = camID;
	_imagePath = path;
}

void ImageFileThread::run()
{
	if(_camID != "" && _imagePath != "")
	{
		_image = cvLoadImage(_imagePath.toUtf8().constData());
		if(_image!=0)
		{
			emit _loader->CameraImageUpdated(_image,_camID, false);
			cvReleaseImage(&_image);
		}
	}
}