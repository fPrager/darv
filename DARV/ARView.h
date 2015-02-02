#pragma once
#ifndef ARVIEW_H
#define ARVIEW_H

#include "stdafx.h"
#include <QGLWidget>
#include <QtWidgets>
#include <QtOpenGL>
#include "Glsu.h"

#include "VirtualReality.h"
#include "PBOUnpack.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "CameraThread.h"

using namespace Eigen;
using namespace std;
using namespace cv;

#define  DEFAULT_MONO FALSE
#define	 DEFAULT_STEREOMODE  GLSU_SPLIT_VERTICALLY
#define	 DEFAULT_ANAGLYPH  GLSU_RED_CYAN
#define	 DEFAULT_FOVY_VST 31.43
#define	 DEFAULT_FOVY_OST 17.57
#define	 DEFAULT_EYESEPERATION 6
#define	 DEFAULT_ZEROZPARALLAX  10
#define	 DEFAULT_ZNEAR  0.1
#define	 DEFAULT_ZFAR  500
#define  CAMERA_IMAGE_CNT 5
#define DEFAULT_OBJECT_TO_RENDER "QTLogo";


class ARView: public QGLWidget
{
	Q_OBJECT

public:
	//EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	ARView(VirtualReality *vReality, QWidget *parent = 0, QSize fullWindowSize = QSize(800, 600));
	~ARView(void);

	QSize minimumSizeHint() const;
    QSize sizeHint() const;
	void SetSize(QSize newSize);
	void SetViewport(QRect geometry);

	//change virtual scene
	void SetObjectPos(Vector3f pos);
	Vector3f GetObjectPos();

	//eyetranslation to optical-see-through
	void SetEyeTranslation(Vector3f translation);
	Vector3f GetEyeTranslation();

	void SetEyeRotation(AngleAxisf* rotation);
	AngleAxisf GetEyeRotation();

	//public stereo parameters 
	float _eyeSeparation;
	float _displaySeparation;
	float _fovy;
	float _zZeroParallax;
	bool _mono;
	bool _useTrackingData;
	GlsuStereoMode _stereo_mode;

	//public draw
	void paintGL();

	bool _inVSTMode;
	bool _usePBO;
	int  _colorChannels;
	int  _imageWidth;
	int  _imageHeight;
	bool _cameraIsReady;
	GLuint GetCameraTexture();

	// the optical-see-through mode
	bool InitializeOSTMode(int camIndex);
	IplImage* _cameraImage;
	bool _newCameraImage;

	// the video-see-through mode
	bool InitializeVSTMode(QString camLeftID, QString camRightID);

	IplImage* _camImageLeft;
	QString _camIDRight;
	QString _camIDLeft;
	IplImage* _camImageRight;
	
	void UpdateCameraImage(IplImage* image, QString id);
	bool _newCameraImageLeft;
	bool _newCameraImageRight;


public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
//! [1]

//! [2]
protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
//! [2]

//! [3]
private:
	QSize _preferedSize;
	void AdjustWindow();

	//objects to draw
	VirtualReality* _vReality; 

    float _xRot;
    float _yRot;
    float _zRot;
    QPoint _lastPos;
    QColor _qtObject;
    QColor _qtBackground;

	Vector3f	_objectPos;
	Vector3f	_eyeTranslation;
	AngleAxisf	_eyeRotation;

	AngleAxisf*	_trackingRotation;
	Vector3f*	_trackingTranslation;

	void Render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, GLuint camtexture = 0);

	glm::mat4 GetStereoViewMatrix(bool leftEye);

	//stereo rendering
	GlsuAnaglyphConfiguration _anaglyph_configuration;
	
	//ar modes 
	GLuint ConvertIplToTexture(IplImage *image, GLuint texture = 0);

	//for optical-see-through
	int	   _camIndex;
	GLuint _camTexture;

	//for video-see-through
	//int	   _camIndexLeft;
	GLuint _camTextureLeft;
	char*  _imageDatasLeft[CAMERA_IMAGE_CNT];

	//int	   _camIndexRight;
	GLuint _camTextureRight;
	char*  _imageDatasRight[CAMERA_IMAGE_CNT];
	void UpdateLeftCameraImage(IplImage* image);
	void UpdateRightCameraImage(IplImage* image);

	//GLuint _fullScreenQuadBuffer;
	
	//helper class to stream a texture
	PBOUnpack*	_pboLeft;
	PBOUnpack*	_pboRight;

	//static void DrawCallback(void* this_ptr);

	//stereo camera parameters
	void    SetViewParameters(GLdouble aspect);
	float	_aspect;
	float	_zNear;
	float	_zFar;
	GLint	_viewPort[4];

	//stereo display
	GLdouble _screenHeight;
};

#endif
