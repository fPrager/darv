#include "stdafx.h"
#include "TrackingManager.h"


TrackingManager::TrackingManager(void)
{
	//_eyeTransDiffs = QList<Vector3f>();
	//_eyeRotDiffs = QList<Vector4f>();
	//_sdkReady = false;
	_sdkReady = true;
	_imu = new IMUManagerVuzix();
	_optiTracker = new OptiTracker();
	_camLoader = 0;
	_additonalTranslation = Vector3f(0,0,0);
	_secondsCounter = 0;
	_rendererUpdated = false;
	_usingRecord = false;
	_readyRecord = false;
	_imuRotation = new Quaternionf(1,0,0,0);
	_imuMagnitude  = 0;
}


TrackingManager::~TrackingManager(void)
{
	delete _mSDK;
	_mSDK = 0;

	//delete _sensors;
	//_sensors = 0;
}

void TrackingManager::SetFrameUpdateFunctions(void (*frameUpdate)(void*, BYTE* ,int ,int), void* userData)
{
	_FrameUpdate = frameUpdate;
	_UserData = userData;
}

bool TrackingManager::Initialize(CameraImageLoader* camLoader)
{
	_camLoader= camLoader;
	_recLoader = new RecordLoader();
	//Create the SDK
	_mSDK = metaio::CreateMetaioSDKWin32();
	_mSDK->initializeRenderer(0, 0,metaio::ESCREEN_ROTATION_0,metaio::ERENDER_SYSTEM_NULL);
	
	//_sensors = metaio::CreateSensorsComponent();
	//_mSDK->registerSensorsComponent(_sensors);

	
	vector<metaio::Camera> cameras = _mSDK->getCameraList();

	if(!SetAugmentedRealityMode())
		return false;
	qDebug() << "Try to start in: ";

	_camTrackingName = QString::fromStdString(cameras[_camTracking].friendlyName);

	if(_vstMode)
	{
		qDebug() << "   Video-See-Through Mode";
		qDebug() << "   with Cameras on ";
		qDebug() << "	left: "<< _camTracking << "(" << _camTrackingName << ") and " << _camOpenCV1 << "(" << QString::fromStdString(cameras[_camOpenCV1].friendlyName) << ")";
		qDebug() << "right: "<< _camOpenCV2 << "(" << QString::fromStdString(cameras[_camOpenCV2].friendlyName) << ")";
	}
	else
	{
		qDebug() << "   Optical-See-Through Mode";
		qDebug() << "   with Camera on " << _camTracking<< "(" << _camTrackingName << ")";
	}
	qDebug() << "   with Resolution " << _camResX << " : " << _camResY;
	qDebug() << "   with 30 FPS ";

	//_sensors->start(_sensors->SENSOR_ALL);

	//set camera calibration parameters
	//set tracking configs
	int res_x = _camResX;
	int res_y = _camResY;
	QString prefix = "OST";
	if(_vstMode)
		prefix = "VST";

	QString calibrationPath = "Resources/CameraCalibration/"+prefix+"CalibrationResults_";
	calibrationPath+=QString::number(res_x);
	calibrationPath+="x";
	calibrationPath+=QString::number(res_y);
	calibrationPath+=".xml";
	qDebug() << "Loading Cameradata: " << calibrationPath;
	if(!_mSDK->setCameraParameters(calibrationPath.toStdString()))
	{
		qDebug() << "ERROR: Failed to load Cameradata from : " << calibrationPath;
		return false;
	}

	// Since the metaio SDK may capture in YUV for performance reasons, we
	// enforce RGB capturing here to make it easier for us to handle the camera image
	cameras[_camTracking].resolution = metaio::Vector2di(res_x, res_y);
	cameras[_camTracking].yuvPipeline = DEFAULT_YUV_MODE;
	_cam = cameras[_camTracking];
	_mSDK->startCamera(_cam);

	//set up markers
	MakeMarker();

	//register the trackingmanager to callback of the sdk
	_mSDK->registerCallback(this);
	
	QString trackingPath = "Resources/TrackingConfiguration/MultipleMarkerOptiTrack";
	//if(HOMEWORKING)
	//	trackingPath += "2";
	trackingPath += ".xml";

	//set tracking configs
	if(!_mSDK->setTrackingConfiguration(trackingPath.toStdString()))
	{
		qDebug() << "ERROR: Failed to load Trackingdata from : Resources/MultipleMarkerOptiTrack.xml";
		return false;
	}

	if(!_imu->Initialize())
	{
		qDebug() << "WARNING: Can't initialize IMU from Vuzix";
	}

	if(!_optiTracker->Initialize())
		qDebug() << "HINT: No OptiTrack available.";

	_mSDK->enableBackgroundProcessing();
	
	MakeCOS();

	return true;
}

