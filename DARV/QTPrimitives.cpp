#include "stdafx.h"
#include "QTPrimitives.h"

static const qreal tee_height = 0.311126;
static const qreal cross_width = 0.25;
static const qreal bar_thickness = 0.113137;
static const qreal logo_depth = 0.1;

//! [0]
struct Geometry
{
    QVector<GLushort> faces;
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    void appendSmooth(const QVector3D &a, const QVector3D &n, int from);
    void appendFaceted(const QVector3D &a, const QVector3D &n);
    void finalize();
    void loadArrays() const;
};
//! [0]

//! [1]
class Patch
{
public:
    enum Smoothing { Faceted, Smooth };
    Patch(Geometry *);
    void setSmoothing(Smoothing s) { sm = s; }
    void translate(const QVector3D &t);
    void rotate(qreal deg, QVector3D axis);
    void draw() const;
    void addTri(const QVector3D &a, const QVector3D &b, const QVector3D &c, const QVector3D &n);
    void addQuad(const QVector3D &a, const QVector3D &b,  const QVector3D &c, const QVector3D &d);

    GLushort start;
    GLushort count;
    GLushort initv;

    GLfloat faceColor[4];
    QMatrix4x4 mat;
    Smoothing sm;
    Geometry *geom;
};
//! [1]

static inline void qSetColor(float colorVec[], QColor c)
{
    colorVec[0] = c.redF();
    colorVec[1] = c.greenF();
    colorVec[2] = c.blueF();
    colorVec[3] = c.alphaF();
}

void Geometry::loadArrays() const
{
    glVertexPointer(3, GL_FLOAT, 0, vertices.constData());
    glNormalPointer(GL_FLOAT, 0, normals.constData());
}

void Geometry::finalize()
{
    // TODO: add vertex buffer uploading here

    // Finish smoothing normals by ensuring accumulated normals are returned
    // to length 1.0.
    for (int i = 0; i < normals.count(); ++i)
        normals[i].normalize();
}

void Geometry::appendSmooth(const QVector3D &a, const QVector3D &n, int from)
{
    // Smooth normals are achieved by averaging the normals for faces meeting
    // at a point.  First find the point in geometry already generated
    // (working backwards, since most often the points shared are between faces
    // recently added).
    int v = vertices.count() - 1;
    for ( ; v >= from; --v)
        if (qFuzzyCompare(vertices[v], a))
            break;

    if (v < from) {
        // The vertex was not found so add it as a new one, and initialize
        // its corresponding normal
        v = vertices.count();
        vertices.append(a);
        normals.append(n);
    } else {
        // Vert found, accumulate normals into corresponding normal slot.
        // Must call finalize once finished accumulating normals
        normals[v] += n;
    }

    // In both cases (found or not) reference the vertex via its index
    faces.append(v);
}

void Geometry::appendFaceted(const QVector3D &a, const QVector3D &n)
{
    // Faceted normals are achieved by duplicating the vertex for every
    // normal, so that faces meeting at a vertex get a sharp edge.
    int v = vertices.count();
    vertices.append(a);
    normals.append(n);
    faces.append(v);
}

Patch::Patch(Geometry *g)
   : start(g->faces.count())
   , count(0)
   , initv(g->vertices.count())
   , sm(Patch::Smooth)
   , geom(g)
{
    qSetColor(faceColor, QColor(Qt::darkGray));
}

void Patch::rotate(qreal deg, QVector3D axis)
{
    mat.rotate(deg, axis);
}

void Patch::translate(const QVector3D &t)
{
    mat.translate(t);
}

//! [2]
void Patch::draw() const
{
    glPushMatrix();
    glMultMatrixf(mat.constData());
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, faceColor);

    const GLushort *indices = geom->faces.constData();
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, indices + start);
    glPopMatrix();
}
//! [2]

void Patch::addTri(const QVector3D &a, const QVector3D &b, const QVector3D &c, const QVector3D &n)
{
    QVector3D norm = n.isNull() ? QVector3D::normal(a, b, c) : n;

    if (sm == Smooth) {
        geom->appendSmooth(a, norm, initv);
        geom->appendSmooth(b, norm, initv);
        geom->appendSmooth(c, norm, initv);
    } else {
        geom->appendFaceted(a, norm);
        geom->appendFaceted(b, norm);
        geom->appendFaceted(c, norm);
    }

    count += 3;
}

