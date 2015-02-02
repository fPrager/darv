#include "stdafx.h"
#include "IMUManager.h"

IMUManagerSimon::IMUManagerSimon():IMUManager()
{
	_name = "Simon's IMU-Manager";
}

IMUManagerSimon::~IMUManagerSimon()
{

}

bool IMUManagerSimon::Initialize()
{
	try
	{
	_imu = new IMU("Resources/IMUConfiguration/magConfig.json");
	}
	catch (int e)
	{
		_isUsable = false;
		return false;
	}
	//if(_imu == NULL)
	//{
	//	qDebug() << "ERROR: Can't initialize IMU";
	//	_isUsable = false;
	//	return false;
	//}
	_isUsable = true;
	//_imu->initRotation(1,0,0,0);
	return true;
}

void IMUManagerSimon::TranslationMagnitude(float* x, float *y, float* z)
{
	double xd = 0, yd = 0, zd = 0;
	_imu->getTranslationAcceleration(&xd,&yd,&zd);
	*x = xd;
	*y = yd;
	*z = zd;
}

float IMUManagerSimon::TranslationMagnitude(int elapsedTime)
{
	float mag = 0;
	if(elapsedTime == 0)
		mag = _imu->getTranslationAccelerationMagnitude();
	else
		mag = _imu->getTranslationAccelerationMagnitude(elapsedTime);

	//if(mag == 0)
	//{
	//	qDebug() << "Falsche Zeit";
	//}
	//else
	//{
	//	qDebug() << "magnitude: " << QString::number(mag);
	//}
	return mag * 9.80665;  //from 1 g to 9,80665 m/s²
}

bool IMUManagerSimon::Update(bool useFilter, float elapsedTime)
{
	bool initRotationSet = _imu->update((int)elapsedTime);
	if(initRotationSet)
	{	
		double q_w,q_x,q_y,q_z;
		_imu->getOrientation(&q_w, &q_x, &q_y, &q_z);
		_rotation = new Quaternionf(q_w,q_x,q_y,q_z);
		*_rotation = _rotation->inverse();

		double w,x,y,z;
		w = _rotation->w(); 
		x = _rotation->x(); 
		y = _rotation->y(); 
		z = _rotation->z();

		_yaw = atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z) * (180.0f/M_PI) ;
		_roll = atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z) * (180.0f/M_PI);
		_pitch   = asin(2*x*y + 2*z*w) * (180.0f/M_PI);
		UpdateRotationSpeed(elapsedTime);
		//if(_roll != _roll)
		//{
		//	_roll = 0;
		//	_pitch = 0;
		//	_yaw = 0;
		//	_imu->setOrientation(1,0,0,0);
		//}	
	}
	return initRotationSet;
}

bool IMUManagerSimon::FreeIMU()
{
	return true;
}

void IMUManagerSimon::ResetRotation(Quaternionf* rotation)
{
	//_imu->setMagnetometerEnabled(false);
	Quaternionf* rotationForImu = new Quaternionf(rotation->inverse());
	_imu->setOrientation(rotationForImu->w(),rotationForImu->x(),rotationForImu->y(),rotationForImu->z());
}