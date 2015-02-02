#pragma once
#include "stdafx.h"

#ifdef USING_OPTITRACK
#include "OptiTrack.h"
#endif

using namespace Eigen;

class ReferenceTracker
{
public:
	ReferenceTracker(void);
	~ReferenceTracker(void);

	virtual void Update(float elapsedTime) = 0;
	virtual void Initialze() = 0;
	virtual void Start(bool withCapture) = 0;
	virtual void Stop() = 0;
	virtual void GetCameraData(Vector3f* position, AngleAxisf* rotation) = 0;
};

