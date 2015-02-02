#include "stdafx.h"
#include "CameraTracker.h"

CameraTrackerFusingIMU::CameraTrackerFusingIMU(AngleAxisf initRotation, float imuInterPol):CameraTracker(BEST_MARKER, Vector3f(0,0,0),initRotation)
{
	_imuInterPol = imuInterPol;
	_latestIMUData = new Vector3f(0,0,0);
	//_imuInterPol = 1;
	_useIMURotation = false;
	_expectedRotationAngle = 0;
	_prevPosition = new Vector3f(0,0,0);
	_prevRotation = new Quaternionf(initRotation);
	_prevRotationFromMarker =new Quaternionf(initRotation); 
	_prevRotationFromIMU =new Quaternionf(initRotation); 
	_latestRotationFromMarker =new Quaternionf(initRotation);
	_latestRotationFromIMU =new Quaternionf(initRotation);
	_accuDiffIMU = 0;
	_accuDiffM = 0;
	//we know an initial rotation, so use it as reference to the imu
	_initialRotation = new AngleAxisf(initRotation);
	_imuTransMag = 0;
	_instantTransMagn = 0;
	_quality = 0;
}

CameraTrackerFusingIMU::~CameraTrackerFusingIMU()
{

}

void CameraTrackerFusingIMU::UpdateIMURotationValue(float yaw, float pitch, float roll)
{
	_latestIMUData->x() =  yaw;
	_latestIMUData->y() =  pitch;
	_latestIMUData->z() =  roll;
	UseIMUData(_latestRotationFromIMU);
	//qDebug() << "yaw: " << QString::number(yaw) << " pitch: " << QString::number(pitch) << "  roll: " << QString::number(roll);
}

void CameraTrackerFusingIMU::UpdateIMURotationValue(Quaternionf* rotation)
{
	_latestRotationFromIMU = rotation;
}

void CameraTrackerFusingIMU::QuickIMUCheck(float instantTransMag)
{
	_instantTransMagn = instantTransMag;
	return;
	if(instantTransMag > 2)
	{
		_quality = 0;
	}
	else
	if(instantTransMag > 0.5)
		_quality *= (1 - instantTransMag/2.f);
	else
		if(_justFlexibleMarker)
			_quality *= 1.1;
}

void CameraTrackerFusingIMU::ResetRotation()
{
	_initialRotation = new AngleAxisf(*_latestRotationFromMarker);
	SetInitialRotation();
}

void CameraTrackerFusingIMU::UseIMUData(Quaternionf* camRotation)
{
	//qDebug() << "imu : " << _latestIMUData->x() << " - "<< _latestIMUData->y() << "-"<< _latestIMUData->z();
	if(!_useIMURotation && *_latestIMUData != Vector3f(0,0,0))
	{
		SetInitialRotation();
	}
	if(_useIMURotation)
	{
		AngleAxisf yaw = AngleAxisf(_latestIMUData->x() * (M_PI/180.0f), Vector3f(0,1,0));
		AngleAxisf pitch = AngleAxisf(_latestIMUData->y() * (M_PI/180.0f), Vector3f(1,0,0));
		AngleAxisf roll = AngleAxisf(_latestIMUData->z() * (M_PI/180.0f), Vector3f(0,0,-1));
		//qDebug() << "yaw: " << _latestIMUData->x()<< " pitch : " << _latestIMUData->y()<< " roll: " << _latestIMUData->z();
		Quaternionf imuRot = Quaternionf(yaw * pitch * roll);
		AngleAxisf imu = AngleAxisf(imuRot);
		*camRotation = imuRot;
		AngleAxisf rot = AngleAxisf(*camRotation);
		_quality = 1;
	}
	//qDebug() << "IMU: " << _latestIMUData->x() << " - " << _latestIMUData->y() << " - " << _latestIMUData->z();
}

