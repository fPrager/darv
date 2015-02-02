#include "stdafx.h"
#include "VirtualReality.h"


VirtualReality::VirtualReality(CameraTracker* camTracker)
{
	_camTracker = camTracker;
	_cameraPosition = Vector3f(0,0,0);
	_cameraRotation = AngleAxisf(0, Vector3f(0,0,0));
	_frameBufferWidth = 800;
	_frameBufferHeight = 900;
	_setups = QList<VirtualSetup*>();

	//if(HOMEWORKING)
	//	BuiltSimpleSetups();
	//else
		//BuiltHolodeckSetups();
	BuiltOptiTrackLabor();

	if(_setups.size() > 0)
			_selectedSetup = _setups[0];

	_usePostRendering = true;
	_showQuality = false;
	_showUncertainty = true;
}

VirtualReality::~VirtualReality()
{
	glDeleteProgram(_simpleShaderID);
	glDeleteProgram(_noShadingShaderID);
	glDeleteProgram(_fullQuadShaderID);
	if(_usePostRendering)
	{
		glDeleteProgram(_postShaderID);
		glDeleteFramebuffers(1, &_framebufferName);
		glDeleteTextures(1, &_renderedTexture);
	}
}

void VirtualReality::BuiltHolodeckSetups()
{
	VirtualRoom* holoDeck = new VirtualRoom(600,300,600, Vector3f(16,-150,-300+7), "Holodeck");
	holoDeck->AddRoomObject(new Vector3f(-226,36.5f,260),new Vector3f(180,73,80),new AngleAxisf(0,Vector3f(0,1,0)), "Schreibtisch1");
	holoDeck->AddRoomObject(new Vector3f(-43,36.5f,260),new Vector3f(180,73,80),new AngleAxisf(0,Vector3f(0,1,0)), "Schreibtisch2");
	holoDeck->AddRoomObject(new Vector3f(-226,36.5f,342),new Vector3f(180,73,80),new AngleAxisf(0,Vector3f(0,1,0)), "Schreibtisch1");
	holoDeck->AddRoomObject(new Vector3f(-43,36.5f,342),new Vector3f(180,73,80),new AngleAxisf(0,Vector3f(0,1,0)), "Schreibtisch4");
	holoDeck->AddRoomObject(new Vector3f(262.5,100,110),new Vector3f(220,200,43),new AngleAxisf(90 * (M_PI/180.0f),Vector3f(0,1,0)), "Schrank");
	holoDeck->AddRoomObject(new Vector3f(284,102.5,315),new Vector3f(110,205,10),new AngleAxisf(90* (M_PI/180.0f),Vector3f(0,1,0)), "Tür");

	//list of virtual markers in the holodeck
	QList<VirtualMarker*> holoDeckMarker = QList<VirtualMarker*>();
	VirtualMarker* vm1 = new VirtualMarker(1,new Vector3f(-176.7f, 152, -7), new AngleAxisf(0.f * (M_PI/180.0f),Vector3f(0,1,0)));
	VirtualMarker* vm2 = new VirtualMarker(2);
	VirtualMarker* vm3 = new VirtualMarker(3,new Vector3f(-23, 0, 27.25f), new AngleAxisf(-90 * (M_PI/180.0f),Vector3f(1,0,0)));
	VirtualMarker* vm4 = new VirtualMarker(4,new Vector3f(-30.0f, 46.1f, -7), new AngleAxisf(0.f * (M_PI/180.0f),Vector3f(0,1,0)));
	VirtualMarker* vm5 = new VirtualMarker(5,new Vector3f(-30.0f, 71.9f, -7), new AngleAxisf(0.f * (M_PI/180.0f),Vector3f(0,1,0)));
	holoDeckMarker.append(vm1);
	holoDeckMarker.append(vm2);
	holoDeckMarker.append(vm4);
	holoDeckMarker.append(vm5);
	holoDeckMarker.append(vm3);

	QList<VirtualMarker*> holoDeckMarker_free = QList<VirtualMarker*>();
	VirtualMarker* vm1_free = new VirtualMarker(1);
	VirtualMarker* vm3_free = new VirtualMarker(3);
	VirtualMarker* vm4_free = new VirtualMarker(4);
	VirtualMarker* vm5_free = new VirtualMarker(5);
	holoDeckMarker_free.append(vm1_free);
	holoDeckMarker_free.append(vm2);
	holoDeckMarker_free.append(vm3_free);
	holoDeckMarker_free.append(vm4_free);
	holoDeckMarker_free.append(vm5_free);


	//a setup with marker-tagged spaceinvader
	QList<VirtualObject*> invaderObjects = QList<VirtualObject*>();
	VirtualObject* invader1 = new InvaderMarkerObject(13.5f, 1);
	invader1->_justInTrackingMode = false;
	VirtualObject* invader2 = new InvaderMarkerObject(13.5f, 2);
	VirtualObject* invader3 = new InvaderMarkerObject(35, 3);
	VirtualObject* invader4 = new InvaderMarkerObject(13.5f, 4);
	VirtualObject* invader5 = new InvaderMarkerObject(13.5f, 5);
	invaderObjects.append(invader1);
	invaderObjects.append(invader2);
	invaderObjects.append(invader3);
	invaderObjects.append(invader4);
	invaderObjects.append(invader5);

	VirtualSetup* holoDeckWithInvader = new VirtualSetup("Holodeck mit Invader", holoDeck,holoDeckMarker, invaderObjects);
	_setups.append(holoDeckWithInvader);
	VirtualSetup* holoDeckWithInvader2 = new VirtualSetup("Holodeck mit Invader frei", holoDeck,holoDeckMarker_free, invaderObjects);
	_setups.append(holoDeckWithInvader2);

	//a setup with attached cubes
	QList<VirtualObject*> cubes = QList<VirtualObject*>();
	VirtualObject* cube1 = new ColoredCubeObject(13,1);
	VirtualObject* cube2 = new ColoredCubeObject(13,2);
	VirtualObject* cube3 = new ColoredCubeObject(13,3);
	VirtualObject* cube4 = new ColoredCubeObject(13,4);
	VirtualObject* cube5 = new ColoredCubeObject(13,5);
	
	cubes.append(cube1);
	cubes.append(cube2);
	cubes.append(cube3);
	cubes.append(cube4);
	cubes.append(cube5);
	//VirtualSetup* holoDeckWithCubes = new VirtualSetup("Holodeck-Wuerfel ", holoDeckMarker, cubes);
	//_setups.append(holoDeckWithCubes);

	VirtualSetup* holoDeckWithCubesFree = new VirtualSetup("Grosse Wuerfel frei", holoDeck,holoDeckMarker_free, cubes);
	_setups.append(holoDeckWithCubesFree);

	QList<VirtualObject*> fixedcubes = QList<VirtualObject*>();
	for(int x  = 0; x < 200; x += 5)
				{
					VirtualObject* fixedcube = new ColoredCubeObject("red.bmp");
					fixedcube->_attachedToMarker = false;
					
					fixedcube->_translation = new Vector3f(x, 0, 0);
					fixedcube->_rotation = new AngleAxisf(0,Vector3f(0,1,0));
					fixedcubes.append(fixedcube);
				}
		for(int y  = 0; y < 200; y += 5)
				{
					VirtualObject* fixedcube = new ColoredCubeObject("blue.bmp");
					fixedcube->_attachedToMarker = false;
					fixedcube->_translation = new Vector3f(0, y, 0);
					fixedcube->_rotation = new AngleAxisf(0,Vector3f(0,1,0));
					fixedcubes.append(fixedcube);
				}
			for(int z  = 0; z < 200; z += 5)
				{
					VirtualObject* fixedcube = new ColoredCubeObject("green.bmp");
					fixedcube->_attachedToMarker = false;
					fixedcube->_translation = new Vector3f(0, 0, z);
					fixedcube->_rotation = new AngleAxisf(0,Vector3f(0,1,0));
					fixedcubes.append(fixedcube);
				}
	VirtualSetup* holoDeckWithCubes = new VirtualSetup("Axen-Wuerfel", holoDeck,holoDeckMarker, fixedcubes);
	_setups.append(holoDeckWithCubes);

	//a setup with a simple marker, a dalipaintings and an invader
	QList<VirtualObject*> simpleObjects = QList<VirtualObject*>();
	VirtualObject* marker1 = new MarkerObject(14.5f,1);
	//marker1->_justInTrackingMode = false;
	VirtualObject* simpleObject = new TutorialObject();
	simpleObject->_markerID = 2;
	VirtualObject* daliPainting1 =  new DaliObject(20, new Vector3f(20, 74, 7), new AngleAxisf(180.f * (M_PI/180.0f),Vector3f(0,1,0)));
	VirtualObject* invader = new TutorialObject("invader.obj","white.bmp", 3, new Vector3f(0, 10, 0), new AngleAxisf(90 * (M_PI/180.0f),Vector3f(1,0,0)),2);
	VirtualObject* dali1 = new DaliObject(69,new Vector3f(120.f, 153, 10), new AngleAxisf(180.f * (M_PI/180.0f),Vector3f(0,1,0)));
	VirtualObject* dali2 = new DaliObject(69,new Vector3f(235.f, 153, 10), new AngleAxisf(180.f * (M_PI/180.0f),Vector3f(0,1,0)));
	//VirtualObject* invader12 = new TutorialObject("invader.obj","white.bmp", 5, new Vector3f(30, 50, 30), new AngleAxisf(0,Vector3f(0,1,0)));
	//VirtualObject* chair = new TutorialObject("chair2.obj","chair2.dds", 70, new Vector3f(0, 0, 40), new AngleAxisf(150 * (M_PI/180.0f),Vector3f(0,1,0)));
	VirtualObject* chair = new TutorialObject("seat1.obj","leather.png", 0.001f, new Vector3f(-50, 0, 40), new AngleAxisf(150 * (M_PI/180.0f),Vector3f(0,1,0)));
	//VirtualObject* chair2 = new TutorialObject("chair.obj","chair.tga", 1, new Vector3f(0, 0, 0), new AngleAxisf(90,Vector3f(1,0,0)),3);
	
	chair->_justInTrackingMode = false;
	//simpleObjects.append(marker1);
	//simpleObjects.append(simpleObject);
	//simpleObjects.append(daliPainting1);
	
	simpleObjects.append(invader);
	simpleObjects.append(dali1);
	simpleObjects.append(dali2);
	simpleObjects.append(chair);
	
	VirtualSetup* holoDeckWithSimpleObject = new VirtualSetup("Holodeck mit Stuhl", holoDeck, holoDeckMarker, simpleObjects);
	_setups.append(holoDeckWithSimpleObject);

}