void Patch::addQuad(const QVector3D &a, const QVector3D &b,  const QVector3D &c, const QVector3D &d)
{
    QVector3D norm = QVector3D::normal(a, b, c);

    if (sm == Smooth) {
        addTri(a, b, c, norm);
        addTri(a, c, d, norm);
    } else {
        // If faceted share the two common vertices
        addTri(a, b, c, norm);
        int k = geom->vertices.count();
        geom->appendSmooth(a, norm, k);
        geom->appendSmooth(c, norm, k);
        geom->appendFaceted(d, norm);
        count += 3;
    }
}

static inline QVector<QVector3D> extrude(const QVector<QVector3D> &vertices, qreal depth)
{
    QVector<QVector3D> extr = vertices;
    for (int v = 0; v < extr.count(); ++v)
        extr[v].setZ(extr[v].z() - depth);
    return extr;
}

class Rectoid
{
public:
    void translate(const QVector3D &t)
    {
        for (int i = 0; i < parts.count(); ++i)
            parts[i]->translate(t);
    }

    void rotate(qreal deg, QVector3D axis)
    {
        for (int i = 0; i < parts.count(); ++i)
            parts[i]->rotate(deg, axis);
    }

    // No special Rectoid destructor - the parts are fetched out of this member
    // variable, and destroyed by the new owner
    QList<Patch *> parts;
};

class RectPrism : public Rectoid
{
public:
    RectPrism(Geometry *g, qreal width, qreal height, qreal depth, bool switchCulling = false);
};

RectPrism::RectPrism(Geometry *g, qreal width, qreal height, qreal depth, bool switchCulling)
{
    enum { bl, br, tr, tl };
    Patch *fb = new Patch(g);
    fb->setSmoothing(Patch::Faceted);

    // front face
    QVector<QVector3D> r(4);
	qreal toSide = width;
	if(switchCulling)
	{
		toSide = -toSide;
	}
	r[br].setX(toSide);
	r[tr].setX(toSide);
    r[tr].setY(height);
    r[tl].setY(height);

    QVector3D adjToCenter(-toSide / 2.0, -height / 2.0, depth / 2.0);
    for (int i = 0; i < 4; ++i)
        r[i] += adjToCenter;
    fb->addQuad(r[bl], r[br], r[tr], r[tl]);

    // back face
    QVector<QVector3D> s = extrude(r, depth);
    fb->addQuad(s[tl], s[tr], s[br], s[bl]);

    // side faces
    Patch *sides = new Patch(g);
    sides->setSmoothing(Patch::Faceted);
    sides->addQuad(s[bl], s[br], r[br], r[bl]);
    sides->addQuad(s[br], s[tr], r[tr], r[br]);
    sides->addQuad(s[tr], s[tl], r[tl], r[tr]);
    sides->addQuad(s[tl], s[bl], r[bl], r[tl]);

    parts << fb << sides;
}

class RectPyramide : public Rectoid
{
public:
    RectPyramide(Geometry *g, qreal width, qreal height, qreal depth, qreal height2);
};

