#include "stdafx.h"
#include "Uncertainty.h"

//first expectations handles the the error as linear influence of the "marker area" 
float distanceFactor[6] = {0,0,0,0,0,0};		//six elements for x,y,z, yaw, pitch, roll
float distanceZero[6] = {0,0,0,0,0,0};
float angleFactor[6] = {0,0,0,0,0,0};
float angleZero[6] = {0,0,0,0,0,0};
float areaPxFactor[6] = {-0.0005,-0.000225,-0.000325,-0.00033333333,-0.000125,-0.00027777777};
float areaPxZero[6] = {10000,10000,10000,12000,10000,15000};
float edgeSharpnessFactor[6] = {0,0,0,0,0};
float edgeSharpnessZero[6] = {0,0,0,0,0};

//further meassurements reaults in a clearer view on error sources of "marker area rate (m)" as a polynomyle function and "marker move rate (A_w)" as a linear function
//we differ between a base-error F_base (without movement) and an accumulation-error F_akku (with movement)
//  F_base(m) = a0 + a1 * x + a2 * x^2 + a3 * x^3 + a4 * x^4 + a5 * x^5 + a6 * x^6 + a7 * x^7 + a8 * x^8  | if m < 0,55 else F_base0
//HINT: the different notation corresponds to the formulas in the diploma thesis

float _maxM  = 0.55;
						//x				y			z			pitch			yaw				roll
float _baseExp[9][6] = {{5.173,			8,			9,			3.2222152155532049,			2.17,			1.82},			//a0
						{-24.9,			-2.48,		-37.16,		-20.725141866598172,			-14.13,			12.65},			//a1
						{467,			1452.63,	123,		236.92575069285942,			60.93,			-58.28},		//a2
						{-2136.22,		-1441.2,	-153.33,	1241.7568428163449,		6824.77,		57.86},			//a3
						{-1358.21,		50730.2,	0,			-20185.257292245020,		-77962.9,		0},				//a4
						{31570.19,		-76918.39,	0,			76653.081108153521,		349010.38,		0},				//a5
						{-85016.82,		42696.81,	0,			-119849.82388825876,		-774251.21,		0},				//a6
						{94375.49,		0,			0,			68166.465794263175,		851290.43,		0},				//a7
						{-38637.55,		0,			0,			0,				-371043.52,		0}};			//a8

float _base0[6]		=	{1.9f,			1.8,		1.5,		1,				0.9f,			0.7f};			//F_base0

//F_akku(A_w) = b * A_w + A_w0

float _akkuB[6]		=	{-35.29,		-27.47,		-23.52,		-14.44,			-11.11,			-17.50};		//b
float _akkuAw0[6]	=	{0.85,			0.91,		0.85,		0.9,			0.9,			0.8};			//A_w0

float MakeLinearFunction(float x, float m, float zero_x)
{
	return x*m - m*zero_x;
}

float GetErrorAccumulation(float A_w,  int component, float elapsedTime)
{
	if(A_w <= 0 )
		return -1;

	float F_akku = MakeLinearFunction(A_w,_akkuB[component],_akkuAw0[component]);
	
	if(F_akku < 0)
		return 0;
	else
		return F_akku*(elapsedTime/1000.f);
}

float GetErrorBase(float m, int component)
{
	float F_base = 0;

	if(m < 0)						//this shouldn't be possible!!! a ratio can't be negetive here
	{
		qDebug() << "ERROR: Negative Marker-Image-Area-Rate (m)!";
		F_base = 0; 
	}
	else
	if(m > _maxM)					//we just know the error function from 0 to _maxM, we expect a constant error with better marker area values (>_maxM)
	{
		F_base = _base0[component];
	}
	else
	for(int i = 0; i < 9; i++)		//m is in a known interval
	{
		F_base += _baseExp[i][component] * pow(m,i);
		//if(component==3)
		//qDebug() << "exp: " + QString::number(_baseExp[i][component]);
	}
	return F_base;
}

