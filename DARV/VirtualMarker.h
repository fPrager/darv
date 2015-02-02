#pragma once
#include <stdafx.h>
#include <SOIL.h>
#include "Uncertainty.h"
#include <Util.h>

using namespace std;
using namespace Eigen;

#define MAX_AVERAGE 50
#define DEFAULT_AVERAGE 5
#define AVRG 5

enum MarkerState
{
	FLICKERING,
	MAVERICK,
	JUSTTRACKED,
	UNTRACKED,
	GOODVIEW,
	BESTVIEW,
	BADVIEW
};

class VirtualMarker
{
public:

	VirtualMarker(int id, QString name = "", float widthInCM = 13.5)
	{
		SetUp(id, name, widthInCM, false, new Vector3f(0,0,0),new AngleAxisf(0,Vector3f(0,1,0)));
	}
	
	VirtualMarker(int id, Vector3f* positionInSetup, AngleAxisf* rotationInSetup, QString name = "", float widthInCM = 13.5)
	{
		SetUp(id, name, widthInCM, true, positionInSetup, rotationInSetup);
	}

	void SetUp(int id, QString name, float width, bool isFixed, Vector3f* positionInSetup, AngleAxisf* rotationInSetup);

	~VirtualMarker(void){}

	Vector3f CameraPosition()
	{
		return CameraPosition(_trackedRotation, _trackedTranslation);
	}

	Vector3f CameraPosition(Quaternionf* camRotation)
	{
		return CameraPosition(camRotation, _trackedTranslation);
	}

	Vector3f CameraPosition(Quaternionf* camRotation,Vector3f* translation)
	{
		Vector3f fromCameraToMarkerInCameraSpace = *translation;
		Vector3f fromCameraToMarkerInWorldSpace = camRotation->_transformVector(fromCameraToMarkerInCameraSpace);
		Vector3f fromMarkerToCameraInWorldSpace = -fromCameraToMarkerInWorldSpace;

		Vector3f camPositionOfThis = *_positionInSetup + fromMarkerToCameraInWorldSpace;
		return camPositionOfThis;
	}

	Vector3f CameraPositionPrev()
	{
		return CameraPosition(_trackedRotationPrev,_trackedTranslationPrev);
	}

	Vector3f CameraTranslation()
	{
		if(!_hasInitValues)
		{
			qDebug() << "WARNING! No reference rotation to get a correct angle of the rotation-difference";
			return Vector3f(0,0,0);
		}
		return CameraPosition() - CameraPositionPrev();
	}

	Quaternionf CameraRotation()
	{
		return  *_trackedRotation * _rotationInSetup->inverse();
	}

