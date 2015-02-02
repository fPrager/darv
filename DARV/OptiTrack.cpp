#include "stdafx.h"
#include "OptiTrack.h"

void __cdecl DataHandler(sFrameOfMocapData* data, void* pUserData);			// receives data from the server
void __cdecl MessageHandler(int msgType, char* msg);		// receives NatNet error mesages
RigidBody* _glasses = new RigidBody("VuzixGlasses", 1);
RigidBody* _cube = new RigidBody("Cube", 2);
bool _hasNewData;

void CheckData(sFrameOfMocapData* data)
{
	if(data == 0) return;
	_glasses->_isTracked = false;
	_cube->_isTracked = false;
	_hasNewData = true;
	for(int i = 0; i < data->nRigidBodies; i++)
	{
		if(data->RigidBodies[i].ID == _glasses->_id)
		{
			if(data->RigidBodies[i].MeanError != 0.0f)
			{
			_glasses->_isTracked = true;
			_glasses->SetPosition(data->RigidBodies[i].x*100,data->RigidBodies[i].y*100,data->RigidBodies[i].z*100);
			_glasses->SetRotation(data->RigidBodies[i].qw,data->RigidBodies[i].qx,data->RigidBodies[i].qy,data->RigidBodies[i].qz);		
			}
			else
				_glasses->_isTracked = false;
		}
		if(data->RigidBodies[i].ID == _cube->_id)
		{
			if(data->RigidBodies[i].MeanError != 0.0f)
			{
			_cube->_isTracked = true;
			_cube->SetPosition(data->RigidBodies[i].x*100,data->RigidBodies[i].y*100,data->RigidBodies[i].z*100);
			_cube->SetRotation(data->RigidBodies[i].qw,data->RigidBodies[i].qx,data->RigidBodies[i].qy,data->RigidBodies[i].qz);
			}
			else
				_cube->_isTracked = false;
		}
	}
}

OptiTracker::OptiTracker(void)
{
	_ipClient[0] = 141;
	_ipClient[1] = 76;
	_ipClient[2] = 67;
	_ipClient[3] = 233;

	_ipServer[0] = 141;
	_ipServer[1] = 76;
	_ipServer[2] = 67;
	_ipServer[3] = 199;

	_available = true;
	_connected = false;
	_useOwnWindow = false;
	_isTracked = false;
	_hasNewData = false;

	_glasses = new RigidBody("VuzixGlasses", 1);
	_cube = new RigidBody("Cube", 2);

	_position = new Vector3f(0,0,0);
	_rotation = new Quaternionf(1,0,0,0);

	_initRotation = new Quaternionf(1,0,0,0);
	_initPosition = new Vector3f(0,0,0);

	_referenceSetted = false;
	_refRotation = new Quaternionf(0,0,0,0);
	_refPosition = new Vector3f(0,0,0);

	_info = "nicht verbunden";
}


OptiTracker::~OptiTracker(void)
{
	UninitializeNetNatClient();
}

bool OptiTracker::Initialize()
{
	if(_useOwnWindow && _available)
	{
		MakeGUI();
	}
	return _available;
}

void OptiTracker::MakeGUI()
{
	_ui.setupUi(this);

	connect(_ui._bConnection, SIGNAL(pressed()), this, SLOT(DoConnection()));
	connect(_ui._bCancel, SIGNAL(pressed()), this, SLOT(Cancel()));

	_ui._IPLocal1->setText("" + QString::number(_ipClient[0]));
	_ui._IPLocal2->setText("" + QString::number(_ipClient[1]));
	_ui._IPLocal3->setText("" + QString::number(_ipClient[2]));
	_ui._IPLocal4->setText("" + QString::number(_ipClient[3]));

	
	_ui._IPServer1->setText("" + QString::number(_ipServer[0]));
	_ui._IPServer2->setText("" + QString::number(_ipServer[1]));
	_ui._IPServer3->setText("" + QString::number(_ipServer[2]));
	_ui._IPServer4->setText("" +QString::number( _ipServer[3]));

	this->show();
}

