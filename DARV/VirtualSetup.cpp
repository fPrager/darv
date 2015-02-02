#include "stdafx.h"
#include "VirtualSetup.h"

VirtualSetup::VirtualSetup(QString name, VirtualRoom* room, QList<VirtualMarker*> marker, QList<VirtualObject*> objects)
{
	_name = name;
	_room = room;
	_allMarker = marker;
	UpdateMarkerLists();
	//_knownCamera = false;

	_objects = objects;
}

void VirtualSetup::Load()
{
	for(int i = 0; i<_objects.size(); i++)
		_objects[i]->Load();
}

void VirtualSetup::UpdateMarkerLists()
{
	//TODO: this is a stupid bug-fix
	//for unkown reason the pointers of the fixedMarker-List will direct to undefined marker after a change of the selected setup in VirtualReality

	_fixedMarker = QList<VirtualMarker*>();
	_freeMarker = QList<VirtualMarker*>();
	for(int i = 0; i<_allMarker.size(); i++)
	{
		if(_allMarker[i]->_isFixed)
			_fixedMarker.append(_allMarker[i]);
		else
			_freeMarker.append(_allMarker[i]);
	}
}

//void VirtualSetup::ComputeCameraDataFromMarker(QList<VirtualMarker*> marker, bool useBestMarker, Vector3f* camPos, AngleAxisf* camRot)
//{
//	//a fixed marker of the setup casts light on the WEBCAM-position in that setup
//		//e.g. we know the marker id1 is in the downer left corner of the setup "Holodeck"
//		//		the webcam sees it from the distance, so we know the webcam must be here
//	float	  bestQuality = 0;
//	float	  qualitySum = 0;
//	int		  bestMIndex = 0;
//	int		  trackedMarker = 0;
//	_knownCamera = false;
//
//	for(int i = 0; i < _fixedMarker.size(); i++)
//	{
//		if(_fixedMarker[i]->_isTracked)
//		{
//			qualitySum +=_fixedMarker[i]->_quality;
//			if(_fixedMarker[i]->_quality > bestQuality)
//			{
//				bestQuality = _fixedMarker[i]->_quality;
//				bestMIndex = i;
//			}
//		}
//	}
//
//	if(bestQuality != 0)
//	{
//		_knownCamera = true;
//		if(useBestMarker)
//		{
//
//				AngleAxisf camRotationOfThis = AngleAxisf(marker[bestMIndex]->_trackedRotation->angle(), marker[bestMIndex]->_trackedRotation->axis());
//				camRotationOfThis = camRotationOfThis* marker[bestMIndex]->_rotationInSetup->inverse();
//				*camRot = camRotationOfThis.inverse();
//
//				Vector3f fromCameraToMarkerInCameraSpace = *(marker[bestMIndex]->_trackedTranslation);
//				Vector3f fromCameraToMarkerInWorldSpace = camRotationOfThis.inverse()._transformVector(fromCameraToMarkerInCameraSpace);
//				Vector3f fromMarkerToCameraInWorldSpace = -fromCameraToMarkerInWorldSpace;
//				
//				Vector3f camPositionOfThis = *(marker[bestMIndex]->_positionInSetup) + fromMarkerToCameraInWorldSpace;  //where is the camera in world space? : go in the direction of the marker and than go to the camera, there is it
//				*camPos = camPositionOfThis;
//		}
//		else
//		{
//			Vector3f position = Vector3f(0,0,0);
//			AngleAxisf rotation = AngleAxisf(0, Vector3f(0,0,0));
//
//			for(int i = 0; i < _fixedMarker.size(); i++)
//			{
//				if(_fixedMarker[i]->_isTracked)
//				{
//					float weight = _fixedMarker[i]->_quality/qualitySum;
//
//					AngleAxisf camRotationOfThis = AngleAxisf(marker[i]->_trackedRotation->angle(), marker[i]->_trackedRotation->axis());
//					camRotationOfThis = camRotationOfThis* marker[i]->_rotationInSetup->inverse();
//					rotation.angle() += camRotationOfThis.inverse().angle() * weight;
//					rotation.axis() += camRotationOfThis.inverse().axis() * weight;
//
//					Vector3f fromCameraToMarkerInCameraSpace = *(marker[bestMIndex]->_trackedTranslation);
//					//qDebug() << "cam to marker "+QString::number(fromCameraToMarkerInCameraSpace.x()) + " " +QString::number(fromCameraToMarkerInCameraSpace.y()) + " "+QString::number(fromCameraToMarkerInCameraSpace.z()); 
//					//qDebug() << "Marker "+QString::number(i) + " weight "+QString::number(weight) + " from "+QString::number(qualitySum); 
//					Vector3f fromCameraToMarkerInWorldSpace = camRotationOfThis.inverse()._transformVector(fromCameraToMarkerInCameraSpace);
//					Vector3f fromMarkerToCameraInWorldSpace = -fromCameraToMarkerInWorldSpace;
//				
//					Vector3f camPositionOfThis = *(marker[bestMIndex]->_positionInSetup) + fromMarkerToCameraInWorldSpace;  //where is the camera in world space? : go in the direction of the marker and than go to the camera, there is it
//					position += camPositionOfThis * weight;
//				}
//			}
//			*camPos = position;
//			*camRot = rotation;
//		}
//	}
//
//	//save the state of the camera if the position is known or not
//	if(!_knownCamera)
//	{
//		//set the camera to the origin
//		*camPos = Vector3f();
//		*camRot = AngleAxisf(0, Vector3f(0,1,0));
//	}
//	
//}

