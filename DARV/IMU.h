#pragma once
#include <iostream>
#include <stdlib.h>
#include <list>
#include <array>
#include <Eigen/geometry>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
//#define IWEAR_ONETIME_DEFINITIONS
//NEEDS TO BE INSERT INTO INCLUDING HEADER FILE!!!
#include "iWearSDK.h"

#define M_DEGTORAD M_PI/180
#define M_RADTODEG 180/M_PI
#define DEFAULT_GRAV_CORRECTION_BASE 0.2
#define DEFAULT_ERROR_CORRECTION 0.1
#define DEFAULT_SENSOR_ASSEMBLY_ANGLE 20

typedef struct {Eigen::Vector3d vec; double time;} transPerTime;

class IMU
{
private:
	double modulus(double a, double b);
	double sensorAssemblyAngle;
	double gyrOffsetX, gyrOffsetY, gyrOffsetZ, lGyrOffsetX, lGyrOffsetY, lGyrOffsetZ;
	double accCorrectionFactor;
	Eigen::Vector3d magCalibration, gyrOffset, lGyrOffset; //for calibration
	Eigen::Vector3d accRaw, lgyrRaw, gyrRaw, magRaw, lgyr, gyr, acc, mag, translationAcceleration, position, translationVelocity;
	Eigen::Quaterniond gravityOrigGlobal, sensorAssemblyRotation, compassFix, mainOrientation, mainToAttachedRotation;
	bool isInitialized, magnetometerEnabled, accelerometerEnabled;
	void retrieveSensorData(void);
	double translationAccelerationMagnitude;
	std::list<transPerTime> translationAccelerations;
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	IMU(char *calibrationFile);
	~IMU(void);
	bool print; // set true to print something once
	bool loadCalibration(char *filename);
	double gravCorrectionBase; // accCorrectionFactor (amount of influence of correction by accelerometer) = X^TranslationMagnitude = [1,0]. X must be [0,1], 0.2 by default.
	double errorCorrection; // The algorithm uses [1-errorCorrection] of gyrometer data and [errorCorrection] of correction
	void setSensorAssembleAngle(double angle); // The Angle (degree) between the front of the glasses and the IMU. Measurement and calculation works in frame of the IMU and need to be translated into the glasses' frame.
	bool update(int elapsedTime); //retrieves new Sensor Data and calculates current position. Returns false if rotation hasn't been set yet.
	void setOrientation(double w,double x,double y,double z); // sets the current rotation. Rotations returned by getRotation() refer to that frame.
	void getOrientation(double *w, double *x, double *y, double *z); // Just reports current state - no calculation here
	void getTranslationAcceleration(double *x, double *y, double *z);
	inline void getPosition(double *x, double *y, double *z){*x = position.x(); *y = position.y(); *z = position.z();};
	void setMagnetometerEnabled(bool enabled);
	bool getMagnetometerEnabled();
	void setAccelerometerEnabled(bool enabled);
	bool getAccelerometerEnabled();
	inline double getTranslationAccelerationMagnitude(){return translationAccelerationMagnitude;}; // norm of translation acceleration vector of current frame
	double getTranslationAccelerationMagnitude(int time); // norm of translation acceleration vector of a given period (ms)
	inline double getAccCorrectionFactor(){return accCorrectionFactor;}
	void IMU::resetCompassFix(void);
};