void Uncertainty::FromMarkerData(float areaRate, float moveRate, float elapsedTime)
{
	//uncertainty of the x-component
	float base = GetErrorBase(areaRate, 0);
	float akku = GetErrorAccumulation(moveRate, 0, elapsedTime);
	if(akku <= 0)
	{
		_translationSpace.x() = base;
		_baseError[0] = true;
	}
	else
	{
		_translationSpace.x() += akku;
		_baseError[0] = false;
	}

	//uncertainty of the y-component
	base = GetErrorBase(areaRate, 0);
	akku = GetErrorAccumulation(moveRate, 0, elapsedTime);
	if(akku <= 0)
	{
		_translationSpace.y() = base;
		_baseError[1] = true;
	}
	else
	{
		_translationSpace.y() += akku;
		_baseError[1] = false;
	}

	//uncertainty of the z-component
	base = GetErrorBase(areaRate, 2);
	akku = GetErrorAccumulation(moveRate, 2, elapsedTime);
	if(akku <= 0)
	{
		_translationSpace.z() = base;
		_baseError[2] = true;
	}
	else
	{
		_translationSpace.z() += akku;
		_baseError[2] = false;
	}

	float akkuFactor = 2;
	//uncertainty of the pitch-component
	base = GetErrorBase(areaRate, 3);
	akku = GetErrorAccumulation(moveRate, 3, elapsedTime);
	if(akku <= 0)
	{
		_rotationSpace.y() = base;
		_baseError[3] = true;
	}
	else
	{
		_rotationSpace.y() += akku*akkuFactor;
		_baseError[3] = false;
	}
	
	//uncertainty of the yaw-component
	base = GetErrorBase(areaRate, 4);
	akku = GetErrorAccumulation(moveRate, 4, elapsedTime);
	if(akku <= 0)
	{
		_rotationSpace.x() = base;
		_baseError[4] = true;
	}
	else
	{
		_rotationSpace.x() += akku*akkuFactor;
		_baseError[4] = false;
	}
	//uncertainty of the roll-component
	base = GetErrorBase(areaRate, 5);
	akku = GetErrorAccumulation(moveRate, 5, elapsedTime);
	if(akku <= 0)
	{
		_rotationSpace.z() = base;
		_baseError[5] = true;
	}
	else
	{
		_rotationSpace.z() += akku*akkuFactor;
		_baseError[5] = false;
	}

	CalculateQuality();
}


