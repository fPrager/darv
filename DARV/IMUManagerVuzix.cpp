#include "stdafx.h"
#include "IMUManager.h"
#include <iWearSDK.h>

#define IWR_RAWTODEG				180.0f / 32768.0f

#define MAX_FILTER_SIZE				16
#define DEFAULT_FILTER_SIZE			8
#define DEFAULT_FILTER_ZONE			3.0f
#define MAX_FILTER_ZONE				8.0f
#define MIN_FILTER_ZONE				0.25f

float				Filtered_yaw[ MAX_FILTER_SIZE ];
float				Filtered_pitch[ MAX_FILTER_SIZE ];
float				Filtered_roll[ MAX_FILTER_SIZE ];
int					Max_Filter_entries= DEFAULT_FILTER_SIZE; 
int					g_Filter_index= 0;
int					Filter_Restart_Count= 1;
// The below settings; could be made adjustable via user input?
	long				g_FilterSize= DEFAULT_FILTER_SIZE;
	float				g_FilterZone= DEFAULT_FILTER_ZONE;

IMUManagerVuzix::IMUManagerVuzix():IMUManager()
{
	_referenceYaw = 0;
	_referencePitch = 0;
	_referenceRoll = 0;
	_name = "VUZIX's IMU-Manager";
}


IMUManagerVuzix::~IMUManagerVuzix()
{
	IWRFreeDll();
}

bool IMUManagerVuzix::Initialize()
{
	// Open the VR920's tracker driver.
	_iwrStatus = IWRLoadDll();
	long	Roll=0,Yaw=0, Pitch=0;
	_iwrStatus = IWRGetTracking( &Yaw, &Pitch, &Roll );
	if( _iwrStatus != IWR_OK ) {
		qDebug() << "NO VR920 iwrdriver support";
		qDebug() << " - VR920 Driver ERROR";
		IWRFreeDll();
		_isUsable = false;
		return false;
	}
	 IWRSetMagAutoCorrect(0); 
	_isUsable = true;
	_referenceRotation = new Quaternionf(1,0,0,0);
	return true;
}

float IMUManagerVuzix::TranslationMagnitude(int elapsedTime)
{
	long	Roll=0,Yaw=0, Pitch=0, X=0, Y=0, Z=0;
	IWRGet6DTracking(&Yaw, &Pitch, &Roll, &X, &Y, &Z);
	Vector3f transl = Vector3f(X,Y,Z);
	float mag = transl.norm();
	if(mag < 0 ) mag = 0;
	return 0;
}

void IMUManagerVuzix::TranslationMagnitude(float* x, float *y, float* z)
{
	long	Roll=0,Yaw=0, Pitch=0, X=0, Y=0, Z=0;
	IWRGet6DTracking(&Yaw, &Pitch, &Roll, &X, &Y, &Z);
	*x = X,
	*y = Y;
	*z = Z;
}

bool IMUManagerVuzix::Update(bool useFilter, float elapsedTime)
{
	bool success = UpdateTracking(useFilter);
	if(success)
		UpdateRotationSpeed(elapsedTime);
	return success;
}

bool IMUManagerVuzix::UpdateTracking(bool useFilter){
	// Poll input devices.
	long	Roll=0,Yaw=0, Pitch=0;
	IWRSetMagAutoCorrect(0); 
	_iwrStatus = IWRGetTracking( &Yaw, &Pitch, &Roll );
	if(	_iwrStatus != IWR_OK ){
		// iWear tracker could be OFFLine: just inform user, continue to poll until its plugged in...
		qDebug() << "IMU is offline";
		_isUsable = false;
		Yaw = Pitch = Roll = 0;
		IWROpenTracker();
		return false;
	}
	// Always provide for a means to disable filtering;
	if(useFilter){ 
		IWRFilterTracking( &Yaw, &Pitch, &Roll );
	}
	//qDebug() << QString::number(Yaw*IWR_RAWTODEG) << " - " << QString::number(Roll*IWR_RAWTODEG) << " - " << QString::number(Roll*IWR_RAWTODEG);
	
	Quaternionf yawRot = Quaternionf(AngleAxisf(Yaw*IWR_RAWTORAD, Vector3f(0,1,0)));
	Quaternionf pitchRot = Quaternionf(AngleAxisf(Pitch*IWR_RAWTORAD, Vector3f(1,0,0)));
	Quaternionf rollRot = Quaternionf(AngleAxisf(Roll*IWR_RAWTORAD, Vector3f(0,0,1)));

	_rotation = new Quaternionf((*_referenceRotation) * rollRot*pitchRot*yawRot);

	Yaw *= IWR_RAWTODEG;
	Pitch *= IWR_RAWTODEG;
	Roll *= IWR_RAWTODEG;
	SetToReferenceRotation(&Yaw, &Pitch, &Roll);
	// Convert the tracker's inputs to angles
	_pitch =  (float)Pitch; 
	_yaw  =  (float)Yaw;
	_roll   =  (float)Roll;
	
	return true;
}

