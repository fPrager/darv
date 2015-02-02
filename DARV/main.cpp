#include "stdafx.h"
#include "Darv.h"
#include <QtWidgets/QApplication>
#include "WindowManager.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	WindowManager wm;
	//wm.MakeConsoleWindow();
	wm.Initialize();

	DARV w;
	w.Initialize();
	w.show();
	w.AdjustWindow(&wm);
	return a.exec();
}
