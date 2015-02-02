#include "stdafx.h"
#include "Util.h"

//a list of global constants of the program

	//camera depanded
	float _fovy = 37;		//in degrees
	int _resX = 480;		//in pixel
	int _resY = 640;		//in pixel
	
Util::Util(void)
{
}


Util::~Util(void)
{
}

QString Util::ToQString(GLdouble number)
{
	ostringstream strs;
	strs << number;
	string str = strs.str();
	return QString::fromStdString(str);
}

string Util::ToStdString(QString text)
{
	return text.toStdString();
}

string Util::ToStdString(int nr)
{
	string s;
	stringstream out;
	out << nr;
	return(out.str());
}
void Util::MakeMeATexturedPlane(GLuint* vertexbuffer, GLuint* uvbuffer, float width, float height)
{
	static GLuint _planeVertexBuffer = 0;
	static GLuint _planeUVBuffer = 0;

	if(_planeVertexBuffer != 0)
	{
		*vertexbuffer = _planeVertexBuffer;
		*uvbuffer = _planeUVBuffer;
		return;
	}

	float halfHeight = height/2.0f;
	float halfWidth = width/2.0f;
	GLfloat g_vertex_buffer_data[] = { 
		-halfWidth, halfHeight, 0,	 //<---- under left
		-halfWidth,-halfHeight, 0,
		 halfWidth,-halfHeight, 0,

		 halfWidth, halfHeight, 0,	 //<---- upper right
		-halfWidth, halfHeight, 0,
		 halfWidth,-halfHeight, 0
	};

	// Two UV coordinatesfor each vertex. They were created withe Blender.
	GLfloat g_uv_buffer_data[] = { 
		
		//left side
		0, 1,    //<--- under right
		0, 0,
		1, 0,

		//back side
		1, 1,		//<---- upper right
		0, 1, 
		1, 0,
	};

	glGenBuffers(1, &_planeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,_planeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


	glGenBuffers(1, &_planeUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _planeUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	*vertexbuffer = _planeVertexBuffer;
	*uvbuffer = _planeUVBuffer;
}

void Util::MakeMeATexturedBox(GLuint* vertexbuffer, GLuint* uvbuffer, float width, float height, float depth)
{
	static GLuint _boxVertexBuffer = 0;
	static GLuint _boxUVBuffer = 0;

	if(_boxVertexBuffer != 0)
	{
		*vertexbuffer = _boxVertexBuffer;
		*uvbuffer = _boxUVBuffer;
		return;
	}

	float halfHeight = height/2.0f;
	float halfWidth = width/2.0f;
	float halfdepth = depth/2.0f;
	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	GLfloat g_vertex_buffer_data[] = { 
		
		//left side
		-halfWidth,-halfHeight, -halfdepth,    //<--- under right
		-halfWidth,-halfHeight, halfdepth,
		-halfWidth, halfHeight, halfdepth,

		//back side
		 halfWidth, halfHeight,-halfdepth,	 //<---- upper right
		-halfWidth,-halfHeight,-halfdepth,
		-halfWidth, halfHeight,-halfdepth,

		 //downer side
		 -halfWidth,-halfHeight, halfdepth,	 //<---- upper right
		 halfWidth,-halfHeight,halfdepth,
		 halfWidth,-halfHeight,-halfdepth,

		 //back side
		 halfWidth, halfHeight,-halfdepth,	 //<---- under left
		 halfWidth,-halfHeight,-halfdepth,
		-halfWidth,-halfHeight,-halfdepth,

		//left side
		-halfWidth, halfHeight,-halfdepth,	 //<---- upper left
		-halfWidth, -halfHeight, -halfdepth,
		-halfWidth, halfHeight, halfdepth,

		 //downer side
		 -halfWidth,-halfHeight, -halfdepth,	 //<---- downer left
		 halfWidth,-halfHeight, -halfdepth,
		-halfWidth,-halfHeight,halfdepth,

		 //front side
		-halfWidth, halfHeight, halfdepth,	 //<---- under left
		-halfWidth,-halfHeight, halfdepth,
		 halfWidth,-halfHeight, halfdepth,

		 //right side
		 halfWidth, halfHeight, halfdepth,	 //<--- upper right
		 halfWidth,-halfHeight,-halfdepth,
		 halfWidth, halfHeight,-halfdepth,

		 //right side
		 halfWidth,-halfHeight,-halfdepth,	 //<---- under left
		 halfWidth, halfHeight, halfdepth,
		 halfWidth,-halfHeight, halfdepth,

		 //upper side
		 halfWidth, halfHeight, halfdepth,	 //<---- upper right
		 halfWidth, halfHeight, -halfdepth,
		-halfWidth, halfHeight, -halfdepth,

		 //upper side
		 halfWidth, halfHeight, halfdepth,	 //<---- under left
		-halfWidth, halfHeight, -halfdepth,
		-halfWidth, halfHeight, halfdepth,

		 //front side
		 halfWidth, halfHeight, halfdepth,	 //<---- upper right
		-halfWidth, halfHeight, halfdepth,
		 halfWidth,-halfHeight, halfdepth,
	};

	// Two UV coordinatesfor each vertex. They were created withe Blender.
	GLfloat g_uv_buffer_data[] = { 
		
		//left side
		0, 0.5f,    //<--- under right
		0.33f, 0.5f,
		0.33f,1,

		 //back side
		 0.66f, 1,		//<---- upper right
		 1, 0.5f, 
		 1, 1,

		 //downer side
		 0.33f, 0.5f,	 //<---- upper right
		 0.66f, 0.5f,
		 0.66f, 0,
		
		 //back side
		 0.66f,1,		//<---- under left
		 0.66f, 0.5f,
		 1, 0.5f,

		//left side
		0,1,			//<---- upper left
		0,0.5f,
		0.33f, 1,

		 //downer side
		 0.33f, 0,		//<---- downer left
		 0.66f, 0,
		 0.33f, 0.5f,
	 
		 //front side
		 0, 0.5f,		//<---- under left
		 0, 0,
		 0.33f, 0,

		 //right side
		 0.33f, 1,		//<--- upper right
		 0.66f, 0.5f,
		 0.66f, 1,

		 //right side
		 0.66f, 0.5f,	 //<---- under left
		 0.33f, 1,
		 0.33f, 0.5f,
		 
		 //upper side
		 1,0,			//<---- upper right
		 1, 0.5f,
		 0.66f, 0.5f,
		 
		 //upper side
		 1,0,			//<---- under left
		 0.66f, 0.5f,
		 0.66f, 0,

		 //front side
		 0.33f, 0.5f,	 //<---- upper right
		 0, 0.5f,
		 0.33f, 0

	};

	glGenBuffers(1, &_boxVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,_boxVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


	glGenBuffers(1, &_boxUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _boxUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
}

float Util::GetCameraFOVY(bool inRadian)
{
			if(inRadian)
				return _fovy * (M_PI/180.0f);
			else
				return _fovy;
}

int Util::GetCameraResX(){return _resX;}
int Util::GetCameraresY(){return _resY;}
float Util::GetCameraAspect(){return (float)_resX/(float)_resY;}

void Util::SetCameraValues(float fovy, int resX, int resY)
{
	_fovy = fovy;
	_resX = resX;
	_resY = resY;
}

void Util::SetCameraResolution(int resX, int resY)
{
	_resX = resX;
	_resY = resY;
}

void Util::SetCameraFOV(float fovy)
{
	_fovy = fovy;
}