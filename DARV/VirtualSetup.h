#pragma once
#include <stdafx.h>
#include <SOIL.h>
using namespace std;
using namespace Eigen;
#include<VirtualObject.h>
#include<VirtualMarker.h>
#include<VirtualRoom.h>
#include<Marker.h>
#include<Uncertainty.h>

class VirtualSetup
{
	public:
		//EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		VirtualSetup(QString name, VirtualRoom* room, QList<VirtualMarker*> marker, QList<VirtualObject*> objects);
		~VirtualSetup(){}

		void Load();
		void UpdateMarkerLists();
		//void ComputeCameraDataFromMarker(QList<VirtualMarker*> marker, bool useBestMarker, Vector3f* camPos, AngleAxisf* camRot);
		//void Update(QList<Marker>, Vector3f* imuValues, float sizeFactor = 0.1f);
		void Update(float sizeFactor = 0.1f);

		//glm::mat4 GetViewMatrix();
		//glm::mat4 GetWorldMatrix();

		QList<VirtualObject*> GetObjectsToDraw(bool useTracking = true);

		VirtualRoom* GetRoom();

		void GetVirtualMarker(QList<VirtualMarker*>* fixedMarker, QList<VirtualMarker*>* freeMarker);
		void GetVirtualMarker(QList<VirtualMarker*>* allMarker);
		void ChangeDARV(bool flag);
		//void GetCameraData(bool* isKnown, Vector3f* position, AngleAxisf* rotation);
		QString GetName();

	private:
		VirtualRoom* _room;
		
		QList<VirtualObject*> _objects;
		QList<VirtualMarker*> _allMarker;
		QList<VirtualMarker*> _fixedMarker;
		QList<VirtualMarker*> _freeMarker;

		//Vector3f _cameraPosition;
		//AngleAxisf _cameraRotation;
		//bool _knownCamera;

		QString _name;
};