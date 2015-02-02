#pragma once
#include <stdafx.h>

using namespace Eigen;

struct RoomObject
{
public:
	Vector3f* _position;
	Vector3f* _dimension;
	AngleAxisf*	_rotation;
	QString _name;

	RoomObject(Vector3f* position, Vector3f* dimension, AngleAxisf* rotation, QString name = "")
	{
		_position = position;
		_dimension = dimension;
		_rotation = rotation;
		_name = name;
	}
};

class VirtualRoom
{
public:
	VirtualRoom(float width, float height, float depth, Vector3f fromOriginToRoom, QString name = "");
	~VirtualRoom(void);
	
	Vector3f _dimensionsInCM;
	Vector3f _fromOriginToRoom;
	QString  _name;

	void AddRoomObject(Vector3f* pos, Vector3f* dim, AngleAxisf* rotation, QString name = "");
	int RoomObjectSize();
	void GetRoomObject(int index, Vector3f* pos, Vector3f* dim, AngleAxisf* rotation, QString* name);
	void GetRoomObject(int index, RoomObject* roomObject);

private:
	QList<RoomObject> _roomObjects;
};

