#pragma once
#include "stdafx.h"
using namespace Eigen;

#include <IMU.h>
#define SPEED_CNT 10
class IMUManager
{
public:
	IMUManager()
	{
		_yaw = 0;
		_pitch = 0;
		_roll = 0;
		_isUsable = false;
		_rotation = 0;
		_prevRotation=0;
		_rotationSpeed = 0;
		_rotationAngle = 0;
		_speedIndex =0;
		for(int i = 0; i < SPEED_CNT; i++)
		{
			_yawSpeed[i] = 0;
			_rollSpeed[i] = 0;
			_pitchSpeed[i] = 0;
		}
	}
	~IMUManager()
	{}
	void InRecordMode(bool flag)
	{
		_inRecordMode = flag;
	}

	virtual bool Initialize() = 0;

	virtual bool Update(bool useFilter, float elapsedTime = 100) = 0;
	virtual void ResetRotation(Quaternionf* rotation) = 0;
	virtual float TranslationMagnitude(int elapsedTime = 0) = 0;
	virtual void TranslationMagnitude(float* x, float* y, float* z) = 0;

	virtual bool FreeIMU() = 0;

	void GetImuValues(float* yaw, float* pitch, float*roll)
	{
		*yaw = _yaw;
		*pitch = _pitch;
		*roll = _roll;
	}

	void GetImuRotation(Quaternionf* rotation)
	{
		*rotation = *_rotation;
	}

	bool _isUsable;
	bool _inRecordMode;
	QString _name;

	float GetRotationSpeed()
	{
		return _rotationSpeed;
	}

	float GetRotationAngle(bool inRadian = false)
	{
		if(inRadian)
			return _rotationAngle * (M_PI/180.0f);
		else
			return _rotationAngle;
	}

	void GetRotationSpeed(float* yawSpeed, float* pitchSpeed, float* rollSpeed)
	{
		float averageYaw = 0;
		float averagePitch = 0;
		float averageRoll = 0;
		for(int i = 0; i < SPEED_CNT; i++)
		{
			averageYaw+= _yawSpeed[i];
			averagePitch+= _pitchSpeed[i];
			averageRoll+= _rollSpeed[i];
		}
		*yawSpeed = averageYaw/(float)SPEED_CNT;
		*pitchSpeed = averagePitch/(float)SPEED_CNT;
		*rollSpeed = averageRoll/(float)SPEED_CNT;
	}

protected:
	float _yaw;
	float _pitch;
	float _roll;
	Quaternionf* _rotation;
	Quaternionf* _prevRotation;
	float _rotationSpeed;
	float _rotationAngle;
	float _yawSpeed[SPEED_CNT];		//in extrinsic view!
	float _rollSpeed[SPEED_CNT];		//in extrinsic view!
	float _pitchSpeed[SPEED_CNT];		//in extrinsic view!
	int _speedIndex;
	void UpdateRotationSpeed(float elapsedTime);

	//void	FromIMUToVuzix();
};

class IMUManagerVuzix: public IMUManager
{
	public:
		IMUManagerVuzix();
		~IMUManagerVuzix();
		bool Initialize();
		void ResetRotation(Quaternionf* rotation);
		bool Update(bool useFilter, float elapsedTime = 100);
		float TranslationMagnitude(int elapsedTime = 0);
		void TranslationMagnitude(float* x, float* y, float* z);
		bool FreeIMU();
	private:
		int		_iwrStatus;
		bool	UpdateTracking(bool useFilter);
		void	IWRFilterTracking( long *yaw, long *pitch, long *roll );
		void	SetToReferenceRotation(long *yaw, long *pitch, long *roll);
		float _referenceRoll;
		float _referencePitch;
		float _referenceYaw;
		Quaternionf* _referenceRotation;
};

class IMUManagerSimon: public IMUManager
{
	public:
		IMUManagerSimon();
		~IMUManagerSimon();
		bool Initialize();
		void ResetRotation(Quaternionf* rotation);
		bool Update(bool useFilter, float elapsedTime = 100);
		float TranslationMagnitude(int elapsedTime = 0);
		void TranslationMagnitude(float* x, float* y, float* z);
		bool FreeIMU();
	private:
		IMU* _imu;
};


