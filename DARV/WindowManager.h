#pragma once
#include "stdafx.h"
#include <QtWidgets>

#define	 MONITOR1_POSX  0
#define	 MONITOR1_POSY  0
#define	 MONITOR1_WIDTH  1920
#define	 MONITOR1_HEIGHT  1080

#define	 MONITOR2_POSX  0
#define	 MONITOR2_POSY -1050
#define	 MONITOR2_WIDTH 1680
#define	 MONITOR2_HEIGHT  1050

class WindowManager
{
private:

public:
	WindowManager();
	~WindowManager();
	void	Initialize();
	int		NumberOfMonitors();
	QRect	GetSizeOfMonitor(int nr);
	QRect	GetAvailableSizeOfMonitor(int nr);
	BOOL	MakeConsoleWindow();
};

