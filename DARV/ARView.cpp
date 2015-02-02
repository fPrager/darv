#include "stdafx.h"
#include "ARView.h"


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif


ARView::ARView(VirtualReality* vReality, QWidget *parent, QSize fullWindowSize)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	_inVSTMode = true;
	_camTextureLeft = -1;
	_camTextureRight = -1;
	_camIDLeft = "";
	_camIDRight = "";
	_preferedSize = fullWindowSize;
	SetViewParameters((double)_preferedSize.width()/(double)_preferedSize.height());

	_qtObject = QColor(100,200,160,255);

	//_eyeTranslation = Vector3f(-23,1,0);
	_eyeTranslation = Vector3f(0,0,0);
	_eyeRotation = Vector4f(0,1,0,0);
	_objectPos = Vector3f(3,5,-300);
	_useTrackingData = true;
	_vReality = vReality;
}

ARView::~ARView()
{

}


#pragma region OpenGL functions

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}


void ARView::SetViewport(QRect geometry)
{
	this->context()->makeCurrent();
	_viewPort[0] = 0;
	_viewPort[1] = 0;
	_viewPort[2] = this->width();
	_viewPort[3] = this->height();

	if(_vReality!=0)
	{
		if(_mono)
			_vReality->SetFrameBufferDim(this->width(),this->height(),true);
		else
			_vReality->SetFrameBufferDim(this->width()/2,this->height(),true);
	}

	glViewport(_viewPort[0],_viewPort[1],_viewPort[2],_viewPort[3]); 
	glScissor(_viewPort[0],_viewPort[1],_viewPort[2],_viewPort[3]);
	glDisable(GL_SCISSOR_TEST);
}

void ARView::SetViewParameters(GLdouble aspect)
{
	_mono = false;
	_stereo_mode = DEFAULT_STEREOMODE;
	_anaglyph_configuration   = DEFAULT_ANAGLYPH;
	_eyeSeparation = DEFAULT_EYESEPERATION;
	_displaySeparation = DEFAULT_EYESEPERATION;
	if(_inVSTMode)
	{
		_fovy = DEFAULT_FOVY_VST;
	}
	else
	{
		_fovy = DEFAULT_FOVY_OST;
	}
	Util::SetCameraFOV(_fovy);

	if(_inVSTMode)
	{
		_aspect = 1.333f;		//aspect ratio of the camera
	}
	else
		_aspect = aspect;		//aspectratio of the screen
	_zZeroParallax = DEFAULT_ZEROZPARALLAX;
	_zNear = DEFAULT_ZNEAR;
	_zFar = DEFAULT_ZFAR;
	_screenHeight = 2*_zZeroParallax*tan(_fovy*.8726646262e-2f);
}

void ARView::initializeGL()
{
	//initializing virtual environment
	_vReality->Initialize();
	_vReality->Load();

	_xRot = 0;
	_yRot = 0;
	_zRot = 0;
}

bool ARView::InitializeOSTMode(int camIndex)
{
	_camIndex = camIndex;
	return true;
}

bool ARView::InitializeVSTMode(QString camLeftId, QString camRightId)
{
	_inVSTMode = true;
	_usePBO = true;
	_pboLeft = 0;
	_pboRight = 0;

	_cameraIsReady = false;
	_newCameraImageLeft = false;
	_newCameraImageRight = false;

	_camImageLeft = 0;
	_camImageRight = 0;

	//setting up video-seethrough mode
	_camIDLeft = camLeftId;
	_camIDRight = camRightId;

	this->context()->makeCurrent();
	glGenTextures(1,&_camTextureLeft); 
	glGenTextures(1,&_camTextureRight); 

	return true;
}

GLuint ARView::GetCameraTexture()
{
	if(_inVSTMode)
		return _camTextureLeft;
	else
		return 0;
}

void ARView::UpdateCameraImage(IplImage* image, QString id)
{
	if(_inVSTMode)
	{
		if(id == _camIDLeft)
			UpdateLeftCameraImage(image);
		if(id == _camIDRight)
			UpdateRightCameraImage(image);
	}
}

