#include "stdafx.h"
#include "VirtualMarker.h"

void VirtualMarker::Update(Vector3f* translation, Quaternionf* rotation, float elapsedTime)
{
		if(_hasInitValues)
		{
			SetPrev();
		}

		*_trackedTranslation	= *translation;
		*_trackedRotation		= *rotation;

		SetMarkerArea();

		float yaw = 0, pitch = 0, roll = 0;
		TrackedRotation(&yaw, &pitch, &roll);
		//qDebug() << "......yaw: " << QString::number(yaw) << "      pitch: " << QString::number(pitch) << "     roll: " << QString::number(roll);
		*_cameraRotation		= CameraRotation();
		*_cameraPosition		= CameraPosition();

		if(_hasInitValues)
		{
			*_deltaTranslation		= *_trackedTranslationPrev - *_trackedTranslation;
			for(int i = AVRG-1; i > 0; i--)
				_trackedMovements[i] = _trackedMovements[i-1];
			_trackedMovements[0] = _deltaTranslation->norm();

			*_deltaRotation			= _trackedRotationPrev->inverse() * *_trackedRotation;

			*_cameraTranslation = *_cameraPosition - CameraPositionPrev();
			
			if(elapsedTime != 0)
			{
				if(_hasAccelValue)
				{
					float accelTime = _timePrev + (elapsedTime/1000);
					_cameraAccel = (*_cameraSpeedPrev - *_cameraSpeed).norm() / accelTime;

					*_cameraSpeedPrev = *_cameraSpeed;
					_cameraAccelarations[_averageIndex] = _cameraAccel;
					
					float accelSum = 0;
					for(int i = 0; i < MAX_AVERAGE; i++)
					accelSum+=_cameraAccelarations[i];
					_averageCameraAccel = accelSum/(float)MAX_AVERAGE;
				}
				else
				{
					_hasAccelValue = true;
				}
				Vector3f distance = *_cameraTranslation/100;		//from cm in m
				float time = elapsedTime/1000;					//from ms in s
				*_cameraSpeed = distance/time;					//so camerSpeed is in m/s
				_timePrev = elapsedTime;
			}
			_cameraMagnitudes[_averageIndex] = _cameraTranslation->norm();

			float sum = 0;
			for(int i = 0; i < MAX_AVERAGE; i++)
				sum+=_cameraMagnitudes[i];
			_averageCameraMove = sum/(float)MAX_AVERAGE;

			if(_averageIndex < MAX_AVERAGE-1)  
				_averageIndex++;
			else
				_averageIndex = 0;

			_hasDeltaValues = true;
		}
		else
		{
			_hasInitValues = true;
		}
}

void VirtualMarker::CalculateExpectedMovement(float cameraRotationAngle)
{
	if(cameraRotationAngle != 0)
		_expectedMovement = 2*tan(0.5f* cameraRotationAngle * (M_PI/180.0f))*_trackedTranslation->norm();
	else
		_expectedMovement = _deltaTranslation->norm();
}

void VirtualMarker::UpdateUncertainty(float elapsedTime)
{
		//calculate uncertainty here
		_uncer.FromMarkerData(AreaRate(), MoveRate(), elapsedTime);
		_uncer.SetKnown(true);
}

void VirtualMarker::Untrack()
	{
		if(_isTracked)
		{
			_state = UNTRACKED;
			_isTracked = false;
			_hasInitValues = false;
			_hasDeltaValues = false;
			_hasAccelValue = false;
			_cameraAccel = 0;
			_timePrev = 0;
			_stabilityFactor = 0;
			for(int i = 0; i < MAX_AVERAGE; i++)
			{
				_cameraMagnitudes[i] = DEFAULT_AVERAGE;
				_cameraAccelarations[i] = DEFAULT_AVERAGE;
			}
			_averageCameraMove = DEFAULT_AVERAGE;
			_qualityTracking = 0;
			_weight = 0;
			_uncer.SetKnown(false);
			_uncer.SetSpaces(Vector3f(1,1,1), Vector3f(5,2,2));
			ClearMovements();
		}
	}

