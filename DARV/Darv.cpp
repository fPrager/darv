#include "stdafx.h"
#include "darv.h"

DARV::DARV(QWidget *parent)
	: QMainWindow(parent)
{
	_ui.setupUi(this);
	
	_showARView = false;
	_arView.show();
	_ignoreTextChange = false;
	_forceQuality = false;
	
	_fileWriting = false;
	_captureLeftImage = true;
	_captureRightImage = true;
	_dataTime = 0;

	//setup system clock to update the scene
	_clock = new QTimer(this);

	_camView = new CameraView(this);	//all OpenGL-Widgets share the same Context to prevent unnecessary graphical-storage (like camera-texture)
	QRect camGeo = _ui._camWidget->geometry();

	QHBoxLayout *camLayout = new QHBoxLayout;

	camLayout->setContentsMargins(0,0,0,0);
	_ui._camWidget->setLayout(camLayout);
	_ui._camWidget->setGeometry(camGeo);
	_camView->SetSize(camGeo);
	_imageView = new ImageView(this);
	QHBoxLayout *viewLayout = new QHBoxLayout;
	camLayout->addWidget(_imageView);
		camLayout->addWidget(_camView);
	_imageView->SetSize(camGeo);
	_imageView->hide();
	_showImageViewer = false;
	
	_rotationView = new RotationViewer(this);	//all OpenGL-Widgets share the same Context to prevent unnecessary graphical-storage (like camera-texture)
	QRect rotGeo = _ui._rotWidget->geometry();

	QHBoxLayout *rotLayout = new QHBoxLayout;
	rotLayout->addWidget(_rotationView);
	rotLayout->setContentsMargins(0,0,0,0);
	_ui._rotWidget->setLayout(rotLayout);
	_ui._rotWidget->setGeometry(rotGeo);
	_rotationView->SetSize(rotGeo);

	_uncerCos = QList<UncertaintyCOS*>();
	QHBoxLayout *cosLayout = new QHBoxLayout;

	for(int i  = 0; i < FPS_COUNT; i++)
		_fpsWithMarkerUpdate[i] = 30;
	_setTime = false;
	_settedTime = 0;
}


DARV::~DARV()
{

}

void DARV::Initialize()
{
	ShowARViewParameters();
	ShowLists();
	_useReferenceData = false;

	//define gui-events
	connect(_ui._editEyeCorrection, SIGNAL(textChanged(QString)), this, SLOT(CheckAsNumberParameters(QString)));
	connect(_ui._editFovy, SIGNAL(textChanged(QString)), this, SLOT(CheckAsNumberParameters(QString)));
	connect(_ui._editEyeSep, SIGNAL(textChanged(QString)), this, SLOT(CheckAsNumberParameters(QString)));

	connect(_ui._editObjectTransX, SIGNAL(textChanged(QString)), this, SLOT(CheckAsNumberParameters(QString)));
	connect(_ui._editObjectTransY, SIGNAL(textChanged(QString)), this, SLOT(CheckAsNumberParameters(QString)));
	connect(_ui._editObjectTransZ, SIGNAL(textChanged(QString)), this, SLOT(CheckAsNumberParameters(QString)));

	connect(_ui._checkboxStereo, SIGNAL(stateChanged(int)), this, SLOT(ToggleStereo(int)));
	connect(_ui._checkBoxObject, SIGNAL(stateChanged(int)), this, SLOT(ToggleTracking(int)));
	connect(_ui._cbDARV, SIGNAL(stateChanged(int)), this, SLOT(ToggleDARV(int)));
	connect(_ui._cbUseRefData, SIGNAL(stateChanged(int)), this, SLOT(ToggleReferenceData(int)));
	connect(_ui._cbClassification, SIGNAL(stateChanged(int)), this, SLOT(ToggleMarkerClassification(int)));

	connect(_ui._pBResetRot, SIGNAL(pressed()), this, SLOT(ResetRotation()));
	connect(_ui._pBData, SIGNAL(pressed()), this, SLOT(ToggleFileWriting()));
	connect(_ui._pBResetRef, SIGNAL(pressed()), this, SLOT(ResetReferenceTracker()));
	connect(_ui._pBConnect, SIGNAL(pressed()), this, SLOT(DoReferenceTracking()));
	connect(_ui._pBBlur, SIGNAL(pressed()), this, SLOT(DoBlurAnalysing()));
	connect(_ui._pBPause, SIGNAL(pressed()), this, SLOT(ToggleTime()));
	connect(_ui._pBData_4, SIGNAL(pressed()), this, SLOT(StepForward()));

	connect(_ui._cbUseRecords, SIGNAL(stateChanged(int)), this, SLOT(ToogleRecordUsing(int)));
	connect(_ui._listRecords, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(LoadRecord(QListWidgetItem *)));

	connect(_ui._listVObject, SIGNAL(itemClicked( QListWidgetItem *)), this, SLOT(ChangeObject(QListWidgetItem *)));

	connect(_ui._hSInterpol, SIGNAL(valueChanged(int)), this, SLOT(ChangeInterpolationValue(int)));

	connect(_ui._rButtonVUZIX, SIGNAL(toggled(bool)), this, SLOT(ChangeIMUTracker()));
	//connect(_ui._rButtonSIMON, SIGNAL(toggled(bool)), this, SLOT(ChangeIMUTracker()));

	//setup clock
	_clock->setInterval(10);
	connect(_clock, SIGNAL(timeout()), this, SLOT(Update()));
	_clock -> start();
	//QTimer::singleShot(50, this, SLOT(Update())); 
	//QTimer::singleShot(5, this, SLOT(Update())); 
	_timer.start();	
	_secondsCounter = 0;
	_elapsedTimeTracking = 0;
	
	//setup camera image loader for threading opencv-pictureloads
	_camLoader = new CameraImageLoader();

	//initilize tracking
	if(_tManager.Initialize(_camLoader))
		qDebug() << "Tracking successfully initialized";
	else
	{
		qDebug() << "Tracking-Initialization failed.";
		return;
	}
	EnableIMUUsing(_tManager._imu->_isUsable);
	//TODO: cameratracker-object should stay the abstract class CameraTracker!
	
	_camValues = new ValueTree(_ui._tWIMU);
	QStringList camValueHeader = QStringList();
	_camHeader = "CAM";
	_markerHeader = "MARKER";
	_imuHeader = "IMU";
	_refHeader = "REFERENZE";

	camValueHeader.append(_camHeader);
	camValueHeader.append(_markerHeader);
	camValueHeader.append(_imuHeader);

	if(_usingIMU)
	{
		//we can use the IMU of the vuzix glasses to improve the camera-rotation
		_camTracker = new CameraTrackerFusingIMU();
		_arView.ChangeCameraTracker(_camTracker);
	}
	else
	{
		_camTracker = 0;
		CameraTracker* sensorFusingTracker = new CameraTrackerNoIMU();
		_arView.ChangeCameraTracker(sensorFusingTracker);
	}

	if(_tManager.HasReferenceTracker())
	{
		camValueHeader.append(_refHeader);
	}
	
	_camValues->SetHeaders(camValueHeader);
	_camValues->AddElement("pos", "Pos ","");
	_camValues->AddElement("pos_x", "", "","pos");
	_camValues->AddElement("pos_y", "", "","pos");
	_camValues->AddElement("pos_z", "", "","pos");

	_camValues->AddElement("rot", "Rot ","");
	_camValues->AddElement("rot_yaw", "", "","rot");
	_camValues->AddElement("rot_pitch", "", "","rot");
	_camValues->AddElement("rot_roll", "", "","rot");

	_camValues->AddElement("accel_d", "a m/ss: ", "");
	_camValues->AddElement("accel_dinstant", "ia m/ss: ", "");
	_camValues->AddElement("accel_x", "ax: ", "");
	_camValues->AddElement("accel_y", "ay: ", "");
	_camValues->AddElement("accel_z", "az: ", "");

	_timeValues = new ValueTree(_ui._tWTimes);
	_timeValues->AddElement("freeLine1", "","");
	_timeValues->AddElement("fps", "FPS: ","");
	_timeValues->AddElement("elapsed", "UZ: "," ms");
	_timeValues->AddElement("freeLine2", "","");
	_timeValues->AddElement("trackTime", "","% TRACKING","elapsed");
	_timeValues->AddElement("trackTimeMs", ""," ms","trackTime");
	if(_usingIMU)
	{
		_timeValues->AddElement("imuTime", "","% IMU","elapsed");
		_timeValues->AddElement("imuTimeMs", ""," ms","imuTime");
	}
	_timeValues->AddElement("calcTime", "","% CALC","elapsed");
	_timeValues->AddElement("calcTimeMs", ""," ms","calcTime");
	if(_tManager._vstMode)
	{
		_timeValues->AddElement("opencvTime", "","% OPENCV","elapsed");
		_timeValues->AddElement("opencvTimeMs", ""," ms","opencvTime");
	}
	_timeValues->AddElement("renderTime", "","% RENDER","elapsed");
	_timeValues->AddElement("renderTimeMs", ""," ms","renderTime");

	_timeValues->AddElement("captureTime", "","% DATACAP","elapsed");
	_timeValues->AddElement("captureTimeMs", ""," ms","editorTime");

	_timeValues->AddElement("editorTime", "","% EDITOR","elapsed");
	_timeValues->AddElement("editorTimeMs", ""," ms","editorTime");
	_timeStamps = QList<float*>();
	_timeIDs = QStringList();

	_ui._rButtonVST->setCheckable(true);
	_ui._rButtonOST->setCheckable(true);
	if(_tManager._vstMode)
	{
		_ui._rButtonVST->setChecked(true);
		_ui._rButtonOST->setChecked(false);
		int camLeft = 0;
		int camRight = 0;
		int resX = 0;
		int resY = 0;
		int fps = 0;
		_tManager.GetVSTData(&camLeft, &camRight, &resX, &resY, &fps);
		Util::SetCameraResolution(resX, resY);
		QString camIDLeft = _camLoader->StartCamera(camLeft, resX, resY, fps);
		QString camIDRight = _camLoader->StartCamera(camRight, resX, resY, fps);
		if(camIDLeft != "")
		{
			qDebug() << "OpenCV successfully initialized on left Camera";
		}
		if(camIDRight != "")
		{
			qDebug() << "OpenCV successfully initialized on right Camera";
		}
		_arView.InitializeVSTMode(camIDLeft, camIDRight);
		connect(_camLoader,SIGNAL(CameraImageUpdated(IplImage*,QString,bool)),&_arView,SLOT(NewCameraImage(IplImage*, QString,bool)));
	}
	else
	{
		_ui._rButtonOST->setChecked(true);
		_ui._rButtonVST->setChecked(false);
		//set position and size of the little cam-preview
		//_camView->SetSize(_ui._camWidget->geometry());
		//_tManager.SetFrameUpdateFunctions(_camView->FrameUpdate, _camView);
	}

	if(_usingIMU)
	{
		//the bluring will be connected to the accelaration values of the imu
		//so the _blurAnalyser just works with a correct imu-connection
		_blurAnalyser = new BlurAnalyser(_tManager._imu);
	}
	else
	{
		_blurAnalyser = new BlurAnalyser();
	}

	_camView->Load(_arView.GetCameraPositionPointer(),_arView.GetCameraRotationPointer(),_arView.GetCameraQualityPointer());
	_camView->ChangeSetup(_arView.GetCurrentSetup());
	_rotationView->Load();
	//add some marker info-fields
	QList<VirtualMarker*> marker = QList<VirtualMarker*>();
	_arView.GetCurrentSetup()->GetVirtualMarker(&marker);
	_markerValues = new ValueTree(_ui._tWMarker);
	for(int i = 0; i<marker.size(); i++)
	{
		UncertaintyCOS* markerCOS = new UncertaintyCOS(this);
		QRect cosGeo = _ui.tW_data->geometry();
		cosGeo.setWidth(cosGeo.height());
		_ui._hLUncer->addWidget(markerCOS);
		markerCOS->SetSize(cosGeo);
		_uncerCos.append(markerCOS);
		markerCOS->Load("Marker "+ QString::number(marker[i]->_id), &marker[i]->_uncer);
	}


	SetMarkerInfo(marker);

	_fps = 0;
}