bool TrackingManager::SetAugmentedRealityMode()
{
	vector<metaio::Camera> cameras = _mSDK->getCameraList();

	if(cameras.size() == 0)
	{
		qDebug() << "ERROR: No Camera available! (TRACK_INIT_FAIL)";
		return false;
	}
	else
	{
		_vstMode = false;
		_camTracking = -1;
		_camOpenCV1 = -1;
		_camOpenCV2 = -1;

		int vstCamIndex1 = -1;
		int vstCamIndex2 = -1;
		int ostCamIndex = -1;
		int virtualCamIndex = -1;

		for(int i = 0; i < cameras.size(); i++)
		{
			metaio::Camera cam = cameras[i];
			QString name = QString::fromStdString(cam.friendlyName);
			if(name.startsWith(VIRTUAL_CAM_NAME_START))
			{
				virtualCamIndex = i;
			}
			if(name.startsWith(VST_CAM_NAME_START))
			{
				bool leftIsFirst = false;
				if(leftIsFirst)
				{
				if(vstCamIndex1 > -1)
					vstCamIndex2 = i;
				else
					vstCamIndex1 = i;
				}
				else
				{
				if(vstCamIndex2 > -1)
					vstCamIndex1 = i;
				else
					vstCamIndex2 = i;
				}
			}
			if(name.startsWith(OST_CAM_NAME_START))
			{
				ostCamIndex = i;
			}
		}

		if(vstCamIndex1 == -1 && vstCamIndex2 == -1 && ostCamIndex == -1)
		{
			//we didn't found any cam we know
			if(virtualCamIndex == -1)
			{
				//to get a working vst-system we need a virtual camera-instance which hasn't just exclisive access-possiblities
				_vstMode = false;
				_camTracking = 0;

				if(cameras.size() > 1)
				{
					_camOpenCV1 = 1;
				}
			}
			else
			{
				_camTracking = virtualCamIndex;
				_camOpenCV1 = virtualCamIndex;
				_camOpenCV2 = virtualCamIndex;
				_vstMode = DEFAULT_IN_VSTMODE;
			}
		}
		else
		{
			if(vstCamIndex1 != -1)
			{
				//we have all cameras attached wich we want chose the prefered setup
				if(virtualCamIndex != -1 && (DEFAULT_IN_VSTMODE || ostCamIndex == -1))
				{
					_vstMode = true;
					_camTracking = virtualCamIndex;
					_camOpenCV1 = virtualCamIndex;
					if(vstCamIndex2 != -1)
						_camOpenCV2 = vstCamIndex2;
					else
						_camOpenCV2 = virtualCamIndex;
				}
			}

			if(ostCamIndex != -1 && !_vstMode)
			{
				_vstMode = false;
				if(virtualCamIndex == -1)
				{
				_camTracking = ostCamIndex;
				_camOpenCV1 = vstCamIndex1;
				_camOpenCV2 = vstCamIndex2;
				}
				else
				{
				_camTracking = virtualCamIndex;
				_camOpenCV1 = virtualCamIndex;
				_camOpenCV2 = virtualCamIndex;
				}
			}
		
		}

		_camResX = 640;
		_camResY = 480;
		_camFPS = 30;

		if(!_vstMode)
		{
			_camResX = DEFAULT_RESOLUTION_X;
			_camResY = DEFAULT_RESOLUTION_Y;
		}
	}
	return true;
}

void TrackingManager::GetVSTData(int* camLeft, int* camRight, int* resX, int* resY, int* fps)
{
	//TODO: Check if there is the possibility to get full resolution of both vst-cams

	*camLeft = _camOpenCV1;
	*camRight = _camOpenCV2;
	*resX = 320;
	*resY = 240;
	*fps = _camFPS;
}

void TrackingManager::GetTrackingCameraData(int* camIndex, int* resX, int* resY, int* fps)
{
	*camIndex = _camTracking;
	*resX = 640;
	*resY = 480;
	*fps = _camFPS;
}

void TrackingManager::GetTrackingCameraData(QString* camName, int* resX, int* resY, int* fps)
{
	*camName = _camTrackingName;
	*resX = 640;
	*resY = 480;
	*fps = _camFPS;
}

