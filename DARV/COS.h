#pragma once
#include <stdafx.h>
#include <Marker.h>
using namespace std;
using namespace Eigen;

class COS
{
public:
	COS(QString name, int id)
	{
		_name = name;
		_id = id;
		_translationFromCamera = new Vector3f(0,0,0);
		_rotationFromCamera = new AngleAxisf(0, Vector3f(0,0,0));
		_isTracked = false;
		_relatedMarker = QList<Marker>();
	}
	~COS(void);

	QString			_name;
	int				_id;
	Vector3f*		_translationFromCamera;
	AngleAxisf*		_rotationFromCamera;
	bool			_isTracked;
	QList<Marker>	_relatedMarker;
};