RectPyramide::RectPyramide(Geometry *g, qreal width, qreal height, qreal depth, qreal height2)
{
	if(height2 == 0)
		height2 = height;
	
	qreal upperSideHeight = height/2 - (height - height2);
	qreal upperSideWidth = width/2 * (height-height2)/height;
	qreal upperSideDepth = depth/2 * (height-height2)/height;

	// bottom face
    QVector<QVector3D> b(4);
	b[0]= QVector3D(-width/2,-height/2,-depth/2);
	b[1]= QVector3D(width/2,-height/2,-depth/2);
	b[2]= QVector3D(width/2,-height/2,depth/2);
	b[3]= QVector3D(-width/2,-height/2,depth/2);

	QVector<QVector3D> t(4);
	t[0]= QVector3D(-upperSideWidth,upperSideHeight,upperSideDepth);
	t[1]= QVector3D(upperSideWidth,upperSideHeight,upperSideDepth);
	t[2]= QVector3D(upperSideWidth,upperSideHeight,-upperSideDepth);
	t[3]= QVector3D(-upperSideWidth,upperSideHeight,-upperSideDepth);

	QVector<QVector3D> sl(4);
	sl[0]= QVector3D(-width/2,-height/2,depth/2);
	sl[1]= QVector3D(-upperSideWidth,upperSideHeight,upperSideDepth);
	sl[2]= QVector3D(-upperSideWidth,upperSideHeight,-upperSideDepth);
	sl[3]= QVector3D(-width/2,-height/2,-depth/2);

	QVector<QVector3D> sr(4);
	sr[0]= QVector3D(width/2,-height/2,-depth/2);
	sr[1]= QVector3D(upperSideWidth,upperSideHeight,-upperSideDepth);
	sr[2]= QVector3D(upperSideWidth,upperSideHeight,upperSideDepth);
	sr[3]= QVector3D(width/2,-height/2,depth/2);

	QVector<QVector3D> sb(4);
	sb[0]= QVector3D(-width/2,-height/2,-depth/2);
	sb[1]= QVector3D(-upperSideWidth,upperSideHeight,-upperSideDepth);
	sb[2]= QVector3D(upperSideWidth,upperSideHeight,-upperSideDepth);
	sb[3]= QVector3D(width/2,-height/2,-depth/2);

	QVector<QVector3D> sf(4);
	sf[0]= QVector3D(width/2,-height/2,depth/2);
	sf[1]= QVector3D(upperSideWidth,upperSideHeight,upperSideDepth);
	sf[2]= QVector3D(-upperSideWidth,upperSideHeight,upperSideDepth);
	sf[3]= QVector3D(-width/2,-height/2,depth/2);

    Patch *sides = new Patch(g);
	sides->addQuad(b[0], b[1],b[2], b[3]);
	sides->addQuad(t[0], t[1],t[2], t[3]);
	sides->addQuad(sl[0], sl[1],sl[2], sl[3]);
	sides->addQuad(sr[0], sr[1],sr[2], sr[3]);
	sides->addQuad(sb[0], sb[1],sb[2], sb[3]);
	sides->addQuad(sf[0], sf[1],sf[2], sf[3]);
	parts<< sides;
}

class RectTorus : public Rectoid
{
public:
    RectTorus(Geometry *g, qreal iRad, qreal oRad, qreal depth, int numSectors);
};

RectTorus::RectTorus(Geometry *g, qreal iRad, qreal oRad, qreal depth, int k)
{
    QVector<QVector3D> inside;
    QVector<QVector3D> outside;
    for (int i = 0; i < k; ++i) {
        qreal angle = (i * 2 * M_PI) / k;
        inside << QVector3D(iRad * qSin(angle), iRad * qCos(angle), depth / 2.0);
        outside << QVector3D(oRad * qSin(angle), oRad * qCos(angle), depth / 2.0);
    }
    inside << QVector3D(0.0, iRad, 0.0);
    outside << QVector3D(0.0, oRad, 0.0);
    QVector<QVector3D> in_back = extrude(inside, depth);
    QVector<QVector3D> out_back = extrude(outside, depth);

    // Create front, back and sides as separate patches so that smooth normals
    // are generated for the curving sides, but a faceted edge is created between
    // sides and front/back
    Patch *front = new Patch(g);
    for (int i = 0; i < k; ++i)
        front->addQuad(outside[i], inside[i],
                       inside[(i + 1) % k], outside[(i + 1) % k]);
    Patch *back = new Patch(g);
    for (int i = 0; i < k; ++i)
        back->addQuad(in_back[i], out_back[i],
                      out_back[(i + 1) % k], in_back[(i + 1) % k]);
    Patch *is = new Patch(g);
    for (int i = 0; i < k; ++i)
        is->addQuad(in_back[i], in_back[(i + 1) % k],
                    inside[(i + 1) % k], inside[i]);
    Patch *os = new Patch(g);
    for (int i = 0; i < k; ++i)
        os->addQuad(out_back[(i + 1) % k], out_back[i],
                    outside[i], outside[(i + 1) % k]);
    parts << front << back << is << os;
}

