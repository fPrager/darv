#include "stdafx.h"
#include "IMU.h"


IMU::IMU(char *calibrationFile):
	gravCorrectionBase(DEFAULT_GRAV_CORRECTION_BASE),
	sensorAssemblyAngle (DEFAULT_SENSOR_ASSEMBLY_ANGLE * M_DEGTORAD),
	errorCorrection (DEFAULT_ERROR_CORRECTION),
	magnetometerEnabled(true),
	accelerometerEnabled(true),
	isInitialized(false),
	print(false),
	gyrOffsetX(0), gyrOffsetY(0), gyrOffsetZ(0), 
	lGyrOffsetX(0), lGyrOffsetY(0), lGyrOffsetZ(0)
{
	if (!IMU::loadCalibration(calibrationFile))
		throw std::runtime_error("Unable to open config file.");
	gravityOrigGlobal = Eigen::Quaterniond(0,0,1,0);
	sensorAssemblyRotation = Eigen::Quaterniond(cos(sensorAssemblyAngle/2), 0, 1*sin(sensorAssemblyAngle/2), 0);
	sensorAssemblyRotation.normalize();
	position = Eigen::Vector3d(0,0,0);
	mainToAttachedRotation = Eigen::Quaterniond(1,0,0,0);
	translationVelocity = Eigen::Vector3d(0,0,0);
	IWRLoadDll();
	if (IWROpenTracker() != ERROR_SUCCESS)
		throw std::runtime_error("Tracker couldn't be opened");
}

void IMU::setSensorAssembleAngle(double angle) {
	sensorAssemblyAngle = angle * M_DEGTORAD;
	sensorAssemblyRotation = Eigen::Quaterniond(cos(sensorAssemblyAngle/2), 0, 1*sin(sensorAssemblyAngle/2), 0);
}

IMU::~IMU(void)
{
	IWRCloseTracker();
}

bool IMU::loadCalibration(char *filename) {
	boost::property_tree::ptree pt;
	boost::property_tree::read_json(filename, pt);
	if (pt.empty())
		return false;
	std::vector<double> vec;
	BOOST_FOREACH (boost::property_tree::ptree::value_type& arrayElem, pt.get_child("gyrOffset")) {
		vec.push_back(arrayElem.second.get_value<double>());
	}
	gyrOffset = Eigen::Vector3d(vec.data());
	vec.clear();
	std::cout << "Gyroskop Offsets gelesen: " << gyrOffset.x() << ", " <<  gyrOffset.y() << ", " << gyrOffset.z() << std::endl;
	BOOST_FOREACH (boost::property_tree::ptree::value_type& arrayElem, pt.get_child("lGyrOffset")) {
		vec.push_back(arrayElem.second.get_value<double>());
	} 
	lGyrOffset = Eigen::Vector3d(vec.data());
	vec.clear();
	std::cout << "LowBand Gyroskop Offsets gelesen: " << lGyrOffset.x() << ", " <<  lGyrOffset.y() << ", " << lGyrOffset.z() << std::endl;
	
	BOOST_FOREACH (boost::property_tree::ptree::value_type& arrayElem, pt.get_child("magMax")) {
		vec.push_back(arrayElem.second.get_value<double>());
	} 
	magCalibration = Eigen::Vector3d(vec.data());
	vec.clear();
	std::cout << "Magnetometer Kallibrierungswerte gelesen: " << magCalibration.x() << ", " <<  magCalibration.y() << ", " << magCalibration.z() << std::endl;
	return true;
}

void IMU::setOrientation(double w, double x, double y, double z){
	Eigen::Quaterniond attachedOrientationGlobal (w,x,y,z);
	attachedOrientationGlobal .normalize();
	mainToAttachedRotation= mainOrientation.conjugate()*attachedOrientationGlobal;
}

void IMU::getOrientation(double *w, double *x, double *y, double *z){
	if (!isInitialized)
		return;
	Eigen::Quaterniond temp = mainOrientation*mainToAttachedRotation;
	*w = temp.w();
	*x = temp.x();
	*y = temp.y();
	*z = temp.z();
}

void IMU::resetCompassFix(void){
	retrieveSensorData();

	mag.x() = magRaw.x() / magCalibration.x();
	mag.y() = magRaw.y() / magCalibration.y();
	mag.z() = magRaw.z() / magCalibration.z();
	
	compassFix = mainOrientation.conjugate()*Eigen::Quaterniond(0, mag.x(), mag.y(), mag.z())*mainOrientation;
}


