#pragma once
#include "stdafx.h"

using namespace Eigen;
using namespace std;
//stores the uncertainty of a pose
//values are splitted on the 6 dimensions of freedom
//x, y, z (position)
//yaw, pitch, roll (rotation)

class Uncertainty
{
public:
	Uncertainty(void)
	{
		_translationSpace = Vector3f(0,0,0);
		_rotationSpace = Vector3f(0,0,0);
		_isKnown = false;
		_up = Vector3f(0,1,0);
		_right = Vector3f(1,0,0);
		_absoluteCenter = Vector3f(0,0,0);
		_autoQAdjusting = true;
		_maxQValue = 0;
		_minQValue = 10000;
		for(int i = 0; i < 6; i++)
			_baseError[i] = false;
	}

	~Uncertainty(void){}
	void GetSpaces(Vector3f* translation, Vector3f* rotation)
	{
		*translation = _translationSpace;
		*rotation = _rotationSpace;
	}
	Vector3f GetRotationSpace(){ return _rotationSpace; }
	Vector3f GetTranslationSpace(){ return _translationSpace;}
	
	void SetSpaces(Vector3f translation, Vector3f rotation)
	{
		_translationSpace = translation;
		_rotationSpace = rotation;
		//CalculateQuality();
	}

	void FromMarkerData(float areaRate, float moveRate = 0, float elapsedTime = 0);
	void FromReference(Uncertainty* sourceUnc, Vector3f* distance);

	void SetKnown(bool isKnown) {_isKnown = isKnown;}
	void SetAbsoluteCenter(Vector3f center){_absoluteCenter = center;}
	Vector3f GetCenter(){return _absoluteCenter;}
	Vector3f GetBack(){return _right.cross(_up);}
	Vector3f GetRigth(){return _right;}
	Vector3f GetUp(){return _up;}
	bool IsKnown(){return _isKnown;}

	float GetQualityValue(){return _quality;}

	bool _autoQAdjusting;
	float _maxQValue;
	float _minQValue;
	bool		_baseError[6];

private:
	void CalculateQuality();
	float GetConeVolume(float height, float angle1, float angle2);
	Vector3f	_translationSpace;
	Vector3f	_rotationSpace;	//as yaw pitch roll
	Vector3f	_up;
	Vector3f	_right;
	bool		_isKnown;
	float		_quality;
	Vector3f	_absoluteCenter;

	//first dummy functions:
	void FromClearance(int areapx, float edgeSharpness = 1);
	void FromDistanceAndPerspective(float distance, float angle);
	
};