bool OptiTracker::DoConnection()
{
	 if(!_connected)
	 {
		 	 bool rightInput = true;
		if(_useOwnWindow)
		{


	 if(_ui._IPLocal1->text() == "" || _ui._IPLocal2->text() == "" ||  _ui._IPLocal3->text() == "" || _ui._IPLocal4->text() =="")
	 {
		 rightInput = false;
	 }

	 if(_ui._IPServer1->text() == "" || _ui._IPServer2->text() == "" ||  _ui._IPServer3->text() == "" || _ui._IPServer4->text() =="")
	 {
		 rightInput = false;
	 }
	}

	 QString localIP = QString::number(_ipClient[0])+"." + QString::number(_ipClient[1])+"." +QString::number(_ipClient[2])+"." + QString::number(_ipClient[3]);
	 qDebug() << "Try to connect to OptiTrack with Client IP: " << localIP;

	 QString serverIP = QString::number(_ipServer[0])+"." + QString::number(_ipServer[1])+"." + QString::number(_ipServer[2])+"." + QString::number(_ipServer[3]);
	 qDebug() << "..... and Server IP: " << serverIP;

	 if(rightInput)
	 {
		 if(!InitalizeNetNatClient(localIP.toLatin1().data(), serverIP.toLatin1().data()))
		 {
			UninitializeNetNatClient();
		 }
		 else
		 {
			_connected = true;
			if(_useOwnWindow)
				_ui._bConnection->setText("Disconnect");
		 }
	 }
	 else
	 {
	 		 qDebug() << "ERROR: IP-Input not allowed!";
	 }
	 }
	 else
	 {

		UninitializeNetNatClient();
		if(_useOwnWindow)
			_ui._bConnection->setText("Connect");
		_connected = false;
		_referenceSetted = false;
	 }
	 
	 if(_useOwnWindow)
	 {
	 _ui._IPLocal1->setEnabled(!_connected);
	 _ui._IPLocal2->setEnabled(!_connected);
	 _ui._IPLocal3->setEnabled(!_connected);
	 _ui._IPLocal4->setEnabled(!_connected);

	 _ui._IPServer1->setEnabled(!_connected);
	 _ui._IPServer2->setEnabled(!_connected);
	 _ui._IPServer3->setEnabled(!_connected);
	 _ui._IPServer4->setEnabled(!_connected);
	 }
	 return _connected;
}

bool OptiTracker::InitalizeNetNatClient(char* ipClient, char* ipServer)
{
	unsigned char ver[4];
	_natnetClient.NatNetVersion(ver);

	// Set callback handlers
	_natnetClient.SetMessageCallback(MessageHandler);
	_natnetClient.SetVerbosityLevel(Verbosity_None);
	 _natnetClient.SetDataCallback( DataHandler, &_natnetClient );	// this function will receive data from the server
	

	 int retCode = _natnetClient.Initialize(ipClient, ipServer);

	 if (retCode != ErrorCode_OK)
  {
	  qDebug() << "ERROR: Unable to connect to server. Error Code " + QString::number(retCode);
    return false;
  }
  else
  {
    // print server info
    sServerDescription ServerDescription;
    memset(&ServerDescription, 0, sizeof(ServerDescription));
    _natnetClient.GetServerDescription(&ServerDescription);
    if(!ServerDescription.HostPresent)
    {
		qDebug() << "ERROR: Unable to connect to server. Host not present.";
      return false;
    }
  }
	 qDebug() << "Connected to OptiTrack-Server!";
	 _connected = true;
	return true;
}

bool OptiTracker::UninitializeNetNatClient()
{
	_natnetClient.Uninitialize();
	return true;
}

void OptiTracker::Cancel()
{
	_available = false;
	_connected = false;
	UninitializeNetNatClient();
	this->close();
}

void OptiTracker::Update(float elapsedTime)
{
	if(!_connected)
	{
		_info = "Keine Verbindung";
		return;
	}
	if(!_hasNewData)
	{
		//CheckData(_natnetClient.GetLastFrameOfData());
		if(!_hasNewData)
		{
			_info = "Keine neuen Daten";
			return;
		}
	}
	_hasNewData = false;

	if(!HasDataFromRigidBodies())
	{
		_info = "Unvollstaendige Daten";
		_isTracked = false;
		return;
	}

	_info = "Aktuelle Daten";
	_isTracked = true;
	Vector3f	optiPosition = Vector3f(0,0,0);
	Quaternionf optiRotation = Quaternionf(0,0,0,0);

	GetDataFromRigidBodies(&optiPosition, &optiRotation);

	if(!_referenceSetted)
	{
		*_refPosition = optiPosition;
		*_refRotation = optiRotation;
		_referenceSetted = true;
		qDebug() << "First Data received from Opti-Track...";
	}

	Vector3f translation = *_refPosition-optiPosition;
	Quaternionf rotation = optiRotation.inverse() * *_refRotation;

	//*_position = *_initPosition + translation;
	//*_rotation = *_initRotation * rotation;
	*_position = optiPosition;
	*_rotation = optiRotation;
}

bool OptiTracker::HasDataFromRigidBodies()
{
	return (_cube->_isTracked && _glasses->_isTracked);
}

void OptiTracker::GetDataFromRigidBodies(Vector3f* position, Quaternionf* rotation)
{
	Vector3f planePosition = Vector3f(0,79.5f,0);
	Vector3f relBodyPosition = *(_glasses->_position)-*(_cube->_position) + planePosition;
	relBodyPosition = *(_glasses->_position);
	Quaternionf relBodyOrientation = *(_glasses->_rotation);
	Quaternionf cubeRotation = *(_cube->_rotation);

	*position = relBodyPosition;
	*rotation = relBodyOrientation;
}

