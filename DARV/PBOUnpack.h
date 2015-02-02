#pragma once
#include "stdafx.h"
#include "GlInfo.h"
// testing Pixel Buffer Object for unpacking (uploading) pixel data to PBO
// using GL_ARB_pixel_buffer_object extension
// It uses 2 PBOs to optimize uploading pipeline; application to PBO, and PBO to
// texture object.


// function pointers for PBO Extension
// Windows needs to get function pointers from ICD OpenGL drivers,
// because opengl32.dll does not support extensions higher than v1.1.
//#ifdef _WIN32
//PFNGLGENBUFFERSARBPROC pglGenBuffersARB = 0;                     // VBO Name Generation Procedure
//PFNGLBINDBUFFERARBPROC pglBindBufferARB = 0;                     // VBO Bind Procedure
//PFNGLBUFFERDATAARBPROC pglBufferDataARB = 0;                     // VBO Data Loading Procedure
//PFNGLBUFFERSUBDATAARBPROC pglBufferSubDataARB = 0;               // VBO Sub Data Loading Procedure
//PFNGLDELETEBUFFERSARBPROC pglDeleteBuffersARB = 0;               // VBO Deletion Procedure
//PFNGLGETBUFFERPARAMETERIVARBPROC pglGetBufferParameterivARB = 0; // return various parameters of VBO
//PFNGLMAPBUFFERARBPROC pglMapBufferARB = 0;                       // map VBO procedure
//PFNGLUNMAPBUFFERARBPROC pglUnmapBufferARB = 0;                   // unmap VBO procedure
//#define glGenBuffersARB           pglGenBuffersARB
//#define glBindBufferARB           pglBindBufferARB
//#define glBufferDataARB           pglBufferDataARB
//#define glBufferSubDataARB        pglBufferSubDataARB
//#define glDeleteBuffersARB        pglDeleteBuffersARB
//#define glGetBufferParameterivARB pglGetBufferParameterivARB
//#define glMapBufferARB            pglMapBufferARB
//#define glUnmapBufferARB          pglUnmapBufferARB
//#endif

#define   CHANNEL_COUNT 3

class PBOUnpack
{
public:
	PBOUnpack(GLuint texture, int textureWidth, int textureHeight, GLenum format, GLenum type, int channels);
	~PBOUnpack(void);
	void UsePBO(GLuint textureID, char* data);
	

private:
	bool	_pboSupported;
	int		_pboMode;
	GLuint	_pboIds[2];                   // IDs of PBO
	char* imageData;
	int		_imageWidth;
	int		_imageHeight;
	GLenum	_format;
	GLenum	_type;
	int		_color;
	int		_index;
	int		_channels;
	void	UpdatePixel(char* dst,char* data, bool useData = true);
};

