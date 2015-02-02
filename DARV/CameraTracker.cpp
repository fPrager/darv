#include "stdafx.h"
#include "CameraTracker.h"


CameraTracker::CameraTracker(MarkerUsing mUsing, Vector3f initPosition, AngleAxisf initRotation)
{
	_mUsing = mUsing;
	_camUnc = new Uncertainty();
	_position = new Vector3f(initPosition);
	_rotation = new Quaternionf(initRotation);
	_prevRotation = new Quaternionf(initRotation);
	_latestRotationFromMarker =new Quaternionf(initRotation);
	_expectedAccelMagn = 0;
	_quality = 0;
	_justFlexibleMarker = false;
	_doClassification = true;
}


CameraTracker::~CameraTracker()
{

}

void CameraTracker::UseThisCameraData(Vector3f* position, Quaternionf* rotation)
{
	*_position = *position;
	*_rotation = *rotation;
	_quality = 1;
}

void CameraTracker::ToggleClassification(bool flag)
{
	_doClassification = flag;
}

void CameraTracker::Update(QList<Marker> realMarker, QList<VirtualMarker*> fixedMarker, QList<VirtualMarker*> freeMarker, float elapsedTime, bool useReference)
{
	_elapsedTime = elapsedTime;
	//look which marker is of interest
	//foreach wanted marker
		//set the tracked information
	for(int i = 0; i < realMarker.size(); i++)
	{
		for(int j = 0; j < fixedMarker.size(); j++)
		{
			if(fixedMarker[j]->_id == realMarker[i]._CosID)
			{
				if(!useReference)
				{
					fixedMarker[j]->SetTracked(realMarker[i]._isTracked);
					fixedMarker[j]->_qualityVision = realMarker[i]._quality;
					fixedMarker[j]->SetYawPitchRoll(realMarker[i]._yaw, realMarker[i]._pitch,realMarker[i]._roll);
				}
				else
				{
					fixedMarker[j]->SetTracked(realMarker[i]._hasRefData);
					fixedMarker[j]->_qualityVision = 0.5;
					fixedMarker[j]->SetYawPitchRoll(0, 0,0);
				}

				if(!_doClassification)
				{
					fixedMarker[j]->_qualityTracking = 1;
					if(fixedMarker[j]->_isTracked)
						fixedMarker[j]->_state = GOODVIEW;
					else
						fixedMarker[j]->_state = UNTRACKED;
				}
				//where? (in relation to the WEBCAM)
				//according to some reference
				fixedMarker[j]->SetReferenceData(realMarker[i]._hasRefData,realMarker[i]._translationFromReference,realMarker[i]._rotationFromReference);
				//according to metaio
				if(useReference)
				{
					if(realMarker[i]._hasRefData)
					{
						fixedMarker[j]->Update(realMarker[i]._translationFromReference,realMarker[i]._rotationFromReference);
						fixedMarker[j]->_uncer.SetKnown(false);
					}
					else
					{
						fixedMarker[j]->_areaPx = 0;
						fixedMarker[j]->_widthPx = 0;
						fixedMarker[j]->_heightPx = 0;
					}
				}
				else
				{
					if(realMarker[i]._isTracked)
					{
						fixedMarker[j]->Update(realMarker[i]._translationFromCamera,realMarker[i]._rotationFromCamera,elapsedTime);
						fixedMarker[j]->CalculateExpectedMovement(ExpectedRotationAngle());
						fixedMarker[j]->UpdateUncertainty(elapsedTime);
					}
					else
					{
						fixedMarker[j]->_areaPx = 0;
						fixedMarker[j]->_widthPx = 0;
						fixedMarker[j]->_heightPx = 0;
					}
				}
			}
		}
		for(int j = 0; j < freeMarker.size(); j++)
		{
			if(freeMarker[j]->_id == realMarker[i]._CosID)
			{
				if(!useReference)
				{
					freeMarker[j]->SetTracked(realMarker[i]._isTracked);
					freeMarker[j]->_qualityVision = realMarker[i]._quality;
					freeMarker[j]->SetYawPitchRoll(realMarker[i]._yaw, realMarker[i]._pitch,realMarker[i]._roll);
				}
				else
				{
					freeMarker[j]->SetTracked(realMarker[i]._hasRefData);
					freeMarker[j]->_qualityVision = 0.5;
					freeMarker[j]->SetYawPitchRoll(0, 0,0);
				}
				
				
				if(!_doClassification)
				{
					freeMarker[j]->_qualityTracking = 1;
					if(freeMarker[j]->_isTracked)
						freeMarker[j]->_state = GOODVIEW;
					else
						freeMarker[j]->_state = UNTRACKED;
				}
				//where? (in relation to the WEBCAM)
				//according to some reference
				freeMarker[j]->SetReferenceData(realMarker[i]._hasRefData,realMarker[i]._translationFromReference,realMarker[i]._rotationFromReference);
				//according to metaio
				if(useReference)
				{
					if(realMarker[i]._hasRefData)
					{
						freeMarker[j]->Update(realMarker[i]._translationFromReference,realMarker[i]._rotationFromReference, elapsedTime);
						freeMarker[j]->_uncer.SetKnown(false);
					}
					else
					{
						freeMarker[j]->_areaPx = 0;
						freeMarker[j]->_widthPx = 0;
						freeMarker[j]->_heightPx = 0;
					}
				}
				else
				{
					if(realMarker[i]._isTracked)
					{
						freeMarker[j]->Update(realMarker[i]._translationFromCamera,realMarker[i]._rotationFromCamera,elapsedTime);
						freeMarker[j]->CalculateExpectedMovement(0);
						freeMarker[j]->UpdateUncertainty(elapsedTime);
					}
					else
					{
						freeMarker[j]->_areaPx = 0;
						freeMarker[j]->_widthPx = 0;
						freeMarker[j]->_heightPx = 0;
					}
				}
			}
		}
	}

	if(fixedMarker.size() != 0)
	{
		if(_doClassification)
			VerifyMarkerData(fixedMarker);
		CalculateCameraData(fixedMarker);
	}
	else
		_justFlexibleMarker = true;

	UpdateFreeMarker(freeMarker);
	//qDebug() << "Rotation : " << _rotation.angle() << " - " <<  _rotation.axis().x() << " - " <<  _rotation.axis().y() << " - " << _rotation.axis().z();
}

