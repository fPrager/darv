// stdafx.h : Includedatei für Standardsystem-Includedateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.
//

#pragma once
#define WIN32_LEAN_AND_MEAN             // Selten verwendete Teile der Windows-Header nicht einbinden.
// Windows-Headerdateien:
#include <windows.h>

// C RunTime-Headerdateien
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//additional includes
#include <iostream>
#include <list>
//#include <math.h>

//openGL stuff
#include <GL/glew.h>
#include <gl\GL.h>
//#if defined(__APPLGLSU__) && !defined (VMDMESA) 
//#include <OpenGL/glu.h>
//#else
//#include <GL/glu.h>
//#endif
#include <glut.h>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//eigen math-library
#include <Eigen\Core>
#include <Eigen\Eigen>
//#define M_PI 3.14159265359

//window management
#include <QtWidgets/QMainWindow>
#include <QtCore/QElapsedTimer>
#include <QDebug>

#include <FileWriter.h>
#define HOMEWORKING true
#define BACKGROUND QColor::fromRgb(50, 50, 50)