bool IMU::update(int elapsedTime){
	double elapsedTimed = (double)elapsedTime;
	retrieveSensorData();

	//Transformation - Koordinatensysteme:
	/*
	(Let mainOrientation be the rotation from global to local)

	von GLOBAL zu LOCAL (Brille)
	local = mainOrientation * global * mainOrientation.conjugate()
	
	von LOCAL (Brille) zu GLOBAL
	global = mainOrientation.conjugate() * local * mainOrientation
	
	// rotation difference between two rotations from q1 to q2: q' = (q1^-1)*q2
	*/

	// Scale Sensor Data
	/* resolution: 0.824° per sencond per count. values in a range of -2048 - 2047.
	=> max. rotation: 1687° per second. */

	gyr = (gyrRaw*0.824)*M_DEGTORAD*elapsedTimed*0.001 /*time step*/ *0.1 /*why? dunno.*/;
	lgyr = (lgyrRaw*0.205)*M_DEGTORAD*elapsedTimed*0.001 /*time step*/ *0.1 /*why? dunno.*/ ;
	if (abs(lgyrRaw.x()) <= 2048)
		gyr.x() = (lgyr.x());
	if (abs(lgyrRaw.y()) <= 2048)
		gyr.y() = (lgyr.y());
	if (abs(lgyrRaw.z()) <= 2048)
		gyr.z() = (lgyr.z());
	
	acc = accRaw  * (1./1024.); //1mg per count

	mag.x() = magRaw.x() / magCalibration.x();
	mag.y() = magRaw.y() / magCalibration.y();
	mag.z() = magRaw.z() / magCalibration.z();
	
	//Initialization
	if (!isInitialized){
		if (abs(1-acc.norm()) < 0.1) {
			//std::cout << abs(1-acc.norm()) << std::endl;
			mainOrientation = (Eigen::Quaterniond::FromTwoVectors(gravityOrigGlobal.vec(), acc).normalized());
			compassFix = mainOrientation.conjugate()*Eigen::Quaterniond(0, mag.x(), mag.y(), mag.z())*mainOrientation;
			isInitialized = true;
		} else {
			//std::cout << acc.norm() << std::endl;
		}
		return false;
	}

	// Gyro Accumulation	
	Eigen::Quaterniond localY(cos(gyr.y()/2), 0, 1*sin(gyr.y()/2), 0);
	Eigen::Quaterniond localX(cos(gyr.x()/2), 1*sin(gyr.x()/2), 0, 0);
	Eigen::Quaterniond localZ(cos(gyr.z()/2), 0, 0, 1*sin(gyr.z()/2));
	Eigen::Quaterniond local = localZ*localX*localY;
	mainOrientation = local*mainOrientation;

	//Accelerometer
	Eigen::Quaterniond accLocal (0, acc.x(), acc.y(), acc.z()); // der GEMESSENE Erdanziehungsvektor im KS der Brille
	Eigen::Quaterniond accGlobal = mainOrientation.conjugate()*accLocal*mainOrientation; //...im Welt-KS
	if (print == true) {
		std::cout << "acc.length: " << acc.norm() << std::endl;
		std::cout << "accGlobal: " << accGlobal.x() << ", " << accGlobal.y() << ", " << accGlobal.z() << ", " << std::endl;
		std::cout << "gravOriginal: " << gravityOrigGlobal.x() << ", " << gravityOrigGlobal.y() << ", " << gravityOrigGlobal.z() << ", " << std::endl;
		print = false;
	}
	translationAcceleration = Eigen::Vector3d(accGlobal.x() - gravityOrigGlobal.x(), accGlobal.y() - gravityOrigGlobal.y(),accGlobal.z() - gravityOrigGlobal.z());
	translationAccelerationMagnitude = translationAcceleration.norm();
	// fill "translations" list
	if (translationAccelerations.size() >= 20) 
		translationAccelerations.pop_back();
	transPerTime myTrans = {translationAcceleration, elapsedTimed};
	translationAccelerations.push_front(myTrans);

	position += translationVelocity*elapsedTimed + 0.5*translationAcceleration*elapsedTimed*elapsedTimed;
	
	accGlobal.normalize();
	
	// Accelerometer
	Eigen::Quaterniond gravCorrectionGlobal = Eigen::Quaterniond::FromTwoVectors(gravityOrigGlobal.vec(),accGlobal.vec()); //Der Korrekturvektor im Welt KS
	accCorrectionFactor = pow(gravCorrectionBase, translationAccelerationMagnitude);
	gravCorrectionGlobal = gravCorrectionGlobal.slerp(1-accCorrectionFactor, Eigen::Quaterniond(1,0,0,0));

	//Magnetometer
	Eigen::Quaterniond magMeasuredGlobal = mainOrientation.conjugate()*Eigen::Quaterniond (0, mag.x(), mag.y(), mag.z())*(mainOrientation);
	Eigen::Quaterniond magCorrectionGlobal = Eigen::Quaterniond::FromTwoVectors(Eigen::Vector3d (compassFix.x(), 0, compassFix.z()), Eigen::Vector3d (magMeasuredGlobal.x(), 0, magMeasuredGlobal.z()));

	Eigen::Quaterniond combCorrectionLocal;
	switch (accelerometerEnabled | (magnetometerEnabled << 1)){
		case 1: //only accelerometer enabled
			combCorrectionLocal = mainOrientation*gravCorrectionGlobal*mainOrientation.conjugate();
			break;
		case 2: //only magnetometer enabled 
			combCorrectionLocal = mainOrientation*gravCorrectionGlobal*mainOrientation.conjugate();
			break;
		case 3: //both sensors enabled 
			combCorrectionLocal = mainOrientation*gravCorrectionGlobal.slerp(0.5, magCorrectionGlobal)*mainOrientation.conjugate();
		break;
		default:
			combCorrectionLocal = Eigen::Quaterniond(1,0,0,0);
	}
	mainOrientation = mainOrientation.slerp(errorCorrection*(elapsedTimed/17.0),combCorrectionLocal*mainOrientation);
	return true;
}


