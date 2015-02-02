#pragma once
#ifndef DARV_H
#define DARV_H

#include "stdafx.h"
#include "ui_Darv.h"
#include "Util.h"

//other windows-classes
#include "ARViewWindow.h"
#include "TrackingManager.h"
#include "CameraView.h"
#include "RotationViewer.h"
#include "UncertaintyCOS.h"
#include "VirtualReality.h"
#include "ValueTree.h"
#include "ReferenceTracker.h"
#include "ImageAnalyser.h"
#include "CameraImageLoader.h"

using namespace Eigen;
#define FPS_COUNT 100

class DARV : public QMainWindow
{
	Q_OBJECT

public:
	DARV(QWidget *parent = 0);
	~DARV();
	void AdjustWindow(WindowManager *wm);

public slots:
	void CheckAsNumberParameters(QString input);
	void ToggleStereo(int flag);
	void ToggleTracking(int flag);
	void ToggleDARV(int flag);
	void ToggleTime();
	void StepForward();
	void ToggleMarkerClassification(int flag);
	void ToggleFileWriting();
	void Update();
	void Initialize();
	void ChangeObject(QListWidgetItem * item);
	void ToogleRecordUsing(int flag);
	void LoadRecord(QListWidgetItem* item);
	void ToggleReferenceData(int flag);
	void ResetReferenceTracker();
	void DoReferenceTracking();
	void AddEyeDiff();
	void ResetRotation();
	void ChangeInterpolationValue(int);
	void ChangeIMUTracker();
	void DoBlurAnalysing();
	void LoadRecords();

private:
	Ui::DARVClass _ui;
	ARViewWindow _arView;
	bool _showARView;
	WindowManager *_wm;
	TrackingManager _tManager;
	ReferenceTracker* _refTracker;
	CameraImageLoader* _camLoader;
	CameraView* _camView;
	ImageView*	_imageView;
	void		ShowImageViewer(bool flag);
	bool		_showImageViewer;

	CameraTrackerFusingIMU* _camTracker;
	RotationViewer* _rotationView;
	QList<UncertaintyCOS*> _uncerCos;

	BlurAnalyser*  _blurAnalyser;

	void closeEvent (QCloseEvent *event);
	void UpdateARViewParameters();
	void ShowARViewParameters();
	void ShowLists();
	void ChangeParameter(QString name, double value, bool counting);
	void keyPressEvent(QKeyEvent* event);

	void SetMarkerInfo(QList<VirtualMarker*> vMarker);
	void UpdateMarkerInfo();

	void EnableIMUUsing(bool enable);
	void UpdateCameraDataView(float elapsedTime);

	QTimer* _clock;

	QList<VirtualMarker*> _vMarker;
	ValueTree* _timeValues;
	QStringList _timeIDs;
	QList<float*> _timeStamps;
	void AddTimeStamp(QString id);
	void WriteTimePercentage();

	ValueTree* _markerValues;
	ValueTree* _camValues;
	QString	   _camHeader;
	QString	   _markerHeader;
	QString	   _imuHeader;
	QString	   _refHeader;

	QElapsedTimer _timer;
	float _secondsCounter;
	float _elapsedTimeTracking;
	float _fps;
	float _fpsWithMarkerUpdate[FPS_COUNT];
	bool _newMarkerData;
	bool _ignoreTextChange;
	bool _usingIMU;
	bool _forceQuality;

	bool _useReferenceData;

	bool	_fileWriting;
	bool	_captureLeftImage;
	bool	_captureRightImage;
	float	_dataTime;

	bool _setTime;
	int _settedTime;

	float	_camTime;
	void	DoFileWriting(float elapsedTime, bool lastCall = false);

	float Round(float value, float decPlace = 2)
	{
		float factor = pow(10.f,decPlace);
		return ((int)(value*factor))/factor;
	}

	QString RoundST(float value, float decPlace = 2)
	{
		return QString::number(Round(value, decPlace));
	}
};

#endif // DARV_H