void DARV::SetMarkerInfo(QList<VirtualMarker*> vMarker)
{
	_vMarker = vMarker;
	_markerValues->Clear();
	
	_markerValues->SetColumnCnt(vMarker.size());
	
	QStringList header = QStringList();

	_markerValues->AddElement("setupInfo", "","");
	_markerValues->AddElement("realMarker", "","","setupInfo");

	_markerValues->AddElement("setupPos", "Pos","","setupInfo");
	_markerValues->AddElement("setupPos_x", "x ","","setupPos");
	_markerValues->AddElement("setupPos_y", "y ","","setupPos");
	_markerValues->AddElement("setupPos_z", "z ","","setupPos");

	_markerValues->AddElement("setupRot", "Rot","","setupInfo");
	_markerValues->AddElement("setupRot_a", "a ","","setupRot");
	_markerValues->AddElement("setupRot_x", "x ","","setupRot");
	_markerValues->AddElement("setupRot_y", "y ","","setupRot");
	_markerValues->AddElement("setupRot_z", "z ","","setupRot");

	_markerValues->AddElement("mQ", "","");
	_markerValues->AddElement("mQ_T", " Q ","","mQ");
	_markerValues->AddElement("mQ_A", "~Q ","","mQ");

	_markerValues->AddElement("mInfo", "","");
	_markerValues->AddElement("mCamPos", "Pos","","mInfo");
	_markerValues->AddElement("mCamPos_x", "x ","","mCamPos");
	_markerValues->AddElement("mCamPos_y", "y ","","mCamPos");
	_markerValues->AddElement("mCamPos_z", "z ","","mCamPos");

	_markerValues->AddElement("mCamRot", "Rot","","mInfo");
	_markerValues->AddElement("mCamRot_y", "y ","","mCamRot");
	_markerValues->AddElement("mCamRot_p", "p ","","mCamRot");
	_markerValues->AddElement("mCamRot_r", "r ","","mCamRot");

	_markerValues->AddElement("mUnc", "","");
	_markerValues->AddElement("mAreaRate", "m","","mUnc");
	_markerValues->AddElement("mMoveRate", "Aw","","mUnc");

	_markerValues->AddElement("mUncPos", "PosUnsicherheit","","mUnc");
	_markerValues->AddElement("mUncPos_x", "x ","","mUncPos");
	_markerValues->AddElement("mUncPos_y", "y ","","mUncPos");
	_markerValues->AddElement("mUncPos_z", "z ","","mUncPos");

	_markerValues->AddElement("mUncRot", "RotUnsicherheit","","mUnc");
	_markerValues->AddElement("mUncRot_y", "y ","","mUncRot");
	_markerValues->AddElement("mUncRot_p", "p ","","mUncRot");
	_markerValues->AddElement("mUncRot_r", "r ","","mUncRot");
	
	_markerValues->AddElement("mCamStab", "sf ","","mInfo");
	_markerValues->AddElement("mCamMove", "s: "," m","mInfo");
	_markerValues->AddElement("mCamAccel", "a: "," m/ss","mInfo");

	_markerValues->AddElement("mVisio", "","");
	_markerValues->AddElement("mVisioQ", "Q: ","","mVisio");
	_markerValues->AddElement("mVisioDis", "D: ","","mVisio");
	_markerValues->AddElement("mVisioAngle", "W: ","","mVisio");

	_markerValues->AddElement("mVisioPx", "Pixel","","mVisio");
	_markerValues->AddElement("mVisioPx_a", "a ","","mVisioPx");
	_markerValues->AddElement("mVisioPx_w", "w ","","mVisioPx");
	_markerValues->AddElement("mVisioPx_h", "h ","","mVisioPx");

	_markerValues->AddElement("mVisioPos", "Pos","","mVisio");
	_markerValues->AddElement("mVisioPos_x", "x ","","mVisioPos");
	_markerValues->AddElement("mVisioPos_y", "y ","","mVisioPos");
	_markerValues->AddElement("mVisioPos_z", "z ","","mVisioPos");

	_markerValues->AddElement("mVisioRot", "Rot","","mVisio");
	_markerValues->AddElement("mVisioRot_y", "y ","","mVisioRot");
	_markerValues->AddElement("mVisioRot_p", "p ","","mVisioRot");
	_markerValues->AddElement("mVisioRot_r", "r ","","mVisioRot");

	_markerValues->AddElement("mRef", "","");
	_markerValues->AddElement("mRefPos", "Pos","","mRef");
	_markerValues->AddElement("mRefPos_x", "x ","","mRefPos");
	_markerValues->AddElement("mRefPos_y", "y ","","mRefPos");
	_markerValues->AddElement("mRefPos_z", "z ","","mRefPos");

	_markerValues->AddElement("mRefRot", "Rot","","mRef");
	_markerValues->AddElement("mRefRot_y", "y ","","mRefRot");
	_markerValues->AddElement("mRefRot_p", "p ","","mRefRot");
	_markerValues->AddElement("mRefRot_r", "r ","","mRefRot");

	for(int i = 0; i <_vMarker.size(); i++)
	{
		VirtualMarker* vm = _vMarker[i];
		if(vm->_name != "")
			header.append(vm->_name);
		else
			header.append("Marker "+QString::number(i));
			
		//free or fixed
		QString flexible = "frei";
		if(vm->_isFixed)
			flexible = "fest";
		_markerValues->UpdateElement("setupInfo",flexible,i);

		//attached marker info
		QString id = "COS "+QString::number(vm->_id);
		if(vm->_cosName != "") id = vm->_cosName;
		_markerValues->UpdateElement("realMarker",id,i);

		//position in setup
		Vector3f pos = *(vm->_positionInSetup);
		_markerValues->UpdateElement("setupPos_x",RoundST(pos.x()),i);
		_markerValues->UpdateElement("setupPos_y",RoundST(pos.y()),i);
		_markerValues->UpdateElement("setupPos_z",RoundST(pos.z()),i);

		//rotation in setup
		AngleAxisf rot = AngleAxisf(*(vm->_rotationInSetup));
		_markerValues->UpdateElement("setupRot_a",RoundST(rot.angle()),i);
		_markerValues->UpdateElement("setupRot_x",RoundST(rot.axis().x()),i);
		_markerValues->UpdateElement("setupRot_y",RoundST(rot.axis().y()),i);
		_markerValues->UpdateElement("setupRot_z",RoundST(rot.axis().z()),i);
		
		//qualityinfo
		_markerValues->UpdateElement("mQ",vm->State(),i);

		//quality of tracking
		_markerValues->UpdateElement("mQ_T",RoundST(vm->_qualityTracking),i);
		_markerValues->UpdateElement("mQ_A",RoundST(vm->_qualityTracking),i);
		
		//camera info
		if(vm->_isFixed && vm->_isTracked)
		{
			_markerValues->UpdateElement("mInfo","Kameradaten",i);

			Vector3f camP = vm->CameraPosition();
			_markerValues->UpdateElement("mCamPos_x",RoundST(camP.x()),i);
			_markerValues->UpdateElement("mCamPos_y",RoundST(camP.y()),i);
			_markerValues->UpdateElement("mCamPos_z",RoundST(camP.z()),i);

			float yaw, pitch, roll = 0;
			vm->CameraRotation(&yaw, &pitch, &roll);

			_markerValues->UpdateElement("mCamRot_y",RoundST(yaw),i);
			_markerValues->UpdateElement("mCamRot_p",RoundST(pitch),i);
			_markerValues->UpdateElement("mCamRot_r",RoundST(roll),i);

			_markerValues->UpdateElement("mCamStab", "0",i);
			_markerValues->UpdateElement("mCamMove", "0",i);
			_markerValues->UpdateElement("mCamAccel", "0",i);
		}
		else
		{
			_markerValues->UpdateElement("mInfo","Keine Kameradaten",i);
			_markerValues->UpdateElement("mCamStab", "0",i);
			_markerValues->UpdateElement("mCamMove", "0",i);
			_markerValues->UpdateElement("mCamAccel", "0",i);
		}

		if(vm->_isTracked)
		{
			_markerValues->UpdateElement("mVisio", "Metaio-Daten",i);
			_markerValues->UpdateElement("mVisioQ", "",i);
			_markerValues->UpdateElement("mVisioDis", "",i);
			_markerValues->UpdateElement("mVisioAngle", "",i);
		}
		else
		{
			_markerValues->UpdateElement("mVisio", "Keine Metaio-Daten",i);
			_markerValues->UpdateElement("mVisioQ", "?",i);
			_markerValues->UpdateElement("mVisioDis", "?",i);
			_markerValues->UpdateElement("mVisioAngle", "?",i);
		}
		_markerValues->UpdateElement("mVisioPos_x", "",i);
		_markerValues->UpdateElement("mVisioPos_y", "",i);
		_markerValues->UpdateElement("mVisioPos_z", "",i);

		_markerValues->UpdateElement("mVisioRot_y", "",i);
		_markerValues->UpdateElement("mVisioRot_p", "",i);
		_markerValues->UpdateElement("mVisioRot_r", "",i);

		_markerValues->UpdateElement("mVisioPx_a", "0",i);
		_markerValues->UpdateElement("mVisioPx_w", "0",i);
		_markerValues->UpdateElement("mVisioPx_h", "0",i);

		if(vm->_hasRefData)
		{
			_markerValues->UpdateElement("mRef", "Referenz",i);
		}
		else
		{
			_markerValues->UpdateElement("mRef", "Keine Referenz",i);
		}
		_markerValues->UpdateElement("mRefPos_x", "",i);
		_markerValues->UpdateElement("mRefPos_y", "",i);
		_markerValues->UpdateElement("mRefPos_z", "",i);

		_markerValues->UpdateElement("mRefRot_y", "",i);
		_markerValues->UpdateElement("mRefRot_p", "",i);
		_markerValues->UpdateElement("mRefRot_r", "",i);
			
		_markerValues->UpdateElement("mUncPos_x","?",i);
			_markerValues->UpdateElement("mUncPos_y","?",i);
			_markerValues->UpdateElement("mUncPos_z","?",i);

			_markerValues->UpdateElement("mUncRot_y","?",i);
			_markerValues->UpdateElement("mUncRot_p","?",i);
			_markerValues->UpdateElement("mUncRot_r","?",i);
			_markerValues->UpdateElement("mMoveRate","?",i);
			_markerValues->UpdateElement("mAreaRate","?",i);
	

}

	_markerValues->SetHeaders(header);
}

