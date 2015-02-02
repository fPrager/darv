#include "stdafx.h"
#include "UncertaintyCOS.h"


UncertaintyCOS::UncertaintyCOS(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers),parent)
{
	_readyToDraw = false;
	setAutoFillBackground(false);
	_source = 0;
}


UncertaintyCOS::~UncertaintyCOS()
{
}

void UncertaintyCOS::Load(QString id, Uncertainty* src)
{
	_id = id;
	_source = src;
	
	_ellipsoid = new Sphere(this);
	_ellipsoid->setColor(QColor(1,1,1,100));

	float axisLength = 5;
	float axisHead = 2;
	float axisWidth = 0.1f;
	float axisHeadWidth = 0.4f;

	_xAxis = new Arrow(this,axisLength, axisHead, axisWidth, axisHeadWidth, axisWidth);
	_xAxis->setColor(Qt::gray);

	_yAxis = new Arrow(this,axisLength, axisHead, axisWidth, axisHeadWidth, axisWidth);
	_yAxis->setColor(Qt::gray);

	_zAxis = new Arrow(this,axisLength, axisHead, axisWidth, axisHeadWidth, axisWidth);
	_zAxis->setColor(Qt::gray);

	_xCone = new Cone(this, 1, 1.f,20);
	_xCone->setColor(Qt::green);

	_yCone = new Cone(this, 1, 1.f,20);
	_yCone->setColor(Qt::blue);

	_zCone = new Cone(this, 1, 1.f,20);
	_zCone->setColor(Qt::red);

	_readyToDraw= true;
}

void UncertaintyCOS::initializeGL()
{
	 glEnable(GL_MULTISAMPLE);
	this->adjustSize();
}

void UncertaintyCOS::SetSize(QRect size)
{
	this->setGeometry(size);
	_geometry = this->geometry();
}

void UncertaintyCOS::resizeGL(int width, int height)
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

void UncertaintyCOS::Update(float x, float y, float z, float yaw, float pitch, float roll)
{
}


void UncertaintyCOS::paintEvent(QPaintEvent *event)
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

    glEnable(GL_MULTISAMPLE);
	//glEnable (GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Vector3f transSpace = _source->GetTranslationSpace();
	float maxima = 1;
	float toBackTranslation = 0;
	if(_source->IsKnown())
	{
		float fromZValue = transSpace.z()*1.3f + 1;
		float fromXYValues = (max(transSpace.x()*1.3f, transSpace.y()*1.3f)+1)/tan(30.f* (M_PI/180.0f)) * 2;
		if(fromZValue > fromXYValues)
			toBackTranslation = fromZValue;
		else
			toBackTranslation = fromXYValues;
	}
	qreal pixelRatio = devicePixelRatio();
    SetupViewport(width() * pixelRatio, height() * pixelRatio);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
	
	glTranslatef(0.0, 0.0, -toBackTranslation);
	//glTranslatef(0.0, 0.0, -25);
	
	glPushMatrix();
			glRotatef(-45, 0,0.5f,0.5f);
			glRotatef(45, 1,0,0);
		if(_source->IsKnown())
		{
			//draw the litghsource
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			static GLfloat lightPosition[4] = { 0, toBackTranslation, toBackTranslation, 1.0f};
			glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
			
			//draw the translation-ellipsoid
			glPushMatrix();
				glScalef(transSpace.x(), transSpace.y(), transSpace.z());
				//_ellipsoid->draw();
			glPopMatrix();

			//draw the rotationcones
			Vector3f rotSpace = _source->GetRotationSpace();
			Vector3f coneLengths = transSpace * 1.3f;
			Vector3f coneScaleX = Vector3f(0,0,0);
			coneScaleX.x() = coneLengths.x();
			coneScaleX.y() = tan(rotSpace.z()* (M_PI/180.0f)) * coneLengths.x();
			coneScaleX.z() = tan(rotSpace.x()* (M_PI/180.0f)) * coneLengths.x();

			Vector3f coneScaleY = Vector3f(0,0,0);
			coneScaleY.x() = tan(rotSpace.z()* (M_PI/180.0f)) * coneLengths.y();
			coneScaleY.y() = coneLengths.y();
			coneScaleY.z() = tan(rotSpace.y()* (M_PI/180.0f)) * coneLengths.y();

			Vector3f coneScaleZ = Vector3f(0,0,0);
			coneScaleZ.x() = tan(rotSpace.x()* (M_PI/180.0f)) * coneLengths.z();
			coneScaleZ.y() = tan(rotSpace.y()* (M_PI/180.0f)) * coneLengths.z();
			coneScaleZ.z() = coneLengths.z();

			glPushMatrix();
				glTranslatef(coneScaleX.x(),0,0);
				glScalef(coneScaleX.x(),coneScaleX.y(),coneScaleX.z());
				glRotatef(-90, 0, 1, 0);
				_xCone->draw();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0,coneScaleY.y(),0);
				glScalef(coneScaleY.x(),coneScaleY.y(),coneScaleY.z());
				glRotatef(90, 1, 0, 0);
				_yCone->draw();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0,0,coneScaleZ.z());
				glScalef(coneScaleZ.x(),coneScaleZ.y(),coneScaleZ.z());
				glRotatef(-180, 0, 1, 0);
				_zCone->draw();
			glPopMatrix();

			//draw the axises
			float axisScaleFactor = toBackTranslation/21.f;
			glPushMatrix();
				glTranslatef(transSpace.x()*1.3f,0,0);
				glScalef(1,axisScaleFactor,axisScaleFactor);
				glRotatef(-90, 0, 1, 0);
				_xAxis->draw();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0,transSpace.y()*1.3f,0);
				glScalef(axisScaleFactor,1,axisScaleFactor);
				glRotatef(90, 1, 0, 0);
				_yAxis->draw();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0,0,transSpace.z()*1.3f);
				glScalef(axisScaleFactor,axisScaleFactor,1);
				glRotatef(-180, 0, 1, 0);
				_zAxis->draw();
			glPopMatrix();

		}
	glPopMatrix();
	

    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
	
	QFontMetrics metrics = QFontMetrics(font());
    int border = qMax(4, metrics.leading());
	float length = height() - 10*border;
	
	QColor colorMarker = QColor(0,100,250,255);
	QString infoText = _id + "\n ";
	if(_source->IsKnown())
		infoText = _id + "\nG: " + QString::number(_source->GetQualityValue());
    QRect rect = metrics.boundingRect(0, 0, width() - 2*border, int(height()*0.5f),
                                      Qt::AlignCenter | Qt::TextWordWrap, infoText);
   	painter.setPen(colorMarker);
    painter.drawText(border, border,
                      width()- 2*border, rect.height(),
					  Qt::AlignCenter, infoText);

    painter.end();
}

void UncertaintyCOS::SetupViewport(int width, int height)
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