void TrackingManager::MakeMarker()
{
	_markerCount = 0;
}

void TrackingManager::MakeCOS()
{
	_cos = QList<COS>();
	int cosCount = _mSDK->getNumberOfDefinedCoordinateSystems();
	
	for(int index = 1; index<=cosCount; index++)
	{
		string name = _mSDK->getCoordinateSystemName(index);
		_cos.append(COS(QString::fromStdString(name), index));
	}
}

QStringList TrackingManager::StartUsingRecord(QString camIdLeft, QString camIdRight)
{
	if(!_usingRecord)
	{
		_mSDK->stopCamera();
		if(_camLoader!=0)
		{
			_camLoader->StopCamera(camIdRight);
			_camLoader->StopCamera(camIdLeft);
			_camIdRight = camIdRight;
			_camIdLeft = camIdLeft;
			IplImage* right;
			IplImage* left;
			_recLoader->GetCameraImages(_camLoader,_camIdLeft,_camIdRight,"Selection");
		}
	}
	_usingRecord = true;
	return _recLoader->UpdateRecordList();
}

bool TrackingManager::StopUsingRecord()
{
	bool newVirtualMarker = false;
	if(_usingRecord)
	{
		_loadedRecord = 0;
		_timeInRecord = 0;
		IplImage* right;
		IplImage* left;
		if(_readyRecord)
		{
			_marker.clear();
			_markerCount = 	_prevMarker.size();
			for(int i = 0; i < _prevMarker.size(); i++)
				_marker.append(_prevMarker[i]);
			newVirtualMarker = true;
		}
		_readyRecord = false;
		_recLoader->GetCameraImages(_camLoader,_camIdLeft,_camIdRight, "RollBack");
		_mSDK->startCamera(_cam);
		if(_camLoader!=0)
		{
			_camLoader->StartCamera(_camIdRight);
			_camLoader->StartCamera(_camIdLeft);
		}
	}
	_usingRecord = false;
	return newVirtualMarker;
}

bool TrackingManager::LoadRecord(QString id)
{
	IplImage* right;
	IplImage* left;
	_loadedRecord = 0;
	_readyRecord = false;
	_recLoader->GetCameraImages(_camLoader,_camIdLeft,_camIdRight,"Loading");
	qDebug() << "Loading Record: "  + id;
	if(_recLoader->LoadRecord(id))
	{
		_timeInRecord = 0;
		_loadedRecord = &(_recLoader->_loadedRecord);
		_readyRecord = true;
		_imuRotation = new Quaternionf(1,0,0,0);
		_imuMagnitude = 0;
		_referencePosition = new Vector3f();
		_referenceRotation = new Quaternionf();
		_ignoreUpdate = true;
		_prevMarker = QList<Marker>();
		for(int i = 0; i < _marker.size(); i++)
		{
			_prevMarker.append(_marker[i]);
		}
		_marker.clear();
		_markerCount = 0;
		for(int mIndex = 0; mIndex < _loadedRecord->MarkerCnt; mIndex++)
		{
			Marker m = Marker(mIndex,"Marker"+QString::number(mIndex),13.5f);
			m._quality = 0;
			_marker.append(m);
			_markerCount++;
		}
		return true;
	}
	else
	{
		qDebug() << "ERROR: Loading failed";
		return false;
	}
}