void DARV::UpdateMarkerInfo()
{
	for(int i = 0; i <_vMarker.size(); i++)
	{
		VirtualMarker* vm = _vMarker[i];
		//free or fixed
		QString flexible = "frei";
		if(vm->_isFixed)
			flexible = "fest";
		_markerValues->UpdateElement("setupInfo",flexible,i);

		//attached marker info
		QString id = "COS "+QString::number(vm->_id);
		if(vm->_cosName != "") id = vm->_cosName;
		_markerValues->UpdateElement("realMarker",id,i);

		//position in setup
		Vector3f pos = *(vm->_positionInSetup);
		_markerValues->UpdateElement("setupPos_x",RoundST(pos.x()),i);
		_markerValues->UpdateElement("setupPos_y",RoundST(pos.y()),i);
		_markerValues->UpdateElement("setupPos_z",RoundST(pos.z()),i);

		//rotation in setup
		AngleAxisf rot = AngleAxisf(*(vm->_rotationInSetup));
		_markerValues->UpdateElement("setupRot_a",RoundST(rot.angle()),i);
		_markerValues->UpdateElement("setupRot_x",RoundST(rot.axis().x()),i);
		_markerValues->UpdateElement("setupRot_y",RoundST(rot.axis().y()),i);
		_markerValues->UpdateElement("setupRot_z",RoundST(rot.axis().z()),i);
		
		//qualityinfo
		_markerValues->UpdateElement("mQ",vm->State(),i);

		//quality of tracking
		_markerValues->UpdateElement("mQ_T",RoundST(vm->_qualityTracking),i);
		_markerValues->UpdateElement("mQ_A",RoundST(vm->_qualityTracking),i);
		
		//camera info
		if(vm->_isFixed && vm->_isTracked)
		{
			_markerValues->UpdateElement("mInfo","Kameradaten",i);

			Vector3f camP = vm->CameraPosition();
			_markerValues->UpdateElement("mCamPos_x",RoundST(camP.x()),i);
			_markerValues->UpdateElement("mCamPos_y",RoundST(camP.y()),i);
			_markerValues->UpdateElement("mCamPos_z",RoundST(camP.z()),i);

			float yaw, pitch, roll = 0;
			vm->CameraRotation(&yaw, &pitch, &roll);

			_markerValues->UpdateElement("mCamRot_y",RoundST(yaw),i);
			_markerValues->UpdateElement("mCamRot_p",RoundST(pitch),i);
			_markerValues->UpdateElement("mCamRot_r",RoundST(roll),i);

			_markerValues->UpdateElement("mCamStab", RoundST(vm->_stabilityFactor, 5),i);
			_markerValues->UpdateElement("mCamMove", RoundST(vm->_averageCameraMove, 5),i);
			_markerValues->UpdateElement("mCamAccel",RoundST(vm->_averageCameraAccel, 5),i);
		}
		else
		{
			_markerValues->UpdateElement("mInfo","Keine Kameradaten",i);
			_markerValues->UpdateElement("mCamStab", "0",i);
			_markerValues->UpdateElement("mCamMove", "0",i);
			_markerValues->UpdateElement("mCamAccel", "0",i);
		}

		if(vm->_isTracked)
		{
			_markerValues->UpdateElement("mVisio", "Metaio-Daten",i);
			_markerValues->UpdateElement("mVisioQ", RoundST(vm->_qualityVision),i);
			_markerValues->UpdateElement("mVisioDis", RoundST(vm->_trackedTranslation->norm()),i);
			_markerValues->UpdateElement("mVisioAngle", RoundST(vm->RotationAngleFromCamera()),i);

			Vector3f mPos = *(vm->_trackedTranslation);
			_markerValues->UpdateElement("mVisioPos_x", RoundST(mPos.x()),i);
			_markerValues->UpdateElement("mVisioPos_y", RoundST(mPos.y()),i);
			_markerValues->UpdateElement("mVisioPos_z", RoundST(mPos.z()),i);

			float yaw, pitch, roll = 0;
			vm->TrackedRotation(&yaw, &pitch, &roll);
			_markerValues->UpdateElement("mVisioRot_y", RoundST(yaw),i);
			_markerValues->UpdateElement("mVisioRot_p", RoundST(pitch),i);
			_markerValues->UpdateElement("mVisioRot_r", RoundST(roll),i);

			Vector3f tranUnc = vm->_uncer.GetTranslationSpace();
			
			_markerValues->UpdateElement("mUncPos_x",RoundST(tranUnc.x()) + "(" + QString::number(vm->_uncer._baseError[0]) + ")",i);
			_markerValues->UpdateElement("mUncPos_y",RoundST(tranUnc.y()) + "(" + QString::number(vm->_uncer._baseError[1]) + ")",i);
			_markerValues->UpdateElement("mUncPos_z",RoundST(tranUnc.z()) + "(" + QString::number(vm->_uncer._baseError[2]) + ")",i);

			Vector3f rotUnc = vm->_uncer.GetRotationSpace();
			_markerValues->UpdateElement("mUncRot_y",RoundST(rotUnc.x())+ "(" + QString::number(vm->_uncer._baseError[3]) + ")",i);
			_markerValues->UpdateElement("mUncRot_p",RoundST(rotUnc.y())+ "(" + QString::number(vm->_uncer._baseError[4]) + ")",i);
			_markerValues->UpdateElement("mUncRot_r",RoundST(rotUnc.z())+ "(" + QString::number(vm->_uncer._baseError[5]) + ")",i);


			_markerValues->UpdateElement("mMoveRate",RoundST(vm->MoveRate()),i);
			_markerValues->UpdateElement("mAreaRate",RoundST(vm->AreaRate()),i);
		}
		else
		{
			_markerValues->UpdateElement("mVisio", "Keine Metaio-Daten",i);
			
			_markerValues->UpdateElement("mUncPos_x","?",i);
			_markerValues->UpdateElement("mUncPos_y","?",i);
			_markerValues->UpdateElement("mUncPos_z","?",i);

			Vector3f rotUnc = vm->_uncer.GetRotationSpace();
			_markerValues->UpdateElement("mUncRot_y","?",i);
			_markerValues->UpdateElement("mUncRot_p","?",i);
			_markerValues->UpdateElement("mUncRot_r","?",i);

			
			_markerValues->UpdateElement("mMoveRate","?",i);
			_markerValues->UpdateElement("mAreaRate","?",i);
		}

		_markerValues->UpdateElement("mVisioPx_a", RoundST(vm->_areaPx),i);
		_markerValues->UpdateElement("mVisioPx_w", RoundST(vm->_widthPx),i);
		_markerValues->UpdateElement("mVisioPx_h", RoundST(vm->_heightPx),i);

		if(vm->_hasRefData)
		{
			_markerValues->UpdateElement("mRef", "Referenz",i);

			Vector3f mPos = *(vm->_trackedRefTranslation);
			_markerValues->UpdateElement("mRefPos_x", RoundST(mPos.x()),i);
			_markerValues->UpdateElement("mRefPos_y", RoundST(mPos.y()),i);
			_markerValues->UpdateElement("mRefPos_z", RoundST(mPos.z()),i);

			float yaw, pitch, roll = 0;
			vm->TrackedRefRotation(&yaw, &pitch, &roll);
			_markerValues->UpdateElement("mRefRot_y", RoundST(yaw),i);
			_markerValues->UpdateElement("mRefRot_p", RoundST(pitch),i);
			_markerValues->UpdateElement("mRefRot_r", RoundST(roll),i);
		}
		else
		{
			_markerValues->UpdateElement("mRef", "Keine Referenz",i);
		}
	}
}

