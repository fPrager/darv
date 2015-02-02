#include "stdafx.h"
#include "RotationViewer.h"


RotationViewer::RotationViewer(QWidget *parent)
	 : QGLWidget(QGLFormat(QGL::SampleBuffers),parent)
{
	_readyToDraw = false;
	_drawIMURot = false;
	_mouseDown = false;
	_clickTime = 0;

	_xRot = 0;
	_yRot = 0;
	_zRot = 0;
	_viewRot = AngleAxisf(0, Vector3f(0,1,0));

	_focusOnCam = true;
	_focusOnIMU = true;
	_focusOnMarker = true;

	setAutoFillBackground(false);
}


RotationViewer::~RotationViewer(void)
{
}

void RotationViewer::Load()
{
	_cameraRotation = AngleAxisf(0, Vector3f(0,1,0));
	_imuRotation = AngleAxisf(0, Vector3f(0,1,0));
	_markerRotation = AngleAxisf(0, Vector3f(0,1,0));

	_camArrow = new Arrow(this, 1, 0.8, 0.1, 0.3, 0.1);
	_camArrow->setColor(COLOR_CAM_ROT);

	_imuArrow = new Arrow(this, 0.8, 0.6, 0.09, 0.29, 0.09);
	_imuArrow->setColor(COLOR_IMU_ROT);

	_markerArrow = new Arrow(this, 0.6, 0.4, 0.09, 0.29, 0.09);
	_markerArrow->setColor(COLOR_MARKER_ROT);

	float axisLength = 1;
	float axisHead = 0.9;
	float axisWidth = 0.01;
	float axisHeadWidth = 0.02;

	_xAxis = new Arrow(this,axisLength, axisHead, axisWidth, axisHeadWidth, axisWidth);
	_xAxis->setColor(Qt::green);

	_yAxis = new Arrow(this,axisLength, axisHead, axisWidth, axisHeadWidth, axisWidth);
	_yAxis->setColor(Qt::blue);

	_zAxis = new Arrow(this,axisLength, axisHead, axisWidth, axisHeadWidth, axisWidth);
	_zAxis->setColor(Qt::red);

	_readyToDraw= true;
}

void RotationViewer::MakeArrows()
{
	
}

void RotationViewer::initializeGL()
{
	 glEnable(GL_MULTISAMPLE);
	this->adjustSize();
}

void RotationViewer::SetSize(QRect size)
{
	this->setGeometry(size);
	_geometry = this->geometry();
}

void RotationViewer::resizeGL(int width, int height)
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


void RotationViewer::Update(float elapsedTime)
{
	if(_mouseDown)
		_clickTime+=elapsedTime;
}

void RotationViewer::UpdateRotations(AngleAxisf camRotation, AngleAxisf markerRotation)
{
	_drawIMURot = false;
	_cameraRotation = camRotation;
	_markerRotation = markerRotation;
	//_cameraRotation =_cameraRotation* AngleAxisf(0.1, Vector3f(0,1,0));
	//_markerRotation =_markerRotation* AngleAxisf(0.1, Vector3f(1,0,0));
	update();
}

void RotationViewer::UpdateRotations(AngleAxisf camRotation, AngleAxisf markerRotation, AngleAxisf imuRotation)
{
	_drawIMURot = true;
	_cameraRotation = camRotation;
	_markerRotation = markerRotation;
	_imuRotation = imuRotation;
	update();
}

void RotationViewer::DrawCameraRotation()
{
	glPushMatrix();
	glRotatef(qRadiansToDegrees(_cameraRotation.angle()), _cameraRotation.axis().x(), _cameraRotation.axis().y(), _cameraRotation.axis().z());
	_camArrow->draw();
	glPopMatrix();
}

void RotationViewer::DrawMarkerRotation()
{
	glPushMatrix();
	glRotatef(qRadiansToDegrees(_markerRotation.angle()), _markerRotation.axis().x(), _markerRotation.axis().y(), _markerRotation.axis().z());
	_markerArrow->draw();
	glPopMatrix();
}

void RotationViewer::DrawIMURotation()
{
	glPushMatrix();
	glRotatef(qRadiansToDegrees(_imuRotation.angle()), _imuRotation.axis().x(), _imuRotation.axis().y(), _imuRotation.axis().z());
	_imuArrow->draw();
	glPopMatrix();
}

