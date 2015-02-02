#pragma once
#ifndef ARView_H
#define ARView_H

#include <QtWidgets/QMainWindow>
#include <qthread.h>
#include "stdafx.h"
#include "ui_ARView.h"
#include "WindowManager.h"
#include "ARView.h"
#include "Marker.h"
#include "VirtualReality.h"
#include <QtOpenGL>



class ARView;
using namespace std;
using namespace Eigen;

class ARViewWindow : public QWidget 
{
	Q_OBJECT

public:
	ARViewWindow();
	~ARViewWindow();
	BOOL AjustWindow(WindowManager *wm);
	
	void SetVirtualReality(VirtualReality* vReality);
	void ChangeCameraTracker(CameraTracker* tracker);
	bool InitializeVSTMode(QString camLeftID, QString camRightID);
	bool InitializeOSTMode(int camIndex);
	bool GetCameraTextureData(GLuint* texture);

	bool IsVisible(){return _isVisible;}

	//initialize editor values
	QGLContext* GetGLContextOfARView();

	QList<QString> GetSetupNames();
	VirtualSetup* GetCurrentSetup();
	int GetMarkerIDOfSelected();

	Vector3f GetObjectPos();
	void GetStereoParameters(bool* mono, bool* sideBySide, GLdouble* fovy, GLdouble* eyeSeparation, GLdouble* zZeroParallax);
	bool GetTrackingFlag();
	float* GetCameraQualityPointer();
	Vector3f* GetCameraPositionPointer();
	AngleAxisf* GetCameraRotationPointer();
	CameraTracker* GetCameraTrackerPointer();

	GLdouble GetFovy();
	GLdouble GetEyeSeparation();
	Vector3f GetEyeTranslation();
	AngleAxisf GetEyeRotation();
	GLdouble GetZZeroParallax();

	//update loop
	void Update(int elapsedTime);
	void UpdateTracker(int elapsedTime, QList<Marker> marker, bool useReference = false);
	//handling input of the editor
	//settings
	void SetStereoValues(GLdouble fovy, GLdouble eyeSeparation, GLdouble zZeroParallax);
	void SetFOVY(GLdouble value);
	void SetEyeTranslation(Vector3f translation);
	void SetEyeRotation(AngleAxisf* rotation);
	void SetEyeSeparation(GLdouble value);
	void SetZZeroParallax(GLdouble value);

	void SetStereoMode(bool isSideBySide);
	void ToggleStereoMode(bool flag);
	void ToggleTracking(bool flag);
	void ToggleDARV(bool flag);
	void SetSetup(QString name);
	void SetObjectPos(Vector3f pos);
	IplImage*  GetLeftCameraImage(){return _glWidget->_camImageLeft;}
	IplImage*  GetRightCameraImage(){return _glWidget->_camImageRight;}
	QString GetLeftCamId(){return _glWidget->_camIDLeft;}
	QString GetRightCamId(){return _glWidget->_camIDRight;}

public slots:
	void NewCameraImage(IplImage* image, QString id, bool forceViewUpdate);

protected:
    void KeyPressEvent(QKeyEvent *event);

private:

	Ui::ARView _ui;
	BOOL FullOnSecondScreen();
	BOOL FullOnFirstScreen();
	void MakeLayout();

	//this object organizes good window-sizes/coordinates
	//TODO: should be one global "communication-manager" later on
	WindowManager *_wm;
	bool	_isVisible;
    ARView *_glWidget;
	VirtualReality* _vReality;
};

#endif // ARView_H
