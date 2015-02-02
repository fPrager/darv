#pragma once

#include "stdafx.h"
#include <QGLWidget>
#include <QtWidgets>
#include <QtOpenGL>
#include <VirtualSetup.h>
#include <QTPrimitives.h>
#include "Glsu.h"
#include <SOIL.h>
#define BACKGROUNG QColor(255,255,255)
#define COLOR_ORIGIN_POINT QColor(250,50,50)
#define COLOR_ROOM QColor(100,100,100)
#define COLOR_FRUSTRUM QColor(100,200,100,100)
#define COLOR_FRUSTRUM_REF QColor(200,100,100,50)
#define COLOR_MARKER_UNTRACKED QColor(200,200,200)
#define COLOR_MARKER_TRACKED_GOOD QColor(100,250,100)
#define COLOR_MARKER_TRACKED_BAD QColor(250,100,100)
#define COLOR_OBJECT QColor(100,100,100)
#define COLOR_VIRTUAL_OBJECT QColor(100,100,100,200)

using namespace Eigen;
using namespace std;

class CameraView: public QGLWidget
{
public:
	CameraView(QGLContext* context,QWidget *parent = 0);
	CameraView(QWidget *parent = 0);
	~CameraView(void);

	void SetRefFrustrum(Vector3f* position, AngleAxisf* rotation, bool show = false);
	void SetSize(QRect size);
	void ChangeSetup(VirtualSetup* setup);
	void Load(Vector3f* camPos, AngleAxisf* camRot, float* camQuality, float sceneScale = 0.01, float fovy = 17.57f, float aspect = 4.f/3.f, float zNear = 5, float zFar = 500);
	void Update(float elapsedTime = 100);
	void Draw();
	int _offset;
	//old stuff
	//static void FrameUpdate(void* this_ptr, BYTE* ,int ,int);
	//void UpdateCameraBuffer(BYTE* buffer, int width, int height);
	//void UpdateTexture();

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void setDistance(float distance);

protected:
    void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent* event);

private:
	void		SetUp();
	void		SetViewport();
	void		SetVirtualSetup();
	void		DrawViewFrustrum();
	void		DrawRefViewFrustrum();
	void		DrawMarker();
	void		DrawRoom();
	void		DrawOrigin();


	QRect		_geometry;
	QColor		_background;
	GLuint		_cameraTexture;
	float		_cameraAspect;
	int			_cameraImageWidth;
	int			_cameraImageHeight;
	
	int			_cameraTextureWidth;
	int			_cameraTextureHeight;
	bool		_cameraUpsideDown;
	float		_sceneScale;
	
	bool		_readyToDraw;

	bool					_newSetup;
	VirtualSetup*			_setup;
	bool					_showFovRef;
	Frustrum*				_fovRef;
	Frustrum*				_fov;
	Box*					_room;
	Sphere*					_origin;
	Vector3f*				_fromOriginToRoom;
	QList<VirtualMarker*>	_marker;
	QList<Box*>				_markerBoxes;
	QList<Box*>				_virtualObjects;
	QList<Box*>				_roomObjects;


	QtLogo*					_logo;
	float					_rotationY;

	//camera parameters
	Vector3f*	_cameraPosition;
	AngleAxisf*	_cameraRotation;

	Vector3f*   _refCameraPosition;
	AngleAxisf* _refCameraRotation;
	float*		_cameraQuality;
	float		_fovy;
	float		_aspect;
	float		_xRot;
	float		_yRot;
	float		_zRot;
	QPoint		_lastPos;
	float		_distance;

};