void DARV::Update()
{
	//_clock -> stop();
	float elapsedTime = _timer.elapsed();
	_elapsedTimeTracking += elapsedTime;

	_timer.restart();
	float lastTime = 0;

	if(!_newMarkerData)
	{
		_fps = 1/((elapsedTime)/1000.f);
	}
	else
	{
		float fpsWithPrevMarkerUpdate = 1/((elapsedTime)/1000.f);
		float fpsSum = 0;
		for(int i = FPS_COUNT - 1; i > 0; i--)
		{
			_fpsWithMarkerUpdate[i] =  _fpsWithMarkerUpdate[i-1];
			fpsSum += _fpsWithMarkerUpdate[i];
		}
		_fpsWithMarkerUpdate[0] = fpsWithPrevMarkerUpdate;
		fpsSum += _fpsWithMarkerUpdate[0];
		float averageFPS = fpsSum/FPS_COUNT;
		//int rounded = _fpsWithMarkerUpdate*1000;
		//_ui._lTime->setText("Szene                FPS:   "+ QString::number(((int)_fpsWithMarkerUpdate*1000)/1000.f));
		
		_timeValues->UpdateElement("fps", ">30");
		_timeValues->UpdateElement("elapsed", RoundST(elapsedTime));
	}
	if(_setTime)
	{
		_tManager.Update(_settedTime, &_newMarkerData);
		_settedTime = 0;
	}
	else
		_tManager.Update(elapsedTime, &_newMarkerData);
	AddTimeStamp("trackTime");

	if(_tManager.IMUUsable())
	{
		if(_tManager.UpdateIMU(true, _elapsedTimeTracking))
		{
			float yaw = 0;
			float pitch = 0;
			float roll = 0;
			//_tManager._imu->GetImuValues(&yaw, &pitch, &roll);

			Quaternionf* rotation =new Quaternionf(1,0,0,0);
			_tManager.GetImuRotation(rotation);
			_camTracker->UpdateIMURotationValue(rotation);
			_camTracker->UpdateIMURotationAngle(_tManager.GetImuRotationAngle());

			if(_newMarkerData)
			{
				float transMagLong = _tManager.TranslationMagnitude(_elapsedTimeTracking);
				_camTracker->UpdateIMUTranslationValue(transMagLong);
				//qDebug() << "long with " << QString::number(_elapsedTimeTracking) << " got " << QString::number(transMagLong);
			}
			//else
			//{
				float transMagInstant = _tManager.TranslationMagnitude(elapsedTime);
				_camTracker->QuickIMUCheck(transMagInstant);
				//qDebug() << "short with " << QString::number(elapsedTime) << " got " << QString::number(transMagInstant);
			//}
		}
		AddTimeStamp("imuTime");
	}
	float opencvTime = 0;
	if(_tManager._vstMode )
	{
			_camLoader->Update();
			AddTimeStamp("opencvTime");
	}

	if(!_useReferenceData || !_tManager.HasReferenceTracker() || true) //if(!_useReferenceData || !_tManager.HasReferenceTracker())
	{
		if(_newMarkerData)
		{
			QList<Marker> marker = _tManager.GetMarkerInfo();
			_arView.UpdateTracker(_elapsedTimeTracking,marker,_useReferenceData);
			_elapsedTimeTracking = 0;
			AddTimeStamp("calcTime");
		}
	}
	else
	{
		CameraTracker* tracker = _arView.GetCameraTrackerPointer();
		Vector3f* position = new Vector3f(0,0,0);
		Quaternionf* rotation = new Quaternionf(1,0,0,0);
		_tManager.GetReferenceCamData(position, rotation);
		tracker->UseThisCameraData(position, rotation);
	}
	if(!_ui._cbFixedQuality->isChecked())
	{
		_ui._vSAD->setEnabled(true);
		float quality = _ui._vSAD->value(); 
		_arView.GetCameraTrackerPointer()->SetQuality(quality/100.f);
	}

	if(!_blurAnalyser->_isAnalysing)
		_arView.Update(elapsedTime);
	
	_blurAnalyser->Update(elapsedTime);
	AddTimeStamp("renderTime");

	UpdateMarkerInfo();
	UpdateCameraDataView(elapsedTime);

	for(int i = 0; i < _uncerCos.size(); i++)
	{
		_uncerCos[i]->update();
	}

	if(_tManager.HasReferenceTracker())
		_ui._lRefData->setText(_tManager.GetReferenceInfo());

	AddTimeStamp("editorTime");

	if(_fileWriting)
		DoFileWriting(elapsedTime);

	AddTimeStamp("captureTime");

	if(_newMarkerData)
	{
		WriteTimePercentage();
	}

	_timeStamps.clear();
	_timeIDs.clear();
	//_clock -> start(2);
	
	//if(_newMarkerData)
		_elapsedTimeTracking = 0;
}

