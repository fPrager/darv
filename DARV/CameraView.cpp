#include "stdafx.h"
#include "CameraView.h"

static unsigned int getNextPowerOf2(unsigned int n)
{
	for(int i = 0; i < sizeof(n) * 8; ++i)
	{
		if((1U << i) >= n)
			return 1U << i;
	}

	// Should not be reached
	assert(false);
	return 0;
}

CameraView::CameraView(QGLContext* context, QWidget *parent)
	 : QGLWidget(context,parent)
{
	SetUp();
}

CameraView::CameraView(QWidget *parent)
	 : QGLWidget(QGLFormat(QGL::SampleBuffers),parent)
{
	SetUp();
}


CameraView::~CameraView(void)
{
}

void CameraView::SetUp()
{
	_readyToDraw = false;
	_newSetup = false;
	_showFovRef = false;
	_offset = 0;
	_rotationY = 0;
	_distance = -15;
	_xRot = 0;
	_yRot = 0;
	_zRot = 0;
	_marker = QList<VirtualMarker*>();
	_markerBoxes = QList<Box*>();
	_virtualObjects = QList<Box*>();
	_roomObjects = QList<Box*>();

	_fromOriginToRoom = new Vector3f(-0.5f,-0.5f,0);

	_refCameraPosition = new Vector3f(0,0,0);
	_refCameraRotation = new AngleAxisf(0,Vector3f(0,1,0));
}

void CameraView::SetRefFrustrum(Vector3f* position, AngleAxisf* rotation, bool flag)
{
	_refCameraPosition = position;
	_refCameraRotation = rotation;
	_showFovRef = flag;
}

void CameraView::ChangeSetup(VirtualSetup* setup)
{
	_setup = setup;
	_newSetup = true;
}

void CameraView::SetVirtualSetup()
{
	Vector3f roomDim = _setup->GetRoom()->_dimensionsInCM;
	_room = new Box(this, roomDim.y() * 0.01, roomDim.x() * 0.01, roomDim.z() * 0.01, true);
	_room->setColor(COLOR_ROOM);

	_fromOriginToRoom = new Vector3f( _setup->GetRoom()->_fromOriginToRoom);
	*_fromOriginToRoom *= 0.01f;

	_roomObjects.clear();
	for(int i = 0; i < _setup->GetRoom()->RoomObjectSize(); i++)
	{
		Vector3f size = Vector3f();
		Vector3f position = Vector3f();
		AngleAxisf rotation = AngleAxisf();
		QString name = QString();
		_setup->GetRoom()->GetRoomObject(i,&position, &size,&rotation,&name);
		_roomObjects.append(new Box(this, size.y()*0.01, size.x()*0.01, size.z()*0.01));
		_roomObjects[i]->setColor(COLOR_OBJECT);
	}

	_setup->GetVirtualMarker(&_marker);
	_markerBoxes.clear();

	for(int i = 0; i < _marker.size(); i++)
	{
		VirtualMarker* m = _marker[i];
		float width = m->_widthInCM*0.01;
		_markerBoxes.append(new Box(this, width, width, 0.02));
		_markerBoxes[i]->setColor(COLOR_MARKER_UNTRACKED);
	}

	_newSetup = false;
}

void CameraView::SetSize(QRect size)
{
	this->setGeometry(size);
	_geometry = this->geometry();
	//_cameraImageWidth = size.width();
	//_cameraImageHeight = size.height();
	SetViewport();
}

void CameraView::Load(Vector3f* camPos, AngleAxisf* camRot, float* camQuality, float sceneScale, float fovy, float aspect, float zNear, float zFar)
{
	_sceneScale = sceneScale;

	_cameraPosition = camPos;
	_cameraRotation = camRot;
	_cameraQuality = camQuality;

	_fov = new Frustrum(this, fovy, aspect, zNear, zFar, 0.01);
	_fov->setColor(COLOR_FRUSTRUM);

	_fovRef = new Frustrum(this, fovy, aspect, zNear, zFar, 0.01);
	_fovRef->setColor(COLOR_FRUSTRUM_REF);
	
	_room = new Box(this, 1, 1, 1, true);
	_room->setColor(COLOR_ROOM);

	_origin = new Sphere(this, 30, 0.03);
	_origin->setColor(COLOR_ORIGIN_POINT); 

	_markerBoxes = QList<Box*>();
	_virtualObjects = QList<Box*>();
	_roomObjects = QList<Box*>();

	_readyToDraw = true;
/*
	_logo = new QtLogo(this);
	_logo->setColor(QColor::fromCmykF(0.40, 0.0, 1.0, 0.0).dark());*/
}

