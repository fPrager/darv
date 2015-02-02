#pragma once

#include "stdafx.h"
#include <QGLWidget>
#include <QtWidgets>
#include <QtOpenGL>
#include <QTPrimitives.h>
#include <VirtualMarker.h>

using namespace Eigen;
using namespace std;

class UncertaintyCOS: public QGLWidget
{
public:
	UncertaintyCOS(QWidget *parent = 0);
	~UncertaintyCOS(void);
	void Update(float x, float y, float z, float yaw, float pitch, float roll);
	void Load(QString id, Uncertainty* src);
	void Draw();
	void SetSize(QRect size);

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintEvent(QPaintEvent *event);

private:
	void MakeArrows();
	void SetupViewport(int width, int height);

	QRect _geometry;

	Arrow* _xAxis;
	Arrow* _yAxis;
	Arrow* _zAxis;

	Sphere* _ellipsoid;
	Cone* _xCone;
	Cone* _yCone;
	Cone* _zCone;

	QString _id;
	Uncertainty* _source;

	bool _readyToDraw;
};