void CameraTrackerFusingIMU::CalculateCameraData(QList<VirtualMarker*> marker)
{
	float mQualityRotation = 0;
	float useOldPosition = 0;

	GetRotationFromMarker(marker, _mUsing,  &mQualityRotation, _latestRotationFromMarker);

	Quaternionf newCamRotation = *_prevRotation;
	//qDebug() << "rot: " << _rotation.angle() << " - " << _rotation.axis().x() << " - " << _rotation.axis().y() << " - " << _rotation.axis().z();
	//qDebug() << "rot: " << _prevRotation.angle() << " - " << _prevRotation.axis().x() << " - " << _prevRotation.axis().y() << " - " << _prevRotation.axis().z();
	if(_quality == 0)
				_quality = 0.01f;
	if(_imuInterPol > 0)
	{
		AngleAxisf prevRot = AngleAxisf(*_prevRotation);
		CalculateDiffRotations();
		if(mQualityRotation > 0)
		{
			//_accuDiffIMU += abs(AngleAxisf(_diffRotationFromIMU).angle());
			//_accuDiffIMU = _accuDiffIMU * 0.9; 
			//_accuDiffM   += abs(AngleAxisf(_diffRotationFromMarker).angle());
			//_accuDiffM = _accuDiffM * 0.9; 
			//float diff = _accuDiffIMU-_accuDiffM;

			//if(diff > 0)
			//	useOldPosition = 1;

			//newCamRotation = newCamRotation.slerp(_imuInterPol, _prevRotation*_diffRotationFromIMU);
			newCamRotation = *_latestRotationFromMarker;
			_quality += _quality * mQualityRotation * 0.5f;
		}
		else
		{
			//_accuDiffIMU = 0;
			//_accuDiffM = 0;
			newCamRotation = *_prevRotation* *_diffRotationFromIMU;
			Quaternionf diffFromMessuredLost = _latestRotationFromMarker->inverse() * newCamRotation;
			float diffAngle = AngleAxisf(diffFromMessuredLost).angle() * (180.0f/M_PI);
			_quality = (_imuInterPol - diffAngle)/_imuInterPol;
			useOldPosition = 1;
		}
	}
	else
	{
		newCamRotation = *_latestRotationFromMarker;
		if(mQualityRotation > 0)
		{
			_quality += _quality * mQualityRotation * 0.5f;
		}
		else
		{
			_quality -= _quality * 0.5;
		}
	}

	if(_quality < 0.01)
				_quality = 0;
	if(_quality > 1)
				_quality = 1;

	//qDebug() << "quality: " << _quality;

	float mQualityPosition = 0;

	Vector3f newPositionFromMarker = *_prevPosition;
	GetPositionFromMarker(marker, _mUsing, &newCamRotation,  &mQualityPosition, &newPositionFromMarker);

	_prevPosition = new Vector3f(*_position);
	_prevRotation = new Quaternionf(newCamRotation);
	_prevRotationFromIMU = _latestRotationFromIMU;
	_prevRotationFromMarker = _latestRotationFromMarker;

	_position = new Vector3f(newPositionFromMarker);
	_rotation = new Quaternionf(newCamRotation);

	UseFilter();
}

void CameraTrackerFusingIMU::SetInitialRotation()
{
	if(*_latestIMUData != Vector3f(0,0,0))
		_useIMURotation = true;
	if(_initialRotation->angle() != 0)
	{
		Quaternionf* initRot = new Quaternionf(*_initialRotation);

		AngleAxisf yaw = AngleAxisf(_latestIMUData->x() * (M_PI/180.0f), Vector3f(0,1,0));
		AngleAxisf pitch = AngleAxisf(_latestIMUData->y() * (M_PI/180.0f), Vector3f(1,0,0));
		AngleAxisf roll = AngleAxisf(_latestIMUData->z()* (M_PI/180.0f), Vector3f(0,0,-1));

		Quaternionf imuRot = Quaternionf(yaw * pitch * roll);
		Quaternionf* fromImuToInitRot = new Quaternionf(imuRot.inverse() * *initRot); 
		_prevRotation = initRot;
		_prevRotationFromIMU = initRot;
		_toInitialRotation = fromImuToInitRot;
	}
}

void CameraTrackerFusingIMU::CalculateDiffRotations()
{
	_diffRotationFromMarker = new Quaternionf(_prevRotationFromMarker->inverse() * *_latestRotationFromMarker);
	_diffRotationFromIMU = new Quaternionf(_prevRotationFromIMU->inverse() * *_latestRotationFromIMU);
}

float CameraTrackerFusingIMU::GetIMUInterpolation()
{
	return _imuInterPol;
}

void CameraTrackerFusingIMU::SetIMUInterpolation(float value)
{
	if(value > 360)
		_imuInterPol = 360;
	else
	if(value < 0)
		_imuInterPol = 0;
	else
		_imuInterPol = value;

	qDebug() << "change interpolation between marker-rotation and imu-rotation to "+QString::number(_imuInterPol);
}

AngleAxisf* CameraTrackerFusingIMU::GetLatestRotationFromIMU()
{
	return new AngleAxisf(*_latestRotationFromIMU);
}

float CameraTrackerFusingIMU::GetAccuDiff()
{
	return _quality;
}

float CameraTrackerFusingIMU::GetIMUTranslationMagnitude()
{
	return _imuTransMag;
}

float CameraTrackerFusingIMU::GetInstantIMUTranslationMagnitude()
{
	return _instantTransMagn;
}

void CameraTrackerFusingIMU::UpdateIMUTranslationValue(float value)
{
	_imuTransMag = value;
	_expectedAccelMagn = value;
}

void CameraTrackerFusingIMU::GetYawPitchRollFromIMU(float* yaw, float* pitch, float* roll)
{
	double w,x,y,z;
	w = _latestRotationFromIMU->w(); 
	x = _latestRotationFromIMU->x(); 
	y = _latestRotationFromIMU->y(); 
	z = _latestRotationFromIMU->z();

	*yaw = atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z) * (180.0f/M_PI) ;
	*roll = atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z) * (180.0f/M_PI);
	*pitch   = asin(2*x*y + 2*z*w) * (180.0f/M_PI);
}

float CameraTrackerFusingIMU::GetAccelFromIMU()
{
	return _expectedAccelMagn;
}