void CameraView::initializeGL()
{
	/*_cameraAspect = 1;
	_cameraUpsideDown = false;
	_cameraTextureWidth = this->size().width();
	_cameraTextureHeight = this->size().height();
	_cameraTexture = 3;
	_cameraTextureAllocated = false;
	glGenTextures(1, &_cameraTexture);*/
	this->adjustSize();
}


void CameraView::resizeGL(int width, int height)
{
	int side = qMin(width, height);
    glViewport(0,0,width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	#ifdef QT_OPENGL_ES
    glOrthof(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
	#else
   // glOrtho(-0.5, +0.5, -0.5, 0.5, 1.0, 500.0);
	gluPerspective(30, (float)width/(float)height, 1.f, 500.f);
	#endif
    glMatrixMode(GL_MODELVIEW);
}

void CameraView::Update(float elapsedTime)
{
	if(_newSetup)
		SetVirtualSetup();

	_rotationY+= elapsedTime/30.f;
	update();
}


void CameraView::SetViewport()
{
	int side = qMin(width(), height());
    glViewport(0,0,width(), height());

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	#ifdef QT_OPENGL_ES
    glOrthof(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
	#else
   // glOrtho(-0.5, +0.5, -0.5, 0.5, 1.0, 500.0);
	gluPerspective(30, 4.f/3.f, 1.f, 500.f);
	#endif
    glMatrixMode(GL_MODELVIEW);
}

void CameraView::DrawViewFrustrum()
{
	glPushMatrix();
		//glRotatef(_rotationY, 0.0, 1.0, 0.0);
		Vector3f cameraPos = *_cameraPosition;
		AngleAxisf camRot = *_cameraRotation;
		float quality = *_cameraQuality;

		if(quality > 0)
		{
			glTranslatef(cameraPos.x() * 0.01, cameraPos.y()*0.01, cameraPos.z()*0.01);
			//glTranslatef(-_fromOriginToRoom->x(), -_fromOriginToRoom->y(), _fromOriginToRoom->z());
			//glRotatef(_rotationY, 0.0, 1.0, 0.0);
			glRotatef(qRadiansToDegrees(camRot.angle()), camRot.axis().x() * 0.01, camRot.axis().y()*0.01, camRot.axis().z()*0.01);
			_fov->draw();
		}
	glPopMatrix();
}

void CameraView::DrawRefViewFrustrum()
{
	glPushMatrix();
		//glRotatef(_rotationY, 0.0, 1.0, 0.0);
		Vector3f cameraPos = *_refCameraPosition;
		AngleAxisf camRot = *_refCameraRotation;

		glTranslatef(cameraPos.x() * 0.01, cameraPos.y()*0.01, cameraPos.z()*0.01);
		glRotatef(qRadiansToDegrees(camRot.angle()), camRot.axis().x() * 0.01, camRot.axis().y()*0.01, camRot.axis().z()*0.01);
		_fovRef->draw();
		
	glPopMatrix();
}

void CameraView::DrawRoom()
{
	glPushMatrix();
		glTranslatef(-_fromOriginToRoom->x(), -_fromOriginToRoom->y(), -_fromOriginToRoom->z());
		_room->draw();
	glPopMatrix();

	for(int i = 0; i < _setup->GetRoom()->RoomObjectSize(); i++)
	{
		if(_roomObjects.size() <= i)
		{
			qDebug() << "WARNING: RoomObjectList in CameraView is inconsistence";
			return;
		}
		Vector3f size;
		Vector3f position;
		AngleAxisf rotation;
		QString name;
		_setup->GetRoom()->GetRoomObject(i,&position, &size,&rotation,&name);
		glPushMatrix();
			glTranslatef(position.x()*0.01,position.y()*0.01,position.z()*0.01);
			glRotatef(qRadiansToDegrees(rotation.angle()), rotation.axis().x(),rotation.axis().y(),rotation.axis().z());
			_roomObjects[i]->draw();
		glPopMatrix();
	}

}

void CameraView::DrawMarker()
{
	for(int i = 0; i < _markerBoxes.size(); i++)
	{
		if(_marker.size() <= i)
		{
			qDebug() << "WARNING: MarkerList in CameraView is inconsistence";
			return;
		}
		VirtualMarker* m = _marker[i];
		if(m->_isFixed || m->_isTracked)
		{
			glPushMatrix();
			glTranslatef(m->_positionInSetup->x()*0.01,m->_positionInSetup->y()*0.01,m->_positionInSetup->z()*0.01);
			AngleAxisf* rotation = new AngleAxisf(*m->_rotationInSetup);
			glRotatef(qRadiansToDegrees(rotation->angle()), rotation->axis().x(),rotation->axis().y(),rotation->axis().z());
			_markerBoxes[i]->draw();
			if(m->_isTracked)
			{
				Vector3f good = Vector3f(COLOR_MARKER_TRACKED_GOOD.red(), COLOR_MARKER_TRACKED_GOOD.green(), COLOR_MARKER_TRACKED_GOOD.blue());
				Vector3f bad = Vector3f(COLOR_MARKER_TRACKED_BAD.red(), COLOR_MARKER_TRACKED_BAD.green(), COLOR_MARKER_TRACKED_BAD.blue());
				float qualityValue = m->_qualityVision;
				if(qualityValue > 1) qualityValue = 1;
				Vector3f markerColor = Vector3f(qualityValue*good + (1-qualityValue) * bad);
				//QColor markerColor = COLOR_MARKER_TRACKED_GOOD;
				_markerBoxes[i]->setColor(QColor(markerColor.x(), markerColor.y(), markerColor.z()));
			}
			else
				_markerBoxes[i]->setColor(COLOR_MARKER_UNTRACKED);
			glPopMatrix();
		}
	}
}

void CameraView::DrawOrigin()
{
	_origin->draw();
}

void CameraView::paintGL()
{
	if(!_readyToDraw || _newSetup)
		return;

	makeCurrent();
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	qglClearColor(BACKGROUND);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);

    static GLfloat lightPosition[4] = { 0, 0.9, 0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    qreal pixelRatio = devicePixelRatio();
    SetViewport();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
	//glScalef(_sceneScale,_sceneScale,_sceneScale);
	glTranslatef(0.0, 0.0, _distance);
	glRotatef(90, 1, 0.0, 0);
	
	glRotatef(_xRot/16, 1, 0, 0);
	glRotatef(_yRot/16, 0, 1, 0);
	glRotatef(_zRot/16, 0, 0, 1);
	glPushMatrix();
		glTranslatef(_fromOriginToRoom->x(), _fromOriginToRoom->y(), _fromOriginToRoom->z());
		DrawRoom();
 		DrawOrigin();
		DrawMarker();
		DrawViewFrustrum();
		if(_showFovRef)
			DrawRefViewFrustrum();
	glPopMatrix();
    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void CameraView::wheelEvent(QWheelEvent* event)
{
	_distance += event->delta()/240.0f;
}

void CameraView::mousePressEvent(QMouseEvent *event)
{
    _lastPos = event->pos();
}

void CameraView::mouseMoveEvent(QMouseEvent *event)
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

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360*16 ;
    while (angle > 360*16 )
        angle -= 360*16 ;
}

void CameraView::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != _xRot) {
        _xRot = angle;
        updateGL();
    }
}

void CameraView::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != _yRot) {
        _yRot = angle;
        updateGL();
    }
}

void CameraView::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != _zRot) {
        _zRot = angle;
        updateGL();
    }
}