void Uncertainty::FromReference(Uncertainty* sourceUnc, Vector3f* distance)
{
	Vector3f trans = *distance;
	if(trans != Vector3f(0,0,0))
	{
		Vector3f toBack = -trans;
		toBack.normalize();
		Vector3f toRight = Vector3f(1,0,0);
		Vector3f toUp = Vector3f(0,1,0);
		if(abs(distance->y()) != 1)
		{
			toRight = Vector3f(0,1,0).cross(toBack);
			toRight.normalize();
			toUp = toBack.cross(toRight);
			toUp.normalize();
		}
		Vector3f transUnc = sourceUnc->GetTranslationSpace();
		float z = Vector3f(toBack.x() * transUnc.x(),toBack.y() * transUnc.y(),toBack.z() * transUnc.z()).norm();
		float x = Vector3f(toRight.x()*transUnc.x(),toRight.y()*transUnc.y(),toRight.z()*transUnc.z()).norm();
		float y = Vector3f(toUp.x()*transUnc.x(),toUp.y()*transUnc.y(),toUp.z()*transUnc.z()).norm();
		Vector3f trans1 = Vector3f(x,y,z);
		Vector3f rot_translation = AngleAxisf(_rotationSpace.x() * (M_PI/180.0f), Vector3f(0,1,0))._transformVector(trans);
		float scalarRightYaw = toRight.x()*rot_translation.x() + toRight.y()*rot_translation.y() + toRight.z()*rot_translation.z();
		float scalarUpYaw = toUp.x()*rot_translation.x() + toUp.y()*rot_translation.y() + toUp.z()*rot_translation.z();
		float scalarBackYaw = toBack.x()*rot_translation.x() + toBack.y()*rot_translation.y() + toBack.z()*rot_translation.z();
		
		rot_translation = AngleAxisf(_rotationSpace.y() * (M_PI/180.0f), Vector3f(1,0,0))._transformVector(trans);
		float scalarRightPitch = toRight.x()*rot_translation.x() + toRight.y()*rot_translation.y() + toRight.z()*rot_translation.z();
		float scalarUpPitch = toUp.x()*rot_translation.x() + toUp.y()*rot_translation.y() + toUp.z()*rot_translation.z();
		float scalarBackPitch = toBack.x()*rot_translation.x() + toBack.y()*rot_translation.y() + toBack.z()*rot_translation.z();
		
		rot_translation = AngleAxisf(_rotationSpace.z() * (M_PI/180.0f), Vector3f(0,0,1))._transformVector(trans);
		float scalarRightRoll = toRight.x()*rot_translation.x() + toRight.y()*rot_translation.y() + toRight.z()*rot_translation.z();
		float scalarUpRoll = toUp.x()*rot_translation.x() + toUp.y()*rot_translation.y() + toUp.z()*rot_translation.z();
		float scalarBackRoll = toBack.x()*rot_translation.x() + toBack.y()*rot_translation.y() + toBack.z()*rot_translation.z();
		
		x += max(abs(scalarRightYaw),max(abs(scalarRightPitch),abs(scalarRightRoll)));
		//if(x>20) x = 20;
		y += max(abs(scalarUpYaw),max(abs(scalarUpPitch),abs(scalarUpRoll)));
		//if(y>20) y = 20;
		z += max(abs(scalarBackYaw),max(abs(scalarBackPitch),abs(scalarBackRoll)));
		//if(z>20) z = 20;

		_translationSpace = Vector3f(x,y,z);
		_rotationSpace = sourceUnc->GetRotationSpace();
		_right = toRight;
		_up = toUp;
	}
	else
	{
		_translationSpace = sourceUnc->GetTranslationSpace();
		_rotationSpace = sourceUnc->GetRotationSpace();
		_right = sourceUnc->GetRigth();
		_up = sourceUnc->GetUp();
	}
	_quality = sourceUnc->GetQualityValue();
	
	//CalculateQuality();
}


void Uncertainty::CalculateQuality()
{
	bool useConeVolume = false;
	float qValue = 0;
	if(!useConeVolume)
	{
		Vector3f xUnit = Vector3f(1,0,0);
		Vector3f yUnit = Vector3f(0,1,0);
		Vector3f zUnit = Vector3f(0,0,1);

		Vector3f xMod = Vector3f(_translationSpace.x(), 0,0);
		Vector3f yMod = Vector3f(0, _translationSpace.x(),0);
		Vector3f zMod = Vector3f(0, 0,_translationSpace.z());

		AngleAxisf rotation = AngleAxisf(AngleAxisf(_rotationSpace.x()* (M_PI/180.f), Vector3f(0,1,0)) *  AngleAxisf(_rotationSpace.y()* (M_PI/180.f), Vector3f(1,0,0))* AngleAxisf(_rotationSpace.z()* (M_PI/180.f), Vector3f(0,0,1)));
		xMod = rotation._transformVector(xMod);
		yMod = rotation._transformVector(yMod);
		zMod = rotation._transformVector(zMod);

		float xDiff = (xUnit-xMod).norm();
		float yDiff = (yUnit-yMod).norm();
		float zDiff = (zUnit-zMod).norm();
		//qDebug() << "diffx : " + QString::number(xDiff) + "  diffy : " + QString::number(yDiff) + "   diffz : " + QString::number(zDiff);

		qValue = (xDiff + yDiff + zDiff)/3.0;
		//qDebug() << "quality :" << QString::number(qValue);
	}
	else
	{
		//the quliaty is the volume of three cones each spanned 
		//by two rotationangles (elliptic groundplane) and one translation space (as height)
			//x-cone
		float height = _translationSpace.x();  //along the x-axis
		float angle1 = _rotationSpace.x();	   //yaw and roll, no pitch
		float angle2 = _rotationSpace.z();
		qValue = GetConeVolume(height, angle1, angle2);

			//y-cone
		height = _translationSpace.x();  //along the y-axis
		angle1 = _rotationSpace.x();	   //roll and pitch, no yaw
		angle2 = _rotationSpace.z();
		qValue += GetConeVolume(height, angle1, angle2);

			//z-cone
		height = _translationSpace.z();  //along the z-axis
		angle1 = _rotationSpace.x();	   //yaw and pitch, no roll
		angle2 = _rotationSpace.z();
		qValue += GetConeVolume(height, angle1, angle2);
	}
	//qDebug() << "quality :" << QString::number(qValue);
	//if(qValue > 20)
	//	qDebug() << "qValue: " + QString::number(qValue);

	if (qValue < 0) qValue = 0;
	//if(qValue < _minQValue) _minQValue = qValue;
	//if(qValue > _maxQValue) _maxQValue = qValue;
	_minQValue = 1;
	_maxQValue = 40;
	if(_minQValue > qValue) qValue = _minQValue;
	if(_maxQValue < qValue) qValue = _maxQValue;
	float val = (qValue - _minQValue)/(_maxQValue-_minQValue);
	
	if(qValue == _maxQValue)
		_quality = 0;
	else
		_quality = 1 - (val*val);
}