void VirtualSetup::Update(float sizeFactor)
{
	//look which marker is of interest
	//foreach wanted marker
		//set the calculated information
	//for(int i = 0; i < marker.size(); i++)
	//{
	//	for(int j = 0; j < _marker.size(); j++)
	//	{
	//		if(_marker[j]._id == marker[i]._CosID)
	//		{
	//				_marker[j]._isTracked = marker[i]._isTracked;
	//				_marker[j]._quality = marker[i]._quality;
	//				//where? (in relation to the WEBCAM)
	//				_marker[j]._trackedTranslation = marker[i]._translationFromCamera;
	//				_marker[j]._trackedRotation = marker[i]._rotationFromCamera;
	//		}
	//	}
	//}

	//now calculate the webcam position in relation to the setup
	//and the marker positions in relation of the setup
	//foreach fixed virtuell marker
	//ComputeCameraDataFromMarker(_fixedMarker, false, &_cameraPosition, &_cameraRotation);

	//foreach flexible virtuell marker
	//for(int i = 0; i < _freeMarker.size(); i++)
	//{
	//	//a free (unfixed) marker has just information relative to the webcam
	//	if(_knownCamera)
	//	{
	//		// to get know where it is relative to the room, we have to know, where the camera is
	//		AngleAxisf reverseCamRot = AngleAxisf(-_cameraRotation.angle(), _cameraRotation.axis());
	//		Vector3f translation = _cameraRotation._transformVector(*(_freeMarker[i]->_trackedTranslation));
	//		*(_freeMarker[i]->_positionInSetup) = _cameraPosition + translation;
	//		*(_freeMarker[i]->_rotationInSetup) = _cameraRotation* *(_freeMarker[i]->_trackedRotation);
	//	}
	//	else
	//	{
	//		*(_freeMarker[i]->_positionInSetup) = *(_freeMarker[i]->_trackedTranslation);
	//		*(_freeMarker[i]->_rotationInSetup) = *(_freeMarker[i]->_trackedRotation);
	//	}
	//}
	Uncertainty* masterMarkerUnc = 0;
	for(int i = 0; i < _allMarker.count(); i++)
	{
		if(_allMarker[i]->_weight == 1)
		{
			masterMarkerUnc = &(_allMarker[i]->_uncer);
			masterMarkerUnc->SetAbsoluteCenter(*_allMarker[i]->_positionInSetup);
		}
	}

	//foreach virtual object set the translation and rotation
	for(int i = 0; i<_objects.size(); i++)
	{
		if(_objects[i]->_attachedToMarker)
		{
		//is the object attached to a marker
			//look for that marker
			for(int j = 0; j < _allMarker.size(); j++)
				if(_allMarker[j]->_id == (_objects[i]->_markerID))
				{
					//if (fixed marker or tracked free marker)-> use its position and rotation in the room
					//else -> save as unkown position
					if(_allMarker[j]->_isTracked)
					{
						*(_objects[i]->_translation) = *(_allMarker[j]->_positionInSetup);
						*(_objects[i]->_rotation) = *(_allMarker[j]->_rotationInSetup);
						_allMarker[j]->_uncer.SetAbsoluteCenter(*_allMarker[j]->_positionInSetup);
						_objects[i]->_sourceUncertainty = &(_allMarker[j]->_uncer);

						if(_objects[i]->_scaleFactor < 1)
							_objects[i]->ChangeSize(abs(sizeFactor));
					}
					else
					{
						if(_objects[i]->_scaleFactor > 0)
							_objects[i]->ChangeSize(-abs(sizeFactor));
					}
				}
		}
		else
		{
		//or is it fixed in the room
			//in this case the position is always the same, the visibilty depands on the view (camera position and rotation)
			_objects[i]->ChangeSize(sizeFactor);
			//and the uncertainty depends on the uncertainty of the origin!
			_objects[i]->_sourceUncertainty = masterMarkerUnc;
		}
		_objects[i]->UpdateUncertainty(sizeFactor);
	}
}

