#pragma once
#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "CameraThread.h"

class CameraImageLoader: public QObject
{
	Q_OBJECT
public:
	CameraImageLoader();
	~CameraImageLoader();
	bool LoadImageFile(QString path, QString camID);
	bool IsCameraStarted(QString camId){return (GetThread(camId) != 0);}
	QString StartCamera(int index, int resX, int resY, int fps, bool forceStart = false);
	QString StartCamera(QString camID, bool forceStart = false){
		int index = 0;
		int resX = 0;
		int resY = 0;
		int fps = 0;
		GetCamDataFromID(camID, &index, &resX, &resY, &fps);
		return StartCamera(index, resX, resY, fps, forceStart);
	}
	bool StopCamera(QString camID);
	
	void Update();

signals:
	void CameraImageUpdated(IplImage* image, QString camId, bool forceViewUpdate);


private:
	QList<CameraThread*> _threads;
	QList<ImageFileThread*> _imageFileThreads;
	CameraThread*	GetThread(QString id);
	QString GetCamID(int index, int resX, int resY, int fps){return QString::number(index)+"_"+QString::number(resX)+"_"+QString::number(resY)+"_"+QString::number(fps);};
	void GetCamDataFromID(QString id, int* index, int* resX, int* resY, int* fps)
	{
		QStringList datas = id.split("_");
		*index = datas[0].toInt();
		*resX = datas[1].toInt();
		*resY = datas[2].toInt();
		*fps = datas[3].toInt();
	}
};

