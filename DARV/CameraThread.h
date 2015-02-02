#pragma once

#include <qthread.h>
#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"

class CameraImageLoader;

class CameraThread: public QThread
{
	Q_OBJECT
public: 
	CameraThread(CameraImageLoader* loader);
	~CameraThread();
   void run();
   bool _hasStoped;
   IplImage* _camImage;
   void SetCamCapture(CvCapture* capture, QString camId);
   QString	GetCameraID(){return _camID;};
signals:
     void ImageReady(IplImage* image, QString camID);

private:
	CameraImageLoader* _loader;
	CvCapture* _capture;
	QString	_camID;
};

class ImageFileThread: public QThread
{
	Q_OBJECT
public: 
	ImageFileThread(CameraImageLoader* loader, QString path, QString camID);
	~ImageFileThread(){}
   void run();

   IplImage* _image;
   QString	GetCameraID(){return _camID;};
   void ChangePath(QString path){_imagePath = path;}
signals:
     void ImageReady(IplImage* image, QString camID);

private:
	CameraImageLoader* _loader;
	QString _imagePath;
	QString _camID;
};
