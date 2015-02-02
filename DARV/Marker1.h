#pragma once
#include <stdafx.h>
#include <SOIL.h>

#define MARKER_WIDTH 17
#define MARKER_HEIGHT 17

class Marker1
{
public:
	Marker1(void);
	~Marker1(void);
public:
	void Draw();
	void Load();

private:
	GLuint _markerTexture;
	void LoadBMP(const char * imagepath, GLuint * texture);
};

