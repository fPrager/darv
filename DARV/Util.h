#pragma once

#include "stdafx.h"
#include <sstream>
using namespace std;
struct LoadedRessource
{
public:
	QString _name;
	GLuint _id;
	LoadedRessource(QString name, GLuint id)
	{
		_name = name;
		_id = id;
	}
};

class Util
{
public:
	Util(void);
	~Util(void);

	static QString ToQString(GLdouble number);
	static string ToStdString(QString text);
	static string ToStdString(int nr);
	static void MakeMeATexturedBox(GLuint* vertexbuffer, GLuint* uvbuffer,float width = 1, float height = 1, float depth = 1);
	static void MakeMeATexturedPlane(GLuint* vertexbuffer, GLuint* uvbuffer,float width = 1, float height = 1);

	static float GetCameraFOVY(bool inRadian = false);
	
	static int GetCameraResX();
	static int GetCameraresY();
	static float GetCameraAspect();
	static void SetCameraValues(float fovy, int resX, int resY);
	static void SetCameraResolution(int resX, int resY);
	static void SetCameraFOV(float fovy);
};

