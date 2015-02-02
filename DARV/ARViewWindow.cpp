#include "stdafx.h"
#include "ARViewWindow.h"


ARViewWindow::ARViewWindow()
{
	_vReality = new VirtualReality();
	_glWidget = new ARView(_vReality);
	_isVisible = true;
	MakeLayout();
}

ARViewWindow::~ARViewWindow()
{
}

void ARViewWindow::MakeLayout()
{
	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addWidget(_glWidget);
	mainLayout->setContentsMargins(0,0,0,0);		//important to get the augmented screen at the right position! otherwise its some pixel too far to the right and down
	setLayout(mainLayout);
}

void ARViewWindow::SetVirtualReality(VirtualReality* vReality)
{
	_vReality = vReality;
}

void ARViewWindow::ChangeCameraTracker(CameraTracker* tracker)
{
	_vReality->ChangeCameraTracker(tracker);
}

void ARViewWindow::KeyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}

BOOL ARViewWindow::AjustWindow(WindowManager *wm)
{
	if(!_isVisible) 
	{
		this->hide();
		return true;
	}
	_wm = wm;
	int monitors = _wm->NumberOfMonitors();
	if(monitors == 2)
		return FullOnSecondScreen();
	else
	{
		return FullOnFirstScreen();
	}
	return TRUE;
}

BOOL ARViewWindow::FullOnSecondScreen()
{
	//make a borderless - always-top window
	setStyleSheet("background-color: black;");

	//start on seconde screen in full size
	QRect secondScreen = _wm->GetAvailableSizeOfMonitor(1);
	/*int left = min(secondScreen.left(), secondScreen.right());
	int right = max(secondScreen.left(), secondScreen.right());
	int top = min(secondScreen.top(), secondScreen.bottom());
	int bottom = max(secondScreen.top(), secondScreen.bottom());*/
	_glWidget->move(secondScreen.left(), secondScreen.top());
	move(secondScreen.left(), secondScreen.top());
	//setFixedSize(secondScreen.width(), secondScreen.height());
	this->showFullScreen();
	_glWidget->SetSize(QSize(secondScreen.width(),secondScreen.height()));
	_glWidget->SetViewport(secondScreen);

	return TRUE;
}

BOOL ARViewWindow::FullOnFirstScreen()
{
	//make a borderless - always-top window
	setStyleSheet("background-color: black;");

	//start on seconde screen in full size
	QRect firstScreen = _wm->GetAvailableSizeOfMonitor(0);
	int width = 800;
	int height = 450;
	QRect freeWindow = QRect(firstScreen.width()-width, 0, width, height);
	/*int left = min(firstScreen.left(), firstScreen.right());
	int right = max(firstScreen.left(), firstScreen.right());
	int top = min(firstScreen.top(), firstScreen.bottom());
	int bottom = max(firstScreen.top(), firstScreen.bottom());*/
	_glWidget->move(freeWindow.left(), freeWindow.top());
	move(freeWindow.left(), freeWindow.top());
	setFixedSize(freeWindow.width(), freeWindow.height());
	//this->showFullScreen();
	_glWidget->SetSize(QSize(freeWindow.width(),freeWindow.height()));
	_glWidget->showFullScreen();
	_glWidget->SetViewport(freeWindow);

	return TRUE;
}

bool ARViewWindow::InitializeVSTMode(QString camLeftID,QString camRightID)
{
	return _glWidget->InitializeVSTMode(camLeftID,camRightID);
}

bool ARViewWindow::InitializeOSTMode(int camIndex)
{
	return _glWidget->InitializeOSTMode(camIndex);
}

void ARViewWindow::NewCameraImage(IplImage* image, QString id, bool forceViewUpdate)
{
	_glWidget->UpdateCameraImage(image, id);
	if(forceViewUpdate)
	{
		this->Update(100);
	}
}

QGLContext* ARViewWindow::GetGLContextOfARView()
{
	return _glWidget->context();
}

Vector3f ARViewWindow::GetObjectPos()
{
	//return _glWidget->GetEyeTranslation();
	return _glWidget->GetObjectPos();
}

void ARViewWindow::SetObjectPos(Vector3f pos)
{
	//_glWidget->SetEyeTranslation(pos);
	_glWidget->SetObjectPos(pos);
}

QList<QString>  ARViewWindow::GetSetupNames()
{
	return _vReality->GetSetupNames();
}

VirtualSetup* ARViewWindow::GetCurrentSetup()
{
	return _vReality->_selectedSetup;
}

int ARViewWindow::GetMarkerIDOfSelected()
{
	return _vReality->GetMarkerIDOfSelected();
}

void ARViewWindow::SetSetup(QString name)
{
	_vReality->SelectSetup(name);
}

