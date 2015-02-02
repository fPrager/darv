#pragma once
#include <stdafx.h>
#include <QDir>
#include <QtXml/QXmlReader>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <CameraImageLoader.h>

using namespace Eigen;

class TrackingEntries{
public:
	QList<int>	TimeStamps;
	QList<bool> IsTracked;
	QList<Vector3f*> Positions;
	QList<Vector3f*> RefPPositions;
	QList<Quaternionf*> Rotations;
	QList<Quaternionf*> RefRotations;

	TrackingEntries()
	{
		TimeStamps = QList<int>();
		IsTracked = QList<bool>();
		Positions = QList<Vector3f*>();
		Rotations = QList<Quaternionf*>();
	}
	~TrackingEntries()
	{}

	TrackingEntries(int timeStamp, bool isTracked, Vector3f* position, Quaternionf* rotation)
	{
		TimeStamps = QList<int>();
		IsTracked = QList<bool>();
		Positions = QList<Vector3f*>();
		Rotations = QList<Quaternionf*>();
		AddEntry(timeStamp, isTracked, position, rotation);
	}

	void AddEntry(int timeStamp, bool isTracked, Vector3f* position, Quaternionf* rotation)
	{
		TimeStamps.append(timeStamp);
		IsTracked.append(isTracked);
		Positions.append(position);
		Rotations.append(rotation);
	}

	bool GetEntry(int timeStamp, bool* isTracked, Vector3f* position, Quaternionf* rotation)
	{
		if(TimeStamps.contains(timeStamp))
		{
			int index = TimeStamps.indexOf(timeStamp);
			*isTracked = IsTracked[index];
			*position = *Positions[index];
			*rotation = *Rotations[index];
			return true;
		}
		return false;
	}

	int Count()
	{
		return TimeStamps.count();
	}
};

class Record
{
public:
	QMap<int, TrackingEntries> MarkerData;
	QMap<int, TrackingEntries> MarkerRefData;
	TrackingEntries ReferenceData;
	QList<int>		TimeStamps;
	QList<Quaternionf*>	IMURotations;
	QList<float>	IMUMagnitudes;
	QList<float>	IMURotationSpeed;
	bool	HasReferenceData;
	bool	HasLeftCamImages;
	QStringList AvailableLeftCamImages;
	QList<int>	AvailableLeftCamTimeStamps;
	bool	HasRightCamImages;
	QStringList AvailableRightCamImages;
	QList<int>	AvailableRightCamTimeStamps;
	bool	HasIMUData;
	int		MarkerCnt;
	QString Date;
	QString Time;
	float	DurationMS;
	QString ID;
	int CurrentTimeIndex;
	int CurrentCamIndex;
	Record()
	{
		MarkerCnt = 0;
		Date = "";
		Time = "";
		DurationMS = 0;
		//MarkerData = QMap<int, TrackingEntries>();
		ReferenceData = TrackingEntries();
		TimeStamps = QList<int>();
		HasReferenceData = false;
		HasLeftCamImages = false;
		HasRightCamImages = false;
		HasIMUData = false;
		CurrentTimeIndex = 0;
		IMURotations = QList<Quaternionf*>();
		IMUMagnitudes = QList<float>();
		IMURotationSpeed = QList<float>();
		AvailableLeftCamImages = QStringList();
		AvailableRightCamImages = QStringList();
	}

	~Record()
	{}

	int GetTimeIndex(int elapsedTime, bool* overLoad)
	{
		if(TimeStamps.last() < elapsedTime)
		{
			*overLoad = true;
			return 0;
		}
		if(TimeStamps.first() > elapsedTime)
			return 0;
		for(int i = 0; i< TimeStamps.count(); i++)
		{
			if(TimeStamps[i] <= elapsedTime && TimeStamps[i+1] > elapsedTime)
				return i;
		}
		return 0;
	}

	int GetCamTimeStamp(int timeIndex, bool* hasCamImages)
	{
		*hasCamImages = true;
		if(!HasLeftCamImages && !HasRightCamImages)
		{
			*hasCamImages = false;
			return 0;
		}
		int timeStamp = TimeStamps[timeIndex];
		QList<int> camList = QList<int>();
		if(HasLeftCamImages)
			camList = AvailableLeftCamTimeStamps;
		else
			camList = AvailableRightCamTimeStamps;
		
		if(camList.size() == 1)
			return camList[0];
		for(int i = 1; i < camList.size(); i++)
		{
			if(timeStamp >= camList[i-1] && timeStamp<camList[i])
				return camList[i];
		}
		return camList.last();
	}

	void AddMarkerData(int id, int timeStamp, bool isTracked, Vector3f* position, Quaternionf* rotation)
	{
		if(MarkerData.keys().contains(id))
			MarkerData[id].AddEntry(timeStamp,isTracked, position, rotation);
		else
			MarkerData.insert(id, TrackingEntries(timeStamp,isTracked, position, rotation));
	}

	void AddMarkerRefData(int id, int timeStamp, bool hasReference, Vector3f* position, Quaternionf* rotation)
	{
		if(MarkerRefData.keys().contains(id))
			MarkerRefData[id].AddEntry(timeStamp,hasReference, position, rotation);
		else
			MarkerRefData.insert(id, TrackingEntries(timeStamp,hasReference, position, rotation));
	}
	
	void AddReferenceData(int timeStamp, bool isTracked, Vector3f* position, Quaternionf* rotation)
	{
		ReferenceData.AddEntry(timeStamp, isTracked, position, rotation);
	}