class RectSphere : public Rectoid
{
public:
    RectSphere(Geometry *g, qreal rad, int numVerticalSectors, int numHorizontalSectors = 0);
};

RectSphere::RectSphere(Geometry *g, qreal rad, int k,int r)
{
    QVector<QVector3D> leftRing;
    QVector<QVector3D> rightRing;
	int rings = r;
	if(r==0)
		rings = k;
	for (int r = 0; r < rings; r++) {
		qreal ringAngle1 = (r * 2 * M_PI) / rings;
		QMatrix4x4 m1;

		m1.rotate(qRadiansToDegrees(ringAngle1), 0,1,0);
		//qDebug() << "ringangle: " << QString::number(qRadiansToDegrees(ringAngle1));
		for (int i = 0; i < k; i++) {
			qreal angle = (i * 2 * M_PI) / k;
			QMatrix4x4 m2;
			m2.rotate(qRadiansToDegrees(angle), 0,0,-1);
			//qDebug() << "---  pointangle: " << QString::number(qRadiansToDegrees(angle));
			if(qRadiansToDegrees(angle) < 180)
				leftRing << m2*QVector3D(0, rad, 0)*m1;	
			else
				leftRing << m2*QVector3D(0, -rad, 0)*m1;	
		}
		leftRing << QVector3D(0.0, -rad, 0.0);
	}

    Patch *os = new Patch(g);
	for (int r = 0; r < rings; r++)
	{
		int startIndexRing = (k+1)*r;
		int startIndexNextRing = ((k+1)*(r+1))%((k+1)*rings);
		//qDebug() << "r:         "<< QString::number(r) << " and " << QString::number((r+1) % rings);
		//qDebug() << "  start: "<< QString::number(startIndexRing) << " : "<< QString::number(startIndexNextRing);
			
		for (int i = 0; i <= k; i++)
		{
			int upperLeft = i+startIndexRing;
			int upperRight = i+startIndexNextRing;
			int downerRight = i+1+startIndexNextRing;
			int downerLeft = i +1+startIndexRing;
			if(i == k)
			{
				downerRight = startIndexNextRing;
				downerLeft = startIndexRing;
			}
			//qDebug() << QString::number(i) << "    upper  indeces: "<< QString::number(upperLeft)<< " : "<< QString::number(upperRight);
			//qDebug() << QString::number(i) << "                  : "<<  QString::number(downerLeft) << " : "<< QString::number(downerRight) ;
			
			os->addQuad(leftRing[upperLeft], leftRing[upperRight],
			leftRing[downerRight],leftRing[downerLeft]);
		}
	}
    parts << os;
}

class RectDisk : public Rectoid
{
public:
    RectDisk(Geometry *g, qreal rad, qreal depth, int numSectors);
};

RectDisk::RectDisk(Geometry *g, qreal rad,  qreal depth, int k)
{
    QVector<QVector3D> outside;
    for (int i = 0; i < k; ++i) {
        qreal angle = (i * 2 * M_PI) / k;
		outside << QVector3D(rad * qSin(angle), rad * qCos(angle), depth / 2.0);
    }
    outside << QVector3D(0.0, rad, 0.0);
    QVector<QVector3D> out_back = extrude(outside, depth);

    // Create front, back and sides as separate patches so that smooth normals
    // are generated for the curving sides, but a faceted edge is created between
    // sides and front/back
    Patch *front = new Patch(g);
    for (int i = 0; i < k; ++i)
        front->addQuad(outside[i], QVector3D(0, 0, depth / 2.0),
                       QVector3D(0, 0, depth / 2.0), outside[(i + 1) % k]);
    Patch *back = new Patch(g);
    for (int i = 0; i < k; ++i)
        back->addQuad(QVector3D(0, 0, -depth / 2.0), out_back[i],
                      out_back[(i + 1) % k], QVector3D(0, 0, -depth / 2.0));

    Patch *os = new Patch(g);
    for (int i = 0; i < k; ++i)
        os->addQuad(out_back[(i + 1) % k], out_back[i],
                    outside[i], outside[(i + 1) % k]);
    parts << front << back << os;
}

