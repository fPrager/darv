#pragma once
#include <VirtualObject.h>
#define MODEL_CNT 9
#define AVERAGE_ANGLE 10
class OBJModels: public VirtualObject
{
public:
	OBJModels(QString modelName, QString textureName, float modelScale, Vector3f* translation, AngleAxisf* rotation);
	~OBJModels(void);

	virtual void UpdateUncertainty(float timeFactor);
	void Draw(bool useTracking = false);
	void Load();

	int _modelCnt;
	int _curModel;
private:

	GLuint _vertexbuffer[MODEL_CNT];
	GLuint _colorbuffer[MODEL_CNT];
	GLuint _normalbuffer[MODEL_CNT];
	GLuint _indexbuffer[MODEL_CNT];

	int _indicesSize[MODEL_CNT];
	float _errorAngles[AVERAGE_ANGLE];
	float GetMaxAngle();

	QString _modelName;

	QString _textureName;
	GLuint _uvbuffer;
	GLuint _texture;
};