void RotationViewer::DrawCoordinateAxis()
{
	QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
	
	QFontMetrics metrics = QFontMetrics(font());
    int border = qMax(4, metrics.leading());
	float length = height() - 10*border;
	


	/*Vector3f unitX = _viewRot._transformVector(Vector3f(length,0,0));
	Vector3f unitY = _viewRot._transformVector(Vector3f(0,length,0));
	Vector3f unitZ = _viewRot._transformVector(Vector3f(0,0,-length));

	painter.setPen(Qt::green);
	QLineF xline(border*5, height()-border*5, border*5 + unitX.x(), height()-border*5 - unitX.y());
	painter.drawLine(xline);

	painter.setPen(Qt::blue);
	QLineF yline(border*5, height()-border*5, border*5 + unitY.x(), height()-border*5 - unitY.y());
	painter.drawLine(yline);

	painter.setPen(Qt::red);
	QLineF zline(border*5, height()-border*5, border*5 + unitZ.x(), height()-border*5 - unitZ.y());
	painter.drawLine(zline);*/


	QColor colorCam = COLOR_CAM_ROT;
	if(!_focusOnCam) colorCam = COLOR_UNFOCUS;

	QColor colorIMU = COLOR_IMU_ROT;
	if(!_focusOnIMU) colorIMU = COLOR_UNFOCUS;

	QColor colorMarker = COLOR_MARKER_ROT;
	if(!_focusOnMarker) colorMarker = COLOR_UNFOCUS;

	QString camRotText = tr("CAMRot");
	QString markerRotText = tr("MRot");
	QString imuRotText = tr("IMURot");
    
    QRect rect = metrics.boundingRect(0, 0, width() - 2*border, int(height()*0.125),
                                      Qt::AlignCenter | Qt::TextWordWrap, camRotText);
    painter.setPen(colorCam);
    painter.drawText(border, border,
                      width()- 2*border, rect.height(),
                      Qt::AlignLeft, camRotText);
	painter.setPen(colorMarker);
    painter.drawText(border, border,
                      width()- 2*border, rect.height(),
                      Qt::AlignCenter, markerRotText);

	if(_drawIMURot)
	{
		painter.setPen(colorIMU);
		painter.drawText(border, border,
						  width()- 2*border, rect.height(),
						  Qt::AlignRight, imuRotText);
	}
    painter.end();
}

void RotationViewer::paintEvent(QPaintEvent *event)
{
	makeCurrent();

	if(!_readyToDraw)
		return;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    qglClearColor(BACKGROUND);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
	//glEnable (GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    static GLfloat lightPosition[4] = { 0, 0.9, 0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    qreal pixelRatio = devicePixelRatio();
    SetupViewport(width() * pixelRatio, height() * pixelRatio);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

	glTranslatef(0.0, 0.0, -3);
	glPushMatrix();
		glTranslatef(-0.5, -0.5, 0.1);
		glRotatef(_xRot/16, 1, 0, 0);
		glRotatef(_yRot/16, 0, 1, 0);
		glRotatef(_zRot/16, 0, 0, 1);
		
		glPushMatrix();
			glRotatef(-90, 0, 1, 0);
			_xAxis->draw();
		glPopMatrix();

		glPushMatrix();
			glRotatef(90, 1, 0, 0);
			_yAxis->draw();
		glPopMatrix();

		_zAxis->draw();

		glTranslatef(0.5, 0.5, -0.1);
		if(_focusOnCam)
			DrawCameraRotation();
		if(_focusOnMarker)
			DrawMarkerRotation();
		if(_drawIMURot && _focusOnIMU)
			DrawIMURotation();
	glPopMatrix();



    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	DrawCoordinateAxis();
}

void RotationViewer::SetupViewport(int width, int height)
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


void RotationViewer::SetFocus()
{
	if(!_focusOnCam)
	{
		_camArrow->setColor(COLOR_UNFOCUS);
	}
	else
	{
		_camArrow->setColor(COLOR_CAM_ROT);
	}

	if(!_focusOnIMU)
	{
		_imuArrow->setColor(COLOR_UNFOCUS);
	}
	else
	{
		_imuArrow->setColor(COLOR_IMU_ROT);
	}

	if(!_focusOnMarker)
	{
		_markerArrow->setColor(COLOR_UNFOCUS);
	}
	else
	{
		_markerArrow->setColor(COLOR_MARKER_ROT);
	}
}


void RotationViewer::mousePressEvent(QMouseEvent *event)
{
    _lastPos = event->pos();
	_clickTime = 0;
	_mouseDown = true;
}

void RotationViewer::mouseReleaseEvent(QMouseEvent *event)
{
	if(_clickTime < 500)
	{
		if(event->y() < 26)
		{
			if(event->x() < 60)
				_focusOnCam = !_focusOnCam;
			else
			if(event->x() < 112)
				_focusOnMarker = !_focusOnMarker;
			else
				_focusOnIMU = !_focusOnIMU;
			SetFocus();
		}
	}
	_mouseDown = false;
	_clickTime = 0;
}

void RotationViewer::mouseMoveEvent(QMouseEvent *event)
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

void RotationViewer::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != _xRot) {
        _xRot = angle;
		_viewRot = AngleAxisf(qDegreesToRadians(_xRot/16), Vector3f(1,0,0))*AngleAxisf(qDegreesToRadians(_yRot/16), Vector3f(0,1,0))*AngleAxisf(qDegreesToRadians(_zRot/16), Vector3f(0,0,1));
    }
}

void RotationViewer::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != _yRot) {
        _yRot = angle;
		_viewRot = AngleAxisf(qDegreesToRadians(_xRot/16), Vector3f(1,0,0))*AngleAxisf(qDegreesToRadians(_yRot/16), Vector3f(0,1,0))*AngleAxisf(qDegreesToRadians(_zRot/16), Vector3f(0,0,1));
    }
}

void RotationViewer::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != _zRot) {
        _zRot = angle;
		_viewRot = AngleAxisf(qDegreesToRadians(_xRot/16), Vector3f(1,0,0))*AngleAxisf(qDegreesToRadians(_yRot/16), Vector3f(0,1,0))*AngleAxisf(qDegreesToRadians(_zRot/16), Vector3f(0,0,1));
    }
}