void CameraTracker::VerifyMarkerData(QList<VirtualMarker*> marker)
{
	QList<VirtualMarker*> trackedM = QList<VirtualMarker*>();
	bool favourMoveOverAccel = true;
	
	for(int i = 0; i<marker.size(); i++)
	{
		if(marker[i]->_hasDeltaValues)
			trackedM.append(marker[i]);

		if(marker[i]->_hasAccelValue && _expectedAccelMagn != 0)
			favourMoveOverAccel = false;
	}

	if(trackedM.size() == 0)
	{
		for(int i = 0; i<marker.size(); i++)
		{
			//TRACKINGQUALITY: if we have no initial tracking values, we aren't able to compare tracking changes, so we trust in the first values we get from the framework
			if(marker[i]->_isTracked)
				marker[i]->_qualityTracking = 0.01f;
		}
		return;
	}

	if(favourMoveOverAccel)
		SetStabilityFromMoving(trackedM);
	else
		SetStabilityFromAcceleration(trackedM);

	//first of all, check per marker if it has a rotation-change which results in possible cameraposition-change
	float maximalHumanSpeed = 100 / 3.6f; //in m/s
	int i = trackedM.size();
	for(int i = 0; i<trackedM.size();i++)
	{
		float camSpeed = trackedM[i]->_cameraSpeed->norm();

		if(camSpeed > maximalHumanSpeed)
		{
			trackedM[i]->_state = FLICKERING;
			trackedM[i]->_qualityTracking = 0.0001f;
		}
		else
		{
			float visionFactor = (trackedM[i]->_qualityVision-0.5f)/0.25f + 2;

			//float increasing = 1 + trackedM[i]->_stabilityFactor + visionFactor;
			//trackedM[i]->_qualityTracking = trackedM[i]->_qualityTracking * increasing;
			float increasing = 1 + visionFactor;
			trackedM[i]->_qualityTracking = trackedM[i]->_qualityTracking * increasing;

			if(trackedM[i]->_qualityTracking < 0.0001f) trackedM[i]->_qualityTracking = 0.0001f;

			//qDebug() << i << " - " << QString::number(trackedM[i]->_qualityTracking) << " - " << QString::number(stabilityFactor) << " - " << QString::number(increasing);
			if(trackedM[i]->_qualityTracking > 1.f)
			{
				trackedM[i]->_qualityTracking = 1.f;
				trackedM[i]->_state = GOODVIEW;
			}
			else
			/*if(trackedM[i]->_qualityTracking >= 0.3f)*/
			{
				trackedM[i]->_state = BADVIEW;
			}
			//else
			//if(trackedM[i]->_qualityTracking > 0.0001f)
			//{
			//	trackedM[i]->_state = MAVERICK;
			//}
			//else
			//{
			//	trackedM[i]->_qualityTracking = 0.0001f;
			//}
		}
	}

	if(trackedM.size() == 0)
	{
		for(int i = 0; i<marker.size(); i++)
		{
			//TRACKINGQUALITY: if we have no initial tracking values, we aren't able to compare tracking changes, so we trust in the first values we get from the framework
			if(marker[i]->_isTracked)
			{

				marker[i]->_qualityTracking = 0.5f;
			}
		}
		return;
	}
}