bool OptiTracker::Start(bool withCapture)
{
	return false;
}

bool OptiTracker::Stop()
{
	return false;
}

void MessageHandler(int msgType, char* msg)
{
  //	printf("\n[SampleClient] Message received: %s\n", msg);
}

void DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	CheckData(data);
}



bool OptiTracker::HasData()
{
	return _referenceSetted;
}

void OptiTracker::GetPosition(Vector3f* position)
{
	*position = *_position; 
}

void OptiTracker::GetRotation(AngleAxisf* rotation)
{
	rotation = new AngleAxisf(*_rotation);
}

void OptiTracker::GetRotation(float* yaw, float* pitch, float* roll)
{
	double w,x,y,z;
	w = _rotation->w(); 
	x = _rotation->x(); 
	y = _rotation->y(); 
	z = _rotation->z();

	*yaw = atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z) * (180.0f/M_PI) ;
	*pitch = atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z) * (180.0f/M_PI);
	*roll = asin(2*x*y + 2*z*w) * (180.0f/M_PI);
}

void OptiTracker::GetData(Vector3f* position, Quaternionf* rotation)
{
	*position = *_position;
	*rotation = *_rotation;
}

void OptiTracker::GetRelativeData(Vector3f* translation, Quaternionf* rotation)
{
	if(!HasDataFromRigidBodies()) return;
	Quaternionf rotationCamToCubeRel = _glasses->_rotation->inverse() * *_cube->_rotation;
		Vector3f translationCamToCube = *_cube->_position - *_glasses->_position;	//translation in optitrack-cos
		float yaw = 0, pitch = 0, roll = 0;
		double w,x,y,z;
		w = rotationCamToCubeRel.w(); 
		x = rotationCamToCubeRel.x(); 
		y = rotationCamToCubeRel.y(); 
		z = rotationCamToCubeRel.z();

		yaw = atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z) * (180.0f/M_PI) ;
		pitch = atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z) * (180.0f/M_PI);
		roll = asin(2*x*y + 2*z*w) * (180.0f/M_PI);

		Vector3f translationCamToCubeRel = _glasses->_rotation->inverse()._transformVector(translationCamToCube); //from optitrack-cos to camera-cos
		//Vector3f axis = AngleAxisf(rotationCamToCubeRel);
		
		qDebug() << " x: " + QString::number(yaw) + " y: " + QString::number(pitch) + " z: " + QString::number(roll);
		
		*translation = translationCamToCubeRel; 
		*rotation = rotationCamToCubeRel;
		//qDebug() << "x: "+QString::number(translationCamToCubeRel.x()) + " y: "+QString::number(translationCamToCubeRel.y()) + " z: "+QString::number(translationCamToCubeRel.z());
}

void OptiTracker::GetRelativeData(Quaternionf* sideRotation, Quaternionf* rotation)
{
	if(!HasDataFromRigidBodies()) return;
	Quaternionf rotationCamToCubeRel = _glasses->_rotation->inverse() * *sideRotation * *_cube->_rotation;
		float yaw = 0, pitch = 0, roll = 0;
		double w,x,y,z;
		w = rotationCamToCubeRel.w(); 
		x = rotationCamToCubeRel.x(); 
		y = rotationCamToCubeRel.y(); 
		z = rotationCamToCubeRel.z();

		yaw = atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z) * (180.0f/M_PI) ;
		pitch = atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z) * (180.0f/M_PI);
		roll = asin(2*x*y + 2*z*w) * (180.0f/M_PI);

		//Vector3f translationCamToCubeRel = _glasses->_rotation->inverse()._transformVector(translationCamToCube); //from optitrack-cos to camera-cos
		//Vector3f axis = AngleAxisf(rotationCamToCubeRel);
		
		//qDebug() << " x: " + QString::number(yaw) + " y: " + QString::number(pitch) + " z: " + QString::number(roll);
		
		*rotation = rotationCamToCubeRel;
		//qDebug() << "x: "+QString::number(translationCamToCubeRel.x()) + " y: "+QString::number(translationCamToCubeRel.y()) + " z: "+QString::number(translationCamToCubeRel.z());
}

void OptiTracker::ResetData(Vector3f* position, Quaternionf* rotation)
{
	_initRotation = rotation;
	_initPosition = position;
	
	if(!HasDataFromRigidBodies())
		qDebug() << "ERROR: Can't reset OptiTrack because there is no (latest) OptiTrack-Data to reset!";

	Vector3f	optiPosition = Vector3f(0,0,0);
	Quaternionf optiRotation = Quaternionf(0,0,0,0);

	GetDataFromRigidBodies(&optiPosition, &optiRotation);

	*_refRotation = optiRotation;
	*_refPosition = optiPosition;
}

QString OptiTracker::GetInfo()
{
	return _info;
}
