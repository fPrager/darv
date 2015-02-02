#include "stdafx.h"
#include "ImageAnalyser.h"

BlurAnalyser::BlurAnalyser()
{
	_imageView = 0;
	_imuManager = 0;
	SetUp();
}

BlurAnalyser::BlurAnalyser(IMUManager* imuManager)
{
	_imuManager = imuManager;
	//connect(_camLoaderLeft, SIGNAL(ImageReady(IplImage*)), this, SLOT(UpdateLeftCameraImage(IplImage*)));
	SetUp();
}

void BlurAnalyser::SetUp()
{
	_camID = "";
	_sampleCnt = 0;
	_calculatedBlur = 0;
	_isAnalysing = false;
	_newImage =  false;
}

BlurAnalyser::~BlurAnalyser(void)
{
}

bool BlurAnalyser::Start(int trackingCam, int resX, int resY, int fps, CameraImageLoader* camLoader)
{
	_camID = camLoader->StartCamera(trackingCam, resX, resY, fps);
	if(_camID != "")
	{
		qDebug() << "OpenCV successfully initialized the TrackingCamera";
		connect(camLoader,SIGNAL(CameraImageUpdated(IplImage*,QString)),this,SLOT(UpdateCameraImage(IplImage*, QString)));
		_camLoader = camLoader;
	}
	else
	{
		qDebug() << "ERROR: Can't start Camera(" << _camID << ") with OpenCV to analyse motion blur.";
		return false;
	}
	_isAnalysing = true;
	return true;
}

void BlurAnalyser::Update(float elapsedTime)
{
	if(_newImage && _camID != "")
	{
		if(_imageView!=0)
		{
			if(!_imageView->HasTexture())
			{
				//choosing "GL_LUMINACE" as internal texture format instead of "GL_BGR", because we know its a grayscale-picture with just one colorvalue  
				_imageView->MakeTexture(_resultImage->width, _resultImage->height, GL_LUMINANCE  ,GL_UNSIGNED_BYTE,_resultImage->nChannels);
			}
			_imageView->UpdateTexture(_resultImage->imageData);
			_imageView->Draw();
			_newImage = false;
		}
	}

}

bool BlurAnalyser::Stop()
{
	if(_camLoader!=0 && _camID != "")
		_camLoader->StopCamera(_camID);
	_isAnalysing = false;
	return true;
}

void BlurAnalyser::Analyse()
{
	if(_camImage == 0)
		return;
	IplImage* gray = cvCreateImage(cvGetSize(_camImage), IPL_DEPTH_8U, 1);
	cvCvtColor (_camImage, gray,CV_BGR2GRAY); // Convert image to grayscale
	cvThreshold(gray,gray,100,255, CV_THRESH_BINARY);
	//IplImage* smaller =  cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 1);
	//cvResize(gray, smaller);
	//IplImage* smallerRGB =  cvCreateImage(cvSize(320,240),_camImage->depth, _camImage->nChannels);
	//cvCvtColor (smaller, smallerRGB,CV_GRAY2BGR);
	//	IplImage* fullImage = cvCreateImage(cvGetSize(_camImageLeft), _camImageLeft->depth, _camImageLeft->nChannels); 
	//	if(_objectPos.x() == 3)
	//	{
 //			cvCvtColor (_camImageLeft, gray,CV_BGR2GRAY); // Convert image to grayscale
	//		cvThreshold(gray,gray,_objectPos.z(),255, THRESH_BINARY);
	//		cvCvtColor (gray, fullImage,CV_GRAY2BGR);
	//		ConvertIplToTexture(fullImage, _camTextureLeft);
	//	} 
	//	else
	//	{
	_resultImage = gray;
	_newImage = true;
}

void BlurAnalyser::UpdateCameraImage(IplImage* image, QString camID)
{
	if((false &&_imuManager == 0) || camID != _camID)
		return;
	_camImage = image;
	Analyse();
}