void	CameraTracker::SetStabilityFromAcceleration(QList<VirtualMarker*> marker)
{
	float nearAccel = 100;
	float farAccel = 0;

	for(int i = 0; i < marker.size();i++)
	{
		if(!marker[i]->_hasAccelValue)
			continue;
		float disFromExpected = abs(marker[i]->_averageCameraAccel - _expectedAccelMagn);

		if(disFromExpected < nearAccel)
		{
			nearAccel = disFromExpected;
		}
		if(disFromExpected > farAccel)
		{
			farAccel = disFromExpected;
		}
	}

	for(int i = 0; i < marker.size();i++)
	{
		float disFromExpected = 0;

		if(!marker[i]->_hasAccelValue)
			disFromExpected = farAccel;
		else
		    disFromExpected = abs(marker[i]->_cameraAccel - _expectedAccelMagn);

		float stabilityFactor = 1 - (disFromExpected - nearAccel)/farAccel;
		stabilityFactor -= 0.5;
		marker[i]->_stabilityFactor = stabilityFactor;
	}
}

void	CameraTracker::SetStabilityFromMoving(QList<VirtualMarker*> marker)
{
	float lowestMove = 100;
	float highestMove = 0;
	for(int i = 0; i < marker.size();i++)
	{
		if(marker[i]->_averageCameraMove < lowestMove)
		{
			lowestMove = marker[i]->_averageCameraMove;
		}
		if(marker[i]->_averageCameraMove > highestMove)
		{
			highestMove = marker[i]->_averageCameraMove;
		}
	}

	for(int i = 0; i < marker.size();i++)
	{
		float stabilityFactor = 1 - (marker[i]->_averageCameraMove - lowestMove)/highestMove;
		stabilityFactor -= 0.5;
		marker[i]->_stabilityFactor = stabilityFactor;
	}
}

void	CameraTracker::UseFilter()
{
}

void CameraTracker::GetMarkerQualityInfo(QList<VirtualMarker*> marker, float* bestQuality, int* bestQualityIndex, float* qualitySum)
{
	for(int i = 0; i < marker.size(); i++)
	{
		if(marker[i]->_state == BADVIEW || marker[i]->_state == GOODVIEW || marker[i]->_state == BESTVIEW)
		{
			*qualitySum += marker[i]->_qualityTracking;
			if(marker[i]->_qualityTracking > *bestQuality)
			{
				*bestQuality = marker[i]->_qualityTracking;
				*bestQualityIndex = i;
			}
			else
			if(marker[i]->_qualityTracking == *bestQuality)
			{
				if(marker[i]->_state == BESTVIEW)
					*bestQualityIndex =i;
			}
		}
	}
	if(marker[*bestQualityIndex]->_state == GOODVIEW)
	{
		marker[*bestQualityIndex]->_state = BESTVIEW;
	}
}