void VirtualReality::BuiltSimpleSetups()
{
	VirtualRoom* spaceArroundBlock = new VirtualRoom(300,200,300, Vector3f(0,-98,0), "Holodeck");
	spaceArroundBlock->AddRoomObject(new Vector3f(0,-1,0),new Vector3f(40,2,30),new AngleAxisf(0,Vector3f(0,1,0)), "Block");

	QList<VirtualMarker*> blockMarkerFixed = QList<VirtualMarker*>();
	VirtualMarker* vm1 = new VirtualMarker(1,new Vector3f(-13, 0, -8), new AngleAxisf(-90 * (M_PI/180.0f),Vector3f(1,0,0)));
	VirtualMarker* vm2 = new VirtualMarker(2,new Vector3f(13, 0, -8), new AngleAxisf(-90 * (M_PI/180.0f),Vector3f(1,0,0)));
	VirtualMarker* vm3 = new VirtualMarker(3,new Vector3f(-13, 0, 8), new AngleAxisf(-90 * (M_PI/180.0f),Vector3f(1,0,0)));
	VirtualMarker* vm4 = new VirtualMarker(4,new Vector3f(13, 0, 8), new AngleAxisf(-90 * (M_PI/180.0f),Vector3f(1,0,0)));
	blockMarkerFixed.append(vm1);
	blockMarkerFixed.append(vm2);
	blockMarkerFixed.append(vm3);
	blockMarkerFixed.append(vm4);

	QList<VirtualMarker*> blockMarkerFree = QList<VirtualMarker*>();
	VirtualMarker* vm1_free = new VirtualMarker(1);
	VirtualMarker* vm2_free = new VirtualMarker(2);
	VirtualMarker* vm3_free = new VirtualMarker(3);
	VirtualMarker* vm4_free = new VirtualMarker(4);
	blockMarkerFree.append(vm1_free);
	blockMarkerFree.append(vm2_free);
	blockMarkerFree.append(vm3_free);
	blockMarkerFree.append(vm4_free);

	//a setup with marker-tagged spaceinvader
	QList<VirtualObject*> invaderObjects = QList<VirtualObject*>();
	for(int i = 0; i<4; i++)
	{
		VirtualObject* invader = new InvaderMarkerObject(13.5f, i+1);
		if(i==0)
			invader->_justInTrackingMode = false;
		invaderObjects.append(invader);
	}

	VirtualSetup* blockWithInvader = new VirtualSetup("Invader", spaceArroundBlock,blockMarkerFixed, invaderObjects);
	_setups.append(blockWithInvader);
	VirtualSetup* blockWithInvader2 = new VirtualSetup("Invader Frei", spaceArroundBlock,blockMarkerFree, invaderObjects);
	_setups.append(blockWithInvader2);

	QList<VirtualObject*> fixedcubes = QList<VirtualObject*>();
	for(int x  = 0; x < 200; x += 5)
				{
					VirtualObject* fixedcube = new ColoredCubeObject(1,1,1,"red.bmp");
					fixedcube->_attachedToMarker = false;
					fixedcube->_translation = new Vector3f(x, 0, 0);
					fixedcube->_rotation = new AngleAxisf(0,Vector3f(0,1,0));
					fixedcubes.append(fixedcube);
				}
		for(int y  = 0; y < 200; y += 5)
				{
					VirtualObject* fixedcube = new ColoredCubeObject(1,1,1,"blue.bmp");
					fixedcube->_attachedToMarker = false;
					fixedcube->_translation = new Vector3f(0, y, 0);
					fixedcube->_rotation = new AngleAxisf(0,Vector3f(0,1,0));
					fixedcubes.append(fixedcube);
				}
			for(int z  = 0; z < 200; z += 5)
				{
					VirtualObject* fixedcube = new ColoredCubeObject(1,1,1,"green.bmp");
					fixedcube->_attachedToMarker = false;
					fixedcube->_translation = new Vector3f(0, 0, z);
					fixedcube->_rotation = new AngleAxisf(0,Vector3f(0,1,0));
					fixedcubes.append(fixedcube);
				}
	VirtualSetup* axesCubes = new VirtualSetup("Axen-Wuerfel", spaceArroundBlock,blockMarkerFixed, fixedcubes);
	_setups.append(axesCubes);

	QList<VirtualObject*> simpleDaliObject = QList<VirtualObject*>();
	VirtualObject* dali = new DaliObject(40, new Vector3f(0, 0, 0), new AngleAxisf(-90 * (M_PI/180.0f),Vector3f(1,0,0)));
	simpleDaliObject.append(dali);
	VirtualSetup* blockWithDali = new VirtualSetup("Dalibild", spaceArroundBlock,blockMarkerFixed, simpleDaliObject);
	_setups.append(blockWithDali);
	
	QList<VirtualObject*> furnitureObject = QList<VirtualObject*>();
	VirtualObject* chair = new TutorialObject("chair2.obj","chair2.dds", 20, new Vector3f(0, 0, 0), new AngleAxisf(150 * (M_PI/180.0f),Vector3f(0,1,0)));
	chair->_justInTrackingMode = false;
	furnitureObject.append(chair);

	VirtualSetup* chairSetup = new VirtualSetup("Sessel", spaceArroundBlock,blockMarkerFixed, furnitureObject);
	_setups.append(chairSetup);
}

