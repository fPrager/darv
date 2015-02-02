#include "stdafx.h"
#include "VirtualRoom.h"


VirtualRoom::VirtualRoom(float width, float height, float depth, Vector3f fromOriginToRoom, QString name)
{
	_dimensionsInCM = Vector3f(width, height, depth);
	_fromOriginToRoom = fromOriginToRoom;
	_roomObjects = QList<RoomObject>();
	_name = name;
}


VirtualRoom::~VirtualRoom(void)
{
}

int VirtualRoom::RoomObjectSize()
{
	return _roomObjects.size();
}

void VirtualRoom::AddRoomObject(Vector3f* pos, Vector3f* dim, AngleAxisf* rotation, QString name)
{
	_roomObjects.append(RoomObject(pos,dim,rotation,name));
}

void VirtualRoom::GetRoomObject(int index, Vector3f* pos, Vector3f* dim, AngleAxisf* rotation, QString* name)
{
	if(index >= _roomObjects.size())
	{
		qDebug() << "WARNING: Index Out Of Range";
	}
	else
	{
		*pos = *_roomObjects[index]._position;
		*dim = *_roomObjects[index]._dimension;
		*rotation = *_roomObjects[index]._rotation;
		*name = _roomObjects[index]._name;
	}
}

void VirtualRoom::GetRoomObject(int index, RoomObject* roomObject)
{
	if(index >= _roomObjects.size())
	{
		qDebug() << "WARNING: Index Out Of Range";
	}
	else
	{
		*roomObject = _roomObjects[index];
	}
}
