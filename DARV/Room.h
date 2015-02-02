#pragma once
#include <stdafx.h>
using namespace std;
using namespace Eigen;

class RoomMarker
{
public:
	RoomMarker();

	RoomMarker(QString name, Vector3f position, Vector3f normal)
	{
		_name = name;
		_position = position;
		_normal = normal;
	}
	~RoomMarker();

	QString _name;
	Vector3f _position;
	Vector3f _normal;
};

class RoomObject
{
public:
	RoomObject();
	RoomObject(QString name, Vector3f position, Vector4f rotation, Vector3f proportion)
	{
		_name = name;
		_position = position;
		_rotation = rotation;
		_proportion = proportion;
	}
	~RoomObject();

	QString _name;
	Vector3f _position;
	Vector4f _rotation;
	Vector3f _proportion;
};

class Room
{
public:
	Vector3f _proportion;
	QString _name;

	QList<RoomMarker> _fixedMarker;
	QList<RoomObject> _objects;

	Room(QString name)
	{
		_name = name;
		SetProportion();
		MakeFixedMarker();
		MakeObjects();
	}
	
	~Room(void);

private:
	virtual void SetProportion() = 0;
	virtual void MakeFixedMarker() = 0;
	virtual void MakeObjects() = 0;

};

class HoloDeck : public Room
{
public:
	HoloDeck():Room("HoloDeck"){}
	~HoloDeck();

private:
	void SetProportion();
	void MakeObjects();
	void MakeFixedMarker();
};



