#pragma once
#include "stdafx.h"
#include "ReferenceTracker.h"
#include "ui_OptiTrackDialog.h"

//NatNet SDK
#include "NatNetTypes.h"
#include "NatNetClient.h"
#include <winsock.h>

using namespace Eigen;
using namespace std;

struct RigidBody
{
public:
	QString		_name;
	int			_id;
	Vector3f*	_position;
	Quaternionf* _rotation;
	bool		_isTracked;

	RigidBody(QString name, int id)
	{
		_name = name;
		_id = id;
		_position =new Vector3f(0,0,0);
		_rotation =new Quaternionf(1,0,0,0);
		_isTracked = false;
	}

	void SetRotation(float w, float x, float y, float z)
	{
		_rotation->w() = w;
		_rotation->x() = x;
		_rotation->y() = y;
		_rotation->z() = z;
	}

	void SetPosition(float x, float y, float z)
	{
		_position->x() = x;
		_position->y() = y;
		_position->z() = z;
	}
};


class OptiTracker : public QDialog
{
	Q_OBJECT

public:
	OptiTracker(void);
	~OptiTracker(void);
	void Update(float elapsedTime);
	bool Initialize();
	bool Start(bool withCapture);
	bool Stop();
	void GetPosition(Vector3f* position);
	void GetRotation(AngleAxisf* rotation);
	void GetRotation(float* yaw, float* pitch, float* roll);
	void GetData(Vector3f* position, Quaternionf* rotation);
	void GetRelativeData(Vector3f* translation, Quaternionf* rotation); //cube relative to camera data
	void GetRelativeData(Quaternionf* sideRotation, Quaternionf* rotation); //cube relative to camera data
	void ResetData(Vector3f* position, Quaternionf* rotation);
	QString GetInfo();
	bool HasData();
	bool _available;
	bool _connected;
	bool _isTracked;

public slots:
	void Cancel();
	bool DoConnection();
private:
	void		MakeGUI();
	bool InitalizeNetNatClient(char* ipClient, char* ipServer);
	bool UninitializeNetNatClient();
	NatNetClient _natnetClient;
	int _ipClient[4];
	int _ipServer[4];

	Ui::Dialog _ui;
	bool _capturing;
	bool _useOwnWindow;
	void GetDataFromRigidBodies(Vector3f* position, Quaternionf* rotation);
	bool HasDataFromRigidBodies();
	QString _info;
	Vector3f*		_position;
	Quaternionf*	_rotation;

	bool			_referenceSetted;
	Quaternionf*	_refRotation;
	Vector3f*		_refPosition;

	Quaternionf*	_initRotation;
	Vector3f*		_initPosition;

	void closeEvent(QCloseEvent *event) {
			
	}
};

