#pragma once
#include <metaioSDK/IMetaioSDKWin32.h>
//#include <AR/ar.h>
#include <stdafx.h>
#include <Marker.h>
#include <COS.h>
#include <IMUManager.h>
#include <OptiTrack.h>
#include <qthread.h>
#include <CameraImageLoader.h>
#include <RecordLoader.h>

using namespace std;
using namespace Eigen;

//default camera settings
#define	 DEFAULT_RESOLUTION_X 640	//width of the camera picture
#define	 DEFAULT_RESOLUTION_Y 480	//height of the camera picture
#define	 DEFAULT_YUV_MODE false		//use YUV-Pipeline or RGB

#define	 DEFAULT_CAMERA_INDEX 3			//used cam, if more than one cam
#define	 DEFAULT_IN_VSTMODE true			//used ar-mode if no specific vuzix-cam is attached to the computer

#define	 VIRTUAL_CAM_NAME_START "ManyCam"
#define	 OST_CAM_NAME_START "Logitech HD Pro Webcam C920"
#define	 VST_CAM_NAME_START "Vimicro USB"

//default tracking data
#define	 DEFAULT_METAIO_TRACKING_FILE "Resources/OneMarker.xml"



class TrackingManager: public QThread, protected metaio::IMetaioSDKCallback
{
	Q_OBJECT
public:
	TrackingManager(void);
	~TrackingManager(void);

	bool Initialize(CameraImageLoader* camLoader);

	void GetVSTData(int* camLeft, int* camRight, int* resX, int* resY, int* fps);
	void GetTrackingCameraData(int* camIndex, int* resX, int* resY, int* fps);
	void GetTrackingCameraData(QString* camName, int* resX, int* resY, int* fps);

	void SetFrameUpdateFunctions(void (*frameUpdate)(void*, BYTE* ,int ,int), void* userData);
	void SetImage(int width, int height, unsigned char * buffer);
	
	QStringList StartUsingRecord(QString camIdLeft, QString camIdRight);
	bool StopUsingRecord();
	bool LoadRecord(QString id);

	bool Update(float elapsedTime, bool* markerUpdate);
	void run();

	bool CleanUp();
	Marker	GetMarkerInfo(int index);
	void	SetMarkerPixelArea(int index);
	QList<Marker>	GetMarkerInfo();

	Vector3f GetAverageEyeTranslation();
	AngleAxisf GetAverageEyeRotation();
	void AddEyeDiff(Vector3f* trans, AngleAxisf* rotation);

	bool HasReferenceTracker();
	bool HasReferenceData();
	void GetReferencePosition(Vector3f* position);
	void GetReferenceRotation(AngleAxisf* rotation);
	void GetReferenceRotation(float* yaw, float* pitch, float* roll);
	void GetReferenceCamData(Vector3f* position, Quaternionf* rotation);
	void ResetReferenceTracker(Vector3f* position, Quaternionf* rotation);
	bool DoReferenceTracking();
	QString GetReferenceInfo();

	int					_markerCount;
	int					_camFPS;
	bool				_camUpdate;
	bool				_vstMode;
	bool				_usingRecord;
	Record*				_loadedRecord;
	bool				_readyRecord;
	int					_timeInRecord;
	int					_prevTimeIndex;
	bool				_ignoreUpdate;
	RecordLoader*		_recLoader;
	IMUManager*			_imu;
	Quaternionf*		_imuRotation;
	float				_imuRotationAngle;
	float				_imuMagnitude;
	Vector3f*			_referencePosition;
	Quaternionf*		_referenceRotation;
	OptiTracker*		_optiTracker;
	Vector3f			_additonalTranslation;

	bool	IMUUsable()
	{
		if(_readyRecord)
			return _loadedRecord->HasIMUData;
		return _imu->_isUsable;
	}

	bool UpdateIMU(bool useFilter, float elapsedTime = 100)
	{
		if(!_readyRecord)
			return _imu->Update(useFilter, elapsedTime);
	}

	void GetImuRotation(Quaternionf* rotation)
	{
		if(_readyRecord)
		{
			*rotation = *_imuRotation;
		}
		else
			_imu->GetImuRotation(rotation);
	}

	float GetImuRotationAngle()
	{
		if(_readyRecord)
		{
			return _imuRotationAngle;
		}
		else
			return _imu->GetRotationAngle();
	}

	float GetImuRotationSpeed()
	{
		return _imu->GetRotationSpeed();
	}

	void GetImuRotationSpeed(float* yawSpeed, float* pitchSpeed, float* rollSpeed)
	{
		_imu->GetRotationSpeed(yawSpeed, pitchSpeed, rollSpeed);
	}

	float TranslationMagnitude(float elapsedTimeTracking)
	{
		if(_readyRecord)
		{
			return _imuMagnitude;
		}
		return _imu->TranslationMagnitude(elapsedTimeTracking);
	}

signals:
     void RendererReady();

private:
	
	void (*_FrameUpdate)(void*, BYTE* ,int ,int);
	void* _UserData;

	bool  SetAugmentedRealityMode();

	//stuff of the metaio tracking toolkit
	metaio::IMetaioSDKWin32* _mSDK;
	bool			_sdkReady;
	bool			_rendererUpdated;
	QList<Marker>	_marker;
	QList<Marker>	_prevMarker;
	QList<COS>		_cos;
	void			MakeCOS();
	void			MakeMarker();

	
	int		_camTracking;
	metaio::Camera _cam;
	QString _camIdRight;
	QString _camIdLeft;
	CameraImageLoader* _camLoader;
	QString _camTrackingName;
	int		_camOpenCV1;
	int		_camOpenCV2;
	int		_camResX;
	int		_camResY;
	float	_secondsCounter;

	metaio::ISensorsComponent*	_sensors;
	
	QList<Vector3f>	_eyeTransDiffs;
	QList<AngleAxisf> _eyeRotDiffs;

	// IMetaioSDKCallback BEGIN
	//virtual void onNewCameraFrame(metaio::ImageStruct* cameraFrame) override;
	//virtual void onSDKReady() override;
	virtual void onTrackingEvent(const metaio::stlcompat::Vector<metaio::TrackingValues>& trackingValues) override;
	// IMetaioSDKCallback END
};

