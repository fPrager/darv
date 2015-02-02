#pragma once

#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "IMUManager.h"
#include "CameraImageLoader.h"
#include "ImageView.h"

class BlurAnalyser: public QObject
{
	Q_OBJECT
public:
	BlurAnalyser();
	BlurAnalyser(IMUManager* imuManager);
	~BlurAnalyser(void);

	bool Start(int trackingCam, int resX, int resY, int fps, CameraImageLoader* camLoader);
	void Update(float elapsedTime);
	void UseThisViewer(ImageView* view){_imageView = view;};
	bool Stop();

	void		SetIMUManager(IMUManager* imuManager){_imuManager = imuManager;};
	int			GetSampleCnt(){return _sampleCnt;};
	float		GetCurrentBlur(){return _calculatedBlur;};
	ImageView*	GetImageView(){return _imageView;};
	bool		_isAnalysing;

public slots:
	void UpdateCameraImage(IplImage* image, QString camID);

private:
	void Analyse();
	void SetUp();

	IplImage*			_camImage;
	IplImage*			_resultImage;
	IMUManager*			_imuManager;
	CameraImageLoader*	_camLoader;
	ImageView*			_imageView;

	QString				_camID;
	int					_sampleCnt;
	float				_calculatedBlur;
	bool				_newImage;
};

