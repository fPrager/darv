/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#pragma once
#ifndef QTPRIMITIVES_H
#define QTPRIMITIVES_H
#include "stdafx.h"

#include <QtOpenGL>
#include <QGLWidget>
#include <QMatrix4x4>
#include <QVector3D>
#include <QObject>
#include <QColor>
#include <qmath.h>

class Patch;
struct Geometry;

//! [0]
class QtLogo : public QObject
{
public:
    explicit QtLogo(QObject *parent, int d = 64, qreal s = 1.0);
    ~QtLogo();
    void setColor(QColor c);
    void draw() const;
private:
    void buildGeometry(int d, qreal s);

    QList<Patch *> parts;
    Geometry *geom;
};
//! [0]

//increased by Fabian Prager
//renamed to QTPRIMITIVES

class Frustrum: public QObject
{
public:
    explicit Frustrum(QObject *parent, qreal height = 1, qreal width = 1, qreal depth = 1 );
	explicit Frustrum(QObject *parent, qreal fovy = 35, qreal aspect = 4.0/3.0, qreal zNear = 1, qreal zFar = 500, qreal scale = 0.1);
    ~Frustrum();
    void setColor(QColor c);
    void draw() const;
private:
    void buildGeometry(qreal circleRad, qreal h1, qreal w, qreal d, qreal h2 = 0);

    QList<Patch *> parts;
    Geometry *geom;
};

class Cone: public QObject
{
public:
    explicit Cone(QObject *parent, float height, qreal rad,int numSectors);
    ~Cone();
    void setColor(QColor c);
    void draw() const;
private:
    void buildGeometry(float height, qreal rad,int numSectors);

    QList<Patch *> parts;
    Geometry *geom;
};

class Sphere: public QObject
{
public:
    explicit Sphere(QObject *parent, int d1 = 30, qreal rad = 1, int d2 = 0);
    ~Sphere();
    void setColor(QColor c);
    void draw() const;
private:
    void buildGeometry(int d1, int d2, qreal rad);

    QList<Patch *> parts;
    Geometry *geom;
};

class Box: public QObject
{
public:
    explicit Box(QObject *parent, qreal height = 1, qreal width = 1, qreal depth = 1, bool inSideOut = false);
    ~Box();
    void setColor(QColor c);
    void draw() const;
private:
    void buildGeometry(qreal h, qreal w, qreal d, bool inSideOut);

    QList<Patch *> parts;
    Geometry *geom;
};

class Arrow: public QObject
{
public:
    explicit Arrow(QObject *parent, qreal length = 1, qreal lengthToHead = 1, qreal width = 1, qreal headWidth = 1, qreal depth = 1);
    ~Arrow();
    void setColor(QColor c);
    void draw() const;
private:
    void buildGeometry(qreal l, qreal lTH, qreal w, qreal hW, qreal d);

    QList<Patch *> parts;
    Geometry *geom;
};

#endif // QTPRIMITIVES_H