void CameraTracker::GetRotationFromMarker(QList<VirtualMarker*> marker, MarkerUsing mUsing, float* rotationQuality, Quaternionf* camRot)
{
	//*camRot = Quaternionf(AngleAxisf(0, Vector3f(0,1,0)));
	float	  bestQuality = 0;
	float	  qualitySum = 0;
	int		  bestMIndex = 0;
	float	  weight = 0;
	GetMarkerQualityInfo(marker, &bestQuality, &bestMIndex, &qualitySum);

	*rotationQuality = bestQuality;
	for(int i = 0; i < marker.size(); i++)
	{
			marker[i]->_weight = 0;
	}

	if(bestQuality != 0)
	{
		switch(mUsing)
		{
			case BEST_MARKER:
				{
					marker[bestMIndex]->_weight = 1;
					Quaternionf camRotationOfThis = *marker[bestMIndex]->_trackedRotation;
					camRotationOfThis = camRotationOfThis* marker[bestMIndex]->_rotationInSetup->inverse();
					*camRot = camRotationOfThis.inverse();
				}
				break;

			case AVERAGE_MARKER:
				{
					Quaternionf rotation = Quaternionf(1,0,0,0);
					AngleAxisf rot = AngleAxisf(0, Vector3f(0,0,0));
					Vector3f vector3Sum = Vector3f(0,0,0);
					int mCount = 0;
					//qDebug() << "-----";
					for(int i = 0; i < marker.size(); i++)
					{
						if(marker[i]->_state == BADVIEW || marker[i]->_state == GOODVIEW || marker[i]->_state == BESTVIEW)
						{
							marker[i]->_weight = marker[i]->_qualityTracking/qualitySum;
							//qDebug()<< i << "  " << marker[i]->_weight;

							Quaternionf camRotationOfThis = *marker[i]->_trackedRotation;
							camRotationOfThis = camRotationOfThis* marker[i]->_rotationInSetup->inverse();
							camRotationOfThis = camRotationOfThis.inverse();
							
							if(mCount == 0)
								rotation = camRotationOfThis;
							else
								rotation = rotation.slerp(marker[i]->_weight, camRotationOfThis);
							float weight = marker[i]->_weight;
							AngleAxisf rotOfThis = AngleAxisf(camRotationOfThis);
							rot.angle() += rotOfThis.angle()*weight;
							rot.axis() += rotOfThis.axis()*weight;
							//qDebug() << i << " - " << QString::number(rotOfThis.angle()) << " - " << QString::number(rotOfThis.axis().x()) << " - " << QString::number(rotOfThis.axis().y()) << " - " << QString::number(rotOfThis.axis().z());
							mCount ++;
						}
					}
					//qDebug() <<"--- " << QString::number(rot.angle()) << " - " << QString::number(rot.axis().x()) << " - " << QString::number(rot.axis().y()) << " - " << QString::number(rot.axis().z());
					*camRot = rotation;
					//if(mCount != 0)
					//	*camRot = rotation;
					//else
					//	*camRot = _rotation;
				}
				break;
			case FUSING_MARKER:
				{
					QList<Quaternionf> rotations = QList<Quaternionf>();
					QList<float*> qualities = QList<float*>();
					QList<float*>  weights = QList<float*>();
					for(int i = 0; i < marker.size(); i++)
					{
						if(marker[i]->_state == GOODVIEW || marker[i]->_state == BADVIEW || marker[i]->_state == BESTVIEW)
						{
							float weight = marker[i]->_qualityTracking/qualitySum;
							Quaternionf camRotationOfThis = *marker[i]->_trackedRotation;
							camRotationOfThis = camRotationOfThis* marker[i]->_rotationInSetup->inverse();
							rotations.append(camRotationOfThis.inverse());
							qualities.append(&marker[i]->_qualityVision);
							weights.append(new float(weight));
						}
					}

					VerifyMarkerRotations(rotations,qualities,weights);
					
					Quaternionf rotation = Quaternionf(1,0,0,0);
					for(int i = 0; i < rotations.count(); i++)
					{
						float weight = *weights[i];
						if(weight != 0)
						{
							qDebug() <<"weight "<<QString::number(weight);
							rotation = rotation.slerp(weight,rotations[i]);
						}
					}
					*camRot = rotation;
				}
				break;
		}
	}
}


