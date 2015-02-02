#pragma once
#include <stdafx.h>
#include <VirtualMarker.h>
#include <Marker.h>
#include <Uncertainty.h>


enum MarkerUsing {BEST_MARKER, AVERAGE_MARKER, FUSING_MARKER};

class CameraTracker
{
public:
	// EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	void Update(QList<Marker> realMarker, QList<VirtualMarker*> fixedMarker, QList<VirtualMarker*> freeMarker, float elapsedTime, bool useReference = false);
	void GetCameraData(float* quality, Vector3f* position, AngleAxisf* rotation);
	Vector3f* GetLatestPosition();
	AngleAxisf* GetLatestRotationFromMarker();
	void GetYawPitchRollFromMarker(float* yaw, float* pitch, float* roll);
	float GetAccelFromMarker();

	AngleAxisf* GetLatestRotation();
	void GetYawPitchRoll(float* yaw, float* pitch, float* roll);
	float GetAccel();
	float GetQuality();
	Vector3f*  GetTranslation();
	void SetQuality(float value);
	void UseThisCameraData(Vector3f* position, Quaternionf* rotation);
	void ToggleClassification(bool flag);
	Uncertainty* GetCamUncertainty(){return _camUnc;} 

protected:
	CameraTracker(MarkerUsing mUsing = FUSING_MARKER, Vector3f initPosition = Vector3f(0,0,0), AngleAxisf initRotation = AngleAxisf(0,Vector3f(0,0,0)));
	~CameraTracker();

			void		UpdateFreeMarker(QList<VirtualMarker*> marker);
	virtual void		CalculateCameraData(QList<VirtualMarker*> marker) = 0;
	virtual float		ExpectedRotationAngle() = 0;
	
	//void		GetDataFromMarker(QList<VirtualMarker*> marker, float* mQuality, Vector3f* camPos, AngleAxisf* camRot) = 0;
			void		UseFilter();
			void		GetRotationFromMarker(QList<VirtualMarker*> marker, MarkerUsing mUsing, float* rotationQuality, Quaternionf* camRot);
			void		GetPositionFromMarker(QList<VirtualMarker*> marker, MarkerUsing mUsing, Quaternionf* camRot, float* positionQuality, Vector3f* camPos);
			void		GetMarkerQualityInfo(QList<VirtualMarker*> marker, float* bestQuality, int* bestQualityIndex, float* qualitySum);
			void		VerifyMarkerRotations(QList<Quaternionf> rotations, QList<float*> qualitys, QList<float*> weights);
			void		VerifyMarkerData(QList<VirtualMarker*> marker);
			void		SetStabilityFromAcceleration(QList<VirtualMarker*> marker);
			void		SetStabilityFromMoving(QList<VirtualMarker*> marker);
			int			IndexOfMostTrustedMarkerRotation(QList<Quaternionf> rotations);
			void		CalculateExpectedMarkerMovement(float camMoveAngle, float markerDistance);
	float			_quality;
	float			_overallQuality;
	float			_elapsedTime;
	Vector3f*		_position;
	Vector3f*		_translation;
	Vector3f*		_prevPosition;
	Quaternionf*	_rotation;
	bool			_doClassification;

	Uncertainty*	_camUnc;

	MarkerUsing		_mUsing;

	bool			_justFlexibleMarker;
	Quaternionf*	_prevRotation;
	
	Quaternionf*	_prevRotationFromMarker;
	Quaternionf*	_latestRotationFromMarker;
	Quaternionf*	_diffRotationFromMarker;		
	float			_expectedAccelMagn;
};

class CameraTrackerNoIMU: public CameraTracker
{
	public:
		 //EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		CameraTrackerNoIMU(MarkerUsing mUsing = AVERAGE_MARKER, Vector3f initPosition = Vector3f(0,0,0), AngleAxisf initRotation = AngleAxisf(0,Vector3f(0,0,0)));
		~CameraTrackerNoIMU();

		float ExpectedRotationAngle(){return 0;}	//TODO: find a way to estimate the camerarotation without imu-data

	private:
		void		CalculateCameraData(QList<VirtualMarker*> marker);
		//void		GetDataFromMarker(QList<VirtualMarker*> marker, float* mQuality, Vector3f* camPos, AngleAxisf* camRot);
};

class CameraTrackerFusingIMU: public CameraTracker
{
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		CameraTrackerFusingIMU(AngleAxisf initRotation = AngleAxisf(0,Vector3f(0,0,0)), float imuInterPol = 0.f);
		~CameraTrackerFusingIMU();
		
		void UpdateIMURotationValue(float yaw, float pitch, float roll);
		void UpdateIMURotationValue(Quaternionf* rotation);
		void UpdateIMURotationAngle(float value){_expectedRotationAngle = value;}
		float ExpectedRotationAngle(){return _expectedRotationAngle;}
		void QuickIMUCheck(float instantTransMag = 0);
		void UpdateIMUTranslationValue(float value);

		void ResetRotation();
		float GetIMUInterpolation();
		float GetAccuDiff();
		float GetIMUTranslationMagnitude();
		float GetInstantIMUTranslationMagnitude();

		void SetIMUInterpolation(float value);
		AngleAxisf* GetLatestRotationFromIMU();
		void GetYawPitchRollFromIMU(float* yaw, float* pitch, float* roll);
		float GetAccelFromIMU();

	private:
		void		CalculateCameraData(QList<VirtualMarker*> marker);
		//void		GetDataFromMarker(QList<VirtualMarker*> marker, float* mQuality, Vector3f* camPos, AngleAxisf* camRot);
		void		CalculateDiffRotations();
		void		SetInitialRotation();
		void		UseIMUData(Quaternionf* camRot);
		float		_imuInterPol;
		float		_accuDiffIMU;
		float		_accuDiffM;
		Quaternionf*	_latestRotationFromIMU;

		Quaternionf*	_prevRotationFromIMU;

		Quaternionf* _diffRotationFromIMU;

		bool		_useIMURotation;

		AngleAxisf*		_initialRotation;
		Quaternionf*	_toInitialRotation;
		Vector3f*		_latestIMUData;
		float			_imuTransMag;
		float			_instantTransMagn;
		float			_expectedRotationAngle;
};