class FlatRectDisk : public Rectoid
{
public:
    FlatRectDisk(Geometry *g,qreal height, qreal rad, int numSectors);
};

FlatRectDisk::FlatRectDisk(Geometry *g,qreal height, qreal rad, int k)
{
    QVector<QVector3D> outside;
    for (int i = 0; i < k; ++i) {
        qreal angle = (i * 2 * M_PI) / k;
		outside << QVector3D(rad * qSin(angle), rad * qCos(angle), 0);
    }
    outside << QVector3D(0.0, rad, 0.0);
    //QVector<QVector3D> out_back = extrude(outside, depth);

    // Create front, back and sides as separate patches so that smooth normals
    // are generated for the curving sides, but a faceted edge is created between
    // sides and front/back
    Patch *bottom = new Patch(g);
    for (int i = 0; i < k; ++i)
        bottom->addQuad(outside[(i + 1) % k], QVector3D(0, 0, 0),
                       QVector3D(0, 0, 0),outside[i]);
    Patch *surface = new Patch(g);
    for (int i = 0; i < k; ++i)
        surface->addQuad(outside[i], QVector3D(0, 0, height),
                       QVector3D(0, 0, height), outside[(i + 1) % k]);

    //Patch *os = new Patch(g);
    //for (int i = 0; i < k; ++i)
    //    os->addQuad(out_back[(i + 1) % k], out_back[i],
    //                outside[i], outside[(i + 1) % k]);
    parts << bottom << surface;
}

QtLogo::QtLogo(QObject *parent, int divisions, qreal scale)
    : QObject(parent)
    , geom(new Geometry())
{
    buildGeometry(divisions, scale);
}

QtLogo::~QtLogo()
{
    qDeleteAll(parts);
    delete geom;
}

void QtLogo::setColor(QColor c)
{
    for (int i = 0; i < parts.count(); ++i)
        qSetColor(parts[i]->faceColor, c);
}

//! [3]
void QtLogo::buildGeometry(int divisions, qreal scale)
{
    qreal cw = cross_width * scale;
    qreal bt = bar_thickness * scale;
    qreal ld = logo_depth * scale;
    qreal th = tee_height *scale;

    RectPrism cross(geom, cw, bt, ld);
    RectPrism stem(geom, bt, th, ld);

    QVector3D z(0.0, 0.0, 1.0);
    cross.rotate(45.0, z);
    stem.rotate(45.0, z);

    qreal stem_downshift = (th + bt) / 2.0;
    stem.translate(QVector3D(0.0, -stem_downshift, 0.0));

    RectTorus body(geom, 0.20, 0.30, 0.1, divisions);

   //parts << stem.parts << cross.parts << body.parts;
	parts << stem.parts;

    geom->finalize();
}
//! [3]

//! [4]
void QtLogo::draw() const
{
    geom->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    for (int i = 0; i < parts.count(); ++i)
        parts[i]->draw();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}
//! [4]

Frustrum::Frustrum(QObject *parent, qreal h, qreal w, qreal d)
    : QObject(parent)
    , geom(new Geometry())
{
    buildGeometry(0.05, h, w, d);

}

Frustrum::Frustrum(QObject *parent, qreal fovy, qreal aspect, qreal zNear, qreal zFar, qreal scale)
    : QObject(parent)
    , geom(new Geometry())
{
	qreal h1 = 1;
	qreal h2 = 1;
	qreal w = tan(fovy/2) * zFar;
	qreal d = w * (1/aspect);

	h1 = zFar;
	h2 = (zFar - zNear);

    buildGeometry(1 * scale, h1 * scale, w * scale, d * scale, h2 * scale);
}

Frustrum::~Frustrum()
{
    qDeleteAll(parts);
    delete geom;
}