void CameraTracker::VerifyMarkerRotations(QList<Quaternionf> rotations, QList<float*> qualitys, QList<float*> weights)
{
	//if we have just one rotation it's not neccessary to check which rotation is the best or most trustful
	if(rotations.count() < 2)
		return;
	//two rotations are the same if the differencial rotation is less than the angleThresshold°(in degree)
	float angleThresshold = 7;

	//the list of equal rotations (per rotation)
	QList<int> equalitiesPerRotation = QList<int>();
	int maxEqualities = 1;
	int maxEqualitiesCount = 0;
	for(int i = 0; i < rotations.count(); i++)
	{
		int equalRotationsWithThat = 0;
		for(int j = 0; j < rotations.count(); j++)
		{
			AngleAxisf  diff = AngleAxisf(rotations[i].inverse()* rotations[j]);
			float diffAngle = abs(diff.angle()*(180.0f/M_PI));
			qDebug() << "index: " << i << " " << QString::number(diffAngle);
			//increase the count of equal rotations
			if(diffAngle < angleThresshold && i != 0)
				equalRotationsWithThat++;
		}
		equalitiesPerRotation.append(equalRotationsWithThat);
		if(equalRotationsWithThat > maxEqualities)
		{
			//we found a new group of equal rotations
			//equalRotationsWithThat is an indcator of the group size (from this perspective)
			maxEqualities = equalRotationsWithThat;
			maxEqualitiesCount = 1;
		}
		else
		if(equalRotationsWithThat == maxEqualities)
		{
			//if this rotation is a member of the big group than increase the group size number 
			maxEqualitiesCount++;
		}
	}

	//we know the biggest match of rotations in maxEqualities
	//now we have to check if we have more than one group of lucky euqal rotations
	//we have to find such a lists like : g0 - g1  - g2 - g2 - g1 ....or.... g0 - g0
	//why? the most trustfull rotation is that rotation which isn't alone with their "opinion" (value)
	//but we have to deal with rotations which have the same number BUT DIFFRENT! backers 
	//than we have to choose the most trustfull rotation-gang

	//how? we now the maximal number of backers in maxEqualities 
	//and we know the number of rotations which have that number of backers
	if(maxEqualities < maxEqualitiesCount)
	{
		//we have more than one rotation-gang
		if(maxEqualities < 2)
		{
			int bestRotation = IndexOfMostTrustedMarkerRotation(rotations);
			for(int i = 0; i< qualitys.count(); i++)
			{
				if(i == bestRotation)
				{
					*weights[i] = 1;
					*qualitys[i] = 0.8f;
				}
				else
				{
					*weights[i] = 0;
					*qualitys[i] = 0;
				}
			}
		}
		else
		{
			qDebug() << "TODO: HANDLE TOO MANY ROTATION GANGS!";
		}
	}
	else
	{
		//we have just one rotation gang
		for(int i = 0; i < equalitiesPerRotation.count(); i++)
		{
			if(equalitiesPerRotation[i] == maxEqualities)
			{
				*weights[i] = 1/(float)maxEqualitiesCount;
				*qualitys[i] = 1;
			}
			else
			{
				qDebug() << "Found a buggy rotation!!!";
				*weights[i] = 0;
				*qualitys[i] = 0;
			}
		}
	}
}

int CameraTracker::IndexOfMostTrustedMarkerRotation(QList<Quaternionf> rotations)
{
	//every rotation is diffrent here, so wich is the best?
	//maybe that rotation with the smallest difference to the previous rotation
	int index = 0;
	float minDelta = -1;
	for(int i = 0; i < rotations.count(); i++)
	{
		AngleAxisf deltaRot = AngleAxisf(_prevRotationFromMarker->inverse() * rotations[i]);
		float deltaAngle = abs(deltaRot.angle() * (180.0f/M_PI));
		if(minDelta < 0 || deltaAngle < minDelta)
		{
			minDelta =  deltaAngle;
			index = i;
		}
	}
	return index;
}