float Uncertainty::GetConeVolume(float height, float angle1, float angle2)
{
	return (1.f/3.f) * M_PI * tan(angle1 * (M_PI/180.f)) * tan(angle2 * (M_PI/180.f)) * pow(height,3);
}


//first dummy functions
void Uncertainty::FromDistanceAndPerspective(float distance, float angle)
{
	float x = max(MakeLinearFunction(distance, distanceFactor[0],distanceZero[0]),1.f);
	float y = max(MakeLinearFunction(distance, distanceFactor[1],distanceZero[1]),1.f);
	float z = max(MakeLinearFunction(distance, distanceFactor[2],distanceZero[2]),1.f);

	//x *= angle*angleFactor[0];
	//y *= angle*angleFactor[1];
	//z *= angle*angleFactor[2];
	_translationSpace = Vector3f(x,y,z);	
	
	float yaw = max(MakeLinearFunction(distance, distanceFactor[3],distanceZero[3]),1.f);
	float pitch =max(MakeLinearFunction(distance, distanceFactor[4],distanceZero[4]),1.f);
	float roll = max(MakeLinearFunction(distance, distanceFactor[5],distanceZero[5]),1.f);

	//yaw *= angle*angleFactor[3];
	//pitch *= angle*angleFactor[4];
	//roll *= angle*angleFactor[5];
	_rotationSpace = Vector3f(yaw,pitch,roll);
	CalculateQuality();
}		

void Uncertainty::FromClearance(int areaPx, float edgeSharpness)
{
	float x = max(MakeLinearFunction(areaPx, areaPxFactor[0],areaPxZero[0]),0.f);
	float y = max(MakeLinearFunction(areaPx, areaPxFactor[1],areaPxZero[1]),0.f);
	float z = max(MakeLinearFunction(areaPx, areaPxFactor[2],areaPxZero[2]),0.f);

	//x *= edgeSharpness*edgeSharpnessFactor[0];
	//y *= edgeSharpness*edgeSharpnessFactor[1];
	//z *= edgeSharpness*edgeSharpnessFactor[2];
	_translationSpace = Vector3f(x,y,z);	
	
	float yaw = max(MakeLinearFunction(areaPx, areaPxFactor[3],areaPxZero[3]),0.f);
	float pitch = max(MakeLinearFunction(areaPx, areaPxFactor[4],areaPxZero[4]),0.f);
	float roll = max(MakeLinearFunction(areaPx, areaPxFactor[5],areaPxZero[5]),0.f);

	//yaw *= edgeSharpness*edgeSharpnessFactor[3];
	//pitch *= edgeSharpness*edgeSharpnessFactor[4];
	//roll *= edgeSharpness*edgeSharpnessFactor[5];
	_rotationSpace = Vector3f(yaw,pitch,roll);
	CalculateQuality();
}