	bool GetMarkerEntry(int id, int timeStamp, bool* isTracked, Vector3f* position, Quaternionf* rotation)
	{
		if(MarkerData.keys().contains(id))
			return MarkerData[id].GetEntry(timeStamp, isTracked, position, rotation);
		else
			return false;
	}

	bool GetMarkerRefEntry(int id, int timeStamp, bool* hasReference, Vector3f* position, Quaternionf* rotation)
	{
		if(MarkerRefData.keys().contains(id))
			return MarkerRefData[id].GetEntry(timeStamp, hasReference, position, rotation);
		else
			return false;
	}

	bool GetReferenceEntry(int timeStamp, bool* isTracked, Vector3f* position, Quaternionf* rotation)
	{
		if(!HasReferenceData) return false;
		return ReferenceData.GetEntry(timeStamp, isTracked, position, rotation);
	}

	void FillCamTimeStamps()
	{
		foreach(QString fileName, AvailableLeftCamImages)
		{
			bool isNumber = false;
			int timeStamp = fileName.remove(".bmp").toInt(&isNumber);
			if(isNumber)
				AvailableLeftCamTimeStamps.append(timeStamp);
		}
		qSort(AvailableLeftCamTimeStamps);
		foreach(QString fileName, AvailableRightCamImages)
		{
			bool isNumber = false;
			int timeStamp = fileName.remove(".bmp").toInt(&isNumber);
			if(isNumber)
				AvailableRightCamTimeStamps.append(timeStamp);
		}
		qSort(AvailableRightCamTimeStamps);
	}

	float GetCamRotationAngle()
	{
		float rotationSpeed = IMURotationSpeed[CurrentTimeIndex];
		float elapsedTime = 0;	//elapsedTime in record!
		if(CurrentTimeIndex > 0 && TimeStamps.size()>1)
			elapsedTime = TimeStamps[CurrentTimeIndex]-TimeStamps[CurrentTimeIndex-1];
		return rotationSpeed * (elapsedTime/1000.f);
	}
};

#define DATA_SUFIX			QString(".xml")
#define METAFILE_PREFIX		QString("MetaData")
#define MARKERFILE_PREFIX	QString("MarkerData")
#define IMUFILE_PREFIX		QString("IMUData")
#define REFERENCEFILE_PREFIX QString("ReferenceData")

class MetaFileHandler: public QXmlDefaultHandler
{
	public:
		MetaFileHandler(Record* target):QXmlDefaultHandler()
		{
			_target = target;
			_currentElement = "";
		};

		~MetaFileHandler(){};
		Record* _target;
		QString _currentElement;

		bool startElement(const QString & namespaceURI, const QString & localName, 
					const QString & qName, const QXmlAttributes & atts )
		{_currentElement = localName; return true;}
		bool characters(const QString &str);
};

class IMUFileHandler: public QXmlDefaultHandler
{
	public:
		IMUFileHandler(Record* target):QXmlDefaultHandler()
		{
			_target = target;
			_currentElement = "";
		};

		~IMUFileHandler(){};
		Record* _target;
		QString _currentElement;

		Quaternionf* _currentRotation;
		float		_currentMagnitude;
		float		_currentRotationSpeed;

		bool startElement(const QString & namespaceURI, const QString & localName, 
					const QString & qName, const QXmlAttributes & atts )
		{_currentElement = localName; return true;}
		bool characters(const QString &str);
};

class MarkerFileHandler: public QXmlDefaultHandler
{
	public:
		MarkerFileHandler(int markerIndex, Record* target):QXmlDefaultHandler()
		{
			_target = target;
			_markerIndex = markerIndex+1;
			_currentElement = "";
		};

		~MarkerFileHandler(){};
		Record* _target;
		QString _currentElement;
		int _markerIndex;

		Quaternionf* _currentRotation;
		Quaternionf* _currentRefRotation;

		Vector3f*	_currentPosition;
		Vector3f*	_currentRefPosition;
		bool _hasReference;

		int		_timeStamp;
		bool	_isTracked;

		bool startElement(const QString & namespaceURI, const QString & localName, 
					const QString & qName, const QXmlAttributes & atts )
		{_currentElement = localName; return true;}

		bool characters(const QString &str);
};

class ReferenceFileHandler: public QXmlDefaultHandler
{
	public:
		ReferenceFileHandler(Record* target):QXmlDefaultHandler()
		{
			_target = target;
			_currentElement = "";
		};

		~ReferenceFileHandler(){};
		Record* _target;
		QString _currentElement;
		int _markerIndex;

		Quaternionf* _currentRotation;
		Vector3f*	_currentPosition;
		int		_timeStamp;
		bool	_isTracked;

		bool startElement(const QString & namespaceURI, const QString & localName, 
					const QString & qName, const QXmlAttributes & atts )
		{_currentElement = localName; return true;}

		bool characters(const QString &str);
};


class RecordLoader
{
public:
	RecordLoader(void);
	~RecordLoader(void);

	QStringList UpdateRecordList();
	bool LoadRecord(QString id);
	bool GetCameraImages(IplImage** left, IplImage** right, QString timeStamp = "");
	bool GetCameraImages(CameraImageLoader* _imageLoader, QString camIDLeft, QString camIDRight, QString timeStamp = "");
	Record _loadedRecord;
private:
	bool LoadReferenceData(QFile* data);
	bool LoadIMUData(QFile* data);
	bool LoadMarkerData(int id, QFile* data);
	bool LoadMetaData(QFile* data);
};

