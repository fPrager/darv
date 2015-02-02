#pragma once
#include "stdafx.h"
#include <QGLWidget>
#include <QtWidgets>
#include <SOIL.h>
#include "PBOUnpack.h"

class ImageView: public QGLWidget
{
public:
	ImageView(QWidget *parent = 0);
	~ImageView(void);
	void Draw();
	void SetSize(QRect size);
	void MakeTexture(int width, int height, int format, int type, int channels);
	void RemoveTexture();
	void UpdateTexture(char* data);
	void SetViewport(int width, int height);
	bool HasTexture(){return _pbo != 0;};

protected:
    void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

private:
	void		SetViewport();
	bool		_ready;
	QRect		_geometry;
	GLuint		_texture;
	int			_texWidth;
	int			_texHeight;
	PBOUnpack*	_pbo;
};

