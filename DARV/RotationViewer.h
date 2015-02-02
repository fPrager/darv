#pragma once
#include "stdafx.h"
#include <QGLWidget>
#include <QtWidgets>
#include <QtOpenGL>
#include <QTPrimitives.h>

#define COLOR_IMU_ROT QColor(250,100,0,255)
#define COLOR_CAM_ROT QColor(100,250,0,255)
#define COLOR_MARKER_ROT QColor(0,100,250,255)
#define COLOR_UNFOCUS QColor(200,200,200,50)

using namespace Eigen;
using namespace std;

class RotationViewer: public QGLWidget
{
	//Q_OBJECT
public:
	 //EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	RotationViewer(QWidget *parent = 0);
	~RotationViewer();
	void Update(float elapsedTime = 100);
	void UpdateRotations(AngleAxisf camRotation, AngleAxisf markerRot);
	void UpdateRotations(AngleAxisf camRotation, AngleAxisf markerRot, AngleAxisf imuRotation);
	void Load();
	void Draw();
	void SetSize(QRect size);

protected:
    void initializeGL();
	void resizeGL(int width, int height);
	//void paintGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);

private:
	void DrawMarkerRotation();
	void DrawCameraRotation();
	void DrawIMURotation();
	void DrawCoordinateAxis();
	void SetupViewport(int width, int height);
	void SetFocus();
	void MakeArrows();

	bool		_readyToDraw;
	bool		_drawIMURot;
	AngleAxisf	_cameraRotation;
	bool		_focusOnCam;

	AngleAxisf	_imuRotation;
	bool		_focusOnIMU;

	AngleAxisf	_markerRotation;
	bool		_focusOnMarker;

	QRect		_geometry;
	Arrow*		_camArrow;
	Arrow*		_imuArrow;
	Arrow*		_markerArrow;

	Arrow*		_xAxis;
	Arrow*		_yAxis;
	Arrow*		_zAxis;


	float		_xRot;
	float		_yRot;
	float		_zRot;
	AngleAxisf	_viewRot;

	float		_clickTime;
	bool		_mouseDown;

	void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

	QPoint		_lastPos;
};