bool TrackingManager::Update(float elapsedTime,  bool* markerUpdate)
{
	_secondsCounter += elapsedTime/1000.f;
	float frameTime = 1.f/_camFPS;
	if(_secondsCounter > frameTime)
	{
		_camUpdate = true;
		float diff = ((int)(_secondsCounter/(1.f/_camFPS)) * (1.f/_camFPS));
		_secondsCounter = _secondsCounter - diff;
	}
	else
	{
		_camUpdate = false;
	}
	//qDebug() << "wanted: " << QString::number(1.f/_camFPS) << "  is: " << QString::number(_secondsCounter);

	*markerUpdate = _camUpdate;
	if(!_usingRecord)
	{
		if(_sdkReady)
		{
			// Note: The metaio SDK itself does not render anything here because we initialized it with the NULL
			// renderer. The call is still necessary to get the camera image and update tracking.
			if(!this->isRunning())
			{
				start();
			}
		}
		if(_optiTracker->_available)
		{
			_optiTracker->Update(elapsedTime);
		}
	}
	else
	{
		if(_readyRecord)
		{
			if(!_ignoreUpdate)
				_timeInRecord += elapsedTime;
			else
				_ignoreUpdate = false;
			//read next entry in records
			bool overLoad = false;
			int index = _loadedRecord->GetTimeIndex(_timeInRecord, &overLoad);
			_loadedRecord->CurrentTimeIndex = index;
			//qDebug() << "index: "+QString::number(index);
			bool hasCamImages = true;
			int camIndex = _loadedRecord->GetCamTimeStamp(index, &hasCamImages);
			//qDebug() << "index: " + QString::number(index);
			int timeStamp = _loadedRecord->TimeStamps[_loadedRecord->CurrentTimeIndex];
			for(int mIndex  = 0; mIndex < _loadedRecord->MarkerCnt; mIndex++)
			{
				bool isTracked = false;
				Vector3f* translation = new Vector3f();
				Quaternionf* rotation = new Quaternionf();
				_loadedRecord->GetMarkerEntry(mIndex, timeStamp, &isTracked, translation, rotation);
				_marker[mIndex]._rotationFromCamera = rotation;
				_marker[mIndex]._translationFromCamera = translation;
				_marker[mIndex]._isTracked = isTracked;

				bool hasReference = false;
				Vector3f* reftranslation = new Vector3f();
				Quaternionf* refrotation = new Quaternionf();
				_loadedRecord->GetMarkerRefEntry(mIndex, timeStamp, &hasReference, reftranslation, refrotation);

				_marker[mIndex]._hasRefData = hasReference;
				_marker[mIndex]._rotationFromReference = refrotation;
				_marker[mIndex]._translationFromReference = reftranslation;

				if(isTracked)
					_marker[mIndex]._quality = 1;
				else
					_marker[mIndex]._quality = 0;
			}
			if(_loadedRecord->HasIMUData)
			{
				if(_loadedRecord->IMURotations.count() > _loadedRecord->CurrentTimeIndex)
				{
					_imuRotation = _loadedRecord->IMURotations[_loadedRecord->CurrentTimeIndex];
					_imuMagnitude = _loadedRecord->IMUMagnitudes[_loadedRecord->CurrentTimeIndex];
					_imuRotationAngle = _loadedRecord->GetCamRotationAngle();
				}
			}
			if(_prevTimeIndex != camIndex && hasCamImages)
			{
				_recLoader->GetCameraImages(_camLoader,_camIdLeft,_camIdRight,QString::number(camIndex));
			}
			
			if(overLoad)
			{
				//qDebug() << "Record end reached.";
				_loadedRecord->CurrentTimeIndex = 0;
				_timeInRecord = 0;
			}
			_prevTimeIndex = camIndex;
		}
	}
	if(_optiTracker != 0)
	{
		if(_optiTracker->_available)
		{
			Vector3f translation = Vector3f();
			Quaternionf rotation = Quaternionf();
			if(_optiTracker->_isTracked)
			{
				_optiTracker->GetRelativeData(&translation, &rotation);
				//TODO: we calculate the marker-trackingdata relative to the camera based on the reference cam-cube-data
				//we shoudle change that in a more abstract, less hacked way!
				for(int i = 0; i<_marker.count(); i++)
				{
					_marker[i]._hasRefData = true;
					Quaternionf relMRotation = rotation;
					Vector3f relMTranlsation = translation;
					Vector3f markerTrans = Vector3f(0,0,0);
					Quaternionf markerRotation = Quaternionf(1,0,0,0);
					switch(_marker[i]._CosID)
					{
						case 1:		//upper marker of the cube
							//qDebug() << "x: " + QString::number(rotation._transformVector(Vector3f(0, 11, 0)).x()) +" y: " + QString::number(rotation._transformVector(Vector3f(0, 11, 0)).y())+" z: " + QString::number(rotation._transformVector(Vector3f(0, 11, 0)).z());
							markerTrans = rotation._transformVector(Vector3f(0, 11, 0));
							//qDebug() << "x: " + QString::number(markerTrans.x()) +" y: " + QString::number(markerTrans.y())+" z: " + QString::number(markerTrans.z());
							markerRotation = Quaternionf(AngleAxisf(180 * (M_PI/180.0f),Vector3f(0,1,0))*AngleAxisf(-90 * (M_PI/180.0f),Vector3f(1,0,0)));
						break;
						case 2:	//front marker
							markerTrans = rotation._transformVector(Vector3f(0, 0, -11));
							markerRotation = Quaternionf(AngleAxisf(180 * (M_PI/180.0f),Vector3f(0,1,0)));
						break;
						case 3:		//left marker
							markerTrans = rotation._transformVector(Vector3f(11, 0, 0));
							markerRotation = Quaternionf(AngleAxisf(90 * (M_PI/180.0f),Vector3f(0,1,0)));
						break;
						case 4:		//right marker
							markerTrans = rotation._transformVector(Vector3f(-11, 0, 0));
							markerRotation = Quaternionf(AngleAxisf(-90 * (M_PI/180.0f),Vector3f(0,1,0)));
						break;
						case 5:		//back marker
							markerTrans = rotation._transformVector(Vector3f(0, 0, 11));
							markerRotation = Quaternionf(AngleAxisf(0 * (M_PI/180.0f),Vector3f(0,1,0)));
						break;
					}
					relMTranlsation = markerTrans + relMTranlsation;
					//relMTranlsation.x()*=-1;
					//relMTranlsation.z()*=-1;
					_optiTracker->GetRelativeData(&markerRotation, &relMRotation);
					//relMRotation = markerRotation * rotation;
					*_marker[i]._translationFromReference = relMTranlsation;
					*_marker[i]._rotationFromReference = relMRotation;
				}
			}
			else
			{
				if(!_usingRecord)
					for(int i = 0; i<_marker.count(); i++)
						_marker[i]._hasRefData = false;
			}	


		}
	}
	return true;
}