void ARView::UpdateLeftCameraImage(IplImage* image)
{
	if(image != 0)
	{
		_camImageLeft	= image;
		this->context()->makeCurrent();
		if(_pboLeft == 0)
		{
			//create a pixel-buffer-object to share the camera-image-buffer with the graphical device
		  _pboLeft = new PBOUnpack(_camTextureLeft, _camImageLeft->width,_camImageLeft->height,GL_BGR,GL_UNSIGNED_BYTE,_camImageLeft->nChannels);
		}
		ConvertIplToTexture(_camImageLeft, _camTextureLeft);
		_newCameraImageLeft = true;
	}
}

void ARView::UpdateRightCameraImage(IplImage* image)
{
	if(image != 0)
	{
		_camImageRight	= image;
		this->context()->makeCurrent();
		if(_pboRight == 0)
		{
			//create a pixel-buffer-object to share the camera-image-buffer with the graphical device
			_pboRight = new PBOUnpack(_camTextureRight, _camImageRight->width,_camImageRight->height,GL_BGR,GL_UNSIGNED_BYTE,_camImageRight->nChannels);
		}
		ConvertIplToTexture(_camImageRight, _camTextureRight);
		_newCameraImageRight = true;
	}
}

GLuint ARView::ConvertIplToTexture(IplImage *image, GLuint texture)
{  
	if(!_usePBO)
	{
	glEnable(GL_TEXTURE);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,image->width, image->height,GL_BGR, GL_UNSIGNED_BYTE, image->imageData);
	}
	else
	{
		if(texture == _camTextureLeft && _pboLeft!=0)
			_pboLeft->UsePBO(texture,image->imageData);
		if(texture == _camTextureRight && _pboRight!=0)
			_pboRight->UsePBO(texture,image->imageData);
	}
	return texture;
}

void ARView::paintGL()
{
	//the projection is every time the same
	makeCurrent();
	if(_mono)
	{
		//non stereo rendering
		glm:: mat4 projection = glm::perspective(_fovy,_aspect,_zNear,_zFar);

		//set the camera in the center
		glm:: mat4 view = glm::lookAt(
			glm::vec3(0,0,0),
			glm::vec3(0,0,-1),
			glm::vec3(0,1,0)
			);

		//no changes to the model matrix
		glm:: mat4 model = glm::mat4(1.0f);
		Render(model, view, projection,_camTextureLeft);
		glFlush();
	}
	else
	{
		glm:: mat4 projection = glm::perspective(_fovy,_aspect,_zNear,_zFar);

		//stereo rendering using the glsu-library to set view ports
		glsuConfigureStereo(_viewPort, GLSU_LEFT, _stereo_mode, _anaglyph_configuration);

		glm:: mat4 view = GetStereoViewMatrix(true);

		glm:: mat4 model = glm::mat4(1.0f);

		//check if its in VST-Mode to draw the left cam

		Render(model, view, projection, _camTextureLeft);
		
		glsuConfigureStereo(_viewPort, GLSU_RIGHT, _stereo_mode, _anaglyph_configuration);
		
		view = GetStereoViewMatrix(false);

		Render(model, view, projection,_camTextureRight);

		GLdouble xOffset = 0;
		GLdouble yOffset = 0;

		glsuConfigureStereo(_viewPort, GLSU_CENTER, _stereo_mode, _anaglyph_configuration);
	}
}