void VirtualReality::BuiltOptiTrackLabor()
{
	VirtualRoom* optiTrackLabor = new VirtualRoom(854,300,600, Vector3f(-83,-148,179), "OptiTrackLabor");
	optiTrackLabor->AddRoomObject(new Vector3f(0,130,95),new Vector3f(520,260,50),new AngleAxisf(0,Vector3f(0,1,0)), "Wall");
	optiTrackLabor->AddRoomObject(new Vector3f(487.5f,61,-20),new Vector3f(45,122,160),new AngleAxisf(0,Vector3f(0,1,0)), "Schrank");
	optiTrackLabor->AddRoomObject(new Vector3f(300,37,-380),new Vector3f(160,74,200),new AngleAxisf(0,Vector3f(0,1,0)), "Grosser Tisch");
	optiTrackLabor->AddRoomObject(new Vector3f(20,37,-407.5f),new Vector3f(160,74,145),new AngleAxisf(0,Vector3f(0,1,0)), "Kleiner Tisch");
	optiTrackLabor->AddRoomObject(new Vector3f(-221,37,-190),new Vector3f(120,74,80),new AngleAxisf(0,Vector3f(0,1,0)), "Kleiner Tisch");
	optiTrackLabor->AddRoomObject(new Vector3f(-323,60,-140),new Vector3f(44,120,280),new AngleAxisf(0,Vector3f(0,1,0)), "Schrank");
	optiTrackLabor->AddRoomObject(new Vector3f(-244,44,-390),new Vector3f(200,88,140),new AngleAxisf(0,Vector3f(0,1,0)), "FootZoomer");
	
	VirtualRoom* simpleRoom = new VirtualRoom(800,300,600, Vector3f(0,-150,0), "Block");
	

	Vector3f cubePositionCorrection(0,0,0);
	Vector3f cubePosition = Vector3f(CUBE_X+cubePositionCorrection.x(), CUBE_Y+cubePositionCorrection.y(), CUBE_Z+cubePositionCorrection.z());
	AngleAxisf cubeRotation = AngleAxisf(AngleAxisf(CUBE_ROLL * (M_PI/180.0f), Vector3f(1,0,0)) * AngleAxisf(CUBE_YAW * (M_PI/180.0f), Vector3f(0,1,0)) * AngleAxisf(CUBE_PITCH * (M_PI/180.0f), Vector3f(0,0,1)));
	QList<VirtualMarker*> blockMarkerFixed = QList<VirtualMarker*>();
	VirtualMarker* vm1 = new VirtualMarker(1,new Vector3f(cubeRotation._transformVector(Vector3f(0, 11, 0)) + cubePosition), new AngleAxisf(cubeRotation * AngleAxisf(180 * (M_PI/180.0f),Vector3f(0,1,0))*AngleAxisf(-90 * (M_PI/180.0f),Vector3f(1,0,0))), "Oben", 19.8f);
	VirtualMarker* vm2 = new VirtualMarker(2,new Vector3f(cubeRotation._transformVector(Vector3f(0, 0, -11)) + cubePosition), new AngleAxisf(cubeRotation*AngleAxisf(180 * (M_PI/180.0f),Vector3f(0,1,0))), "Vorn", 19.8f);
	VirtualMarker* vm3 = new VirtualMarker(3,new Vector3f(cubeRotation._transformVector(Vector3f(11, 0, 0)) + cubePosition), new AngleAxisf(cubeRotation*AngleAxisf(90 * (M_PI/180.0f),Vector3f(0,1,0))), "Links", 19.8f);
	VirtualMarker* vm4 = new VirtualMarker(4,new Vector3f(cubeRotation._transformVector(Vector3f(-11, 0, 0)) + cubePosition), new AngleAxisf(cubeRotation*AngleAxisf(-90 * (M_PI/180.0f),Vector3f(0,1,0))), "Rechts", 19.8f);
	VirtualMarker* vm5 = new VirtualMarker(5,new Vector3f(cubeRotation._transformVector(Vector3f(0, 0, 11)) + cubePosition), new AngleAxisf(cubeRotation*AngleAxisf(0,Vector3f(0,1,0))), "Hinten", 19.8f);
	
	blockMarkerFixed.append(vm1);
	blockMarkerFixed.append(vm2);
	blockMarkerFixed.append(vm3);
	blockMarkerFixed.append(vm4);
	blockMarkerFixed.append(vm5);

	QList<VirtualMarker*> simpleMarker = QList<VirtualMarker*>();
	VirtualMarker* vm = new VirtualMarker(2,new Vector3f(0,0,0), new AngleAxisf(AngleAxisf(-90 * (M_PI/180.0f),Vector3f(1,0,0))), "Ein Marker", 19.8f);
	simpleMarker.append(vm);

	QList<VirtualObject*> invaderObjects = QList<VirtualObject*>();
	for(int i = 0; i<5; i++)
	{
		VirtualObject* invader = new InvaderMarkerObject(19.98f, i+1);
		if(i==0)
			invader->_justInTrackingMode = false;
		invaderObjects.append(invader);
	}
	VirtualSetup* blockWithInvader = new VirtualSetup("Feste Marker", optiTrackLabor,blockMarkerFixed, invaderObjects);
	_setups.append(blockWithInvader);

		QList<VirtualObject*> fixedcubes = QList<VirtualObject*>();
	for(int x  = 0; x < 200; x += 5)
				{
					VirtualObject* fixedcube = new ColoredCubeObject(1,1,1,"red.bmp");
					fixedcube->_attachedToMarker = false;
					fixedcube->_translation = new Vector3f(x, 0, 0);
					fixedcube->_rotation = new AngleAxisf(0,Vector3f(0,1,0));
					fixedcubes.append(fixedcube);
				}
		for(int y  = 0; y < 200; y += 5)
				{
					VirtualObject* fixedcube = new ColoredCubeObject(1,1,1,"blue.bmp");
					fixedcube->_attachedToMarker = false;
					fixedcube->_translation = new Vector3f(0, y, 0);
					fixedcube->_rotation = new AngleAxisf(0,Vector3f(0,1,0));
					fixedcubes.append(fixedcube);
				}
			for(int z  = 0; z < 200; z += 5)
				{
					VirtualObject* fixedcube = new ColoredCubeObject(1,1,1,"green.bmp");
					fixedcube->_attachedToMarker = false;
					fixedcube->_translation = new Vector3f(0, 0, z);
					fixedcube->_rotation = new AngleAxisf(0,Vector3f(0,1,0));
					fixedcubes.append(fixedcube);
				}
	VirtualSetup* axesCubes = new VirtualSetup("Axen-Wuerfel", optiTrackLabor,blockMarkerFixed, fixedcubes);
	//_setups.append(axesCubes);

	QList<VirtualObject*> furnitureObject2 = QList<VirtualObject*>();
	VirtualObject* chair2 = new OBJModels("new","leather.png", 50, new Vector3f(5, 10, 5), new AngleAxisf(10 * (M_PI/180.0f),Vector3f(0,1,0)));
	chair2->_justInTrackingMode = false;
	furnitureObject2.append(chair2);

	VirtualSetup* simpleSetup = new VirtualSetup("Sessel auf Block", simpleRoom, simpleMarker, furnitureObject2);
	_setups.append(simpleSetup);



	QList<VirtualObject*> furnitureObject = QList<VirtualObject*>();
	//VirtualObject* chair = new TutorialObject("chair3.obj","chair2.dds", 60, new Vector3f(0, 0, 0), new AngleAxisf(10 * (M_PI/180.0f),Vector3f(0,1,0)));
	VirtualObject* chair = new OBJModels("new","leather.png", 150, new Vector3f(0, 30, 40), new AngleAxisf(190 * (M_PI/180.0f),Vector3f(0,1,0)));
	
	chair->_justInTrackingMode = false;
	furnitureObject.append(chair);

	VirtualSetup* chairSetup = new VirtualSetup("Sessel auf Wuerfel", optiTrackLabor,blockMarkerFixed, furnitureObject);
	_setups.append(chairSetup);

	/*QList<VirtualObject*> objObjects = QList<VirtualObject*>();
	VirtualObject* obj = new OBJScene("Armchair.obj", new Vector3f(0, 0, 0), new AngleAxisf(10 * (M_PI/180.0f),Vector3f(0,1,0)), 100.f);
	obj->_justInTrackingMode = false;
	objObjects.append(obj);*/

	//VirtualSetup* objTestSetup = new VirtualSetup("OBJTest", optiTrackLabor,blockMarkerFixed, objObjects);
	//_setups.append(objTestSetup);

	QList<VirtualObject*> simpleObjects = QList<VirtualObject*>();
	VirtualObject* invader = new TutorialObject("invader.obj","white.bmp", 7, new Vector3f(CUBE_X, CUBE_Y+10, CUBE_Z), new AngleAxisf(0 * (M_PI/180.0f),Vector3f(1,0,0)));
	simpleObjects.append(invader);
	VirtualSetup* invaderRoom = new VirtualSetup("Invader auf Wuerfel", optiTrackLabor,blockMarkerFixed, simpleObjects);
	_setups.append(invaderRoom);

	QList<VirtualObject*> tags = QList<VirtualObject*>();
	VirtualObject* wallTag = new TexturedPlane(50,30,  new Vector3f(0, 0, 0), new AngleAxisf(-90 * (M_PI/180.0f),Vector3f(1,0,0)),"test.png");
	tags.append(wallTag);
	VirtualSetup* taggedRoom = new VirtualSetup("Tags", optiTrackLabor,blockMarkerFixed, tags);
	//_setups.append(taggedRoom);
}