void VirtualMarker::SetUp(int id, QString name, float width, bool isFixed, Vector3f* positionInSetup, AngleAxisf* rotationInSetup)
	{
		_id = id;
		_cosName = "";
		_name = name;
		_widthInCM = width;
		_isFixed = isFixed;

		_isTracked = false;
		_state = UNTRACKED;
		_hasInitValues = false;
		_hasDeltaValues = false;
		_hasAccelValue = false;
		_cameraAccel = 0;
		_timePrev = 0;

		_qualityVision = 0;
		_qualityTracking = 0;
		_stabilityFactor = 0;
		_weight = 0;

		_positionInSetup = positionInSetup;
		_rotationInSetup = new Quaternionf(*rotationInSetup);

		_uncer = Uncertainty();
		_uncer.SetSpaces(Vector3f(1,1,1), Vector3f(4,4,4));
		_uncer.SetKnown(false);
		ClearMovements();

		_trackedTranslation =new Vector3f(0,0,0);
		_trackedTranslationPrev =new Vector3f(0,0,0);
		_deltaTranslation = new Vector3f(0,0,0);

		_trackedRotation=new Quaternionf(1,0,0,0);
		_trackedRotationPrev=new Quaternionf(1,0,0,0);
		_deltaRotation = new Quaternionf(1,0,0,0);

		_yaw = 0;
		_pitch = 0;
		_roll = 0;

		_trackedRefTranslation =new Vector3f(0,0,0);
		_trackedRefTranslationPrev =new Vector3f(0,0,0);
		_deltaRefTranslation = new Vector3f(0,0,0);

		_trackedRefRotation=new Quaternionf(1,0,0,0);
		_trackedRefRotationPrev=new Quaternionf(1,0,0,0);
		_deltaRefRotation = new Quaternionf(1,0,0,0);
		_hasRefData = false;

		_expectedMovement = 0;
		_areaPx = 0;
		_widthPx = 0;
		_heightPx = 0;

		_cameraPosition = new Vector3f(0,0,0);
		_cameraTranslation = new Vector3f(0,0,0);
		_cameraRotation = new Quaternionf(1,0,0,0);
		_cameraSpeed = new Vector3f(0,0,0);
		_cameraSpeedPrev = new Vector3f(0,0,0);

		_averageIndex = 0;
		for(int i = 0; i < MAX_AVERAGE; i++)
		{
			_cameraAccelarations[i] = DEFAULT_AVERAGE;
			_cameraMagnitudes[i] = DEFAULT_AVERAGE;
		}
		_averageCameraMove = DEFAULT_AVERAGE;
		_averageCameraAccel = DEFAULT_AVERAGE;

		_uncertaintyVector = new Vector3f(0,0,0);
	}

void VirtualMarker::SetMarkerArea()
{
	float widthCAMpx = 640;
	float heightCAMpx = 480;
	float FOVYrad = 31.43779f * M_PI/180.f;
	float FOVXrad = 41.15262f * M_PI/180.f;

	float distance = _trackedTranslation->z();
	float widthCAMcm = 2*tan(FOVXrad / 2)*distance;
	float heightCAMcm = 2*tan(FOVYrad / 2)*distance;

	float widthMcm = _widthInCM, heightMcm = _widthInCM; //we expect a quadric marker!!!
	float widthMpx = widthCAMpx * (widthMcm/widthCAMcm);
	float heightMpx = heightCAMpx * (heightMcm/heightCAMcm);

	Vector3f rotatedPlaneVector = _trackedRotation->_transformVector(Vector3f(0,0,-1));
	float    angleOfMarker = 1;
	float	 rotatedWidthMpx = widthMpx;
	float	 rotatedHeightMpx = heightMpx;
	if(rotatedPlaneVector != Vector3f(0,0,1))
	{
		angleOfMarker = M_PI - acos(rotatedPlaneVector.dot(Vector3f(0,0,1)));

		Vector3f projXY = _trackedRotation->_transformVector(Vector3f(rotatedWidthMpx,0,0));
		projXY.z() = 0;
		rotatedWidthMpx = projXY.norm();

		projXY = _trackedRotation->_transformVector(Vector3f(0,rotatedHeightMpx,0));
		projXY.z() = 0;
		rotatedHeightMpx = projXY.norm();
	}
	
	float areaMpx = widthMpx * heightMpx * cos(angleOfMarker); //projection of rotated shape (marker-rectangle) on plane (cameraimage) 
	_areaPx = areaMpx;
	_widthPx = rotatedWidthMpx;
	_heightPx = rotatedHeightMpx;
}


//calculations of the error indicators

float VirtualMarker::AreaRate()
{
	//the area of the marker to the area of the camera picture
	//the bigger the marker is seen, the better we expect the results of his location
	//the formula to calculate the marker area rate is derived from the projection of the marker image 
	float dis = Distance();
	float fovy = Util::GetCameraFOVY(true);
	float alpha = RotationAngleFromCamera(true);
	//qDebug() << " dis : " << QString::number(dis) << "    "  << " fovy :" << QString::number(fovy) << "    alpha: " + QString::number(alpha);
	return (cos(alpha) *  _widthInCM)/(2.0f * tan(fovy/2.f) * dis);
}

float VirtualMarker::RotationAngleFromCamera(bool inRadian)
	{
		float y = 0, p = 0, r = 0;
		TrackedRotation(&y,&p,&r);
		if(inRadian)
			return acos(cos(y * (M_PI/180.0f)) * cos(p * (M_PI/180.0f)));
		else
			return acos(cos(y * (M_PI/180.0f)) * cos(p * (M_PI/180.0f))) * (180.0f/M_PI);
	}

float VirtualMarker::Distance()
{
	return _trackedTranslation->norm();
}

float VirtualMarker::MoveRate()
{
//this is the ratio between the tracked movement and the expected movement (via imu-datas)
	//qDebug() << "exp: " + QString::number(_expectedMovement) + "       " + " tracked " + QString::number(_deltaTranslation->norm()); 
	
	float trackedMove = GetTrackedMovement();
	
	if(_expectedMovement < 1)
		return 1;
	if(_expectedMovement >= 0)					
		return trackedMove/_expectedMovement;
	else
		return 0;		//it seems that no reference movement could be tracked
}