void TrackingManager::run()
{
	_mSDK->render();
			//get marker information
		for(int i = 0; i < _markerCount; i++)
		{
			if(_marker[i]._isTracked)
			{
				int id = _marker[i]._CosID;
				metaio::TrackingValues tvalue;
				tvalue = _mSDK->getTrackingValues(id, true);
				Quaternionf* rotation = new Quaternionf();
				rotation->x() = tvalue.rotation.getQuaternion().x;
				rotation->y() = tvalue.rotation.getQuaternion().y;
				rotation->z() = tvalue.rotation.getQuaternion().z;
				rotation->w() = tvalue.rotation.getQuaternion().w;

				_marker[i]._pitch = tvalue.rotation.getEulerAngleDegrees().x;		//x-y-z-order !!!
				_marker[i]._yaw = tvalue.rotation.getEulerAngleDegrees().y;
				_marker[i]._roll = tvalue.rotation.getEulerAngleDegrees().z;

				_marker[i]._rotationFromCamera = rotation;
				//Vector3f axis = Vector3f( tvalue.rotation.getAxisAngle().x, tvalue.rotation.getAxisAngle().y, tvalue.rotation.getAxisAngle().z);
				//_marker[i]._rotationFromCamera->axis() = axis;
				//_marker[i]._rotationFromCamera->angle()= tvalue.rotation.getAxisAngle().w;
				//				double yaw = 0, pitch = 0, roll = 0;
				//				yaw = tvalue.rotation.getEulerAngleDegrees().y;
				//				pitch =tvalue.rotation.getEulerAngleDegrees().x;
				//				roll = tvalue.rotation.getEulerAngleDegrees().z;
								//qDebug() << "yaw: " << QString::number(yaw) << "      pitch: " << QString::number(pitch) << "     roll: " << QString::number(roll);
				//SetMarkerPixelArea(i);
				//qDebug() << "AREA: " << QString::number(area) << " px2";
				_marker[i]._translationFromCamera->x() = tvalue.translation.x/10+ _additonalTranslation.x();		//from millimeter in cm
				_marker[i]._translationFromCamera->y() = tvalue.translation.y/10+ _additonalTranslation.y();		//from millimeter in cm
				_marker[i]._translationFromCamera->z() = tvalue.translation.z/10+ _additonalTranslation.z();		//from millimeter in cm

				_marker[i]._quality = tvalue.quality;

			}
			else
			{
				_marker[i]._quality = 0;
			}
	}
}


bool TrackingManager::CleanUp()
{
	_sdkReady = false;
	try{
		_mSDK->stopCamera();
	}
	catch (exception e)
	{
		qDebug() << "ERROR: Metaio couldn't easily stop the camera.";
	}
	delete _mSDK;
	_mSDK = 0;
	return true;
}

Marker TrackingManager::GetMarkerInfo(int index)
{
	if(_marker.size() > index-1)
		return _marker[index-1];
	else
		return _marker[0];
}

QList<Marker> TrackingManager::GetMarkerInfo()
{
	return _marker;
}

