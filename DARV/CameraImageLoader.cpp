#include "stdafx.h"
#include "CameraImageLoader.h"


CameraImageLoader::CameraImageLoader(void)
{
	_threads = QList<CameraThread*>();
	_imageFileThreads = QList<ImageFileThread*>();
}


CameraImageLoader::~CameraImageLoader(void)
{
}

void CameraImageLoader::Update()
{
	foreach(CameraThread* t, _threads)
	{
		if(!t->isRunning())
			t->start();
	}
	int i = _imageFileThreads.size()-1;
	while(i>0)
	{
		if(!_imageFileThreads[i]->isRunning())
			_imageFileThreads.removeAt(i);
		i=i-1;
	}
}

QString CameraImageLoader::StartCamera(int index, int resX, int resY, int fps, bool forceStart)
{
	QString camID = GetCamID(index,resX,resY,fps);
	qDebug() << "starting : " << camID;
	if(GetThread(camID) != 0 && !forceStart)
	{
		qDebug() << "WARNING: Camera with index " << index<< " already started."; 
		return camID;
	}

	CvCapture* cap = 0;
	cap = cvCaptureFromCAM(index);
	if(!cap)
	{
		qDebug() << "ERROR: Can't open camera with index " << index << "!"; 
		return "";
	}
	cvSetCaptureProperty(cap,CV_CAP_PROP_FRAME_WIDTH,resX);
	cvSetCaptureProperty(cap,CV_CAP_PROP_FRAME_HEIGHT,resY);
	cvSetCaptureProperty(cap,CV_CAP_PROP_FPS,fps);

	CameraThread* thread = new CameraThread(this);
	thread->SetCamCapture(cap, camID);
	_threads.append(thread);

	return camID;
}

bool CameraImageLoader::LoadImageFile(QString path, QString camID)
{
	for(int i = 0; i < _imageFileThreads.size(); i++)
	{
		if(_imageFileThreads[i]->GetCameraID() == camID) 
		{
			if(!_imageFileThreads[i]->isRunning())
			{
				_imageFileThreads[i]->ChangePath(path);
				_imageFileThreads[i]->run();
				return true;
			}
			else
				return false;
		}
	}

	ImageFileThread* thread = new ImageFileThread(this, path, camID);
	_imageFileThreads.append(thread);
	thread->run();
	return true;
}

bool CameraImageLoader::StopCamera(QString camID)
{
	//return true;
	bool hasCamera = false;
	foreach(CameraThread* t, _threads)
	{
		if(t->GetCameraID() == camID)
		{
			t->_hasStoped = true;
			t->terminate();
			_threads.removeOne(t);
			hasCamera = true;
		}
	}
	if(!hasCamera)
	{
		qDebug() << "Can not stop a Camera (" << camID << ") which wasn't started!";
		return false;
	}
	else
		return true;
}

CameraThread* CameraImageLoader::GetThread(QString id)
{
	CameraThread* result = 0;
	foreach(CameraThread* t, _threads)
	{
		if(t->GetCameraID() == id)
			result = t;
	}
	return result;
}