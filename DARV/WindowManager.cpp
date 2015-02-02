#include "stdafx.h"
#include "WindowManager.h"
using namespace std;

//BOOL CALLBACK EnumDispProc(HMONITOR hMon, HDC dcMon, RECT* pRcMon, LPARAM lParam);
list<QRect> 	_monitorsSizes;
list<QRect> 	_monitorsASizes;

WindowManager::WindowManager(void)
{
}


WindowManager::~WindowManager(void)
{
}

void WindowManager::Initialize()
{
	//EnumDisplayMonitors(0, 0, EnumDispProc, 0);
	list<QRect>::iterator itSizes;
	itSizes = _monitorsSizes.end();

	list<QRect>::iterator itASizes;
	itASizes = _monitorsASizes.end();
	foreach (QScreen *screen, QGuiApplication::screens()) {
		qDebug() << "Information for screen:" << screen->name();
        qDebug() << "  Available geometry:" << screen->availableGeometry().x() << screen->availableGeometry().y() << screen->availableGeometry().width() << "x" << screen->availableGeometry().height();
        qDebug() << "  Available size:" << screen->availableSize().width() << "x" << screen->availableSize().height();
        qDebug() << "  Available virtual geometry:" << screen->availableVirtualGeometry().x() << screen->availableVirtualGeometry().y() << screen->availableVirtualGeometry().width() << "x" << screen->availableVirtualGeometry().height();
        qDebug() << "  Available virtual size:" << screen->availableVirtualSize().width() << "x" << screen->availableVirtualSize().height();
		_monitorsSizes.insert(itSizes,QRect(screen->availableGeometry().x(), screen->availableGeometry().y(), screen->availableGeometry().width(), screen->availableGeometry().height()));
		_monitorsASizes.insert(itASizes,QRect(screen->geometry().x(), screen->geometry().y(), screen->geometry().width(), screen->geometry().height()));
	}

	//_monitors.insert(it,QRect(MONITOR1_POSX, MONITOR1_POSY, MONITOR1_WIDTH, MONITOR1_HEIGHT));

	if(QApplication::desktop()->screenCount() > 1)
	{
	//	_monitors.insert(it,QRect(MONITOR2_POSX, MONITOR2_POSY, MONITOR2_WIDTH, MONITOR2_HEIGHT));
	}


	//TODO: BUGFIX, QT 5.1.0< calculates wrong screen resolutions
	for(int i = 0; i<QApplication::desktop()->screenCount();i++)
	{
		//QRect screenres = QApplication::desktop()->screenGeometry(i);
		//_monitors.insert(it,screenres);
	}
}

//// callback function called by EnumDisplayMonitors for each enabled monitor
//BOOL CALLBACK EnumDispProc(HMONITOR hMon, HDC dcMon, RECT* pRcMon, LPARAM lParam)
//{
//	//printf("Monitor: %d x %d @ %d,%d\n", pRcMon->right - pRcMon->left, pRcMon->bottom - pRcMon->top, pRcMon->left, pRcMon->top);
//	
//	list<QRect>::iterator it;
//	it = _monitors.end();
//	_monitors.insert(it,*pRcMon);
//	return TRUE;
//}

int WindowManager::NumberOfMonitors()
{
	return _monitorsSizes.size();
}

QRect WindowManager::GetSizeOfMonitor(int nr)
{
	int i = 0;
	list<QRect>::iterator it;
	for (it=_monitorsSizes.begin(); it != _monitorsSizes.end(); ++it)
	{
		if(i==nr)
			return *it;
		i++;
	}
	printf("Error: Requested Size of monitor %i. (just %i monitors)\n", (1+nr), _monitorsSizes.size());
	return *it;
}

QRect WindowManager::GetAvailableSizeOfMonitor(int nr)
{
	int i = 0;
	list<QRect>::iterator it;
	for (it=_monitorsASizes.begin(); it != _monitorsASizes.end(); ++it)
	{
		if(i==nr)
			return *it;
		i++;
	}
	printf("Error: Requested Size of monitor %i. (just %i monitors)\n", (1+nr), _monitorsASizes.size());
	return *it;
}

BOOL WindowManager::MakeConsoleWindow()
{
	if(!AllocConsole())
		return FALSE;
	FILE *stream;
	freopen_s(&stream, "CONIN$", "rb", stdin);
	freopen_s(&stream, "CONOUT$", "wb", stdout);
	freopen_s(&stream, "CONOUT$", "wb", stderr);
	printf("Successfully set console\n");
	return TRUE;
}