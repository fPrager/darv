#include "stdafx.h"
#include "IMUManager.h"

void IMUManager::UpdateRotationSpeed(float elapsedTime)
{
		if(_prevRotation == 0)
		{
			_prevRotation = new Quaternionf();
			*_prevRotation = *_rotation;
		}
		AngleAxisf deltaRotation = AngleAxisf(_prevRotation->inverse() * *_rotation);
		_rotationAngle = deltaRotation.angle()* (180.0f/M_PI);
		_rotationSpeed = _rotationAngle/(elapsedTime/1000.f);

		//calculate the imu-rotations in an extrinsic view!
		//Vector3f nz_axis = Vector3f(0,0,1);
		//Vector3f nz_axis_rotated = deltaRotation._transformVector(nz_axis);
		//nz_axis_rotated.y() = 0;
		//float yawAngle = acos(nz_axis.dot(nz_axis_rotated)) * (180.0f/M_PI);
		float yawAngle = _rotationAngle * abs(deltaRotation.axis().y());

		//Vector3f y_axis = Vector3f(0,1,0);
		//Vector3f y_axis_rotated = deltaRotation._transformVector(y_axis);
		//y_axis_rotated.x() = 0;
		//float pitchAngle = acos(y_axis.dot(y_axis_rotated)) * (180.0f/M_PI);
		float pitchAngle = _rotationAngle * abs(deltaRotation.axis().x());

		//Vector3f x_axis = Vector3f(1,0,0);
		//Vector3f x_axis_rotated = deltaRotation._transformVector(x_axis);
		//x_axis_rotated.z() = 0;
		//float rollAngle = acos(x_axis.dot(x_axis_rotated)) * (180.0f/M_PI);
		float rollAngle = _rotationAngle * abs(deltaRotation.axis().z());
		//Quaternionf rot = _prevRotation->inverse() * *_rotation;
		//double w,x,y,z;
		//w = rot.w(); 
		//x = rot.x(); 
		//y = rot.y(); 
		//z = rot.z();

		//float yawAngle = atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z) * (180.0f/M_PI) ;
		//float pitchAngle = atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z) * (180.0f/M_PI);
		//float rollAngle   = asin(2*x*y + 2*z*w) * (180.0f/M_PI);

		if(_speedIndex >= SPEED_CNT) _speedIndex = 0;
		_yawSpeed[_speedIndex] = yawAngle/(elapsedTime/1000.f);
		_pitchSpeed[_speedIndex] = pitchAngle/(elapsedTime/1000.f);
		_rollSpeed[_speedIndex] = rollAngle/(elapsedTime/1000.f);
		_speedIndex++;
		*_prevRotation = *_rotation;
}