void ARViewWindow::GetStereoParameters(bool* mono, bool* sideBySide, GLdouble* fovy, GLdouble* eyeSeparation, GLdouble* zZeroParallax)
{
	*mono = _glWidget->_mono;
	if(_glWidget->_stereo_mode == GLSU_SPLIT_VERTICALLY)
		*sideBySide = TRUE;
	else
		*sideBySide = FALSE;
	*fovy = _glWidget->_fovy;
	*eyeSeparation = _glWidget->_eyeSeparation;
	*zZeroParallax = _glWidget->_zZeroParallax;
}

float* ARViewWindow::GetCameraQualityPointer()
{
	if(_vReality!=NULL)
	{
		return _vReality->GetCameraQuality();
	}
	else
	{
		qDebug() << "WARNING: No Virtual Reality definde to get a pointer on the camera quality!";
		float none = 0;
		return &none;
	}
}

Vector3f* ARViewWindow::GetCameraPositionPointer()
{
	if(_vReality!=NULL)
	{
		return _vReality->GetCameraPosition();
	}
	else
	{
		qDebug() << "WARNING: No Virtual Reality definde to get a pointer on the camera position!";
		Vector3f none = Vector3f(0,0,0);
		return &none;
	}
}

AngleAxisf* ARViewWindow::GetCameraRotationPointer()
{
	if(_vReality!=NULL)
	{
		return _vReality->GetCameraRotation();
	}
	else
	{
		qDebug() << "WARNING: No Virtual Reality definde to get a pointer on the camera rotation!";
		AngleAxisf none = AngleAxisf(0,Vector3f(0,0,0));
		return &none;
	}
}

CameraTracker* ARViewWindow::GetCameraTrackerPointer()
{
	if(_vReality!=NULL)
	{
		return _vReality->GetCameraTracker();
	}
	else
	{
		qDebug() << "WARNING: No Virtual Reality definde to get a pointer on the camera tracker!";
		
		return new CameraTrackerNoIMU();
	}
}


GLdouble ARViewWindow::GetFovy()
{
	return _glWidget->_fovy;
}

GLdouble ARViewWindow::GetEyeSeparation()
{
	return _glWidget->_eyeSeparation;
}

void ARViewWindow::SetEyeTranslation(Vector3f translation)
{
	_glWidget->SetEyeTranslation(translation);
}

void ARViewWindow::SetEyeRotation(AngleAxisf* rotation)
{
	_glWidget->SetEyeRotation(rotation);
}

AngleAxisf ARViewWindow::GetEyeRotation()
{
	return _glWidget->GetEyeRotation();
}

Vector3f ARViewWindow::GetEyeTranslation()
{
	return _glWidget->GetEyeTranslation();
}

GLdouble ARViewWindow::GetZZeroParallax()
{
	return _glWidget->_zZeroParallax;
}

void ARViewWindow::SetStereoValues(GLdouble fovy, GLdouble eyeSeparation, GLdouble zZeroParallax)
{
	_glWidget->_fovy = fovy;
	_glWidget->_eyeSeparation = eyeSeparation;
	_glWidget->_zZeroParallax = zZeroParallax;
}

void ARViewWindow::SetFOVY(GLdouble value)
{
	Util::SetCameraFOV(value);
	_glWidget->_fovy = value;
}

void ARViewWindow::SetEyeSeparation(GLdouble value)
{
	_glWidget->_eyeSeparation = value;
	if(_glWidget->_inVSTMode)
		_glWidget->_displaySeparation = value;
}

void ARViewWindow::SetZZeroParallax(GLdouble value)
{
	_glWidget->_zZeroParallax = value;
}

void ARViewWindow::ToggleStereoMode(bool flag)
{
	_glWidget->_mono = !flag;
	_glWidget->SetViewport(_glWidget->geometry());
}

void ARViewWindow::ToggleTracking(bool flag)
{
	if(!flag)
		_glWidget->SetObjectPos(Vector3f(0,0,-302));
	_glWidget->_useTrackingData = flag;
}

void ARViewWindow::ToggleDARV(bool flag)
{
	_vReality->ShowQualtiy(flag);
}

void ARViewWindow::SetStereoMode(bool sideBySide)
{
	if(sideBySide)
		_glWidget->_stereo_mode = GLSU_SPLIT_VERTICALLY;
	else
		_glWidget->_stereo_mode = GLSU_ANAGLYPH;
}

void ARViewWindow::Update(int elapsedTime)
{
	_vReality->Update(elapsedTime);
	_glWidget->update();
	this->update();
}

void ARViewWindow::UpdateTracker(int elapsedTime, QList<Marker> marker, bool useReference)
{
	_vReality->UpdateMarker(elapsedTime, marker, useReference);
	_glWidget->update();
	this->update();
}

bool ARViewWindow::GetCameraTextureData(GLuint* texture)
{
	*texture = _glWidget->GetCameraTexture();
	return true;
}

bool ARViewWindow::GetTrackingFlag()
{
	return _glWidget->_useTrackingData;
}
