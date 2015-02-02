#include "stdafx.h"
#include "OBJModels.h"


OBJModels::OBJModels(QString modelName, QString textureName, float modelScale, Vector3f* translation, AngleAxisf* rotation): VirtualObject(modelName,translation, rotation, true)
{
	_textureName = textureName;
	_modelName = modelName;
	_modelScaleFactorX = modelScale;
	_modelScaleFactorY = modelScale;
	_modelScaleFactorZ = modelScale;
	for(int i = 0; i < AVERAGE_ANGLE; i++)
	{
		_errorAngles[i] = 0;
	}
}


OBJModels::~OBJModels(void)
{
}

void OBJModels::UpdateUncertainty(float timeFactor)
{
	Vector3f trans = *_translation;
	if(_sourceUncertainty != 0)
		_myUncertainty->FromReference(_sourceUncertainty, new Vector3f(trans - _sourceUncertainty->GetCenter()));
	_transFactor += timeFactor;
	if(_transFactor > 360) _transFactor = 0;
	float xFactor = sin(_transFactor*5*(M_PI/180.0f));
	float yFactor = sin(_transFactor*7*(M_PI/180.0f));
	float zFactor = sin(_transFactor*3*(M_PI/180.0f));
	//_myUncertainty->SetSpaces(Vector3f(10,10,10), Vector3f(10,10,10));
	_uncerTranslation = new Vector3f(0,0,0);
	_curModel = 0;

	if(!_makeDARV) return;
	if(true)
	{
		*_uncerTranslation = *_uncerTranslation + _myUncertainty->GetTranslationSpace().x() * _myUncertainty->GetRigth() *xFactor;
		*_uncerTranslation = *_uncerTranslation + _myUncertainty->GetTranslationSpace().y() * _myUncertainty->GetUp() *yFactor;
		*_uncerTranslation = *_uncerTranslation + _myUncertainty->GetTranslationSpace().z() * _myUncertainty->GetBack() *zFactor;
	}
	//qDebug() << " xFactor " + QString::number(_myUncertainty->GetTranslationSpace().x()) + " yFactor " + QString::number(_myUncertainty->GetTranslationSpace().y()) +  " zFactor " + QString::number(_myUncertainty->GetTranslationSpace().z());

	float maxErrorAngle = GetMaxAngle();
	
	//qDebug() << "error angle ............. "+ QString::number(maxErrorAngle);
	float minAngle = 3.f;
	float maxAngle = 10.f;
	if(maxErrorAngle < minAngle)
		_curModel = 0;
	else
		if(maxErrorAngle > maxAngle)
			_curModel = MODEL_CNT-1;
		else
		{
			int modelNum = (((maxErrorAngle - minAngle)/(maxAngle-minAngle))*MODEL_CNT)-1;
			if(modelNum < 0 ) modelNum = 0;
			_curModel = modelNum;
		}

	float scale = 0.1;
	*_uncerTranslation*=scale;
}

float OBJModels::GetMaxAngle()
{
	float angle = max(_myUncertainty->GetRotationSpace().x(),max(_myUncertainty->GetRotationSpace().y(),_myUncertainty->GetRotationSpace().z()));
	for(int i = AVERAGE_ANGLE-1; i > 0; i--)
	{
		_errorAngles[i] = _errorAngles[i-1];
	}
	_errorAngles[0] = angle;
	float sum = 0;
	for(int i = 0; i < AVERAGE_ANGLE; i++)
		sum += _errorAngles[i];
	return sum/(float)AVERAGE_ANGLE;
}

void OBJModels::Load()
{
	_curModel = 0;
	for(int i = 0; i<MODEL_CNT;i++)
	{
		bool res = LoadModel(_modelName+QString::number(i*2)+".obj", &_vertexbuffer[i], &_uvbuffer, &_normalbuffer[i], &_indexbuffer[i], &_indicesSize[i]);
	}
	LoadTexture(_textureName, &_texture);
}

void OBJModels::Draw(bool useTracking)
{
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer[_curModel]);
	glVertexAttribPointer(
	   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, _normalbuffer[_curModel]);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexbuffer[_curModel]);

	// Draw the triangles !
	glDrawElements(
			GL_TRIANGLES,      // mode
			_indicesSize[_curModel],    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
}