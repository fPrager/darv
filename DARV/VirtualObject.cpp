#include "stdafx.h"
#include "VirtualObject.h"

void VirtualObject::LoadTexture(QString textureName, GLuint * texture)
{
	static QList<LoadedRessource*> loadedTextures = QList<LoadedRessource*>();

	for(int i = 0; i<loadedTextures.size(); i++)
	{
		if(loadedTextures[i]->_name == textureName)
		{
				*texture = loadedTextures[i]->_id;
				return;
		}
	}

	QString path = "Resources/Textures/"+ textureName;
	QByteArray ba = path.toLocal8Bit();
	const char *c_str2 = ba.data();

	*texture = SOIL_load_OGL_texture(
	c_str2,
	SOIL_LOAD_AUTO,
	SOIL_CREATE_NEW_ID,
	SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	
	/* check for an error during the load process */
	if( 0 == *texture )
	{
		printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
	}
	loadedTextures.append(new LoadedRessource(textureName, *texture));
}

bool VirtualObject::LoadModel(QString modelName, GLuint* vertexbuffer, GLuint* uvbuffer, GLuint* normalbuffer, GLuint* indexbuffer, int* indexSize)
{
	static QList<LoadedRessource*> loadedModelData = QList<LoadedRessource*>();
	bool modelAlreadyLoaded = false;
	for(int i = 0; i<loadedModelData.size(); i++)
	{
		LoadedRessource* data = loadedModelData[i];
		if(data->_name == modelName + "V")
		{
			modelAlreadyLoaded = true;
			*vertexbuffer = data->_id;
		}
		if(data->_name == modelName + "UV")
			*uvbuffer = data->_id;

		if(data->_name == modelName + "N")
			*normalbuffer = data->_id;

		if(data->_name == modelName + "I")
			*indexbuffer = data->_id;
		//TODO: maybe bad conversion from GLuint to int!
		if(data->_name == modelName + "IS")
			*indexSize = data->_id;
	}

	if(modelAlreadyLoaded)
		return true;

	// Read our .obj file
	vector<unsigned short> indices;
	vector<glm::vec3> indexed_vertices;
	vector<glm::vec2> indexed_uvs;
	vector<glm::vec3> indexed_normals;

	QString path = "Resources/Models/"+ modelName;
	QByteArray ba = path.toLocal8Bit();
	const char *c_str2 = ba.data();
	LoadAssImp(c_str2, indices, indexed_vertices, indexed_uvs, indexed_normals);

	*indexSize = indices.size();

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, vertexbuffer);

	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, *vertexbuffer);
 
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER,  indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	//make uv-buffer
	glGenBuffers(1, uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	//make normal-buffer
	glGenBuffers(1, normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	glGenBuffers(1, indexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indexbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

	loadedModelData.append(new LoadedRessource(modelName+"V",*vertexbuffer));
	loadedModelData.append(new LoadedRessource(modelName+"UV",*uvbuffer));
	loadedModelData.append(new LoadedRessource(modelName+"N",*normalbuffer));
	loadedModelData.append(new LoadedRessource(modelName+"I",*indexbuffer));
	loadedModelData.append(new LoadedRessource(modelName+"IS",*indexSize));
	return true;
}

glm::mat4 VirtualObject::GetModelMatrix(bool asTracked)
{
	if(asTracked)
	{
		//Vector3f translate = _rotation->inverse()._transformVector(*(_translation));
		Vector3f translate = _rotation->inverse()._transformVector(*(_translation));
			glm::mat4 translation = glm::translate(glm::mat4(1.f), glm::vec3(translate.x(),translate.y(),translate.z()));
			glm::mat4 scaling = glm::scale(glm::mat4(1.f), glm::vec3(_scaleFactor,_scaleFactor,_scaleFactor));
			glm::mat4 modelScaling = glm::scale(glm::mat4(1.f), glm::vec3(_modelScaleFactorX,_modelScaleFactorY,_modelScaleFactorZ));
			glm::mat4 rotation = glm::rotate(glm::mat4(1.f), glm::degrees(_rotation->angle()), glm::vec3(_rotation->axis().x(),_rotation->axis().y(),_rotation->axis().z()));
			glm::mat4 uncertaintyTranslation = glm::translate(glm::mat4(1.f), glm::vec3(_uncerTranslation->x(),_uncerTranslation->y(),_uncerTranslation->z())); 

			if(!_attachedToMarker)
			{
				return rotation*translation*uncertaintyTranslation *modelScaling;
			}

			glm::mat4 additionalTranslation = glm::translate(glm::mat4(1.f), glm::vec3(_additionalTranslation->x(),_additionalTranslation->y(),_additionalTranslation->z())); 
			glm::mat4 additionalRotation = glm::rotate(glm::mat4(1.f),glm::degrees(_additionalRotation->angle()), glm::vec3(_additionalRotation->axis().x(),_additionalRotation->axis().y(),_additionalRotation->axis().z()));
			
			return  rotation*translation*additionalTranslation * uncertaintyTranslation  * additionalRotation*modelScaling;
			//return  translation* additionalTranslation  * additionalRotation* scaling*modelScaling;
	
	}
	else
	{
			glm::mat4 additionalTranslation = glm::translate(glm::mat4(1.f), glm::vec3(_additionalTranslation->x(),_additionalTranslation->y(),_additionalTranslation->z())); 
			glm::mat4 uncertaintyTranslation = glm::translate(glm::mat4(1.f), glm::vec3(_uncerTranslation->x(),_uncerTranslation->y(),_uncerTranslation->z())); 
			glm::mat4 additionalRotation = glm::rotate(glm::mat4(1.f),glm::degrees(_additionalRotation->angle()), glm::vec3(_additionalRotation->axis().x(),_additionalRotation->axis().y(),_additionalRotation->axis().z()));
			//glm::mat4 scaling = glm::translate(glm::mat4(1.f), glm::vec3((30 * _scaleFactor)-15,0,0));
			glm::mat4 scaling = glm::scale(glm::mat4(1.f), glm::vec3(_scaleFactor,_scaleFactor,_scaleFactor));
			glm::mat4 modelScaling = glm::scale(glm::mat4(1.f), glm::vec3(_modelScaleFactorX,_modelScaleFactorY,_modelScaleFactorZ));
			
			return  additionalTranslation* uncertaintyTranslation  * additionalRotation*modelScaling;
	}
}

void VirtualObject::UpdateUncertainty(float timeFactor)
{
	if(_shading == WithMaterial)
	{
		_transFactor += timeFactor;
		if(_transFactor > 360) _transFactor = 0;
		float xFactor = sin(_transFactor*5*(M_PI/180.0f));
		float yFactor = sin(_transFactor*7*(M_PI/180.0f));
		float zFactor = sin(_transFactor*3*(M_PI/180.0f));
		_myUncertainty->SetSpaces(Vector3f(10,10,10), Vector3f(10,10,10));
		_uncerTranslation->x() = _myUncertainty->GetTranslationSpace().x() * xFactor;
		_uncerTranslation->y() = _myUncertainty->GetTranslationSpace().y() * yFactor;
		_uncerTranslation->z() = _myUncertainty->GetTranslationSpace().z() * zFactor;
		//qDebug() << " xFactor " + QString::number(xFactor) + " yFactor " + QString::number(yFactor) +  " zFactor " + QString::number(zFactor);
	}
	else
		CalculateMyUncertainty();
}

void MarkerObject::Load()
{
	LoadTexture(_textureName, &_texture);
	Util::MakeMeATexturedPlane(&_vertexbuffer,&_uvbuffer);
}

void MarkerObject::Draw(bool useTracking)
{
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
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

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
}

void InvaderMarkerObject::Load()
{
	LoadTexture(_textureName, &_texture);
	Util::MakeMeATexturedPlane(&_vertexbuffer,&_uvbuffer);
}

void InvaderMarkerObject::Draw(bool useTracking)
{
	// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
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

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 2*3); // 2*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
}

void ColoredCubeObject::Load()
{
	LoadTexture(_textureName, &_texture);
	Util::MakeMeATexturedBox(&_vertexbuffer,&_uvbuffer);
}

void ColoredCubeObject::Draw(bool useTracking)
{
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
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

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12*3); // 2*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
}

