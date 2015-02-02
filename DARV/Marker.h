#pragma once
#include <stdafx.h>

using namespace std;
using namespace Eigen;

class Marker
{
public:

	Marker(int cosid, QString name, float widthInCM, Vector3f* position = new Vector3f(0,0,0), AngleAxisf* rotation = new AngleAxisf(0, Vector3f(0,1,0)));
	~Marker(void);

	QString _name;

	AngleAxisf* _rotationInCos;
	Vector3f* _positionInCos;

	Quaternionf* _rotationFromCamera;
	Vector3f*	_translationFromCamera;

	float		_yaw;
	float		_pitch;
	float		_roll;

	bool			_hasRefData;
	Quaternionf*	_rotationFromReference;
	Vector3f*	_translationFromReference;
	float		_areaPx;
	float		_widthPx;
	float		_heightPx;

	int		_CosID;
	bool	_isTracked;
	float	_quality;
	//float	_widthInCM;
	bool	_hasChanged;

private:

};
