#include "stdafx.h"
#include <iWearSDK.h>

	HMODULE						IWRTrackerInstance=NULL;
	PIWROPENTRACKER				IWROpenTracker=NULL;
	PIWRCLOSETRACKER			IWRCloseTracker=NULL;
	PIWRZEROSET					IWRZeroSet=NULL;
	PIWRGETTRACKING				IWRGetTracking=NULL;
	PIWRGET6DTRACKING			IWRGet6DTracking=NULL;
	PIWRGETFILTEREDSENSORDATA	IWRGetFilteredSensorData=NULL;
	PIWRGETSENSORDATA			IWRGetSensorData=NULL;
	PIWRGETVERSION				IWRGetVersion=NULL;
	PIWRBEGINCALIBRATE			IWRBeginCalibrate=NULL;
	PIWRENDCALIBRATE			IWREndCalibrate=NULL;
	PIWRSETFILTERSTATE			IWRSetFilterState=NULL;
	PIWRGETFILTERSTATE			IWRGetFilterState=NULL;
	PIWRGETPRODUCTID			IWRGetProductID=NULL;
	PIWRGETPRODUCTDETAILS		IWRGetProductDetails=NULL;
	PIWRSETMAGAUTOCORRECT		IWRSetMagAutoCorrect=NULL;
	PIWRGETMAGYAW				IWRGetMagYaw=NULL;

	HMODULE						IWRStereoInstance=NULL;
	PIWRSTEREOOPEN				IWRSTEREO_Open=NULL;
	PIWRSTEREOCLOSE				IWRSTEREO_Close=NULL;
	PIWRSTEREOSETSTEREO			IWRSTEREO_SetStereo=NULL;
	PIWRSTEREOWACK				IWRSTEREO_WaitForAck=NULL;
	PIWRSTEREOSETLR				IWRSTEREO_SetLR=NULL;
	PIWRSTEREOSETLREX			IWRSTEREO_SetLREx=NULL;
	PIWRSTEREOGETVERSION		IWRSTEREO_GetVersion=NULL;

long	IWRLoadDll(void){
	// Tracker/interface subsystem support.
	LPCWSTR str = TEXT("IWEARDRV.DLL");
	IWRTrackerInstance = LoadLibrary(str);
	if( IWRTrackerInstance == NULL) 
		return IWR_NO_TRACKER;
	IWROpenTracker			= (PIWROPENTRACKER)			GetProcAddress(IWRTrackerInstance, "IWROpenTracker" );
	IWRCloseTracker			= (PIWRCLOSETRACKER)		GetProcAddress(IWRTrackerInstance, "IWRCloseTracker" );
	IWRZeroSet				= (PIWRZEROSET)				GetProcAddress(IWRTrackerInstance, "IWRZeroSet" );
	IWRGetTracking			= (PIWRGETTRACKING)			GetProcAddress(IWRTrackerInstance, "IWRGetTracking" );
	IWRGet6DTracking		= (PIWRGET6DTRACKING)		GetProcAddress(IWRTrackerInstance, "IWRGet6DTracking" );
	IWRGetFilteredSensorData= (PIWRGETFILTEREDSENSORDATA)GetProcAddress(IWRTrackerInstance,"IWRGetFilteredSensorData" );
	IWRGetSensorData		= (PIWRGETSENSORDATA)		GetProcAddress(IWRTrackerInstance, "IWRGetSensorData" );
	IWRBeginCalibrate		= (PIWRBEGINCALIBRATE)		GetProcAddress(IWRTrackerInstance, "IWRBeginCalibrate" );
	IWREndCalibrate			= (PIWRENDCALIBRATE)		GetProcAddress(IWRTrackerInstance, "IWREndCalibrate" );
	IWRGetFilterState		= (PIWRGETFILTERSTATE)		GetProcAddress(IWRTrackerInstance, "IWRGetFilterState" );
	IWRSetFilterState		= (PIWRSETFILTERSTATE)		GetProcAddress(IWRTrackerInstance, "IWRSetFilterState" );
	IWRGetProductID			= (PIWRGETPRODUCTID)		GetProcAddress(IWRTrackerInstance, "IWRGetProductID" );
	IWRGetProductDetails	= (PIWRGETPRODUCTDETAILS)	GetProcAddress(IWRTrackerInstance, "IWRGetProductDetails" );
	IWRGetVersion			= (PIWRGETVERSION)			GetProcAddress(IWRTrackerInstance, "IWRGetVersion" );
	IWRSetMagAutoCorrect	= (PIWRSETMAGAUTOCORRECT)	GetProcAddress(IWRTrackerInstance, "IWRSetMagAutoCorrect" );
	IWRGetMagYaw			= (PIWRGETMAGYAW)			GetProcAddress(IWRTrackerInstance, "IWRGetMagYaw" );
	if( !IWROpenTracker || !IWRCloseTracker || !IWRZeroSet || !IWRGetTracking || !IWRGet6DTracking || !IWRBeginCalibrate ||
		!IWRGetFilteredSensorData || !IWRGetSensorData || !IWRGetFilterState || !IWRSetFilterState || !IWREndCalibrate ||
		!IWRGetProductID || !IWRGetProductDetails || !IWRGetVersion || !IWRSetMagAutoCorrect || !IWRGetMagYaw )
		return IWR_TRACKER_CORRUPT;
	// Stereo/video subsystem support.
	LPCWSTR str2 = TEXT("IWRSTDRV.DLL");
	IWRStereoInstance = LoadLibrary(str2);
	if( IWRStereoInstance == NULL) 
		return IWR_NO_STEREO;
	IWRSTEREO_Open			= (PIWRSTEREOOPEN)			GetProcAddress(IWRStereoInstance, "IWRSTEREO_Open" );
	IWRSTEREO_Close			= (PIWRSTEREOCLOSE)			GetProcAddress(IWRStereoInstance, "IWRSTEREO_Close" );
	IWRSTEREO_SetStereo		= (PIWRSTEREOSETSTEREO)		GetProcAddress(IWRStereoInstance, "IWRSTEREO_SetStereo" );
	IWRSTEREO_WaitForAck	= (PIWRSTEREOWACK)			GetProcAddress(IWRStereoInstance, "IWRSTEREO_WaitForAck" );
	IWRSTEREO_SetLR			= (PIWRSTEREOSETLR)			GetProcAddress(IWRStereoInstance, "IWRSTEREO_SetLR" );
	IWRSTEREO_GetVersion	= (PIWRSTEREOGETVERSION)	GetProcAddress(IWRStereoInstance, "IWRSTEREO_GetVersion" );
	if( !IWRSTEREO_Open || !IWRSTEREO_Close || !IWRSTEREO_SetStereo || !IWRSTEREO_WaitForAck || !IWRSTEREO_SetLR || !IWRSTEREO_GetVersion )
		return IWR_STEREO_CORRUPT;
	IWROpenTracker();
	return IWR_OK;
}
// Name:	IWRFreeDll
// Desc:	Frees the dynamically loaded tracker/stereo driver.
// Retu:    IWR_OK, IWR_NO_INSTANCE
long	IWRFreeDll( void ){
	if( IWRStereoInstance ) {
		FreeLibrary(IWRStereoInstance);
		IWRStereoInstance = NULL;
	}
	if( IWRTrackerInstance ) {
		if( IWRCloseTracker )
			IWRCloseTracker();	
		FreeLibrary( IWRTrackerInstance );
		IWRTrackerInstance = NULL;
		return IWR_OK;		
	}
	return IWR_NOTRACKER_INSTANCE;
}