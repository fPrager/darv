#include "stdafx.h"
#include <Marker.h>


Marker::Marker(int id,QString name, float widthInCM , Vector3f* position, AngleAxisf* rotation)
{
	_CosID = id;
	_isTracked = false;
	_hasChanged = false;
	_hasRefData = false;
	_rotationInCos = rotation;
	_positionInCos = position;
	//_widthInCM = widthInCM;
	_areaPx = 0;
	_widthPx = 0;
	_heightPx = 0;
	_name = name;
	_rotationFromCamera = new Quaternionf();
	_translationFromCamera = new Vector3f(0,0,0);

	_yaw = 0;
	_pitch = 0;
	_roll = 0;

	_translationFromReference = new Vector3f(0,0,0);
	_rotationFromReference = new Quaternionf();
}

Marker::~Marker(void)
{
}