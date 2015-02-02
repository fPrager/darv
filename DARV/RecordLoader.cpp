#include "stdafx.h"
#include "RecordLoader.h"


RecordLoader::RecordLoader(void)
{
	_loadedRecord = Record();
}


RecordLoader::~RecordLoader(void)
{
}

bool RecordLoader::LoadRecord(QString id)
{
	QDir directory = QDir("Resources/Records/"+id);
	bool validRecord = true;
	_loadedRecord = Record();
	if(directory.entryList(QStringList("*"+ DATA_SUFIX)).count() < 2)
	{
		qDebug() << "ERROR: Can't load an invalid record-directory (less than two required "+DATA_SUFIX+"-files, "+METAFILE_PREFIX+ DATA_SUFIX +" and "+ MARKERFILE_PREFIX + "0" + DATA_SUFIX + ").";
		validRecord = false;
	}
	else
	{
		_loadedRecord.ID = id;
		if(directory.entryList(QStringList("*"+DATA_SUFIX)).contains(METAFILE_PREFIX+DATA_SUFIX))
		{
			//loading metafile
			QString path = "Resources/Records/"+id +"/"+ METAFILE_PREFIX+DATA_SUFIX;
			qDebug() << "Parsing: " + path;
			QFile data(path);
			if(!LoadMetaData(&data))
				return false;
		}
		else
		{
			qDebug() << "ERROR: Missing file, "+METAFILE_PREFIX+DATA_SUFIX;
			return false;
		}

		if(directory.entryList(QStringList("*"+DATA_SUFIX)).contains(IMUFILE_PREFIX+DATA_SUFIX))
		{
			//loading metafile
			QString path = "Resources/Records/"+id +"/"+ IMUFILE_PREFIX+DATA_SUFIX;
			qDebug() << "Parsing: " + path;
			QFile data(path);
			if(!LoadIMUData(&data))
			{
				return false;
			}
		}

		if(_loadedRecord.MarkerCnt > 0)
		{
			for(int mIndex = 0; mIndex < _loadedRecord.MarkerCnt; mIndex++)
			{
				QString path = "Resources/Records/"+id+"/"+ MARKERFILE_PREFIX+QString::number(mIndex)+DATA_SUFIX;
				qDebug() << "Parsing: " + path;
				QFile data(path);
				if(!LoadMarkerData(mIndex, &data))
					return false;
			}
		}
		else
		{
			qDebug() << "ERROR: Zero Marker in Metafile";
		}
		if(_loadedRecord.HasLeftCamImages)
			_loadedRecord.AvailableLeftCamImages = QDir("Resources/Records/"+_loadedRecord.ID+"/LeftImage").entryList(QStringList("*.bmp"));
		if(_loadedRecord.HasRightCamImages)
			_loadedRecord.AvailableRightCamImages = QDir("Resources/Records/"+_loadedRecord.ID+"/RightImage").entryList(QStringList("*.bmp"));
		_loadedRecord.FillCamTimeStamps();
	}
	return validRecord;
}

QStringList RecordLoader::UpdateRecordList()
{
	QDir directory = QDir("Resources/Records/");
	QStringList records = directory.entryList(QDir::AllDirs|QDir::NoDotAndDotDot);
	return records;
}

bool RecordLoader::GetCameraImages(IplImage** left, IplImage** right, QString timeStamp)
{
	if(timeStamp == "")
	{
		//load placeholder image
		QString path = "Resources/Records/NoImage.bmp";
		*left = cvLoadImage(path.toUtf8().constData());
		*right = *left;
	}
	else if(timeStamp == "Selection")
	{
		//load placeholder image
		QString path = "Resources/Records/Selection.bmp";
		*left = cvLoadImage(path.toUtf8().constData());
		*right = *left;
	}
	else if(timeStamp == "RollBack")
	{
		//load placeholder image
		QString path = "Resources/Records/BackToNormal.bmp";
		*left = cvLoadImage(path.toUtf8().constData());
		*right = *left;
	}
	else if(timeStamp == "Loading")
	{
		//load placeholder image
		QString path = "Resources/Records/LoadingRecord.bmp";
		*left = cvLoadImage(path.toUtf8().constData());
		*right = *left;
	}
	else
	{
		if(_loadedRecord.HasLeftCamImages)
		{
			QString path = "Resources/Records/";
			QDir directory = QDir("Resources/Records/"+_loadedRecord.ID+"/LeftImage");
			if(_loadedRecord.AvailableLeftCamImages.contains(timeStamp+".bmp"))
			{
					*left = cvLoadImage((path+_loadedRecord.ID+"/LeftImage/"+timeStamp+".bmp").toUtf8().constData());
			}
		}
	}
	return true;
}