void DARV::UpdateCameraDataView(float elapsedTime)
{
	_camView->Update(elapsedTime);
	if(_tManager.HasReferenceData())
	{
		Vector3f* position = new Vector3f(0,0,0);
		Quaternionf* rotation = new Quaternionf(1,0,0,0);
		_tManager.GetReferenceCamData(position, rotation);
		_camView->SetRefFrustrum(position, new AngleAxisf(*rotation), true);
	}
	CameraTracker* currentCamTracker = _arView.GetCameraTrackerPointer();
	AngleAxisf* latestCamRot = currentCamTracker->GetLatestRotation();
	AngleAxisf* latestMarkerRot = currentCamTracker->GetLatestRotationFromMarker();
	_rotationView->Update(elapsedTime);

	float quality = currentCamTracker->GetQuality();
	_ui._lADValue->setText(RoundST(quality,2));

	if(_ui._cbFixedQuality->isChecked())
	{
		_ui._vSAD->setEnabled(true);
		_ui._vSAD->setValue(quality*100);
	}

	if(_newMarkerData)
	{
		int camIndex = _camValues->GetColumnIndex(_camHeader);
		Vector3f* position = currentCamTracker->GetLatestPosition();
		_camValues->UpdateElement("pos_x", RoundST(position->x()),camIndex);
		_camValues->UpdateElement("pos_y", RoundST(position->y()), camIndex);
		_camValues->UpdateElement("pos_z", RoundST(position->z()), camIndex);

		float yaw, pitch, roll;
		currentCamTracker->GetYawPitchRoll(&yaw, &pitch, &roll);
		_camValues->UpdateElement("rot_yaw", RoundST(yaw,0),camIndex);
		_camValues->UpdateElement("rot_pitch", RoundST(pitch,0), camIndex);
		_camValues->UpdateElement("rot_roll", RoundST(roll,0), camIndex);
		_camValues->UpdateElement("accel_d", RoundST(currentCamTracker->GetAccel(),4),camIndex);

		int markerIndex = _camValues->GetColumnIndex(_markerHeader);
		currentCamTracker->GetYawPitchRollFromMarker(&yaw, &pitch, &roll);
		_camValues->UpdateElement("rot_yaw", RoundST(yaw,0),markerIndex);
		_camValues->UpdateElement("rot_pitch", RoundST(pitch,0), markerIndex);
		_camValues->UpdateElement("rot_roll", RoundST(roll,0), markerIndex);
		_camValues->UpdateElement("accel_d", RoundST(currentCamTracker->GetAccelFromMarker(),4),markerIndex);
	}

	if(_usingIMU)
	{
		AngleAxisf* latestIMURot = _camTracker->GetLatestRotationFromIMU();
		_rotationView->UpdateRotations(*latestCamRot,*latestMarkerRot,*latestIMURot);

		if(_newMarkerData)
		{
			int imuIndex = _camValues->GetColumnIndex(_imuHeader);
			_camValues->UpdateElement("pos_x", "-", imuIndex);
			_camValues->UpdateElement("pos_y", "-", imuIndex);
			_camValues->UpdateElement("pos_z", "-", imuIndex);

			float imuYaw, imuPitch, imuRoll;
			_camTracker->GetYawPitchRollFromIMU(&imuYaw, &imuPitch, &imuRoll);
			_camValues->UpdateElement("rot_yaw", RoundST(imuYaw,0),imuIndex);
			_camValues->UpdateElement("rot_pitch", RoundST(imuPitch,0), imuIndex);
			_camValues->UpdateElement("rot_roll", RoundST(imuRoll,0), imuIndex);
			_camValues->UpdateElement("accel_d", RoundST(_camTracker->GetAccelFromIMU(),4),imuIndex);
			_camValues->UpdateElement("accel_dinstant", RoundST(_camTracker->GetInstantIMUTranslationMagnitude(),4),imuIndex);
			float x = 0, y = 0, z = 0;
			_tManager._imu->GetRotationSpeed(&x, &y, &z);
			_camValues->UpdateElement("accel_x", RoundST(x,0),imuIndex);
			_camValues->UpdateElement("accel_y", RoundST(y,0),imuIndex);
			_camValues->UpdateElement("accel_z", RoundST(z,0),imuIndex);
		}
	}
	else
	{
		_rotationView->UpdateRotations(*latestCamRot,*latestMarkerRot);
	}

	if(_tManager.HasReferenceTracker())
	{
		int refIndex = _camValues->GetColumnIndex(_refHeader);
		Vector3f* position = new Vector3f(0,0,0);
		_tManager.GetReferencePosition(position);
		_camValues->UpdateElement("pos_x", RoundST(position->x()),refIndex);
		_camValues->UpdateElement("pos_y", RoundST(position->y()), refIndex);
		_camValues->UpdateElement("pos_z", RoundST(position->z()), refIndex);

		float yaw, pitch, roll;
		_tManager.GetReferenceRotation(&yaw, &pitch, &roll);
		_camValues->UpdateElement("rot_yaw", RoundST(yaw,0),refIndex);
		_camValues->UpdateElement("rot_pitch", RoundST(pitch,0), refIndex);
		_camValues->UpdateElement("rot_roll", RoundST(roll,0), refIndex);
	}
}

void DARV::AddTimeStamp(QString id)
{
	//qDebug() << "Adding stamp: " << id;
	float time = _timer.elapsed();
	_timeIDs.append(id);
	_timeStamps.append(new float(time));
}

void DARV::WriteTimePercentage()
{
	float wholeTime = 0;
	QList<float*> timeDiffs = QList<float*>();

	for(int i = 0; i < _timeStamps.size(); i++)
	{
		float timeDiff = *_timeStamps[i];
		if(i != 0)
			timeDiff = *_timeStamps[i] - *_timeStamps[i-1];

		timeDiffs.append(new float(timeDiff));
		wholeTime += timeDiff;
		//qDebug() << "timeDiff " << QString::number(timeDiff);
	}
	for(int i = 0; i < timeDiffs.size(); i++)
	{
		float percentage = *timeDiffs[i]/wholeTime * 100;
		if(!_timeValues->HasElement(_timeIDs[i]))
		{
			_timeValues->AddElement(_timeIDs[i] ,""," % "+_timeIDs[i]);
			_timeValues->AddElement(_timeIDs[i]+"Ms" ,""," ms",_timeIDs[i]);
		}
		_timeValues->UpdateElement(_timeIDs[i], RoundST(percentage,0));
		_timeValues->UpdateElement(_timeIDs[i]+"Ms", RoundST(*timeDiffs[i],0));
	}
	_timeStamps.clear();
	_timeIDs.clear();
	//_fpsWithMarkerUpdate = 1/((wholeTime)/1000.f);
	//_timeValues->UpdateElement("fps", RoundST(_fpsWithMarkerUpdate));
	//_timeValues->UpdateElement("elapsed", RoundST(wholeTime));
	//qDebug() << "Clear stamps";
}