	void CameraRotation(float* yaw, float* pitch, float* roll)
	{
		Quaternionf rotation = *_trackedRotation;
		double w,x,y,z;
		w = rotation.w(); 
		x = rotation.x(); 
		y = rotation.y(); 
		z = rotation.z();

		*yaw	= atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z) * (180.0f/M_PI) ;
		*pitch	= atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z) * (180.0f/M_PI);
		*roll   = asin(2*x*y + 2*z*w) * (180.0f/M_PI);
	}

	Quaternionf CameraRotation(Quaternionf* rotation)
	{
		return  *rotation * _rotationInSetup->inverse();
	}

	Quaternionf CameraRotationPrev()
	{
		return  *_trackedRotationPrev * _rotationInSetup->inverse();
	}

	void Update(Vector3f* translation, Quaternionf* rotation, float elapsedTime = 0);

	void CalculateExpectedMovement(float cameraRotationAngle);
	void UpdateUncertainty(float elapsedTime);

	void SetYawPitchRoll(float yaw, float pitch, float roll)
	{
		_yaw = yaw;
		_pitch = pitch;
		_roll = roll;
	}

	float DiffAngle(bool inRadian = false)
	{
		if(!_hasInitValues)
		{
			qDebug() << "WARNING! No reference rotation to get a correct angle of the rotation-difference";
			return 0;
		}
		AngleAxisf deltaRot = AngleAxisf(*_deltaRotation);
		if (inRadian)
			return deltaRot.angle();
		else
			return deltaRot.angle()*(180.0f/M_PI);
	}

	float DeltaAngle(bool inRadian = false)
	{
		return abs(DiffAngle(inRadian));
	}

	void SetPrev()
	{
		*_trackedTranslationPrev = *_trackedTranslation;
		*_trackedRotationPrev = * _trackedRotation;
	}

	void SetTracked(bool flag)
	{
		if(flag)
			Track();
		else
			Untrack();
	}

	void Track()
	{
		if(!_isTracked)
		{
			_state = JUSTTRACKED;
			_isTracked = true;
		}
	}

	void Untrack();

	bool IsConstant()
	{
		return (_state == GOODVIEW || _state == BADVIEW || _state == BESTVIEW || _state == UNTRACKED || _state == MAVERICK);
	}
	
	QString State()
	{
		QString MarkerStateString [] = {"juckelt", "falsch", "neu", "     ", "normal", "master", "schlecht"};
		return MarkerStateString[_state];
	}

	void TrackedRotation(float* yaw, float* pitch, float* roll)
	{
		Quaternionf rotation = *_trackedRotation;
		double w,x,y,z;
		w = rotation.w(); 
		x = rotation.x(); 
		y = rotation.y(); 
		z = rotation.z();

		*yaw = atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z) * (180.0f/M_PI) ;
		*pitch = atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z) * (180.0f/M_PI);
		*roll   = asin(2*x*y + 2*z*w) * (180.0f/M_PI);

		//*yaw = _yaw;
		//*pitch = _pitch;
		//*roll   = _roll;
	}

	void TrackedRefRotation(float* yaw, float* pitch, float* roll)
	{
		Quaternionf rotation = *_trackedRefRotation;
		double w,x,y,z;
		w = rotation.w(); 
		x = rotation.x(); 
		y = rotation.y(); 
		z = rotation.z();
		//euler angles in x-y-z convention! like the marker yaw-pitch-roll-setting
		*yaw = atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z) * (180.0f/M_PI) ;
		*pitch = atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z) * (180.0f/M_PI);
		*roll   = asin(2*x*y + 2*z*w) * (180.0f/M_PI);
		Quaternionf diffRot =*_trackedRotation * rotation.inverse();
		float radian = AngleAxisf(diffRot).angle();
		//if(_id == 2 && _isTracked)
		//qDebug() << "diff: "+QString::number(radian*(180.0f/M_PI));
	}

	void TrackedRotation(float* x, float* y, float* z,float* w)
	{
		Quaternionf rotation = *_trackedRotation;
		*w = rotation.w(); 
		*x = rotation.x(); 
		*y = rotation.y(); 
		*z = rotation.z();
	}

	void TrackedRefRotation(float* x, float* y, float* z,float* w)
	{
		Quaternionf rotation = *_trackedRefRotation;
		*w = rotation.w(); 
		*x = rotation.x(); 
		*y = rotation.y(); 
		*z = rotation.z();
	}

	void SetReferenceData(bool hasData, Vector3f* translation = new Vector3f(0,0,0), Quaternionf* rotation = new Quaternionf())
	{
		if(!hasData && _hasRefData)		//we lost reference data
		{
			*_trackedRefTranslation = *_trackedRefTranslationPrev;
			*_trackedRefRotation = *_trackedRefRotationPrev;
			*_deltaRefTranslation = Vector3f(0,0,0);
			*_deltaRefRotation = Quaternionf();

		}
		if(hasData && !_hasRefData)		//we got the first referecne data
		{
			*_trackedRefTranslation = *translation;
			*_trackedRefTranslationPrev = *translation;
			*_deltaRefTranslation = Vector3f(0,0,0);

			*_trackedRefRotation = *rotation;
			*_trackedRefRotationPrev = *rotation;
			*_deltaRefRotation = Quaternionf();
		}
		if(hasData && _hasRefData)		//we continue using reference data
		{
			*_trackedRefTranslationPrev = *_trackedRefTranslation;
			*_trackedRefTranslation = *translation;
			*_deltaRefTranslation = *_trackedRefTranslation - *_trackedRefTranslationPrev;

			*_trackedRefRotationPrev = *rotation;
			*_trackedRefRotation = *rotation;
			*_deltaRefRotation = _trackedRefRotationPrev->inverse() * *_trackedRefRotation;
		}
		_hasRefData = hasData;
	}

	void SetMarkerArea();

	void SetExpectedMovement(float val)
	{
		_expectedMovement = val;	
	}

	QString _name;
	float _widthInCM;

	bool _isTracked;
	bool _hasInitValues;
	bool _hasDeltaValues;
	bool _hasAccelValue;

	float		_stabilityFactor;

	Vector3f*	_cameraSpeed;
	Vector3f*	_cameraSpeedPrev;
	float		_cameraAccel;
	float		_cameraAccelarations[MAX_AVERAGE];
	float		_averageCameraAccel;

	float		_timePrev;
	Vector3f*	_cameraTranslation;
	Vector3f*		_cameraPosition;
	Quaternionf*	_cameraRotation;
	int			_averageIndex;
	float		_cameraMagnitudes[MAX_AVERAGE];
	float		_averageCameraMove;

	Vector3f* _trackedTranslation;
	Vector3f* _trackedTranslationPrev;
	Vector3f* _deltaTranslation;

	float _yaw;
	float _pitch;
	float _roll;

	Quaternionf* _trackedRotation;
	Quaternionf* _trackedRotationPrev;
	Quaternionf* _deltaRotation;

	//reference data, if available (e.g. from OptiTrack-Stream)
	Quaternionf* _trackedRefRotation;
	Quaternionf* _trackedRefRotationPrev;
	Quaternionf* _deltaRefRotation;

	Vector3f* _trackedRefTranslation;
	Vector3f* _trackedRefTranslationPrev;
	Vector3f* _deltaRefTranslation;

	bool	_hasRefData;
	bool _isFixed;

	int _id;
	QString _cosName;

	Vector3f* _positionInSetup;
	Quaternionf* _rotationInSetup;

	float _weight;
	float _qualityVision;
	float _qualityTracking;

	Uncertainty _uncer;
	Vector3f*	_uncertaintyVector;
	//error indicators
	float Distance();
	float RotationAngleFromCamera(bool inRadian = false);
	float MoveRate();
	float _expectedMovement;
	float AreaRate();

	float _areaPx;
	float _widthPx;
	float _heightPx;
	
	float _trackedMovements[AVRG];

	void ClearMovements()
	{
		for(int i = 0; i < AVRG; i++)
			_trackedMovements[i] = 0;
	}
	
	float GetTrackedMovement()
	{
		float sum = 0;
		for(int i = 0; i <AVRG; i++)
		{
			sum += _trackedMovements[i];
		}
		return sum/(float)AVRG;
	}

	MarkerState _state;
};