void	IMUManagerVuzix::IWRFilterTracking( long *yaw, long *pitch, long *roll )
{
	int		i, FilterCnt, Previous_Stored_index;
	float	FilterSum_yaw = 0.0f, FilterSum_pitch = 0.0f, FilterSum_roll = 0.0f;
	float	Current_yaw, Previous_yaw;
	float	Current_pitch, Previous_pitch;
	Previous_Stored_index = g_Filter_index;
	g_Filter_index++;
	if( g_Filter_index >= MAX_FILTER_SIZE ) 
		g_Filter_index = 0;
	Filtered_yaw[ g_Filter_index ]	= (float)*yaw;
	Filtered_pitch[ g_Filter_index ]= (float)*pitch;
	Filtered_roll[ g_Filter_index ] = (float)*roll;
	// compute max filter entries based on noise in signal.
	// This should be StdDev of current set or other rms solution.
	Current_yaw    = (float)Filtered_yaw[ g_Filter_index ] * IWR_RAWTODEG;
	Previous_yaw   = (float)Filtered_yaw[ Previous_Stored_index ] * IWR_RAWTODEG;
	Current_pitch  = (float)Filtered_pitch[ g_Filter_index ] * IWR_RAWTODEG;
	Previous_pitch = (float)Filtered_pitch[ Previous_Stored_index ] * IWR_RAWTODEG;
	// If the filter noise goes above a threashold turn off filtering .
	if( fabs(Previous_yaw - Current_yaw) > g_FilterZone ||
		fabs(Previous_pitch - Current_pitch) > g_FilterZone ) {
		// Disable filtering when outside of quiet zone operation.
		Filter_Restart_Count = (g_FilterSize * 2);
		}
	// Determine when to enter into filter mode again.
	if( Filter_Restart_Count ) {
        Max_Filter_entries = 1;
		Filter_Restart_Count--;
		if( Filter_Restart_Count == 0 ) {
			// Fill up filter que with current reading.
			for( i=0; i < MAX_FILTER_SIZE; i++ ) {
				Filtered_yaw[ i ]  = (float)*yaw;
				Filtered_pitch[ i ]= (float)*pitch;
				Filtered_roll[ i ] = (float)*roll;
				}
			}
		}
	else {// Setup number of filter values to process.
		Max_Filter_entries = g_FilterSize;
		}
	// Simple running average. from most recent to front of que.
	for( i=g_Filter_index, FilterCnt=0; i; i-- ) {
		if( FilterCnt++ >= Max_Filter_entries ) 
			break;
		FilterSum_yaw   += Filtered_yaw[ i ];
		FilterSum_pitch += Filtered_pitch[ i ];
		FilterSum_roll  += Filtered_roll[ i ];
		}
	// Continue from the end of the que.
	for( i=MAX_FILTER_SIZE-1; i > g_Filter_index; i-- ) {
		if( FilterCnt++ >= Max_Filter_entries ) 
			break;
		FilterSum_yaw   += Filtered_yaw[ i ];
		FilterSum_pitch += Filtered_pitch[ i ];
		FilterSum_roll  += Filtered_roll[ i ];
		}
	*yaw	= (long)(FilterSum_yaw	/ (float)Max_Filter_entries);
	*pitch	= (long)(FilterSum_pitch/ (float)Max_Filter_entries);
	*roll	= (long)(FilterSum_roll / (float)Max_Filter_entries);
}

bool IMUManagerVuzix::FreeIMU()
{
	return IWRFreeDll();
}

void IMUManagerVuzix::ResetRotation(Quaternionf* rotation)
{
	float w = rotation->w();
	float x = rotation->x();
	float y = rotation->y();
	float z = rotation->z();

	_referenceRoll  = atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z) * (180.0f/M_PI) ;
	_referencePitch = atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z) * (180.0f/M_PI);
	_referenceYaw   = asin(2*x*y + 2*z*w) * (180.0f/M_PI);
	_referenceRotation = rotation;
	 IWRZeroSet();
}

void IMUManagerVuzix::SetToReferenceRotation(long *yaw, long *pitch, long *roll)
{
	*yaw += _referenceYaw;
	if(*yaw > 180) *yaw = -180 + (*yaw-180);
	if(*yaw < -180) *yaw = 180 + (*yaw+180);

	*pitch += _referencePitch;
	if(*pitch > 90) *pitch = 90;
	if(*pitch < -90) *pitch = 90;

	*roll += _referenceYaw;
	if(*roll > 180) *roll = -180 + (*roll-180);
	if(*roll < -180) *roll = 180 + (*roll+180);
}

//void IMUManager::FromIMUToVuzix()
//{
//	//the imu-sensor isn't exactly in the center of the vuzix glass
//	//to derive the glasses-orientation, we have to use the corresponding position and rotation of the tracker
//	Vector3f toCenter = Vector3f();
//	AngleAxisf rotationInCenter = AngleAxisf();
//}