void DaliObject::Load()
{
   LoadTexture(_textureName, &_texture);
	Util::MakeMeATexturedPlane(&_vertexbuffer,&_uvbuffer);
}

void DaliObject::Draw(bool useTracking)
{
// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
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

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 2*3); // 2*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
}

void TexturedPlane::Load()
{
   LoadTexture(_textureName, &_texture);
	Util::MakeMeATexturedPlane(&_vertexbuffer,&_uvbuffer);
}

void TexturedPlane::Draw(bool useTracking)
{
// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
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

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 2*3); // 2*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
}

void Furniture::Load()
{
	bool res = LoadModel(_modelName, &_vertexbuffer, &_uvbuffer, &_normalbuffer, &_indexbuffer, &_indicesSize);
	LoadTexture(_textureName, &_texture);
}

void Furniture::Draw(bool useTracking)
{
		// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
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
		glBindBuffer(GL_ARRAY_BUFFER, _normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexbuffer);

	// Draw the triangles !
	glDrawElements(
			GL_TRIANGLES,      // mode
			_indicesSize,    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
}

void TutorialObject::Load()
{
	bool res = LoadModel(_modelName, &_vertexbuffer, &_uvbuffer, &_normalbuffer, &_indexbuffer, &_indicesSize);
	LoadTexture(_textureName, &_texture);
}

void TutorialObject::Draw(bool useTracking)
{
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
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
		glBindBuffer(GL_ARRAY_BUFFER, _normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexbuffer);

	// Draw the triangles !
	glDrawElements(
			GL_TRIANGLES,      // mode
			_indicesSize,    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
}