void VirtualReality::Initialize()
{
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		qDebug() << "Failed to initialize GLEW";
		return;
	}

	//_background = QColor(0,200,210,255);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 
}

void VirtualReality::SetFrameBufferDim(int width, int height, bool reloadBuffer)
{
	if(_frameBufferWidth == width && _frameBufferHeight == height)
		return;

	_frameBufferWidth = width;
	_frameBufferHeight = height;
	if(_usePostRendering && reloadBuffer)
		if(!LoadPostRendering())
		{
				qDebug() << "Can't create framebuffer on that GPU.";
				_usePostRendering = false;
		}
}

bool VirtualReality::LoadPostRendering()
{
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	_framebufferName = 0;
	glGenFramebuffers(1, &_framebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferName);

	// The texture we're going to render to
	glGenTextures(1, &_renderedTexture);
 
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, _renderedTexture);
 
	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, _frameBufferWidth, _frameBufferHeight, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);
 
	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// The depth buffer
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _frameBufferWidth, _frameBufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _renderedTexture, 0);
 
	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	return false;

	// Render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0,0,1024,768); // Render on the whole framebuffer, complete from the lower left corner to the upper right
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return true;
}

GLuint VirtualReality::LoadShaders(const char * vertex_file_path,const char * fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}



	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);
	//glBindAttribLocation(VertexShaderID,0,"vertexPosition_modelspace");
	//glBindAttribLocation(VertexShaderID,1,"vertexUV");
	//glBindAttribLocation(VertexShaderID,2,"vertexNormal_modelspace");

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void VirtualReality::Load()
{
	// Create and compile our GLSL program from the shaders
	_noShadingShaderID = LoadShaders("Resources/Shader/NoShadingShader.vertexShader", "Resources/Shader/NoShadingShader.fragmentShader" );
	_simpleShaderID = LoadShaders( "Resources/Shader/SimpleVertexShader.vertexShader", "Resources/Shader/SimpleFragmentShader.fragmentShader" );
	_materialShaderID = LoadShaders( "Resources/Shader/MaterialVertexShader.vertexShader", "Resources/Shader/MaterialFragmentShader.fragmentShader" );
	_fullQuadShaderID = LoadShaders("Resources/Shader/FullQuadVertexShader.vertexShader", "Resources/Shader/FullQuadFragmentShader.fragmentShader" );

	if(_usePostRendering)
	{
		if(!LoadPostRendering())
		{
				qDebug() << "Can't create framebuffer on that GPU.";
				_usePostRendering = false;
		}
		else
		{
				_postShaderID = LoadShaders("Resources/Shader/Passthrough.vertexshader", "Resources/Shader/WobblyTexture.fragmentshader" );
				_postForegroundID = glGetUniformLocation(_postShaderID, "renderedTexture");
				_postBackgroundID = glGetUniformLocation(_postShaderID, "backgroundTexture");
				_postValueID = glGetUniformLocation(_postShaderID, "quality");
		}
	}

	//get unifoirms of the regular shader
	// Get a handle for our matrices uniform
	_mvpMatrixID = glGetUniformLocation(_noShadingShaderID, "MVP");

	// Get a handle for our texture sampler uniforms
	_texSamplerID  = glGetUniformLocation(_noShadingShaderID, "myTextureSampler");

	//get unifoirms of the regular shader
	// Get a handle for our matrices uniform
	_simpleMvpMatrixID = glGetUniformLocation(_simpleShaderID, "MVP");
	_simpleMMatrixID = glGetUniformLocation(_simpleShaderID, "M");
	_simpleVMatrixID = glGetUniformLocation(_simpleShaderID, "V");

	// Get a handle for our texture sampler uniforms
	_simpleTexSamplerID  = glGetUniformLocation(_simpleShaderID, "myTextureSampler");

	// Get a handle for our "LightPosition" uniform
	_simpleLightID = glGetUniformLocation(_simpleShaderID, "LightPosition_worldspace");

	//get unifoirms of the material shader
	// Get a handle for our matrices uniform
	_materialMvpMatrixID = glGetUniformLocation(_materialShaderID, "MVP");
	_materialMMatrixID = glGetUniformLocation(_materialShaderID, "M");
	_materialVMatrixID = glGetUniformLocation(_materialShaderID, "V");

	// Get a handle for our texture sampler uniforms
	_materialTexSamplerID  = glGetUniformLocation(_materialShaderID, "myTextureSampler");

	// Get a handle for our "LightPosition" uniform
	_materialLightID = glGetUniformLocation(_materialShaderID, "LightPosition_worldspace");

	//get the handle for uncertainty 
	_objectTranslationID = glGetUniformLocation(_materialShaderID, "ObjectTranslation");
	_objectCoreID = glGetUniformLocation(_materialShaderID, "ObjectCore");
	_objectMorphFactorID = glGetUniformLocation(_materialShaderID, "MorphingFactor");
	_objectSphereSizeID = glGetUniformLocation(_materialShaderID, "SphereSize");


	//get uniforms of the full quad shader
	_fullQuadSamplerID  = glGetUniformLocation(_fullQuadShaderID, "myTextureSampler");
	_fullQuadUpSideDownID  = glGetUniformLocation(_fullQuadShaderID, "UpSideDown");

	//make a full quad of two triangles
	static const GLfloat g_vertex_buffer_data[] = { 
		 -1.0f, 1.0f, 0.0f,		//under left corner
		 -1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 -1.0f, 1.0f, 0.0f,		//upper right corner
		 1.0f, -1.0f, 0.0f,
		 1.0f, 1.0f, 0.0f,
	};

	glGenBuffers(1, &_fullQuadVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _fullQuadVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	//two triangles -> four corners
	static const GLfloat g_uv_buffer_data[] = { 
		0, 1,
		0, 0,
		1, 0,
		0, 1,
		1, 0,
		1, 1,
	};

	glGenBuffers(1, &_fullQuadUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _fullQuadUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	_testTexture = SOIL_load_OGL_texture(
	"painting1.bmp",
	SOIL_LOAD_AUTO,
	SOIL_CREATE_NEW_ID,
	SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

	//load all possible setups
	//TODO: just load the selected setup?!
	for(int i = 0; i<_setups.size(); i++)
	{
		_setups[i]->Load();
	}
}

void VirtualReality::Update(float elapsedTime)
{
	_camTracker->GetCameraData(&_cameraQuality, &_cameraPosition, &_cameraRotation);
	float scaleFromQuality = elapsedTime/10.0f;
	if(_cameraQuality == 0)
		scaleFromQuality *= -1;
	_selectedSetup->Update(elapsedTime/100.0);
}

void VirtualReality::UpdateMarker(float elapsedTime, QList<Marker> realMarker, bool useReference)
{
	QList<VirtualMarker*> freeMarker = QList<VirtualMarker*>();
	QList<VirtualMarker*> fixedMarker = QList<VirtualMarker*>();
	_selectedSetup->GetVirtualMarker(&fixedMarker, &freeMarker);
	_camTracker->Update(realMarker, fixedMarker, freeMarker, elapsedTime,useReference); 
}

void VirtualReality::Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, GLuint camtexture, bool useTracking)
{
	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	if(_usePostRendering && _showQuality)
	{
		// Render to our framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, _framebufferName);
		//glViewport(0,0,1024,768); // Render on the whole framebuffer, complete from the lower left corner to the upper right
	}
	glClear(GL_COLOR_BUFFER_BIT);

	// Clear the screen
	if(camtexture != -1 && !(_usePostRendering && _showQuality))
	{
		//darw a full screen cam texture here
		DrawFullQuad(camtexture, true);
	}

	glClear(GL_DEPTH_BUFFER_BIT);

	DrawSetup(model, view, projection, useTracking);

	if(_usePostRendering && _showQuality)
	{
		// Render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0,0,1024,768); // Render on the whole framebuffer, complete from the lower left corner to the upper right
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//darw a full screen cam texture here
		DoPostRendering(_renderedTexture, camtexture);
	}
}

void VirtualReality::DrawFullQuad(GLuint texture, bool upSideDown)
{
	//draw a quad with a texture which fills the current viewport
	glUseProgram(_fullQuadShaderID);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(_fullQuadSamplerID, 0);
	glUniform1i(_fullQuadUpSideDownID, upSideDown);
	
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _fullQuadVertexBuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, _fullQuadUVBuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void VirtualReality::DoPostRendering(GLuint texture, GLuint background)
{
	// Use our shader
	glUseProgram(_postShaderID);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "renderedTexture" sampler to user Texture Unit 0
	glUniform1i(_postForegroundID, 0);

	// Bind our texture in Texture Unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, background);
	// Set our "backgroundTexture" sampler to user Texture Unit 1
	glUniform1i(_postBackgroundID, 1);

	glUniform1f(_postValueID, (float)(_cameraQuality) );

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _fullQuadVertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

	glDisableVertexAttribArray(0);
}

void VirtualReality::DrawSetup(glm::mat4 model, glm::mat4 view, glm::mat4 projection, bool useTracking)
{
	QList<VirtualObject*> objectsInSetup = _selectedSetup->GetObjectsToDraw(useTracking);

	//clear the depth, everthing that will be drawn after this is "on top"
	if(objectsInSetup.size() == 0 || _selectedSetup == NULL || (useTracking && _camTracker->GetQuality() == 0))
	{
		//do nothing
	}
	else
	{
		for(int i = 0; i<objectsInSetup.size(); i++)
		{
			VirtualObject* objectToDraw = objectsInSetup[i];
			glm::mat4 combinedModel = model;
			glm::mat4 combinedView = view;
			if(useTracking && _cameraQuality>0)
			{
				glm::mat4 translate = glm::translate(glm::mat4(1.f), -glm::vec3(_cameraPosition.x(),_cameraPosition.y(),_cameraPosition.z()));	
				glm::mat4 rotate = glm::rotate(glm::mat4(1.f),-glm::degrees(_cameraRotation.angle()),glm::vec3(_cameraRotation.axis().x(),_cameraRotation.axis().y(),_cameraRotation.axis().z()));
				glm::mat4 viewOnScene = rotate * translate;

				combinedView =   combinedView*viewOnScene;
			}

			combinedModel = combinedModel * objectToDraw->GetModelMatrix(useTracking);

			// Our ModelViewProjection : multiplication of our 3 matrices
			glm::mat4 MVP        = projection * combinedView * combinedModel; // Remember, matrix multiplication is the other way around
			glm::vec3 lightPos = glm::vec3(-150,250,100);	
			
			GLfloat light[] = {lightPos.x, lightPos.y, lightPos.z, 0};
			glm::mat4 modelView = combinedView * combinedModel;

			// Use the simple shader
			switch(objectToDraw->_shading){
			case Simple:
					glUseProgram(_simpleShaderID);
					// Send our transformation to the currently bound shader, 
					// in the "MVP" uniform
					//TODO: hier einfacher machen, nicht bei jdem object neu setzen
					glUniformMatrix4fv(_simpleMvpMatrixID, 1, GL_FALSE, &MVP[0][0]);
					glUniformMatrix4fv(_simpleMMatrixID, 1, GL_FALSE, &combinedModel[0][0]);
					glUniformMatrix4fv(_simpleVMatrixID, 1, GL_FALSE, &view[0][0]);
								
					glUniform3f(_simpleLightID, lightPos.x, lightPos.y, lightPos.z);

					// Set our "myTextureSampler" sampler to user Texture Unit 0
					glUniform1i(_simpleTexSamplerID, 0);
					break;

			case WithMaterial:
					glUseProgram(_materialShaderID);
					// Send our transformation to the currently bound shader, 
					// in the "MVP" uniform
					//TODO: hier einfacher machen, nicht bei jdem object neu setzen
					glUniformMatrix4fv(_materialMvpMatrixID, 1, GL_FALSE, &MVP[0][0]);
					glUniformMatrix4fv(_materialMMatrixID, 1, GL_FALSE, &combinedModel[0][0]);
					glUniformMatrix4fv(_materialVMatrixID, 1, GL_FALSE, &view[0][0]);
								
					glUniform3f(_materialLightID, lightPos.x, lightPos.y, lightPos.z);

					// Set our "myTextureSampler" sampler to user Texture Unit 0
					glUniform1i(_materialTexSamplerID, 0);

					//glUniform3f(_objectTranslationID, objectToDraw->_uncerTranslation->x(), objectToDraw->_uncerTranslation->y(),objectToDraw->_uncerTranslation->z());
					//glUniform3f(_objectCoreID, objectToDraw->_coreArea->x(), objectToDraw->_coreArea->y(),objectToDraw->_coreArea->z());
					//glUniform1f(_objectMorphFactorID, objectToDraw->_uncerMorphFactor);
					//glUniform1f(_objectSphereSizeID, objectToDraw->_uncerMorphingSize);

					break;

			case NoShading:
					glUseProgram(_noShadingShaderID);
					// Send our transformation to the currently bound shader, 
					// in the "MVP" uniform
					glUniformMatrix4fv(_mvpMatrixID, 1, GL_FALSE, &MVP[0][0]);

					// Set our "myTextureSampler" sampler to user Texture Unit 0
					glUniform1i(_texSamplerID, 0);
					break;
			}
			objectToDraw->Draw(useTracking);
		}
	}
}

int VirtualReality::GetMarkerIDOfSelected()
{
	return 1;
}

void VirtualReality::SelectSetup(QString name)
{
	bool foundObject = false;
	for(int i = 0; i<_setups.size(); i++)
	{
		if(_setups[i]->GetName() == name)
		{
			_selectedSetup = _setups[i];
			_selectedSetup->UpdateMarkerLists();
			foundObject = true;
		}
	}
	if(!foundObject)
	{
		if(_setups.size() > 0)
		{
			_selectedSetup = _setups[0];
			_selectedSetup->UpdateMarkerLists();
		}
		qDebug() << "ERROR: Can't find object with name: " << name;
	}
}

void VirtualReality::ChangeCameraTracker(CameraTracker* tracker)
{
	_camTracker = tracker;
}

QList<QString> VirtualReality::GetSetupNames()
{
	QList<QString> names = QList<QString>();
	for(int i = 0; i<_setups.size(); i++)
	{
		names.append(_setups[i]->GetName());
	}
	return names;
}

Vector3f* VirtualReality::GetCameraPosition()
{
	return &_cameraPosition;
}

AngleAxisf* VirtualReality::GetCameraRotation()
{
	return &_cameraRotation;
}

float* VirtualReality::GetCameraQuality()
{
	return &_cameraQuality;
}

CameraTracker* VirtualReality::GetCameraTracker()
{
	return _camTracker;
}

void VirtualReality::ShowQualtiy(bool flag)
{
	_showQuality = flag;
	for(int i = 0; i<_setups.count(); i++)
		_setups[i]->ChangeDARV(flag);
}