void DARV::DoFileWriting(float elapsedTime, bool lastCall)
{
	FileWriter* fw = FileWriter::GetInstance();
	_dataTime += elapsedTime;
	//write camera datas
	/*CameraTracker* currentCamTracker = _arView.GetCameraTrackerPointer();
	fw->UpdateEntry("CameraData", "timeStamp",	QString::number(_dataTime));
	fw->UpdateEntry("CameraData", "Quality",	QString::number(currentCamTracker->GetQuality()));
	fw->UpdateEntry("CameraData", "X",	QString::number(currentCamTracker->GetLatestPosition()->x()));
	fw->UpdateEntry("CameraData", "Y",	QString::number(currentCamTracker->GetLatestPosition()->y()));
	fw->UpdateEntry("CameraData", "Z",	QString::number(currentCamTracker->GetLatestPosition()->z()));
	float yaw, pitch, roll = 0;
	currentCamTracker->GetYawPitchRoll(&yaw,&pitch,&roll);
	fw->UpdateEntry("CameraData", "Yaw",	QString::number(yaw));
	fw->UpdateEntry("CameraData", "Pitch",	QString::number(pitch));
	fw->UpdateEntry("CameraData", "Roll",	QString::number(roll));*/
	//if(_newMarkerData)
	//	fw->UpdateEntry("CameraData", "newMarker",	"1");	
	//else
	//	fw->UpdateEntry("CameraData", "newMarker",	"0");	
	//fw->WriteToFile(QString::number(_dataTime), "CameraData");

	//write marker datas
	for(int i = 0; i < _vMarker.size(); i++)
	{
		fw->UpdateEntry("MarkerData"+ QString::number(i), "timeStamp",	QString::number(_dataTime));
		VirtualMarker* vm = _vMarker[i];
		if(vm->_isTracked)
			fw->UpdateEntry("MarkerData"+ QString::number(i), "Tracked" ,"1");
		else
			fw->UpdateEntry("MarkerData"+ QString::number(i), "Tracked","0");
		fw->UpdateEntry("MarkerData"+ QString::number(i), "X",QString::number(vm->_trackedTranslation->x()));
		fw->UpdateEntry("MarkerData"+ QString::number(i), "Y",QString::number(vm->_trackedTranslation->y()));
		fw->UpdateEntry("MarkerData"+ QString::number(i), "Z",QString::number(vm->_trackedTranslation->z()));

		float x = 0, y = 0, z = 0, w = 0;
		vm->TrackedRotation(&x, &y, &z, &w);
		fw->UpdateEntry("MarkerData"+ QString::number(i), "QX",QString::number(x));
		fw->UpdateEntry("MarkerData"+ QString::number(i), "QY",QString::number(y));
		fw->UpdateEntry("MarkerData"+ QString::number(i), "QZ",QString::number(z));
		fw->UpdateEntry("MarkerData"+ QString::number(i), "QW",QString::number(w));

		if(vm->_hasRefData)
			fw->UpdateEntry("MarkerData"+ QString::number(i), "Reference" ,"1");
		else
			fw->UpdateEntry("MarkerData"+ QString::number(i), "Reference","0");

		fw->UpdateEntry("MarkerData"+ QString::number(i), "RefX",QString::number(vm->_trackedRefTranslation->x()));
		fw->UpdateEntry("MarkerData"+ QString::number(i), "RefY",QString::number(vm->_trackedRefTranslation->y()));
		fw->UpdateEntry("MarkerData"+ QString::number(i), "RefZ",QString::number(vm->_trackedRefTranslation->z()));

		vm->TrackedRefRotation(&x, &y, &z, &w);
		fw->UpdateEntry("MarkerData"+ QString::number(i), "RefQX",QString::number(x));
		fw->UpdateEntry("MarkerData"+ QString::number(i), "RefQY",QString::number(y));
		fw->UpdateEntry("MarkerData"+ QString::number(i), "RefQZ",QString::number(z));
		fw->UpdateEntry("MarkerData"+ QString::number(i), "RefQW",QString::number(w));
		
		fw->UpdateEntry("MarkerData"+ QString::number(i), "CamAngle",QString::number(vm->RotationAngleFromCamera()));
		fw->UpdateEntry("MarkerData"+ QString::number(i), "CamDistance",QString::number(vm->_trackedTranslation->norm()));

		//fw->UpdateEntry("MarkerData"+ QString::number(i), "AreaPx",QString::number(vm->_areaPx));
		//fw->UpdateEntry("MarkerData"+ QString::number(i), "WidthPx",QString::number(vm->_widthPx));
		//fw->UpdateEntry("MarkerData"+ QString::number(i), "HeigthPx",QString::number(vm->_heightPx));

		//fw->UpdateEntry("MarkerData"+ QString::number(i), "CameraSpeedm-s", QString::number(vm->_cameraSpeed->norm()));	
		//fw->UpdateEntry("MarkerData"+ QString::number(i), "CameraAccelm-s2",QString::number(vm->_cameraAccel));	

		//Vector3f camPosition = vm->CameraPosition();
		//fw->UpdateEntry("MarkerData"+QString::number(i), "CameraX", QString::number(camPosition.x()));
		//fw->UpdateEntry("MarkerData"+QString::number(i), "CameraY", QString::number(camPosition.y()));
		//fw->UpdateEntry("MarkerData"+QString::number(i), "CameraZ", QString::number(camPosition.z()));

		//float yaw = 0, pitch = 0, roll = 0;
		//vm->CameraRotation(&yaw, &pitch, &roll);
		//fw->UpdateEntry("MarkerData"+QString::number(i), "CameraYaw", QString::number(yaw));
		//fw->UpdateEntry("MarkerData"+QString::number(i), "CameraPitch", QString::number(pitch));
		//fw->UpdateEntry("MarkerData"+QString::number(i), "CameraRoll", QString::number(roll));

		//fw->WriteToFile(QString::number(_dataTime), "MarkerData"+ QString::number(i));
		if(lastCall)
		{
				fw->WriteToFile(QString::number(_dataTime), "MarkerData"+ QString::number(i));
				fw->CloseFile("MarkerData"+ QString::number(i));
		}
	}

	if(_usingIMU)
	{
		fw->UpdateEntry("IMUData", "timeStamp",	QString::number(_dataTime));
		/*float yaw = 0, pitch = 0, roll = 0;
		_camTracker->GetYawPitchRollFromIMU(&yaw, &pitch, &roll);

		fw->UpdateEntry("IMUData", "Yaw",QString::number(yaw));
		fw->UpdateEntry("IMUData", "Pitch",QString::number(pitch));
		fw->UpdateEntry("IMUData", "Roll",QString::number(roll));*/
		Quaternionf imuRotation = Quaternionf();
		_tManager.GetImuRotation(&imuRotation);
		
		fw->UpdateEntry("IMUData", "q_x",QString::number(imuRotation.x()));
		fw->UpdateEntry("IMUData", "q_y",QString::number(imuRotation.y()));
		fw->UpdateEntry("IMUData", "q_z",QString::number(imuRotation.z()));
		fw->UpdateEntry("IMUData", "q_w",QString::number(imuRotation.w()));
		
		
		float rotationSpeed = _tManager.GetImuRotationSpeed();
		fw->UpdateEntry("IMUData", "RotSpeedd-s",QString::number(rotationSpeed));
		float yawSpeed = 0, pitchSpeed = 0, rollSpeed = 0;
		_tManager.GetImuRotationSpeed(&yawSpeed, &pitchSpeed, &rollSpeed);
		fw->UpdateEntry("IMUData", "YawSpeedd-s",QString::number(yawSpeed));
		fw->UpdateEntry("IMUData", "PitchSpeedd-s",QString::number(pitchSpeed));
		fw->UpdateEntry("IMUData", "RollSpeedd-s",QString::number(rollSpeed));

		//fw->UpdateEntry("IMUData", "Accelm-s2",QString::number(_camTracker->GetIMUTranslationMagnitude()));
		//fw->UpdateEntry("IMUData", "InstantAccelm-s2",QString::number(_camTracker->GetInstantIMUTranslationMagnitude()));
		//float x = 0, y = 0, z = 0;
		//_tManager._imu->TranslationMagnitude(&x, &y, &z);
		//fw->UpdateEntry("IMUData", "AccelX",QString::number(x));
		//fw->UpdateEntry("IMUData", "AccelY",QString::number(y));
		//fw->UpdateEntry("IMUData", "AccelZ",QString::number(z));
		//fw->WriteToFile(QString::number(_dataTime), "IMUData");
	}

	if(_tManager.HasReferenceTracker())
	{
		fw->UpdateEntry("ReferenceData", "timeStamp",	QString::number(_dataTime));
		//Vector3f* position = new Vector3f(0,0,0);
		//_tManager.GetReferencePosition(position);

		Vector3f* position = new Vector3f(0,0,0);
		Quaternionf* rotation = new Quaternionf(1,0,0,0);
		_tManager.GetReferenceCamData(position, rotation);

		//_tManager.GetReferenceRotation(&yaw, &pitch, &roll);
		if(_tManager.HasReferenceData())
			fw->UpdateEntry("ReferenceData", "Connected",	"1");	
		else
			fw->UpdateEntry("ReferenceData", "Connected",	"0");

		fw->UpdateEntry("ReferenceData", "X",	QString::number(position->x()));
		fw->UpdateEntry("ReferenceData", "Y",	QString::number(position->y()));
		fw->UpdateEntry("ReferenceData", "Z",	QString::number(position->z()));	

		fw->UpdateEntry("ReferenceData", "q_x",	QString::number(rotation->x()));
		fw->UpdateEntry("ReferenceData", "q_y",	QString::number(rotation->y()));
		fw->UpdateEntry("ReferenceData", "q_z",	QString::number(rotation->z()));	
		fw->UpdateEntry("ReferenceData", "q_w",	QString::number(rotation->w()));

		//fw->WriteToFile(QString::number(_dataTime), "ReferenceData");
	}
	bool saveCamImage = _camTime+(1000.f/30.f) < _dataTime; 

	if(_captureLeftImage && saveCamImage)
	{
		_camTime = _dataTime;
		fw->SaveImage("LeftImage",QString::number(_dataTime), _arView.GetLeftCameraImage());
	}

	if(_captureRightImage && saveCamImage)
	{
		fw->SaveImage("RightImage",QString::number(_dataTime), _arView.GetRightCameraImage());
	}

	if(lastCall)
	{
		fw->WriteToFile(QString::number(_dataTime), "CameraData");
		fw->CloseFile("CameraData");
		if(_usingIMU)
		{
			fw->WriteToFile(QString::number(_dataTime), "IMUData");
			fw->CloseFile("IMUData");
		}
		if(_tManager.HasReferenceTracker())
		{
			fw->WriteToFile(QString::number(_dataTime), "ReferenceData");
			fw->CloseFile("ReferenceData");
		}
	}
}


void DARV::ToggleFileWriting()
{
	if(!_fileWriting)
	{
		_ui._pBData->setText("STOP");
		_dataTime = 0;
		_camTime = 0;
		FileWriter* fw = FileWriter::GetInstance();
		time_t  rawtime;
		struct tm * timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		char buffer[80];
		strftime(buffer,80,"%d_%m_%Y_%I_%M_%S",timeinfo);
		std::string timeString(buffer);
		QStringList imageList = QStringList();
		if(_captureLeftImage)
			imageList.append("LeftImage");
		if(_captureRightImage)
			imageList.append("RightImage");
		fw->MakeANewFolder(QString::fromStdString(timeString), imageList);

		fw->ClearFiles();
		int fps = 0, resX = 0, resY = 0;
		fw->UpdateEntry("MetaData", "Date",	QString::number(timeinfo->tm_mday)+"."+QString::number(timeinfo->tm_mon)+"."+QString::number(timeinfo->tm_year));
		fw->UpdateEntry("MetaData", "Time",	QString::number(timeinfo->tm_hour)+":"+QString::number(timeinfo->tm_min)+":"+QString::number(timeinfo->tm_sec));
		fw->UpdateEntry("MetaData", "MarkerCnt",QString::number(_vMarker.size()));
		if(_captureLeftImage)
			fw->UpdateEntry("MetaData", "LeftCaptured",	"True");
		else
			fw->UpdateEntry("MetaData", "LeftCaptured",	"False");
		if(_captureRightImage)
			fw->UpdateEntry("MetaData", "RightCaptured",	"True");
		else
			fw->UpdateEntry("MetaData", "RightCaptured",	"False");

		QString camName = "";
		_tManager.GetTrackingCameraData(&camName, &resX, &resY, &fps);
		fw->UpdateEntry("MetaData", "Camera",	camName);
		fw->UpdateEntry("MetaData", "FPS",	QString::number(fps));
		fw->UpdateEntry("MetaData", "ResX",	QString::number(resX));
		fw->UpdateEntry("MetaData", "ResY",	QString::number(resY));

		fw->UpdateEntry("MetaData", "CubeX",	QString::number(CUBE_X));	
		fw->UpdateEntry("MetaData", "CubeY",	QString::number(CUBE_Y));	
		fw->UpdateEntry("MetaData", "CubeZ",	QString::number(CUBE_Z));

		fw->UpdateEntry("MetaData", "CubeYaw",	QString::number(CUBE_YAW));	
		fw->UpdateEntry("MetaData", "CubePitch",	QString::number(CUBE_PITCH));	
		fw->UpdateEntry("MetaData", "CubeRoll",	QString::number(CUBE_ROLL));

		if(_usingIMU)
		{
			fw->UpdateEntry("MetaData", "IMU",	_tManager._imu->_name);	
		}
		else
			fw->UpdateEntry("MetaData", "IMU",	"None");	
		
		if(_tManager.HasReferenceTracker())
			fw->UpdateEntry("MetaData", "UsingOptiTrack",	"True");	
		else
			fw->UpdateEntry("MetaData", "UsingOptiTrack",	"False");	

		fw->WriteToFile(QString::number(_dataTime), "MetaData");
		fw->CloseFile("MetaData");
	}
	else
	{
		DoFileWriting(0, true);
		_ui._pBData->setText("START");	
	}
	_fileWriting = !_fileWriting;
}