void VirtualSetup::ChangeDARV(bool flag)
{
	for(int i = 0; i<_objects.count(); i++)
		_objects[i]->_makeDARV = flag;
}

QList<VirtualObject*> VirtualSetup::GetObjectsToDraw(bool useTracking)
{
	//make empty list of objects
	QList<VirtualObject*> objectsToDraw = QList<VirtualObject*>();

	//foreach object
	//is it fixed in the room and cameraposition known?
		//check -> add to list
	//is it attached to a marker and HIS position is known?
		//check -> add to list

	for(int i = 0; i<_objects.size(); i++)
	{
		if(useTracking)
		{
			//look on the scale factor of object
			//its an indicator of the visbility of the object
			if(_objects[i]->_scaleFactor > 0)
				objectsToDraw.append(_objects[i]);
		}
		else
		{
			//we doesn't use the tracking states, so we show the objects which want to be drawn in tis mode too 
			//this possiblity refers to simple calibration methods where it is nesseccary to draw something
			if(!_objects[i]->_justInTrackingMode)
				objectsToDraw.append(_objects[i]);
		}
	}
	return objectsToDraw;
}

QString VirtualSetup::GetName()
{
	return _name;
}

//glm::mat4 VirtualSetup::GetViewMatrix()
//{
//	//if the camera location is known, than combine position and rotation to viewmatrix
//	//if not, return the identity matrix
//	if(_knownCamera)
//	{
//
//		glm::mat4 translate = glm::translate(glm::mat4(1.f), -glm::vec3(_cameraPosition.x(),_cameraPosition.y(),_cameraPosition.z()));	
//		glm::mat4 rotate = glm::rotate(glm::mat4(1.f),-glm::degrees(_cameraRotation.angle()),glm::vec3(_cameraRotation.axis().x(),_cameraRotation.axis().y(),_cameraRotation.axis().z()));
//
//		return rotate*translate;
//	}
//	else
//		return glm::mat4(1.f);
//}
//
//glm::mat4 VirtualSetup::GetWorldMatrix()
//{
//	//if the camera location is known, than combine position and rotation to viewmatrix
//	//if not, return the identity matrix
//	if(_knownCamera)
//	{
//		glm::mat4 translate = glm::translate(glm::mat4(1.f), -glm::vec3(-_cameraPosition.x(),_cameraPosition.y(),_cameraPosition.z()));	
//		glm::mat4 rotate = glm::rotate(glm::mat4(1.f), glm::degrees(_cameraRotation.angle()),glm::vec3(_cameraRotation.axis().x(),_cameraRotation.axis().y(),_cameraRotation.axis().z()));
//		
//		return rotate;
//	}
//	else
//		return glm::mat4(1.f);
//}

void VirtualSetup::GetVirtualMarker(QList<VirtualMarker*>* allMarker)
{
	*allMarker = _allMarker;
}

void VirtualSetup::GetVirtualMarker(QList<VirtualMarker*>* fixedMarker, QList<VirtualMarker*>* freeMarker)
{
	*fixedMarker = _fixedMarker;
	*freeMarker = _freeMarker;
}

VirtualRoom* VirtualSetup::GetRoom()
{
	return _room;
}

//void VirtualSetup::GetCameraData(bool* isKnown, Vector3f* position, AngleAxisf* rotation)
//{
//	*isKnown = _knownCamera;
//	*position = _cameraPosition;
//	*rotation = _cameraRotation;
//}