#pragma once
#include <stdafx.h>
#include <SOIL.h>
using namespace std;
using namespace Eigen;
#include <fstream>
using std::ifstream;
#include <sstream>
using std::istringstream;

//#include <Objloader.hpp>
#include <Objloader.h>
#include <Util.h>
#include <Uncertainty.h>

enum ShaderType
{
	NoShading,
	Simple,
	WithMaterial
};

class VirtualObject
{
	public:
		QString _name;
		int _markerID;
		bool _attachedToMarker;

		bool _justInTrackingMode;

		enum ShaderType _shading;
		
		Vector3f* _translation;
		AngleAxisf* _rotation;

		Uncertainty* _myUncertainty;
		Uncertainty* _sourceUncertainty;
		
		Vector3f* _uncerTranslation;
		float _transFactor;
		float	_uncerMorphingSize;
		float	_uncerMorphFactor;
		Vector3f* _coreArea;
		bool _makeDARV;

		Vector3f* _additionalTranslation;
		AngleAxisf* _additionalRotation;

		virtual void Load() = 0;
		virtual void Draw(bool useTracking = false) = 0;
		void CalculateMyUncertainty()
		{
			_myUncertainty->SetSpaces(Vector3f(10,10,10), Vector3f(10,10,10));
		}
		virtual void UpdateUncertainty(float timeFactor);

		//void UseTracking()
		//{
		//		glTranslatef(_translation->x(),_translation->y(),_translation->z());
		//		glRotatef(_rotation->w() * (180.0/M_PI),_rotation->x(),_rotation->y(),_rotation->z());
		//		glScalef(_scaleFactor,_scaleFactor,_scaleFactor);
		//}

		void ChangeSize(float v)
		{
			_scaleFactor += v;
			if(_scaleFactor < 0) _scaleFactor = 0;
			if(_scaleFactor > 1) _scaleFactor = 1;
		}

		glm::mat4 GetModelMatrix(bool asTracked);
		bool  _increase;
		float _scaleFactor;
		float _modelScaleFactorX;
		float _modelScaleFactorY;
		float _modelScaleFactorZ;

protected:
		VirtualObject()
		{
			_name ="NONE";
			_markerID = 0;
			_attachedToMarker = false;
			_shading = NoShading;
			_translation = new Vector3f();
			_rotation = new AngleAxisf(0, Vector3f(0,1,0));
			SetUp();
		}

		VirtualObject(QString name, int markerID, bool simpleShading = false, Vector3f* additionalTranslation = new Vector3f(0,0,0), AngleAxisf* additionalRotation = new AngleAxisf(0,Vector3f(0,1,0)))
		{
			_name =name;
			_markerID = markerID;
			_attachedToMarker = true;
			_translation = new Vector3f();
			_additionalTranslation = additionalTranslation;
			_additionalRotation = additionalRotation;
			_rotation = new AngleAxisf(0, Vector3f(0,1,0));
			_shading = Simple;
			SetUp();
		}

		VirtualObject(QString name, Vector3f* positionInRoom, AngleAxisf* rotationInRoom, bool simpleShading = false)
		{
			_name =name;
			_markerID = -1;
			_attachedToMarker = false;
			_translation = positionInRoom;
			_rotation = rotationInRoom;
			_additionalTranslation = positionInRoom;
			_additionalRotation = rotationInRoom;
			_shading = Simple;
			SetUp();
		}

		void SetUp()
		{
			_makeDARV = false;
			_justInTrackingMode = true;
			_scaleFactor = 1;
			_modelScaleFactorX = 1;
			_modelScaleFactorY = 1;
			_modelScaleFactorZ = 1;
			_myUncertainty = new Uncertainty();
			_sourceUncertainty = 0;
			_coreArea = new Vector3f(0,0,0);
			_uncerTranslation = new Vector3f(0,0,0);
			_transFactor = 0;
		}

		~VirtualObject(){
		
		}

		void LoadTexture(QString textureName, GLuint * texture);
		bool LoadModel(QString modelName, GLuint* vertexBuffer, GLuint* uvBuffer, GLuint* normalBuffer, GLuint* indexBuffer, int* indexSize);

};

class MarkerObject: public VirtualObject
{
	public:
		MarkerObject(float width, int markerNR): VirtualObject("Marker"+QString::number(markerNR), markerNR)
		{
			QString path = "marker"+QString::number(markerNR)+".png";
			_textureName = path.toStdString().c_str();
			_modelScaleFactorX = width;
			_modelScaleFactorY = width;
		}
		~MarkerObject(){}

		void Draw(bool useTracking = false);
		void Load();

	private:
		QString _textureName;
		GLuint _texture;
		// This will identify our vertex buffer
		GLuint _vertexbuffer;
		GLuint _uvbuffer;
		float		_markerWidth;
};

class InvaderMarkerObject: public VirtualObject
{
	public:
		InvaderMarkerObject(float width, int markerID): VirtualObject("SpaceInvaderMarker", markerID)
		{
			_textureName = "invadermarker.png";
			_modelScaleFactorX = width;
			_modelScaleFactorY = width;
			_shading = NoShading;
		}
		~InvaderMarkerObject(){}

