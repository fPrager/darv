#include "stdafx.h"
#include "CameraTracker.h"

CameraTrackerNoIMU::CameraTrackerNoIMU(MarkerUsing mUsing, Vector3f initPosition, AngleAxisf initRotation):CameraTracker(mUsing, initPosition, initRotation)
{
}

CameraTrackerNoIMU::~CameraTrackerNoIMU()
{

}

void CameraTrackerNoIMU::CalculateCameraData(QList<VirtualMarker*> marker)
{
	Vector3f positionFromMarker = Vector3f();
	Quaternionf rotationFromMarker = Quaternionf(AngleAxisf(0, Vector3f(0,0,0)));
	float mQualityRotation = 0;
	float mQualityPosition = 0;

	GetRotationFromMarker(marker, _mUsing, &mQualityRotation, &rotationFromMarker);
	GetPositionFromMarker(marker, _mUsing, &rotationFromMarker, &mQualityPosition, &positionFromMarker);

	UseFilter();

	_quality = min(mQualityRotation,mQualityPosition);
	//float value = 0.01;
	//if(_quality > 1)
	//	_quality = 0;
	//_quality+=value;

	_position = new Vector3f(positionFromMarker);
	_rotation = new Quaternionf(rotationFromMarker);
}