void DARV::LoadRecords()
{

}

void DARV::ToogleRecordUsing(int flag)
{
	if(_ui._cbUseRecords->isChecked())
	{
		_ui._listRecords->addItems(_tManager.StartUsingRecord(_arView.GetLeftCamId(),_arView.GetRightCamId()));
	}
	else
	{
		if(_tManager.StopUsingRecord())
		{
			if(_tManager.IMUUsable())
			{
				//we can use the IMU of the vuzix glasses to improve the camera-rotation
				_camTracker = new CameraTrackerFusingIMU();
				_arView.ChangeCameraTracker(_camTracker);
			}
			else
			{
				CameraTracker* sensorFusingTracker = new CameraTrackerNoIMU();
				_arView.ChangeCameraTracker(sensorFusingTracker);
			}
			////add some marker info-fields
			//QList<VirtualMarker*> marker = QList<VirtualMarker*>();
			//_arView.GetCurrentSetup()->GetVirtualMarker(&marker);
			//_markerValues = new ValueTree(_ui._tWMarker);
			//QLayoutItem* item;
			//while ( ( item = _ui._hLUncer->layout()->takeAt( 0 ) ) != NULL )
			//{
			//	delete item->widget();
			//	delete item;
			//}
			//_uncerCos.clear();
			//for(int i = 0; i<marker.size(); i++)
			//{
			//	UncertaintyCOS* markerCOS = new UncertaintyCOS(this);
			//	QRect cosGeo = _ui.tW_data->geometry();
			//	cosGeo.setWidth(cosGeo.height());
			//	_ui._hLUncer->addWidget(markerCOS);
			//	markerCOS->SetSize(cosGeo);
			//	_uncerCos.append(markerCOS);
			//	markerCOS->Load("Marker "+ QString::number(marker[i]->_id), &marker[i]->_uncer);
			//}
			//SetMarkerInfo(marker);
		}
		_ui._listRecords->clear();
		_setTime = false;
	}
}

void DARV::LoadRecord(QListWidgetItem* item)
{
	if(_tManager.LoadRecord(item->text()))
	{
		if(_tManager.IMUUsable())
		{
			//we can use the IMU of the vuzix glasses to improve the camera-rotation
			_camTracker = new CameraTrackerFusingIMU();
			_arView.ChangeCameraTracker(_camTracker);
		}
		else
		{
			CameraTracker* sensorFusingTracker = new CameraTrackerNoIMU();
			_arView.ChangeCameraTracker(sensorFusingTracker);
		}
		//	//add some marker info-fields
		//QList<VirtualMarker*> marker = QList<VirtualMarker*>();
		//_arView.GetCurrentSetup()->GetVirtualMarker(&marker);
		//QLayoutItem* item;
		//while ( ( item = _ui._hLUncer->layout()->takeAt( 0 ) ) != NULL )
		//{
		//	delete item->widget();
		//	delete item;
		//}
		//_uncerCos.clear();
		//for(int i = 0; i<marker.size(); i++)
		//{
		//	UncertaintyCOS* markerCOS = new UncertaintyCOS(this);
		//	QRect cosGeo = _ui.tW_data->geometry();
		//	cosGeo.setWidth(cosGeo.height());
		//	_ui._hLUncer->addWidget(markerCOS);
		//	markerCOS->SetSize(cosGeo);
		//	_uncerCos.append(markerCOS);
		//	markerCOS->Load("Marker "+ QString::number(marker[i]->_id), &marker[i]->_uncer);
		//}
		//SetMarkerInfo(marker);
	}
}

void DARV::ToggleMarkerClassification(int flag)
{
	_arView.GetCameraTrackerPointer()->ToggleClassification(_ui._cbClassification->isChecked());
}

void DARV::ChangeObject(QListWidgetItem * item)
{
	
	qDebug() << "Change Setup to " << item->text(); 
	_arView.SetSetup(item->text());
	_camView->ChangeSetup(_arView.GetCurrentSetup());
	
	//add some marker info-fields
	QList<VirtualMarker*> marker = QList<VirtualMarker*>();
	_arView.GetCurrentSetup()->GetVirtualMarker(&marker);
	SetMarkerInfo(marker);
}

void DARV::ResetRotation()
{
	if(_usingIMU)
	{
		qDebug() << "reset Imu-Orientation to Marker-Orientation";
		AngleAxisf* camRot = _arView.GetCameraRotationPointer();
		_tManager._imu->ResetRotation(new Quaternionf(*camRot));
	}
}

void DARV::ChangeIMUTracker()
{
	bool useVuzix = false;
	if(_ui._rButtonVUZIX->isChecked())
		useVuzix = true;
	_tManager._imu->FreeIMU();
	if(useVuzix)
		_tManager._imu = new IMUManagerVuzix();
	else
		_tManager._imu = new IMUManagerSimon();
	_tManager._imu->Initialize();
}

void DARV::ChangeInterpolationValue(int value)
{
	if(_usingIMU)
	{
		float degree = value; 
		//float newValue = percent/100.f;
		_ui._lInterpol->setText(" "+QString::number(degree)+ " Grad - IMU-ERSATZ");
		_camTracker->SetIMUInterpolation(degree);
	}
}

void DARV::AddEyeDiff()
{
//	Vector3f* seenObjectPos =  new Vector3f(_arView.GetObjectPos());
//	Vector4f* seenObjectRot =new Vector4f(0,0,0,0);
//
//	Marker m = _tManager.GetMarkerInfo(_arView.GetMarkerIDOfSelected());
//	if(!m._isTracked)
//	{
//		qDebug() << "No Marker info to compute difference!";
//		return;
//	}
//	
//	Vector3f* meassuredObjectPos = m._translationFromCamera;
//	AngleAxisf* meassuredObjectRot = m._rotationFromCamera;
//
//	Vector3f* translation = new Vector3f(*meassuredObjectPos - *seenObjectPos);
//	AngleAxisf* rotation = meassuredObjectRot;
//
//	//qDebug() << "Adding EyeDiff tx:"<< translation->x() << "  ty:" << translation->y()<< "  tz:" << translation->z()<< "  rx:" << rotation->x()<< "  ry:" << rotation->y()<< "  rz:" << rotation->z()<< "  rw:" << rotation->w();
//	_tManager.AddEyeDiff(translation, rotation);
//	_ignoreTextChange = true;
//
//	Vector3f averageTrans = _tManager.GetAverageEyeTranslation();
//	_ui._editEyeTransX->setText(QString::number(averageTrans.x()));
//	_ui._editEyeTransY->setText(QString::number(averageTrans.y()));
//	_ui._editEyeTransZ->setText(QString::number(averageTrans.z()));
//	_arView.SetEyeTranslation(averageTrans);
//
//	AngleAxisf averageRot = _tManager.GetAverageEyeRotation();
//	_ui._editEyeRotX->setText(QString::number(averageRot.axis().x()));
//	_ui._editEyeRotY->setText(QString::number(averageRot.axis().y()));
//	_ui._editEyeRotZ->setText(QString::number(averageRot.axis().z()));
//	_ui._editEyeRotW->setText(QString::number(averageRot.angle() *(180.0/M_PI)));
//	_arView.SetEyeRotation(&averageRot);
//
//	_ignoreTextChange = false;
}

void DARV::keyPressEvent(QKeyEvent* event)
{
	Qt::Key pressedKey = static_cast<Qt::Key>(event->key());
	
	QString fieldName("");
	double value = 1;
	qDebug() << "Key: "<< pressedKey;
	if(pressedKey == Qt::Key_Up || pressedKey == Qt::Key_P || pressedKey == Qt::Key_Down || pressedKey == Qt::Key_M)
	{
		if(_ui._editFovy->hasFocus())
			fieldName = "_editFovy";
		if(_ui._editEyeSep->hasFocus())
			fieldName = "_editEyeSep";
		if(_ui._editEyeCorrection->hasFocus())
			fieldName = "_editEyeCorrection";
		if(_ui._editObjectTransX->hasFocus())
			fieldName = "_editObjectTransX";
		if(_ui._editObjectTransY->hasFocus())
			fieldName = "_editObjectTransY";
		if(_ui._editObjectTransZ->hasFocus())
			fieldName = "_editObjectTransZ";
		if(pressedKey == Qt::Key_Down || pressedKey == Qt::Key_M)
			value = -1;
	}
	if(pressedKey == Qt::Key_Escape)
	{
		_arView.close();
		this->close();
		//_tManager.CleanUp();
	}
	if(fieldName != "")
		ChangeParameter(fieldName, value, true);
}

void DARV::AdjustWindow(WindowManager *wm)
{
	_wm = wm;
	_arView.AjustWindow(_wm);
	//start on first screen in full size
	QRect firstScreen = _wm->GetSizeOfMonitor(0);
	move(firstScreen.left(), firstScreen.top());
	//setFixedSize(firstScreen.width(), firstScreen.height());
}

void DARV::closeEvent (QCloseEvent *event)
{
	_tManager.CleanUp();
	_arView.close();
	printf("close application\n");
}

