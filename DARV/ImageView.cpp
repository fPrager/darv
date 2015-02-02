#include "stdafx.h"
#include "ImageView.h"


ImageView::ImageView(QWidget *parent)
	 : QGLWidget(QGLFormat(QGL::SampleBuffers),parent)
{
	_pbo = 0;
	_texture = -1;
	_ready = false;
}

ImageView::~ImageView(void)
{
}

void ImageView::SetSize(QRect size)
{
	this->setGeometry(size);
	_geometry = this->geometry();
	//_cameraImageWidth = size.width();
	//_cameraImageHeight = size.height();
	SetViewport();
}

void ImageView::initializeGL()
{
	makeCurrent();
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1,&_texture); 
	//glBindTexture(GL_TEXTURE_2D, _texture);
 //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
 //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//QString path = "Resources/Textures/painting2.bmp";
	//QByteArray ba = path.toLocal8Bit();
	//const char *c_str2 = ba.data();
	//_texture = SOIL_load_OGL_texture(
	//c_str2,
	//SOIL_LOAD_AUTO,
	//SOIL_CREATE_NEW_ID,
	//SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	//
	///* check for an error during the load process */
	//if( 0 == _texture )
	//{
	//	printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
	//}
	this->adjustSize();
}

void ImageView::resizeGL(int width, int height)
{
	makeCurrent();
	int side = qMin(width, height);
    glViewport(0,0,width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	#ifdef QT_OPENGL_ES
    glOrthof(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
	#else
    glOrtho(-1, 1, -1, 1, 1.0, 500.0);
	#endif
    glMatrixMode(GL_MODELVIEW);
}

void ImageView::SetViewport()
{
	int side = qMin(width(), height());
    glViewport(0,0,width(), height());

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	#ifdef QT_OPENGL_ES
    glOrthof(-1, 1, -1, 1, 1.0, 500.0);
	#else
    glOrtho(-1, 1,-1, 1, 1.0, 500.0);
	#endif
    glMatrixMode(GL_MODELVIEW);
}

void ImageView::MakeTexture(int width, int height, int format, int type, int channels)
{
	this->context()->makeCurrent();
	_texWidth = width;
	_texHeight = height;
	_pbo = new PBOUnpack(_texture, width,height,format,type,channels);
	//glEnable(GL_TEXTURE);
	//glGenTextures(1,&_texture); 

}

void ImageView::RemoveTexture()
{
	makeCurrent();
	_pbo = 0;
	//glDeleteTextures(1, &_texture);
	_texture = -1;
}

void ImageView::Draw()
{
	_ready = true;
	paintGL();
	update();
	_ready = false;
}

void ImageView::UpdateTexture(char* data)
{
	if(_texture == -1)
		return;
	this->context()->makeCurrent();
	//glBindTexture(GL_TEXTURE_2D,_texture);
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,_texWidth, _texHeight,GL_BGR, GL_UNSIGNED_BYTE, data);
	if(_pbo != 0)
		_pbo->UsePBO(_texture,data);
}

void ImageView::paintGL()
{
	if(!_ready)
		return;
	makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	qglClearColor(QColor::fromRgb(74, 74, 165));
 
    qreal pixelRatio = devicePixelRatio();
    SetViewport();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();  
	glTranslatef(0.0f,0.0f,-5.0f);
	glBindTexture(GL_TEXTURE_2D, _texture);
	//glColor3f(0.0f, 0.0f, 1.0f); // Set the colour of the square to blue
	glBegin(GL_QUADS); // Start drawing a quad primitive
	glTexCoord2f(0, 1);
	glVertex3f(-1, -1, -2.0f); // The bottom left corner
	glTexCoord2f(0, 0);
	glVertex3f(-1, 1, -2.0f); // The top left corner
	glTexCoord2f(1, 0);
	glVertex3f(1, 1, -2.0f); // The top right corner
	glTexCoord2f(1, 1);
	glVertex3f(1, -1, -2.0f); // The bottom right corner
	glEnd();
}