void IMU::retrieveSensorData(void){
	IWRSENSDATA sensdata;
	if (IWRGetSensorData(&sensdata) != ERROR_SUCCESS)
		throw std::exception("Tracker not ready yet.");
	gyrOffsetX += gyrOffset.x();
	gyrOffsetY += gyrOffset.y();
	gyrOffsetZ += gyrOffset.z();
	lGyrOffsetX += lGyrOffset.x();
	lGyrOffsetY += lGyrOffset.y();
	lGyrOffsetZ += lGyrOffset.z();

	gyrRaw.z() = ((signed short)(((unsigned short)sensdata.gyro_sensor.gyx_msb << 8) | (unsigned short)sensdata.gyro_sensor.gyx_lsb)-gyrOffsetX);
	gyrRaw.x() = ((signed short)(((unsigned short)sensdata.gyro_sensor.gyy_msb << 8) | (unsigned short)sensdata.gyro_sensor.gyy_lsb)-gyrOffsetY);
	gyrRaw.y() = ((signed short)(((unsigned short)sensdata.gyro_sensor.gyz_msb << 8) | (unsigned short)sensdata.gyro_sensor.gyz_lsb)-gyrOffsetZ);
	
	lgyrRaw.z() = ((signed short)(((unsigned short)sensdata.lbgyro_sensor.gyx_msb << 8) | (unsigned short)sensdata.lbgyro_sensor.gyx_lsb)-lGyrOffsetX);
	lgyrRaw.x() = ((signed short)(((unsigned short)sensdata.lbgyro_sensor.gyy_msb << 8) | (unsigned short)sensdata.lbgyro_sensor.gyy_lsb)-lGyrOffsetY);
	lgyrRaw.y() = ((signed short)(((unsigned short)sensdata.lbgyro_sensor.gyz_msb << 8) | (unsigned short)sensdata.lbgyro_sensor.gyz_lsb)-lGyrOffsetZ);

	accRaw.z() = (-(signed short)(((unsigned short)sensdata.acc_sensor.accx_msb << 8) | (unsigned short)sensdata.acc_sensor.accx_lsb));
	accRaw.x() = ((signed short)(((unsigned short)sensdata.acc_sensor.accy_msb << 8) | (unsigned short)sensdata.acc_sensor.accy_lsb));
	accRaw.y() = ((signed short)(((unsigned short)sensdata.acc_sensor.accz_msb << 8) | (unsigned short)sensdata.acc_sensor.accz_lsb));

	magRaw.z() = (-(signed short)(((unsigned short)sensdata.mag_sensor.magx_msb << 8) | (unsigned short)sensdata.mag_sensor.magx_lsb));
	magRaw.x() = ((signed short)(((unsigned short)sensdata.mag_sensor.magy_msb << 8) | (unsigned short)sensdata.mag_sensor.magy_lsb));
	magRaw.y() = ((signed short)(((unsigned short)sensdata.mag_sensor.magz_msb << 8) | (unsigned short)sensdata.mag_sensor.magz_lsb));
	/*
	X-Achse nach VORN/hinten
	Y-Achse nach RECHTS/links
	Z-Achse nach OBEN/unten
	*/
	double intpart;
	gyrOffsetX = modf (gyrOffsetX , &intpart);
	gyrOffsetY = modf (gyrOffsetY , &intpart);
	gyrOffsetZ = modf (gyrOffsetZ , &intpart);
	lGyrOffsetX = modf (lGyrOffsetX , &intpart);
	lGyrOffsetY = modf (lGyrOffsetY , &intpart);
	lGyrOffsetZ = modf (lGyrOffsetZ , &intpart);
}
void IMU::getTranslationAcceleration(double *x, double *y, double *z){
	*x = translationAcceleration.x();
	*y = translationAcceleration.y();
	*z = translationAcceleration.z();
}
bool IMU::getMagnetometerEnabled(){
	return magnetometerEnabled;
}
void IMU::setMagnetometerEnabled(bool enabled){
	magnetometerEnabled = enabled;
}
bool IMU::getAccelerometerEnabled(){
	return accelerometerEnabled;
}
void IMU::setAccelerometerEnabled(bool enabled){
	accelerometerEnabled = enabled;
}
double IMU::getTranslationAccelerationMagnitude(int time){
	if (time <= 0) return 0;
	double timeAcc = 0;
	Eigen::Vector3d velAcc (0,0,0);
	for (std::list<transPerTime>::iterator it = translationAccelerations.begin(); it != translationAccelerations.end(); it++){
		timeAcc += it->time;
		velAcc += (it->vec * it->time);
		if (timeAcc >= (double)time)
			return (velAcc/timeAcc).norm();
	}
	return 0;
}