bool RecordLoader::GetCameraImages(CameraImageLoader* loader, QString camIDLeft, QString camIDRight, QString timeStamp)
{
	if(timeStamp == "")
	{
		//load placeholder image
		QString path = "Resources/Records/NoImage.bmp";
		loader->LoadImageFile(path, camIDLeft);
		if(camIDLeft!=camIDRight)
			loader->LoadImageFile(path, camIDRight);
	}
	else if(timeStamp == "Selection")
	{
		//load placeholder image
		QString path = "Resources/Records/Selection.bmp";
		loader->LoadImageFile(path, camIDLeft);
		if(camIDLeft!=camIDRight)
			loader->LoadImageFile(path, camIDRight);
	}
	else if(timeStamp == "RollBack")
	{
		//load placeholder image
		QString path = "Resources/Records/BackToNormal.bmp";
		loader->LoadImageFile(path, camIDLeft);
		if(camIDLeft!=camIDRight)
			loader->LoadImageFile(path, camIDRight);
	}
	else if(timeStamp == "Loading")
	{
		//load placeholder image
		QString path = "Resources/Records/LoadingRecord.bmp";
		loader->LoadImageFile(path, camIDLeft);
		if(camIDLeft!=camIDRight)
			loader->LoadImageFile(path, camIDRight);
	}
	else
	{
		if(_loadedRecord.HasLeftCamImages)
		{
			QString path = "Resources/Records/";
			if(_loadedRecord.AvailableLeftCamImages.contains(timeStamp+".bmp"))
			{
					loader->LoadImageFile(path+_loadedRecord.ID+"/LeftImage/"+timeStamp+".bmp", camIDLeft);
			}
		}
		if(_loadedRecord.HasRightCamImages && camIDLeft!=camIDRight)
		{
			QString path = "Resources/Records/";
			if(_loadedRecord.AvailableRightCamImages.contains(timeStamp+".bmp"))
			{
					loader->LoadImageFile(path+_loadedRecord.ID+"/RightImage/"+timeStamp+".bmp", camIDRight);
			}
		}
	}
	return true;
}

bool MetaFileHandler::characters(const QString &str)
{
	if(str.contains("\n")) return true;
	if(_currentElement == "MarkerCnt")
	{
		bool isNumber = false;
		_target->MarkerCnt = str.toInt(&isNumber);
		return isNumber;
	}
	else 
	if(_currentElement == "Date")
		_target->Date = str;
	else 
	if(_currentElement == "Time")
		_target->Time = str;
	else 
	if(_currentElement == "LeftCaptured")
			_target->HasLeftCamImages = (str == "True");
	else 
	if(_currentElement == "RightCaptured")
			_target->HasRightCamImages = (str == "True");
	else 
	if(_currentElement == "IMU")
		_target->HasIMUData = !(str == "None");
	else 
	if(_currentElement == "UsingOptiTrack")
		_target->HasReferenceData = (str == "True");

	qDebug() << "reading " + _currentElement + " with-" + str+"-";
	return true;
}

bool RecordLoader::LoadMetaData(QFile* data)
{
	bool validXml = true;
	QXmlSimpleReader* xmlParser = new QXmlSimpleReader();
	MetaFileHandler* handler = new MetaFileHandler(&_loadedRecord);
	xmlParser->setContentHandler(handler);
	QXmlInputSource* source = new QXmlInputSource(data);
	validXml = xmlParser->parse(source);
	if(!validXml)
		qDebug() << "ERROR: Parsing of MetaFile failed!";
	return validXml;
}

bool RecordLoader::LoadReferenceData(QFile* data)
{
	bool validXml = true;
	return validXml;
}