		void Draw(bool useTracking = false);
		void Load();

	private:
		QString _textureName;
		GLuint _texture;
		// This will identify our vertex buffer
		GLuint _vertexbuffer;
		GLuint _uvbuffer;
};

class ColoredCubeObject: public VirtualObject
{
	public:
		ColoredCubeObject(float size, int id): VirtualObject("Bunter Würfel", id)
		{
			_modelScaleFactorX = size;
			_modelScaleFactorY = size;
			_modelScaleFactorZ = size;
			_textureName = "coloredcube.png";
		}
		ColoredCubeObject(float width, float height, float depth, QString texName): VirtualObject("Box", 1)
		{
			_modelScaleFactorX = width;
			_modelScaleFactorY = height;
			_modelScaleFactorZ = depth;
			_textureName = texName;
		}
		ColoredCubeObject(QString texName): VirtualObject("Box", 1)
		{
			_modelScaleFactorX = 1;
			_modelScaleFactorY = 1;
			_modelScaleFactorZ = 1;
			_textureName = texName;
		}
		~ColoredCubeObject(){}

		void Draw(bool useTracking = false);
		void Load();

	private:
		QString _textureName;
		GLuint _texture;
		// This will identify our vertex buffer
		GLuint _vertexbuffer;
		GLuint _uvbuffer;
};

class DaliObject: public VirtualObject
{
	public:
		DaliObject(int width, Vector3f* position, AngleAxisf* rotation, QString textureName = "painting1.bmp"): VirtualObject("Dali-Picture", position, rotation)
		{
			_textureName = textureName;
			_modelScaleFactorX = width;
			_modelScaleFactorY = (49.0f/69.0f) * width;
		}
		~DaliObject(){}

		void Draw(bool useTracking = false);
		void Load();

	private:
		QString _textureName;
		GLuint _texture;
		GLuint _vertexbuffer;
		GLuint _uvbuffer;
};

class TexturedPlane: public VirtualObject
{
	public:
		TexturedPlane(int width, int height, Vector3f* position, AngleAxisf* rotation, QString textureName): VirtualObject("Textured Plane", position, rotation)
		{
			_textureName = textureName;
			_modelScaleFactorX = width;
			_modelScaleFactorY = height;
		}
		~TexturedPlane(){}

		void Draw(bool useTracking = false);
		void Load();

	private:
		QString _textureName;
		GLuint _texture;
		GLuint _vertexbuffer;
		GLuint _uvbuffer;
};

class Furniture: public VirtualObject
{
	public:
		Furniture(Vector3f* position, AngleAxisf* rotation): VirtualObject("Furniture", position, rotation)
		{
			_textureName = "leather.png";
			_modelName = "chair.obj";
		}
		~Furniture(){
			//glDeleteBuffers(1, &_vertexbuffer);
		}

		void Draw(bool useTracking = false);
		void Load();

	private:
		// This will identify our vertex buffer
		GLuint _vertexbuffer;
		GLuint _colorbuffer;
		GLuint _uvbuffer;
		GLuint _normalbuffer;
		GLuint _indexbuffer;

		int _indicesSize;

		QString _modelName;

		QString _textureName;
		GLuint _texture;
};

class TutorialObject: public VirtualObject
{
	public:
		TutorialObject(): VirtualObject("TutorialObject", 1, true, new Vector3f(0,0,10), new AngleAxisf(90, Vector3f(1,0,0)))
		{
			_textureName = "white.bmp";
			_modelName = "invader.obj";
			_modelScaleFactorX = 5;
			_modelScaleFactorY = 5;
			_modelScaleFactorZ = 5;
		}

		//constructor to a flexible model tagged on a marker
		TutorialObject(QString modelName, QString textureName, float modelScale, Vector3f* translation, AngleAxisf* rotation,int markerid): VirtualObject(modelName, markerid, true, translation, rotation)
		{
			_textureName = textureName;
			_modelName = modelName;
			_modelScaleFactorX = modelScale;
			_modelScaleFactorY = modelScale;
			_modelScaleFactorZ = modelScale;
		}

		//constructor to a fixed marker
		TutorialObject(QString modelName, QString textureName, float modelScale, Vector3f* translation, AngleAxisf* rotation): VirtualObject(modelName,translation, rotation, true)
		{
			_textureName = textureName;
			_modelName = modelName;
			_modelScaleFactorX = modelScale;
			_modelScaleFactorY = modelScale;
			_modelScaleFactorZ = modelScale;
		}

		~TutorialObject(){
			//glDeleteBuffers(1, &_vertexbuffer);
		}

		void Draw(bool useTracking = false);
		void Load();

	private:
		// This will identify our vertex buffer
		GLuint _vertexbuffer;
		GLuint _colorbuffer;
		GLuint _uvbuffer;
		GLuint _normalbuffer;
		GLuint _indexbuffer;

		int _indicesSize;

		QString _modelName;

		QString _textureName;
		GLuint _texture;
};