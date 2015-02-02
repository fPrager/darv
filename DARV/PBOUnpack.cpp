#include "stdafx.h"
#include "PBOUnpack.h"


PBOUnpack::PBOUnpack(GLuint texture, int textureWidth, int textureHeight, GLenum format, GLenum type, int channels)
{
	//make sure that we are using a correct OpenGL-Context at this point
	//otherwise it will get no information about the drivers!!!!

	_imageWidth = textureWidth;
	_imageHeight = textureHeight;
	_format = format;
	_type = type;
	_channels = channels;
	_pboMode = 0;
	 // get OpenGL info
    GlInfo glInfo;
    glInfo.getInfo();
    //glInfo.printSelf();
	// check PBO is supported by your video card
    if(glInfo.isExtensionSupported("GL_ARB_pixel_buffer_object") && false)
    {
        // get pointers to GL functions
        glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
        glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
        glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
        glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
        glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
        glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
        glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
        glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");

        // check once again PBO extension
        if(glGenBuffersARB && glBindBufferARB && glBufferDataARB && glBufferSubDataARB &&
           glMapBufferARB && glUnmapBufferARB && glDeleteBuffersARB && glGetBufferParameterivARB)
        {
            _pboSupported = true;
            _pboMode = 2;    // using 1 PBO
            qDebug() << "Video card supports GL_ARB_pixel_buffer_object.";
        }
        else
        {
            _pboSupported = false;
            _pboMode = 0;    // without PBO
            qDebug() << "Video card does NOT support GL_ARB_pixel_buffer_object.";
        }
    }
	int dataSize = _imageWidth * _imageHeight * _channels;

	if(_pboSupported)
    {
        // create 2 pixel buffer objects, you need to delete them when program exits.
        // glBufferDataARB with NULL pointer reserves only memory space.
		glGenBuffersARB(2, _pboIds);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pboIds[0]);
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, dataSize, 0, GL_STREAM_DRAW_ARB);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pboIds[1]);
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, dataSize, 0, GL_STREAM_DRAW_ARB);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    }

	imageData = new char[dataSize];

	glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _imageWidth, _imageHeight, 0, _format, _type, imageData);

	glBindTexture(GL_TEXTURE_2D, 0);

	_color = 0;
	_index = 0;
}

void PBOUnpack::UpdatePixel(char* dst,char* data, bool useData)
{
    if(!dst)
        return;

    GLubyte* ptr = (GLubyte*)dst;
	GLubyte* ptr_d = (GLubyte*)data;

    // copy 4 bytes at once
    for(int i = 0; i < _imageHeight; ++i)
    {
        for(int j = 0; j < _imageWidth; ++j)
        {
			if(useData)
			{
				for(int b = 0; b<_channels; b++)
				{
					//if(*ptr_d > 100)
					//	*ptr = 255;
					//else
					//	*ptr = 0;
					*ptr = *ptr_d;
					++ptr;
					++ptr_d;
				}
			}
			else
			{
				*ptr = _color;
				++ptr;
			}
        }
		//if(!useData)
			//_color += 257;   // add an arbitary number (no meaning)
    }
	//if(!useData)
	++_color;            // scroll down		
}

void PBOUnpack::UsePBO(GLuint textureID, char* data)
{
	if(_pboMode > 0)
    {
		// "index" is used to copy pixels from a PBO to a texture object
		static int index = 0;

		// "nextIndex" is used to update pixels in a PBO
		int nextIndex  = 0;


        // "nextIndex" is used to update pixels in a PBO
        if(_pboMode == 1)
        {
            // In single PBO mode, the index and nextIndex are set to 0
            index = nextIndex = 0;
        }
		else if(_pboMode == 2)
        {
            // In dual PBO mode, increment current index first then get the next index
            index = (index + 1) % 2;
            nextIndex = (index + 1) % 2;
        }

        // bind the texture and PBO
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pboIds[index]);

        // copy pixels from PBO to texture object
        // Use offset instead of pointer.
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _imageWidth, _imageHeight, _format, _type, 0);

        // bind PBO to update pixel values
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pboIds[nextIndex]);

        // map the buffer object into client's memory
        // Note that glMapBufferARB() causes sync issue.
        // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
        // for GPU to finish its job. To avoid waiting (stall), you can call
        // first glBufferDataARB() with NULL pointer before glMapBufferARB().
        // If you do that, the previous data in PBO will be discarded and
        // glMapBufferARB() returns a new allocated pointer immediately
        // even if GPU is still working with the previous data.
		int dataSize = _imageWidth * _imageHeight * _channels;

        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, dataSize, 0, GL_STREAM_DRAW_ARB);
        char* ptr = (char*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
        if(ptr)
        {
            // update data directly on the mapped buffer
            UpdatePixel(ptr,data);
            glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
        }

        // it is good idea to release PBOs with ID 0 after use.
        // Once bound with 0, all pixel operations behave normal ways.
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
		//gluBuild2DMipmaps(GL_TEXTURE_2D,3,_imageWidth,_imageHeight,_format,_type,(GLvoid*)imageData);
		//glTexImage2D(GL_TEXTURE_2D,0,format,_imageWidth,_imageHeight,0,	format,	GL_UNSIGNED_BYTE,0);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _imageWidth, _imageHeight, _format, _type, imageData);
        UpdatePixel(imageData,data);
    }
}


PBOUnpack::~PBOUnpack(void)
{
	// clean up PBOs
    if(_pboSupported)
    {
        glDeleteBuffersARB(2, _pboIds);
    }
}