bool IMUFileHandler::characters(const QString &str)
{
if(str.contains("\n")) return true;
	if(_currentElement == "timeStamp")
	{
		bool isNumber = true;
		str.toInt(&isNumber);
		_currentRotation = new Quaternionf();
		return isNumber;
	}
	if(_currentElement == "q_x")
		_currentRotation->x() = str.toFloat();
	else 
	if(_currentElement == "q_y")
		_currentRotation->y() = str.toFloat();
	else 
	if(_currentElement == "q_z")
		_currentRotation->z() = str.toFloat();
	else 
	if(_currentElement == "q_w")
	{
		_currentRotation->w() = str.toFloat();
	} 
	if(_currentElement == "RotSpeedd-s")
	{
		_currentRotationSpeed = str.toFloat();
		_currentMagnitude = 0;					//TODO: check fpr recorded magnitudes if so

		//end of interesting in xml-entry
		//so save the temp-data in the recordlist
		_target->IMURotations.append(_currentRotation); 
		_target->IMURotationSpeed.append(_currentRotationSpeed);
		_target->IMUMagnitudes.append(_currentMagnitude);
	}
	return true;
}

bool RecordLoader::LoadIMUData(QFile* data)
{
	bool validXml = true;
	QXmlSimpleReader* xmlParser = new QXmlSimpleReader();
	IMUFileHandler* handler = new IMUFileHandler(&_loadedRecord);
	xmlParser->setContentHandler(handler);
	QXmlInputSource* source = new QXmlInputSource(data);
	validXml = xmlParser->parse(source);
	if(!validXml)
	{
		qDebug() << "ERROR: Parsing of IMUFile failed!";
	}
	else
		_loadedRecord.HasIMUData = true;
	return validXml;
}

bool MarkerFileHandler::characters(const QString &str)
{
	if(str.contains("\n")) return true;
	if(_currentElement == "timeStamp")
	{
		bool isNumber = false;
		_timeStamp = str.toInt(&isNumber);
		if(_markerIndex == 1)
			_target->TimeStamps.append(_timeStamp);
		_currentRotation = new Quaternionf();
		_currentPosition = new Vector3f();
		_isTracked = false;

		_currentRefRotation = new Quaternionf();
		_currentRefPosition = new Vector3f();
		_hasReference = false;
		return isNumber;
	}
	else 
	if(_currentElement == "Tracked")
		_isTracked = (str == "1");
	else 
	if(_currentElement == "X")
		_currentPosition->x() = str.toFloat();
	else 
	if(_currentElement == "Y")
		_currentPosition->y() = str.toFloat();
	else 
	if(_currentElement == "Z")
		_currentPosition->z() = str.toFloat();
	else 
	if(_currentElement == "QX")
		_currentRotation->x() = str.toFloat();
	else 
	if(_currentElement == "QY")
		_currentRotation->y() = str.toFloat();
	else 
	if(_currentElement == "QZ")
		_currentRotation->z() = str.toFloat();
	else 
	if(_currentElement == "QW")
	{
		_currentRotation->w() = str.toFloat();
		_target->AddMarkerData(_markerIndex, _timeStamp, _isTracked, _currentPosition, _currentRotation); 
	}
	else
	if(_currentElement == "Reference")	//load reference data
		_hasReference = (str == "1");
	else 
	if(_currentElement == "RefX")
		_currentRefPosition->x() = str.toFloat();
	else 
	if(_currentElement == "RefY")
		_currentRefPosition->y() = str.toFloat();
	else 
	if(_currentElement == "RefZ")
		_currentRefPosition->z() = str.toFloat();
	else 
	if(_currentElement == "RefQX")
		_currentRefRotation->x() = str.toFloat();
	else 
	if(_currentElement == "RefQY")
		_currentRefRotation->y() = str.toFloat();
	else 
	if(_currentElement == "RefQZ")
		_currentRefRotation->z() = str.toFloat();
	else 
	if(_currentElement == "RefQW")
	{
		_currentRefRotation->w() = str.toFloat();
		_target->AddMarkerRefData(_markerIndex, _timeStamp, _hasReference, _currentRefPosition, _currentRefRotation); 
	}
	return true;
}

bool RecordLoader::LoadMarkerData(int id, QFile* data)
{
	bool validXml = true;
	QXmlSimpleReader* xmlParser = new QXmlSimpleReader();
	MarkerFileHandler* handler = new MarkerFileHandler(id, &_loadedRecord);
	xmlParser->setContentHandler(handler);
	QXmlInputSource* source = new QXmlInputSource(data);
	validXml = xmlParser->parse(source);
	if(!validXml)
		qDebug() << "ERROR: Parsing of MarkerFile failed!";
	return validXml;
}