void TrackingManager::AddEyeDiff(Vector3f* translation, AngleAxisf* rotation)
{
	_eyeTransDiffs.append(*translation);
	_eyeRotDiffs.append(*rotation);
}

Vector3f TrackingManager::GetAverageEyeTranslation()
{
	int size = _eyeTransDiffs.size();
	Vector3f averageTrans = Vector3f(0,0,0);
	for(int i = 0; i<size; i++)
		averageTrans += _eyeTransDiffs[i];
	averageTrans = averageTrans/(float)size;
	return averageTrans;
}

AngleAxisf TrackingManager::GetAverageEyeRotation()
{
	//TODO: make real average
	int size = _eyeRotDiffs.size();
	AngleAxisf averageRot = AngleAxisf(0,Vector3f(0,1,0));
	for(int i = 0; i<size; i++)
		averageRot = _eyeRotDiffs[i];
	//averageRot = averageRot/(float)size;
	return averageRot;
}

void TrackingManager::onTrackingEvent(const metaio::stlcompat::Vector<metaio::TrackingValues>& trackingValues)
{
	int size = trackingValues.size();
	int tracked = _mSDK->getNumberOfValidCoordinateSystems();
	int total = _mSDK->getNumberOfDefinedCoordinateSystems();

	if(_marker.size() < trackingValues.size())
	{
		for(int i = _marker.size(); i<trackingValues.size(); i++)
		{
			metaio::TrackingValues tvalue = trackingValues[i];
			string name = tvalue.cosName;
			Marker m = Marker(tvalue.coordinateSystemID,QString::fromStdString(name),13.5f);
			m._quality = 0;
			_marker.append(m);
			_markerCount++;
		}
	}
	if(trackingValues.size() > 0 && _sdkReady)
	{
		for(int i = 0; i<trackingValues.size(); i++)
		{
			metaio::TrackingValues tvalue = trackingValues[i];
			for(int i = 0; i < _cos.size();i++)
			{
				if(QString::fromStdString(tvalue.cosName) == _cos[i]._name)
				{

				}
			}

			for(int i = 0; i < _marker.size(); i++)
			{
				if(tvalue.coordinateSystemID == _marker[i]._CosID)
				{
					//qDebug() << "Marker ID" << tvalue.coordinateSystemID << " state changed: " << tvalue.state;
					if(tvalue.state == 4)
						_marker[i]._isTracked = true;
					else
						_marker[i]._isTracked = false;
				}
			}
		}
	}

}

bool TrackingManager::HasReferenceTracker()
{
	if(!_usingRecord)
	{
		if(_optiTracker == 0)
			return false;
		return _optiTracker->_available;
	}
	else
	{
		if(_readyRecord)
		{
			return _loadedRecord->HasReferenceData;
		}
		else
			return false;
	}
}

bool TrackingManager::HasReferenceData()
{
	if(!_usingRecord)
	{
		if(_optiTracker == 0)
			return false;
		return _optiTracker->HasData();
	}
	else
	{
		if(_readyRecord)
		{
			return _loadedRecord->HasReferenceData;
		}
		else
			return false;
	}
}

bool TrackingManager::DoReferenceTracking()
{
	if(!_usingRecord)
		return _optiTracker->DoConnection();
	else
		return true;
}

void TrackingManager::GetReferencePosition(Vector3f* position)
{
	_optiTracker->GetPosition(position);
}

void TrackingManager::GetReferenceRotation(AngleAxisf* rotation)
{
	_optiTracker->GetRotation(rotation);
}

void TrackingManager::GetReferenceRotation(float* yaw, float* pitch, float* roll)
{
	_optiTracker->GetRotation(yaw,pitch,roll);
}

void TrackingManager::GetReferenceCamData(Vector3f* position, Quaternionf* rotation)
{
	if(!_readyRecord)
	{
		if(_optiTracker->_available)
		{
			_optiTracker->GetData(position, rotation);
		}
		else
		{
			*position = Vector3f(0,0,0);
			*rotation = AngleAxisf(0,Vector3f(0,1,0));
		}
	}
	else
	{
		*position = *_referencePosition;
		*rotation = *_referenceRotation;
	}
}

void TrackingManager::ResetReferenceTracker(Vector3f* position, Quaternionf* rotation)
{
	if(_optiTracker->_available)
	{
		_optiTracker->ResetData(position, rotation);
	}
}

QString TrackingManager::GetReferenceInfo()
{
	return _optiTracker->GetInfo();
}