void CameraTracker::GetPositionFromMarker(QList<VirtualMarker*> marker, MarkerUsing mUsing, Quaternionf* camRot, float* positionQuality, Vector3f* camPos)
{
	//a fixed marker of the setup casts light on the WEBCAM-position in that setup
		//e.g. we know the marker id1 is in the downer left corner of the setup "Holodeck"
		//		the webcam sees it from this distance and direction, so we know the webcam must be here
	
	*positionQuality	  = 0;

	int		  trackedMarker = 0;
	float	  bestQuality = 0;
	float	  qualitySum = 0;
	int		  bestMIndex = 0;

	GetMarkerQualityInfo(marker, &bestQuality, &bestMIndex, &qualitySum);
	if(bestQuality != 0)
	{
		*positionQuality = 1;
		switch(mUsing)
		{
			case BEST_MARKER:
				{
					Vector3f fromCameraToMarkerInCameraSpace = *(marker[bestMIndex]->_trackedTranslation);
					Vector3f fromCameraToMarkerInWorldSpace = camRot->_transformVector(fromCameraToMarkerInCameraSpace);
					Vector3f fromMarkerToCameraInWorldSpace = -fromCameraToMarkerInWorldSpace;
				
					Vector3f camPositionOfThis = *(marker[bestMIndex]->_positionInSetup) + fromMarkerToCameraInWorldSpace;  //where is the camera in world space? : go in the direction of the marker and than go to the camera, there is it
					*camPos = camPositionOfThis;
					_camUnc->FromReference(&(marker[bestMIndex]->_uncer), &fromMarkerToCameraInWorldSpace);
				}
				break;

			case FUSING_MARKER: case AVERAGE_MARKER:
				{
					Vector3f position = Vector3f(0,0,0);
					AngleAxisf rotation = AngleAxisf(0, Vector3f(0,0,0));
					int trackedMarker = 0;
					for(int i = 0; i < marker.size(); i++)
					{
						if(marker[i]->_isTracked && marker[i]->IsConstant())
						{
							trackedMarker++;
						}
					}
					for(int i = 0; i < marker.size(); i++)
					{
						if(marker[i]->_isTracked && marker[i]->IsConstant())
						{
							float weight = (float)1/(float)trackedMarker;

							Vector3f fromCameraToMarkerInCameraSpace = *(marker[bestMIndex]->_trackedTranslation);
							Vector3f fromCameraToMarkerInWorldSpace = camRot->_transformVector(fromCameraToMarkerInCameraSpace);
							Vector3f fromMarkerToCameraInWorldSpace = -fromCameraToMarkerInWorldSpace;
				
							Vector3f camPositionOfThis = *(marker[bestMIndex]->_positionInSetup) + fromMarkerToCameraInWorldSpace;  //where is the camera in world space? : go in the direction of the marker and than go to the camera, there is it
							position += camPositionOfThis * weight;
						}
					}
					*camPos = position;
				}
				break;
		}
	}

}

void CameraTracker::UpdateFreeMarker(QList<VirtualMarker*> marker)
{
	//foreach flexible virtuell marker
	for(int i = 0; i < marker.size(); i++)
	{
		//a free (unfixed) marker has just information relative to the webcam
		if(_quality > 0)
		{
			// to get know where it is relative to the room, we have to know, where the camera is
			Vector3f translation = _rotation->_transformVector(*(marker[i]->_trackedTranslation));
			*(marker[i]->_positionInSetup) = *_position + translation;
			*(marker[i]->_rotationInSetup) = *_rotation * *(marker[i]->_trackedRotation);
		}
		else
		{
			*(marker[i]->_positionInSetup) = *(marker[i]->_trackedTranslation);
			*(marker[i]->_rotationInSetup) = *(marker[i]->_trackedRotation);
		}
	}
}

void CameraTracker::GetCameraData(float* quality, Vector3f* position, AngleAxisf* rotation)
{
	float uncQuality = _camUnc->GetQualityValue();
	if(_doClassification)
		_overallQuality = min(_quality,uncQuality);

	*quality = _overallQuality;
	*position = *_position;
	*rotation = AngleAxisf(*_rotation);
}

AngleAxisf* CameraTracker::GetLatestRotationFromMarker()
{
	return new AngleAxisf(*_latestRotationFromMarker);
}

AngleAxisf* CameraTracker::GetLatestRotation()
{
	return new AngleAxisf(*_rotation);
}

Vector3f* CameraTracker::GetLatestPosition()
{
	return _position;
}

void CameraTracker::GetYawPitchRollFromMarker(float* yaw, float* pitch, float* roll)
{
	double w,x,y,z;
	w = _latestRotationFromMarker->w(); 
	x = _latestRotationFromMarker->x(); 
	y = _latestRotationFromMarker->y(); 
	z = _latestRotationFromMarker->z();

	*yaw = atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z) * (180.0f/M_PI) ;
	*roll = atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z) * (180.0f/M_PI);
	*pitch   = asin(2*x*y + 2*z*w) * (180.0f/M_PI);
}

float CameraTracker::GetAccelFromMarker()
{
	return 0;
}

void CameraTracker::GetYawPitchRoll(float* yaw, float* pitch, float* roll)
{
	double w,x,y,z;
	w = _rotation->w(); 
	x = _rotation->x(); 
	y = _rotation->y(); 
	z = _rotation->z();

	*yaw = atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z) * (180.0f/M_PI) ;
	*roll = atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z) * (180.0f/M_PI);
	*pitch   = asin(2*x*y + 2*z*w) * (180.0f/M_PI);
}

float CameraTracker::GetAccel()
{
	return 0;
}

float CameraTracker::GetQuality()
{
	return _overallQuality;
}

void CameraTracker::SetQuality(float value)
{
	_overallQuality = value;
}