void Frustrum::setColor(QColor c)
{
    for (int i = 0; i < parts.count(); ++i)
        qSetColor(parts[i]->faceColor, c);
}
void Frustrum::buildGeometry(qreal circleRad, qreal h1, qreal w, qreal d, qreal h2)
{

    RectPyramide body(geom, w, h1, d, h2);
	body.rotate(90, QVector3D(1,0,0));
	body.translate(QVector3D(0,-h1/2,0));

	RectTorus circle(geom, circleRad, circleRad*1.1, circleRad, 20);
	//circle.translate(QVector3D(0,h,0));
	//circle.rotate(90, QVector3D(1,0,0));
   //parts << stem.parts << cross.parts << body.parts;
	parts << body.parts << circle.parts;

    geom->finalize();
}
void Frustrum::draw() const
{
    geom->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    for (int i = 0; i < parts.count(); ++i)
        parts[i]->draw();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

Box::Box(QObject *parent, qreal h, qreal w, qreal d, bool inSideOut)
    : QObject(parent)
    , geom(new Geometry())
{
    buildGeometry(h, w, d, inSideOut);
}

Box::~Box()
{
    qDeleteAll(parts);
    delete geom;
}

void Box::setColor(QColor c)
{
    for (int i = 0; i < parts.count(); ++i)
        qSetColor(parts[i]->faceColor, c);
}
void Box::buildGeometry(qreal h, qreal w, qreal d, bool inSideOut)
{
    RectPrism body(geom, w, h, d, inSideOut);
	parts << body.parts;
    geom->finalize();
}
void Box::draw() const
{
    geom->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    for (int i = 0; i < parts.count(); ++i)
        parts[i]->draw();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

Cone::Cone(QObject *parent, float height, qreal rad, int numSectors)
    : QObject(parent)
    , geom(new Geometry())
{
		buildGeometry(height, rad,numSectors);
}

Cone::~Cone()
{
    qDeleteAll(parts);
    delete geom;
}

void Cone::setColor(QColor c)
{
    for (int i = 0; i < parts.count(); ++i)
        qSetColor(parts[i]->faceColor, c);
}
void Cone::buildGeometry(float height, qreal rad,int numSectors)
{
	FlatRectDisk body(geom, height, rad, numSectors);
    //RectSphere body(geom, rad, d1, d2);
	//body.rotate(90, QVector3D(-1,0,0));
	parts << body.parts;
    geom->finalize();
}

void Cone::draw() const
{
    geom->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    for (int i = 0; i < parts.count(); ++i)
        parts[i]->draw();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

Sphere::Sphere(QObject *parent, int d1, qreal rad, int d2)
    : QObject(parent)
    , geom(new Geometry())
{
	if(d2 == 0)
		buildGeometry(d1, d1, rad);
	else
		buildGeometry(d1, d2, rad);
}

Sphere::~Sphere()
{
    qDeleteAll(parts);
    delete geom;
}

void Sphere::setColor(QColor c)
{
    for (int i = 0; i < parts.count(); ++i)
        qSetColor(parts[i]->faceColor, c);
}
void Sphere::buildGeometry(int d1, int d2, qreal rad)
{
    RectSphere body(geom, rad, d1, d2);
	parts << body.parts;
    geom->finalize();
}
void Sphere::draw() const
{
    geom->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    for (int i = 0; i < parts.count(); ++i)
        parts[i]->draw();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

Arrow::Arrow(QObject *parent, qreal length, qreal lengthToHead, qreal width, qreal headWidth, qreal depth)
    : QObject(parent)
    , geom(new Geometry())
{
    buildGeometry(length, lengthToHead, width, headWidth, depth);
}

Arrow::~Arrow()
{
    qDeleteAll(parts);
    delete geom;
}

void Arrow::setColor(QColor c)
{
    for (int i = 0; i < parts.count(); ++i)
        qSetColor(parts[i]->faceColor, c);
}
void Arrow::buildGeometry(qreal l, qreal lTH, qreal w, qreal hW, qreal d)
{
    RectPrism body(geom, w, lTH, d, false);
	body.rotate(90, QVector3D(-1,0,0));
	body.translate(QVector3D(0,lTH/2,0));


	RectPyramide head(geom, hW, l-lTH, d, 0);
	head.rotate(90, QVector3D(-1,0,0));
	head.translate(QVector3D(0,(lTH+(l-lTH)/2),0));
	

	parts << head.parts << body.parts;
    geom->finalize();
}
void Arrow::draw() const
{
    geom->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    for (int i = 0; i < parts.count(); ++i)
        parts[i]->draw();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}