void DARV::CheckAsNumberParameters(QString input)
{
	if(_ignoreTextChange)
		return;

	QObject* s = QObject::sender();
	QLineEdit* e = static_cast<QLineEdit*>(s);
	QString name = s->objectName();
	bool ok = false;
	double inputNum = input.toDouble(&ok);
		
	if(!ok)
	{
		//the new input isn't a number!
		e->setStyleSheet("QLineEdit{background-color:#F8E0E6;}");
		qDebug() << "invalid number in " << e->objectName();
		return;
	}
	
	e->setStyleSheet("QLineEdit{background-color:#FFF;}");
	
	ChangeParameter(e->objectName(), inputNum, false);
}

void DARV::ChangeParameter(QString name, double value, bool counting)
{
	if(name == "_editParallax")
	{
		if(counting)
			value = _arView.GetZZeroParallax()+value*0.1f;
		qDebug() << "Change Z-0-Parallax to :" << value;
		_arView.SetZZeroParallax(value);
	}
	else
	if(name == "_editFovy")
	{
		if(counting)
			value = _arView.GetFovy()+value*0.1f;
		qDebug() << "Change FOVY to :" << value;
		//_arView.SetFOVY(value);
		_arView.SetFOVY(value);
	}
		else
	if(name == "_editEyeSep")
	{
		if(counting)
		{
			value = _arView.GetEyeSeparation()+value*0.01f;
		}
		qDebug() << "Change EyeSeparation to :" << value;
		_arView.SetEyeSeparation(value);
	}
	else
	if(name == "_editObjectTransX")
	{
		if(counting)
			value = _arView.GetObjectPos().x()+value;
		qDebug() << "Change Object-X-Pos to :" << value;
		//_camView->_offset =value*10;
		Vector3f o = _arView.GetObjectPos();
		_arView.SetObjectPos(Vector3f(value, o.y(), o.z()));
	}
	else
	if(name == "_editObjectTransY")
	{
		if(counting)
			value = _arView.GetObjectPos().y()+value;
		qDebug() << "Change Object-Y-Pos to :" << value;
		Vector3f o = _arView.GetObjectPos();
		_arView.SetObjectPos(Vector3f(o.x(),value, o.z()));
	}
		else
	if(name == "_editObjectTransZ")
	{
		if(counting)
			value = _arView.GetObjectPos().z()+value;
		qDebug() << "Change Object-Z-Pos to :" << value;
		Vector3f o = _arView.GetObjectPos();
		_arView.SetObjectPos(Vector3f(o.x(), o.y(),value));
	}
	else
	if(name == "_editEyeCorrection")
	{
	/*	if(counting)
			value =  _tManager._additonalTranslation.x()+value*0.1f;*/
		qDebug() << "Change Eye-Correction to :" << value;
		//Vector3f o = _tManager._additonalTranslation;
		// _tManager._additonalTranslation = Vector3f(value, o.y(), o.z());
	}
	if(counting)
		ShowARViewParameters();
}

void DARV::ToggleStereo(int flag)
{
	bool checked = _ui._checkboxStereo->isChecked();
	qDebug() << "toggle stereo: " << checked;
	_arView.ToggleStereoMode(checked);
	_ui._editEyeSep->setEnabled(checked);
}

void DARV::ToggleReferenceData(int flag)
{
	qDebug() << "Change OptiTrack-Using";
	_useReferenceData = !_useReferenceData;
}

void DARV::ResetReferenceTracker()
{
	if(_tManager.HasReferenceTracker())
	{
		qDebug() << "reset OptiTrack-Tracking to Marker-Tracking";
		AngleAxisf* camRot = _arView.GetCameraRotationPointer();
		Vector3f*	camPos = _arView.GetCameraPositionPointer();
		_tManager.ResetReferenceTracker(camPos, new Quaternionf(*camRot));
	}
}

void DARV::DoReferenceTracking()
{

	if(_tManager.HasReferenceTracker())
	{
		if(_tManager.DoReferenceTracking())
			_ui._pBConnect->setText("Trennen");
		else
			_ui._pBConnect->setText("Verbinden");
	}
}

void DARV::DoBlurAnalysing()
{
	if(_blurAnalyser == 0)
		return;

	if(!_blurAnalyser->_isAnalysing)
	{
		int trackingCamera = 0;
		int resX = 0;
		int resY = 0;
		int fps = 0;
		_tManager.GetTrackingCameraData(&trackingCamera,&resX, &resY, &fps);
		if(_blurAnalyser->Start(trackingCamera, resX, resY, fps,_camLoader))
			_ui._pBBlur->setText("Stop");
		_blurAnalyser->UseThisViewer(_imageView);
		ShowImageViewer(true);
		_arView.hide();
	}
	else
		if(_blurAnalyser->Stop())
		{
			_ui._pBBlur->setText("Start");
			ShowImageViewer(false);
			_imageView->RemoveTexture();
			_arView.show();
		}
}

void DARV::ToggleDARV(int flag)
{
	bool checked = _ui._cbDARV->isChecked();
	qDebug() << "toggle dynamic ar-view: " << checked;
	_arView.ToggleDARV(checked);
}

void DARV::ToggleTime()
{
	if(_tManager._usingRecord)
	{
		_setTime = !_setTime;
		if(_setTime)
			_ui._pBPause->setText("PLAY");
		else
			_ui._pBPause->setText("PAUSE");
	}
}

void DARV::StepForward()
{
	if(_setTime)
	{
		if(_setTime)
		{
			_settedTime  += 60; 
		}
	}
}

void DARV::ToggleTracking(int flag)
{
	bool checked = _ui._checkBoxObject->isChecked();
	qDebug() << "toggle tracking: "<< checked;
	_arView.ToggleTracking(checked);
	if(!checked)
	{
		_ui._editObjectTransX->setEnabled(true);
		_ui._editObjectTransY->setEnabled(true);
		_ui._editObjectTransZ->setEnabled(true);

		_ignoreTextChange = true;
		Vector3f objectPos = _arView.GetObjectPos();
		_ui._editObjectTransX->setText(QString::number(objectPos.x()));
		_ui._editObjectTransY->setText(QString::number(objectPos.y()));
		_ui._editObjectTransZ->setText(QString::number(objectPos.z()));
		_ignoreTextChange = false;
	}
	else
	{
		_ui._editObjectTransX->setEnabled(false);
		_ui._editObjectTransY->setEnabled(false);
		_ui._editObjectTransZ->setEnabled(false);
	}
}

void DARV::UpdateARViewParameters()
{

}

void DARV::EnableIMUUsing(bool enable)
{
	_usingIMU = enable;
	if(enable)
	{
		qDebug() << " YEAH, use the CameraTracker with IMU-Data to improve the rotation-trajectory!";
		_ui._lIMU->setText("IMU     - verbunden");
	}
	else
	{
		_ui._lIMU->setText("IMU     - getrennt");
	}

	_ui._vSAD->setEnabled(enable);
	_ui._hSInterpol->setEnabled(enable);
	_ui._pBResetRot->setEnabled(enable);
	_ui._rButtonSIMON->setEnabled(enable);
	_ui._rButtonVUZIX->setEnabled(enable);
	//_ui._pBBlur->setEnabled(enable);
}

void DARV::ShowImageViewer(bool flag)
{
	_showImageViewer = flag;
	if(_showImageViewer)
	{
		_imageView->show();
		_camView->hide();
	}
	else
	{
		_imageView->hide();
		_camView->show();
	}
}

void DARV::ShowLists()
{
	QList<QString> names = _arView.GetSetupNames();

	for (int i = 0; i<names.size(); i++)
	{
		_ui._listVObject->insertItem(i,names[i]);
	}
}

void DARV::ShowARViewParameters()
{
	GLdouble fovy;
	GLdouble es;
	GLdouble zzp;
	bool mono;
	bool split;
	_ignoreTextChange = true;

	//get current parameters
	_arView.GetStereoParameters(&mono, &split, &fovy, &es, &zzp);
	_ui._editEyeSep->setText(Util::ToQString(es));
	_ui._editFovy->setText(Util::ToQString(fovy));
	_ui._editEyeCorrection->setText(0);

	_ui._checkboxStereo->setChecked(!mono);
	_ui._editEyeSep->setDisabled(mono);
	_ui._editEyeCorrection->setDisabled(mono);
	
	//QString tackedID = _arView.GetMarkerIDOfSelected();
	//_ui._checkBoxObject->setText("An Marker ID"+tackedID+" taggen");
	_ui._checkBoxObject->setChecked(_arView.GetTrackingFlag());

	bool checked = _ui._checkBoxObject->isChecked();
		if(!checked || true)
	{
		_ui._editObjectTransX->setEnabled(true);
		_ui._editObjectTransY->setEnabled(true);
		_ui._editObjectTransZ->setEnabled(true);

		_ignoreTextChange = true;
		Vector3f objectPos = _arView.GetObjectPos();
		_ui._editObjectTransX->setText(QString::number(objectPos.x()));
		_ui._editObjectTransY->setText(QString::number(objectPos.y()));
		_ui._editObjectTransZ->setText(QString::number(objectPos.z()));
		_ignoreTextChange = false;
	}
	else
	{
		_ui._editObjectTransX->setEnabled(false);
		_ui._editObjectTransY->setEnabled(false);
		_ui._editObjectTransZ->setEnabled(false);
	}
	
	_ignoreTextChange = false;
}