void ARView::Render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, GLuint camtexture)
{
		//glLoadIdentity();
		if(!_useTrackingData)
		{
			glm::vec3 translation = glm::vec3(_objectPos.x(), _objectPos.y(),_objectPos.z());
			view =  glm::translate(view, translation);

			//use the mouse rotation in object-view-mode
			view =  glm::rotate(view, _xRot/16.0f, glm::vec3(1.f,0.f,0.f));
			view =  glm::rotate(view, _yRot/16.0f, glm::vec3(0.f,1.f,0.f));
			view =  glm::rotate(view, _zRot/16.0f, glm::vec3(0.f,0.f,1.f));

			//glRotatef(_xRot / 16.0, 1.0, 0.0, 0.0);
			//glRotatef(_yRot / 16.0, 0.0, 1.0, 0.0);
			//glRotatef(_zRot / 16.0, 0.0, 0.0, 1.0);
		}
		else
		{
			//glm::vec3 translation = glm::vec3(0, 0,0);
			//view =  glm::translate(view, translation);

			////use the mouse rotation in object-view-mode
			//view =  glm::rotate(view, _xRot/16.0f, glm::vec3(1.f,0.f,0.f));
			//view =  glm::rotate(view, _yRot/16.0f, glm::vec3(0.f,1.f,0.f));
			//view =  glm::rotate(view, _zRot/16.0f, glm::vec3(0.f,0.f,1.f));

			//glm::vec3 translation = glm::vec3(-_eyeTranslation.x() * _zZeroParallax/302.0f,-_eyeTranslation.y()* _zZeroParallax/302.0f,-_eyeTranslation.z()* _zZeroParallax/302.0f);
			//model = model * glm::translate(glm::mat4(1.f), translation);
			//glRotatef(-_eyeRotation.w()* (180.0/M_PI), _eyeRotation.x(), _eyeRotation.y(), _eyeRotation.z());
		}
		_vReality->Draw(model, view, projection,camtexture, _useTrackingData);
		glDisable(GL_SCISSOR_TEST);
}

glm::mat4 ARView::GetStereoViewMatrix(bool leftEye)
{
	glm::mat4 view;
	float xTranslation = 0;
	if(!leftEye)
		xTranslation += _displaySeparation;

	if(!_inVSTMode)
	{
		xTranslation -= _displaySeparation/2.f;
	}
	float eyeDisDiff = _displaySeparation-_eyeSeparation;

	if(leftEye)
		eyeDisDiff*=-1;

	view = glm::lookAt(
			glm::vec3(xTranslation,0,0),
			glm::vec3(xTranslation - eyeDisDiff/2.f,0,-1),
			glm::vec3(0,1,0)
			);

	if(!_inVSTMode)
		view = glm::translate(view, glm::vec3(_eyeTranslation.x(),_eyeTranslation.y(),_eyeTranslation.z()));

	return view;
}

#pragma endregion

#pragma region widget functions

QSize ARView::minimumSizeHint() const
{
    return QSize(800, 450);
}

QSize ARView::sizeHint() const
{
	//return QSize(600, 600);
	return _preferedSize;
}

void ARView::AdjustWindow()
{
	SetViewParameters((double)_preferedSize.width()/(double)_preferedSize.height());
	this->setFixedSize(_preferedSize.width(), _preferedSize.height());
	showFullScreen();
}

void ARView::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

#pragma endregion

#pragma region mouse-events

void ARView::mousePressEvent(QMouseEvent *event)
{
    _lastPos = event->pos();
}

void ARView::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - _lastPos.x();
    int dy = event->y() - _lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(_xRot + 8 * dy);
        setYRotation(_yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(_xRot + 8 * dy);
        setZRotation(_zRot + 8 * dx);
    }
   _lastPos = event->pos();
}


void ARView::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != _xRot) {
        _xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void ARView::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != _yRot) {
        _yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void ARView::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != _zRot) {
        _zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}


#pragma endregion

#pragma region public getter and setter

Vector3f ARView::GetObjectPos()
{
	return _objectPos;
}

void ARView::SetObjectPos(Vector3f pos)
{
	_objectPos = pos;
}

Vector3f ARView::GetEyeTranslation()
{
	return _eyeTranslation;
}


void ARView::SetEyeTranslation(Vector3f translation)
{
	_eyeTranslation = translation;
}


AngleAxisf ARView::GetEyeRotation()
{
	return _eyeRotation;
}

void ARView::SetEyeRotation(AngleAxisf* rotation)
{
	_eyeRotation = *rotation;
}


void ARView::SetSize(QSize newSize)
{
	_preferedSize = newSize;
	AdjustWindow();
}


#pragma endregion