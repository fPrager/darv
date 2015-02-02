#pragma once
#include <stdafx.h>
#include <SOIL.h>
#include <Marker.h>
#include <CameraTracker.h>
#include <VirtualSetup.h>
#include <VirtualObject.h>
#include <OBJScene.h>
#include <OBJModels.h>
using namespace std;
using namespace Eigen;

#define CUBE_X -68.81f
#define CUBE_Y 74.63f
#define CUBE_Z 40.426f
#define CUBE_YAW 0.052
#define CUBE_PITCH -0.046
#define CUBE_ROLL 0.749


class VirtualReality
{
public:
	VirtualReality(CameraTracker* camTracker = new CameraTrackerNoIMU());
	~VirtualReality();
	VirtualSetup* _selectedSetup;

	QList<QString> GetSetupNames();
	int GetMarkerIDOfSelected();
	void SelectSetup(QString name);
	void Initialize();
	void Load();
	void SetFrameBufferDim(int width, int height, bool reloadBuffer = false);
	void Update(float elapsedTime);
	void UpdateMarker(float elapsedTime, QList<Marker> realMarker, bool useReference);
	void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, GLuint camtexture = -1, bool useTracking = false);
	void ChangeCameraTracker(CameraTracker* tracker);
	void ShowQualtiy(bool flag);

	CameraTracker* GetCameraTracker();
	Vector3f*	GetCameraPosition();
	AngleAxisf* GetCameraRotation();
	float*		GetCameraQuality();

private:
	QList<VirtualSetup*> _setups;
	CameraTracker*	_camTracker;
	Vector3f		_cameraPosition;
	AngleAxisf		_cameraRotation;
	float			_cameraQuality;

	void BuiltHolodeckSetups();
	void BuiltSimpleSetups();
	void BuiltOptiTrackLabor();

	GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

	void DrawSetup(glm::mat4 model, glm::mat4 view, glm::mat4 projection, bool useTracking = false);

	bool _inVSTMode;
	GLuint _camTexture;
	void DrawFullQuad(GLuint texture, bool upSideDown = false);

	bool	_showQuality;
	bool	_showUncertainty;
	bool	_usePostRendering;
	GLuint	_framebufferName;
	int		_frameBufferWidth;
	int		_frameBufferHeight;
	GLuint	_renderedTexture;
	bool	LoadPostRendering();
	void	DoPostRendering(GLuint texture, GLuint background);

	QColor _background;

	//shader stuff
		//regular shader without shading (no light)
		GLuint _noShadingShaderID;
		GLuint _mvpMatrixID;
		GLuint _texSamplerID;

		//regular scene shader with light (object normals nad lightsources)
		GLuint _simpleShaderID;
		GLuint _simpleMvpMatrixID;
		GLuint _simpleMMatrixID;
		GLuint _simpleVMatrixID;
		GLuint _simpleTexSamplerID;
		GLuint _simpleLightID;

		//scene shader for objects with material with light (object normals nad lightsources)
		GLuint _materialShaderID;
		GLuint _materialMvpMatrixID;
		GLuint _materialMMatrixID;
		GLuint _materialVMatrixID;
		GLuint _materialTexSamplerID;
		GLuint _materialLightID;
		GLuint _objectTranslationID;
		GLuint _objectCoreID;
		GLuint _objectMorphFactorID;
		GLuint _objectSphereSizeID;


		//full quad shader
		GLuint _fullQuadShaderID;
		GLuint _fullQuadSamplerID;
		GLuint _fullQuadUpSideDownID;
		GLuint _fullQuadVertexBuffer;
		GLuint _fullQuadUVBuffer;
		GLuint _testTexture;

		//post rendering stuff
		GLuint _postShaderID;
		GLuint _postForegroundID;
		GLuint _postBackgroundID;
		GLuint _postValueID;
};


