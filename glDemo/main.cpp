
#include "core.h"
#include "TextureLoader.h"
#include "shader_setup.h"
#include "ArcballCamera.h"
#include "GUClock.h"
#include "AIMesh.h"
#include "Cylinder.h"


/*
 To Do List Delete Later!

 Have The scale of objects assigned to a variable to increase readability
 Sort out the scale and placement of objects in the scene too speradic and all over thje place atm while im testing them tidy up before submission


 Figue out how to import textures from the pagoda and lamp models to conveine into one cohesive pattern.
 Delete Irrelevant CPP and Header Files








*/
using namespace std;
using namespace glm;


struct DirectionalLight {

	vec3 direction;
	vec3 colour;
	
	DirectionalLight() {

		direction = vec3(0.0f, 1.0f, 0.0f); // default to point upwards
		colour = vec3(1.0f, 1.0f, 1.0f);
	}

	DirectionalLight(vec3 direction, vec3 colour = vec3(1.0f, 1.0f, 1.0f)) {

		this->direction = direction;
		this->colour = colour;
	}


	void DirectionalRed(vec3 direction, vec3 colour = vec3(1.0f, 0.0f, 0.0f)) {

		this->direction = direction;
		this->colour = colour;
	}
};

struct PointLight {

	vec3 pos;
	vec3 colour;
	vec3 attenuation; // x=constant, y=linear, z=quadratic

	PointLight() {

		pos = vec3(0.0f, 0.0f, 0.0f);
		colour = vec3(1.0f, 1.0f, 1.0f);
		attenuation = vec3(1.0f, 1.0f, 1.0f);
	}

	PointLight(vec3 pos, vec3 colour = vec3(1.0f, 1.0f, 1.0f), vec3 attenuation = vec3(1.0f, 1.0f, 1.0f)) {

		this->pos = pos;
		this->colour = colour;
		this->attenuation = attenuation;
	}
};


#pragma region Global variables

// Window size
unsigned int		windowWidth = 1024;
unsigned int		windowHeight = 768;

// Main clock for tracking time (for animation / interaction)
GUClock*			gameClock = nullptr;

// Main camera
ArcballCamera*		mainCamera = nullptr;

// Mouse tracking
bool				mouseDown = false;
double				prevMouseX, prevMouseY;

// Keyboard tracking
bool				forwardPressed;
bool				backPressed;
bool				rotateLeftPressed;
bool				rotateRightPressed;


// Scene objects
//AIMesh*				terrainMesh = nullptr;
AIMesh*				creatureMesh = nullptr;

Cylinder*			cylinderMesh = nullptr;


// Shaders

// Basic colour shader
GLuint				basicShader;
GLint				basicShader_mvpMatrix;

// Texture-directional light shader
GLuint				texDirLightShader;
GLint				texDirLightShader_modelMatrix;
GLint				texDirLightShader_viewMatrix;
GLint				texDirLightShader_projMatrix;
GLint				texDirLightShader_texture;
GLint				texDirLightShader_lightDirection;
GLint				texDirLightShader_lightColour;

// Texture-point light shader
GLuint				texPointLightShader;
GLint				texPointLightShader_modelMatrix;
GLint				texPointLightShader_viewMatrix;
GLint				texPointLightShader_projMatrix;
GLint				texPointLightShader_texture;
GLint				texPointLightShader_lightPosition;
GLint				texPointLightShader_lightColour;
GLint				texPointLightShader_lightAttenuation;

//  *** normal mapping *** Normal mapped texture with Directional light
// This is the same as the texture direct light shader above, but with the addtional uniform variable
// to set the normal map sampler2D variable in the fragment shader.
GLuint				nMapDirLightShader;
GLint				nMapDirLightShader_modelMatrix;
GLint				nMapDirLightShader_viewMatrix;
GLint				nMapDirLightShader_projMatrix;
GLint				nMapDirLightShader_diffuseTexture;
GLint				nMapDirLightShader_normalMapTexture;
GLint				nMapDirLightShader_lightDirection;
GLint				nMapDirLightShader_lightColour;

// cylinder model
vec3 cylinderPos = vec3(-2.0f, 2.0f, 0.0f);

// Torii Gate 1 Scaling Rotating and Positioning
vec3 toriiGatePos = vec3(0.0f, 0.0f, 0.0f);
vec3 toriiGateScale = vec3(2.0f, 2.0f, 2.0f);
float toriiGateRotX = 0.0f; float toriiGateRotY = 0.0f; float toriiGateRotZ = 0.0f; // Rotate Model in XYZ Coordinates


// Torii Gate 2 Scaling Rotating and Positioning
vec3 toriiGatePos2 = vec3(10.0f, 0.0f, 0.0f);
//vec3 toriiGateScale2 = vec3(2.0f, 2.0f, 2.0f);
//float toriiGateRotX2 = 0.0f; float toriiGateRotY = 0.0f; float toriiGateRotZ = 0.0f; // Rotate Model in XYZ Coordinates

// Torii Gate 3 Scaling Rotating and Positioning
vec3 toriiGatePos3 = vec3(5.0f, 0.0f, 0.0f);
//vec3 toriiGateScale3 = vec3(2.0f, 2.0f, 2.0f);
//float toriiGateRotX3 = 0.0f; float toriiGateRotY3 = 0.0f; float toriiGateRotZ3 = 0.0f; // Rotate Model in XYZ Coordinates

// Lamp Scaling Rotating and Positioning
vec3 lampPos = vec3(-1.0f, 2.3f, 2.5f);
vec3 lampScale = vec3(0.1f, 0.1f, 0.1f);
float lampRotX = 90.0f; float lampRotY = 90.0f; float lampRotZ = 0.0f; // Rotate Model in XYZ Coordinates

// Lamp 2 Scaling Rotating and Positioning
vec3 lampPos2 = vec3(-1.0f, 2.3f, -2.5f);
//vec3 lampScale = vec3(0.1f, 0.1f, 0.1f);
//float lampRotX = 90.0f; float lampRotY = 90.0f; float lampRotZ = 0.0f; // Rotate Model in XYZ Coordinates


// Katana Scaling Rotating and Positioning
vec3 katanaPos = vec3(5.0f, 1.3f, 0.0f);
vec3 katanaScale = vec3(0.015f, 0.015f, 0.015f);
float katanaRotX = 0.0f; float katanaRotY = 0.0f; float katanaRotZ = 90.0f; // Rotate Model in XYZ Coordinates

// Pagoda Scaling Rotating and Positioning
vec3 pagodaPos = vec3(30.0f, 1.4f, 0.0f);
vec3 pagodaScale = vec3(0.3f, 0.3f, 0.3f);
float pagodaRotX = 0.0f; float pagodaRotY = -90.0f; float pagodaRotZ = 0.0f; // Rotate Model in XYZ Coordinates

// Terrain Scaling Rotating and Positioning
vec3 terrainPos = vec3(-90.0f, -1.8f, 0.0f);
vec3 terrainScale = vec3(50.0f, 50.0f, 50.0f);
float terrainRotX = 0.0f; float terrainRotY = 0.0f; float terrainRotZ = 0.0f; // Rotate Model in XYZ Coordinates

// beast model
vec3 beastPos = vec3(2.0f, 0.0f, 0.0f);


float beastRotation = 0.0f;

float beastX = 0.0f; float beastY = 0.0f; float beastZ = 100.0f;




// Directional light example (declared as a single instance)
float directLightTheta = 0.0f;
DirectionalLight directLight = DirectionalLight(vec3(cosf(directLightTheta), sinf(directLightTheta), 0.0f));




// Setup point light example light (use array to make adding other lights easier later)
PointLight lights[2] = {
	PointLight(vec3(0.0f, 1.0f, 0.0), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.1f, 0.001f)),
	PointLight(vec3(5.0f, 5.0f, 5.0), vec3(0.0f, 0.0f, 1.0f), vec3(1.0f, 0.1f, 0.001f))
};

bool rotateDirectionalLight = true;


// Multi Mesh Models
vector<AIMesh*> terrainModel = vector<AIMesh*>();

vector<AIMesh*> katanaModel = vector<AIMesh*>();
//Torii Gates
vector<AIMesh*> toriiModel = vector<AIMesh*>();
vector<AIMesh*> toriiModel2 = vector<AIMesh*>();
vector<AIMesh*> toriiModel3 = vector<AIMesh*>();

vector<AIMesh*> pagodaModel = vector<AIMesh*>();

vector<AIMesh*> lampModel = vector<AIMesh*>();
vector<AIMesh*> lampModel2 = vector<AIMesh*>();

#pragma endregion


// Function prototypes
void renderScene();
void renderNoTextureModels();
void renderTexturedModels();
void renderWithPointLight();
void renderWithMultipleLights();
void updateScene();
void resizeWindow(GLFWwindow* window, int width, int height);
void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseMoveHandler(GLFWwindow* window, double xpos, double ypos);
void mouseButtonHandler(GLFWwindow* window, int button, int action, int mods);
void mouseScrollHandler(GLFWwindow* window, double xoffset, double yoffset);
void mouseEnterHandler(GLFWwindow* window, int entered);



int main() {

	//
	// 1. Initialisation
	//
	
	gameClock = new GUClock();

#pragma region OpenGL and window setup

	// Initialise glfw and setup window
	glfwInit();

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_TRUE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "CIS5013", NULL, NULL);

	// Check window was created successfully
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	

	// Set callback functions to handle different events
	glfwSetFramebufferSizeCallback(window, resizeWindow); // resize window callback
	glfwSetKeyCallback(window, keyboardHandler); // Keyboard input callback
	glfwSetCursorPosCallback(window, mouseMoveHandler);
	glfwSetMouseButtonCallback(window, mouseButtonHandler);
	glfwSetScrollCallback(window, mouseScrollHandler);
	glfwSetCursorEnterCallback(window, mouseEnterHandler);

	// Initialise glew
	glewInit();

	
	// Setup window's initial size
	resizeWindow(window, windowWidth, windowHeight);

#pragma endregion


	// Initialise scene - geometry and shaders etc
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // setup background colour to be black
	glClearDepth(1.0f);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	//
	// Setup Textures, VBOs and other scene objects
	//
	mainCamera = new ArcballCamera(-33.0f, 45.0f, 40.0f, 55.0f, (float)windowWidth/(float)windowHeight, 0.1f, 5000.0f);

	creatureMesh = new AIMesh(string("Assets\\beast\\beast.obj"));
	if (creatureMesh) {
		creatureMesh->addTexture(string("Assets\\beast\\beast_texture.bmp"), FIF_BMP);
	}

	cylinderMesh = new Cylinder(string("Assets\\cylinder\\cylinderT.obj"));
	

	// Load shaders
	basicShader = setupShaders(string("Assets\\Shaders\\basic_shader.vert"), string("Assets\\Shaders\\basic_shader.frag"));
	texPointLightShader = setupShaders(string("Assets\\Shaders\\texture-point.vert"), string("Assets\\Shaders\\texture-point.frag"));
	texDirLightShader = setupShaders(string("Assets\\Shaders\\texture-directional.vert"), string("Assets\\Shaders\\texture-directional.frag"));
	nMapDirLightShader = setupShaders(string("Assets\\Shaders\\nmap-directional.vert"), string("Assets\\Shaders\\nmap-directional.frag"));

	// Get uniform variable locations for setting values later during rendering
	basicShader_mvpMatrix = glGetUniformLocation(basicShader, "mvpMatrix");

	texDirLightShader_modelMatrix = glGetUniformLocation(texDirLightShader, "modelMatrix");
	texDirLightShader_viewMatrix = glGetUniformLocation(texDirLightShader, "viewMatrix");
	texDirLightShader_projMatrix = glGetUniformLocation(texDirLightShader, "projMatrix");
	texDirLightShader_texture = glGetUniformLocation(texDirLightShader, "texture");
	texDirLightShader_lightDirection = glGetUniformLocation(texDirLightShader, "lightDirection");
	texDirLightShader_lightColour = glGetUniformLocation(texDirLightShader, "lightColour");

	texPointLightShader_modelMatrix = glGetUniformLocation(texPointLightShader, "modelMatrix");
	texPointLightShader_viewMatrix = glGetUniformLocation(texPointLightShader, "viewMatrix");
	texPointLightShader_projMatrix = glGetUniformLocation(texPointLightShader, "projMatrix");
	texPointLightShader_texture = glGetUniformLocation(texPointLightShader, "texture");
	texPointLightShader_lightPosition = glGetUniformLocation(texPointLightShader, "lightPosition");
	texPointLightShader_lightColour = glGetUniformLocation(texPointLightShader, "lightColour");
	texPointLightShader_lightAttenuation = glGetUniformLocation(texPointLightShader, "lightAttenuation");

	nMapDirLightShader_modelMatrix = glGetUniformLocation(nMapDirLightShader, "modelMatrix");
	nMapDirLightShader_viewMatrix = glGetUniformLocation(nMapDirLightShader, "viewMatrix");
	nMapDirLightShader_projMatrix = glGetUniformLocation(nMapDirLightShader, "projMatrix");
	nMapDirLightShader_diffuseTexture = glGetUniformLocation(nMapDirLightShader, "diffuseTexture");
	nMapDirLightShader_normalMapTexture = glGetUniformLocation(nMapDirLightShader, "normalMapTexture");
	nMapDirLightShader_lightDirection = glGetUniformLocation(nMapDirLightShader, "lightDirection");
	nMapDirLightShader_lightColour = glGetUniformLocation(nMapDirLightShader, "lightColour");



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////
	//                                                              //
	//      Setting Up and importing models and texture maps        //
	//                                                              //
	//////////////////////////////////////////////////////////////////



	string terrainFilename = string("Assets\\terrain-model\\TerrainModel.obj");
	const struct aiScene* terrainScene = aiImportFile(terrainFilename.c_str(),
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (terrainScene) {

		cout << "Terrain model: " << terrainFilename << " has " << terrainScene->mNumMeshes << " meshe(s)\n";

		if (terrainScene->mNumMeshes > 0) {

			// For each sub-mesh, setup a new AIMesh instance in the terrainModel array
			for (int i = 0; i < terrainScene->mNumMeshes; i++) {

				cout << "Loading house sub-mesh " << i << endl;
				terrainModel.push_back(new AIMesh(terrainScene, i));
				terrainModel[i]->addTexture(string("Assets\\terrain-model\\TerrainModel_DiffuseMap.TIF"), FIF_TIFF);
				terrainModel[i]->addNormalMap(string("Assets\\terrain-model\\TerrainModel_NormalMap.TIF"), FIF_TIFF);
			}
		}
	}


	string katanaFilename = string("Assets\\katana-model\\KatanaModel.obj");
	const struct aiScene* katanaScene = aiImportFile(katanaFilename.c_str(),
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);


	if (katanaScene) {

		cout << "Katana model: " << katanaFilename << " has " << katanaScene->mNumMeshes << " meshe(s)\n";

		if (katanaScene->mNumMeshes > 0) {

			// For each sub-mesh, setup a new AIMesh instance in the houseModel array
			for (int i = 0; i < katanaScene->mNumMeshes; i++) {

				cout << "Loading Katana sub-mesh " << i << endl;
				katanaModel.push_back(new AIMesh(katanaScene, i));
				katanaModel[i]->addTexture(string("Assets\\katana-model\\KatanaModel_DiffuseMap.bmp"), FIF_BMP);
				katanaModel[i]->addNormalMap(string("Assets\\katana-model\\KatanaModel_NormalMap"), FIF_BMP);
			}
		}
	}

	

	string toriiFilename = string("Assets\\torii-gate\\ToriiGateModel.obj");
	const struct aiScene* toriiScene = aiImportFile(toriiFilename.c_str(),
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (toriiScene) {

		cout << "Torii Gate Model: " << toriiFilename << " has " << toriiScene->mNumMeshes << " meshe(s)\n";

		if (toriiScene->mNumMeshes > 0) {

			// For each sub-mesh, setup a new AIMesh instance in the toriiModel array
			for (int i = 0; i < toriiScene->mNumMeshes; i++) {

				cout << "Loading Torii Gate sub-mesh " << i << endl;
				toriiModel.push_back(new AIMesh(toriiScene, i));
				toriiModel[i]->addTexture(string("Assets\\torii-gate\\ToriiGateModel_DiffuseMap.bmp"), FIF_BMP);
				toriiModel[i]->addNormalMap(string("Assets\\torii-gate\\ToriiGateModel_NormalMap.bmp"), FIF_BMP);

				toriiModel2.push_back(new AIMesh(toriiScene, i));
				toriiModel2[i]->addTexture(string("Assets\\torii-gate\\ToriiGateModel_DiffuseMap.bmp"), FIF_BMP);
				toriiModel2[i]->addNormalMap(string("Assets\\torii-gate\\ToriiGateModel_NormalMap.bmp"), FIF_BMP);

				toriiModel3.push_back(new AIMesh(toriiScene, i));
				toriiModel3[i]->addTexture(string("Assets\\torii-gate\\ToriiGateModel_DiffuseMap.bmp"), FIF_BMP);
				toriiModel3[i]->addNormalMap(string("Assets\\torii-gate\\ToriiGateModel_NormalMap.bmp"), FIF_BMP);
			}
		}
	}





	//Pagoda
	string pagodaFilename = string("Assets\\pagoda-model\\PagodaModel.obj");
	const struct aiScene* pagodaScene = aiImportFile(pagodaFilename.c_str(),
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);


	if (pagodaScene) {

		cout << "pagoda model: " << pagodaFilename << " has " << pagodaScene->mNumMeshes << " meshe(s)\n";

		if (pagodaScene->mNumMeshes > 0) {

			// For each sub-mesh, setup a new AIMesh instance in the pagodaModel array
			for (int i = 0; i < pagodaScene->mNumMeshes; i++) {

				cout << "Loading pagoda sub-mesh " << i << endl;
				pagodaModel.push_back(new AIMesh(pagodaScene, i));
				pagodaModel[i]->addTexture(string("Assets\\pagoda-model\\PagodaModel.mtl"), FIF_BMP);
				pagodaModel[i]->addNormalMap(string("Assets\\pagoda-model\\PagodaModel.mtl"), FIF_BMP);
			}
		}
	}


	//Lamp
	string lampFilename = string("Assets\\lamp-model\\LampModel.obj");
	const struct aiScene* lampScene = aiImportFile(lampFilename.c_str(),
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);


	if (lampScene) {

		cout << "Lamp model: " << lampFilename << " has " << lampScene->mNumMeshes << " meshe(s)\n";

		if (lampScene->mNumMeshes > 0) {

			// For each sub-mesh, setup a new AIMesh instance in the lampModel array
			for (int i = 0; i < lampScene->mNumMeshes; i++) {

				cout << "Loading Lamp sub-mesh " << i << endl;
				lampModel.push_back(new AIMesh(lampScene, i));
				lampModel[i]->addTexture(string("Assets\\lamp-model\\LampModelDiffuse.bmp"), FIF_BMP);
				lampModel[i]->addNormalMap(string("Assets\\lamp-model\\LampModelNormal.bmp"), FIF_BMP);

				cout << "Loading Lamp sub-mesh " << i << endl;
				lampModel2.push_back(new AIMesh(lampScene, i));
				lampModel2[i]->addTexture(string("Assets\\lamp-model\\LampModelDiffuse.bmp"), FIF_BMP);
				lampModel2[i]->addNormalMap(string("Assets\\lamp-model\\LampModelNormal.bmp"), FIF_BMP);
			}
		}
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	//////////////////////////////////////////////////////////////////
	//                                                              //
	//                          Main Loop                           //
	//                                                              //
	//////////////////////////////////////////////////////////////////

	while (!glfwWindowShouldClose(window)) {

		updateScene();
		renderScene();						// Render into the current buffer
		glfwSwapBuffers(window);			// Displays what was just rendered (using double buffering).

		glfwPollEvents();					// Use this version when animating as fast as possible
	
		// update window title
		char timingString[256];
		sprintf_s(timingString, 256, "CIS5013: Average fps: %.0f; Average spf: %f", gameClock->averageFPS(), gameClock->averageSPF() / 1000.0f);
		glfwSetWindowTitle(window, timingString);
	}

	glfwTerminate();

	if (gameClock) {

		gameClock->stop();
		gameClock->reportTimingData();
	}

	return 0;
}   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	//                                                              //
	//                Rendering The Desired Scene                   //
	//                                                              //
	//////////////////////////////////////////////////////////////////


void renderScene()
{

	//renderNoTextureModels();
	renderTexturedModels();
	
	//renderWithPointLight();
	//renderWithMultipleLights();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Rendering multiple untextured objects into a scene




	//////////////////////////////////////////////////////////////////
	//                                                              //
	//             Rendering Models With No Textures                //
	//                                                              //
	//////////////////////////////////////////////////////////////////





void renderNoTextureModels() {

	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get camera matrices
	mat4 cameraProjection = mainCamera->projectionTransform();
	mat4 cameraView = mainCamera->viewTransform() * translate(identity<mat4>(), -beastPos);

#pragma region Render opaque objects

	// Plug-in texture-directional light shader and setup relevant uniform variables
	// (keep this shader for all textured objects affected by the light source)
	glUseProgram(texDirLightShader);

	glUniformMatrix4fv(texDirLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(texDirLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(texDirLightShader_texture, 0); // set to point to texture unit 0 for AIMeshes
	glUniform3fv(texDirLightShader_lightDirection, 1, (GLfloat*)&(directLight.direction));
	glUniform3fv(texDirLightShader_lightColour, 1, (GLfloat*)&(directLight.colour));

	if (creatureMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), beastPos) * eulerAngleY<float>(glm::radians<float>(beastRotation));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		//creatureMesh->setupTextures();
		creatureMesh->render();
	}

	
	//  *** normal mapping ***  Render the normal mapped column
	// Plug in the normal map directional light shader
	glUseProgram(nMapDirLightShader);

	// Setup uniforms
	glUniformMatrix4fv(nMapDirLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(nMapDirLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(nMapDirLightShader_diffuseTexture, 0);
	glUniform1i(nMapDirLightShader_normalMapTexture, 1);
	glUniform3fv(nMapDirLightShader_lightDirection, 1, (GLfloat*)&(directLight.direction));
	glUniform3fv(nMapDirLightShader_lightColour, 1, (GLfloat*)&(directLight.colour));


	// Render Terrain (follows same pattern / code structure as other objects)

	int test = 1; //Remove Later

	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(terrainPos))
			* glm::scale(identity<mat4>(), vec3(terrainScale))
			* eulerAngleZ<float>(glm::radians<float>(terrainRotZ))
			* eulerAngleY<float>(glm::radians<float>(terrainRotY))
			* eulerAngleX<float>(glm::radians<float>(terrainRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);
		
		for (AIMesh* mesh : terrainModel) 
		{
			//mesh->setupTextures();
			mesh->render();
		}
	
	}
	

	//Render katanaMesh
	if (test == 1) {
		
		mat4 modelTransform = glm::translate(identity<mat4>(), 
			vec3(katanaPos)) 
			* glm::scale(identity<mat4>(), vec3(katanaScale)) 
			* eulerAngleZ<float>(glm::radians<float>(beastZ))
			* eulerAngleY<float>(glm::radians<float>(beastY)) 
			* eulerAngleX<float>(glm::radians<float>(beastX));


		//mat4 modelTransform = glm::translate(identity<mat4>(), beastPos) * eulerAngleY<float>(glm::radians<float>(beastRotation));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : katanaModel)
		{
			//mesh->setupTextures();
			mesh->render();
		}
	}
	
	//Render toriiMesh

	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(toriiGatePos))
			* glm::scale(identity<mat4>(), vec3(toriiGateScale))
			* eulerAngleZ<float>(glm::radians<float>(toriiGateRotZ))
			* eulerAngleY<float>(glm::radians<float>(toriiGateRotY))
			* eulerAngleX<float>(glm::radians<float>(toriiGateRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : toriiModel)
		{
			//mesh->setupTextures();
			mesh->render();
		}
	}

	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(toriiGatePos2))
			* glm::scale(identity<mat4>(), vec3(toriiGateScale))
			* eulerAngleZ<float>(glm::radians<float>(toriiGateRotZ))
			* eulerAngleY<float>(glm::radians<float>(toriiGateRotY))
			* eulerAngleX<float>(glm::radians<float>(toriiGateRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : toriiModel2)
		{
			//mesh->setupTextures();
			mesh->render();
		}
	}

	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(toriiGatePos3))
			* glm::scale(identity<mat4>(), vec3(toriiGateScale))
			* eulerAngleZ<float>(glm::radians<float>(toriiGateRotZ))
			* eulerAngleY<float>(glm::radians<float>(toriiGateRotY))
			* eulerAngleX<float>(glm::radians<float>(toriiGateRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : toriiModel3)
		{
			//mesh->setupTextures();
			mesh->render();
		}
	}

	//Render Pagoda Mesh
	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(pagodaPos))
			* glm::scale(identity<mat4>(), vec3(pagodaScale))
			* eulerAngleZ<float>(glm::radians<float>(pagodaRotZ))
			* eulerAngleY<float>(glm::radians<float>(pagodaRotY))
			* eulerAngleX<float>(glm::radians<float>(pagodaRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : pagodaModel)
		{
			//mesh->setupTextures();
			mesh->render();
		}
	}



	//Render Lamp Mesh
	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(lampPos))
			* glm::scale(identity<mat4>(), vec3(lampScale))
			* eulerAngleZ<float>(glm::radians<float>(lampRotZ))
			* eulerAngleY<float>(glm::radians<float>(lampRotY))
			* eulerAngleX<float>(glm::radians<float>(lampRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : lampModel)
		{
			//mesh->setupTextures();
			mesh->render();
		}
	}

	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(lampPos2))
			* glm::scale(identity<mat4>(), vec3(lampScale))
			* eulerAngleZ<float>(glm::radians<float>(lampRotZ))
			* eulerAngleY<float>(glm::radians<float>(lampRotY))
			* eulerAngleX<float>(glm::radians<float>(lampRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : lampModel2)
		{
			//mesh->setupTextures();
			mesh->render();
		}
	}



#pragma endregion


#pragma region Render transparant objects

	// Done with textured meshes - render transparent objects now (cylinder in this example)...

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (cylinderMesh) {

		mat4 T = cameraProjection * cameraView * glm::translate(identity<mat4>(), cylinderPos);

	  //cylinderMesh->setupTextures();
		cylinderMesh->render(T);
	}

	//Testing Rendering a transparent Object Effect
	if (test) {

		mat4 T = cameraProjection * cameraView * glm::translate(identity<mat4>(), cylinderPos);

		for (AIMesh* mesh : toriiModel)
		{
			//mesh->setupTextures();
			mesh->render();
		}
	}


	glDisable(GL_BLEND);

#pragma endregion

	
	//
	//render directional light source
	//
	
	// Restore fixed-function pipeline
	glUseProgram(0);
	glBindVertexArray(0);
	glDisable(GL_TEXTURE_2D);

	mat4 cameraT = cameraProjection * cameraView;
	glLoadMatrixf((GLfloat*)&cameraT);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glColor3f(directLight.colour.r, directLight.colour.g, directLight.colour.b);
	glVertex3f(directLight.direction.x * 10.0f, directLight.direction.y * 10.0f, directLight.direction.z * 10.0f);
	glEnd();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Demonstrate the use of texture mapping to render textures diffuse and normal textures to models




	//////////////////////////////////////////////////////////////////
	//                                                              //
	//                 Rendering With Textures                      //
	//                                                              //
	//////////////////////////////////////////////////////////////////





void renderTexturedModels() {

	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get camera matrices
	mat4 cameraProjection = mainCamera->projectionTransform();
	mat4 cameraView = mainCamera->viewTransform() * translate(identity<mat4>(), -beastPos);

#pragma region Render opaque objects

	// Plug-in texture-directional light shader and setup relevant uniform variables
	// (keep this shader for all textured objects affected by the light source)
	glUseProgram(texDirLightShader);

	glUniformMatrix4fv(texDirLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(texDirLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(texDirLightShader_texture, 0); // set to point to texture unit 0 for AIMeshes
	glUniform3fv(texDirLightShader_lightDirection, 1, (GLfloat*)&(directLight.direction));
	glUniform3fv(texDirLightShader_lightColour, 1, (GLfloat*)&(directLight.colour));

	if (creatureMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), beastPos) * eulerAngleY<float>(glm::radians<float>(beastRotation));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		creatureMesh->setupTextures();
		creatureMesh->render();
	}


	//  *** normal mapping ***  Render the normal mapped column
	// Plug in the normal map directional light shader
	glUseProgram(nMapDirLightShader);

	// Setup uniforms
	glUniformMatrix4fv(nMapDirLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(nMapDirLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(nMapDirLightShader_diffuseTexture, 0);
	glUniform1i(nMapDirLightShader_normalMapTexture, 1);
	glUniform3fv(nMapDirLightShader_lightDirection, 1, (GLfloat*)&(directLight.direction));
	glUniform3fv(nMapDirLightShader_lightColour, 1, (GLfloat*)&(directLight.colour));


	// Render Terrain (follows same pattern / code structure as other objects)

	int test = 1; //Remove Later

	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(terrainPos))
			* glm::scale(identity<mat4>(), vec3(terrainScale))
			* eulerAngleZ<float>(glm::radians<float>(terrainRotZ))
			* eulerAngleY<float>(glm::radians<float>(terrainRotY))
			* eulerAngleX<float>(glm::radians<float>(terrainRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : terrainModel)
		{
			mesh->setupTextures();
			mesh->render();
		}

	}


	//Render katanaMesh
	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(katanaPos))
			* glm::scale(identity<mat4>(), vec3(katanaScale))
			* eulerAngleZ<float>(glm::radians<float>(beastZ))
			* eulerAngleY<float>(glm::radians<float>(beastY))
			* eulerAngleX<float>(glm::radians<float>(beastX));


		//mat4 modelTransform = glm::translate(identity<mat4>(), beastPos) * eulerAngleY<float>(glm::radians<float>(beastRotation));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : katanaModel)
		{
			mesh->setupTextures();
			mesh->render();
		}
	}

	//Render toriiMesh

	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(toriiGatePos))
			* glm::scale(identity<mat4>(), vec3(toriiGateScale))
			* eulerAngleZ<float>(glm::radians<float>(toriiGateRotZ))
			* eulerAngleY<float>(glm::radians<float>(toriiGateRotY))
			* eulerAngleX<float>(glm::radians<float>(toriiGateRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : toriiModel)
		{
			mesh->setupTextures();
			mesh->render();
		}
	}

	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(toriiGatePos2))
			* glm::scale(identity<mat4>(), vec3(toriiGateScale))
			* eulerAngleZ<float>(glm::radians<float>(toriiGateRotZ))
			* eulerAngleY<float>(glm::radians<float>(toriiGateRotY))
			* eulerAngleX<float>(glm::radians<float>(toriiGateRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : toriiModel2)
		{
			mesh->setupTextures();
			mesh->render();
		}
	}

	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(toriiGatePos3))
			* glm::scale(identity<mat4>(), vec3(toriiGateScale))
			* eulerAngleZ<float>(glm::radians<float>(toriiGateRotZ))
			* eulerAngleY<float>(glm::radians<float>(toriiGateRotY))
			* eulerAngleX<float>(glm::radians<float>(toriiGateRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : toriiModel3)
		{
			mesh->setupTextures();
			mesh->render();
		}
	}

	//Render Pagoda Mesh
	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(pagodaPos))
			* glm::scale(identity<mat4>(), vec3(pagodaScale))
			* eulerAngleZ<float>(glm::radians<float>(pagodaRotZ))
			* eulerAngleY<float>(glm::radians<float>(pagodaRotY))
			* eulerAngleX<float>(glm::radians<float>(pagodaRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : pagodaModel)
		{
			mesh->setupTextures();
			mesh->render();
		}
	}



	//Render Lamp Mesh
	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(lampPos))
			* glm::scale(identity<mat4>(), vec3(lampScale))
			* eulerAngleZ<float>(glm::radians<float>(lampRotZ))
			* eulerAngleY<float>(glm::radians<float>(lampRotY))
			* eulerAngleX<float>(glm::radians<float>(lampRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : lampModel)
		{
			mesh->setupTextures();
			mesh->render();
		}
	}

	if (test == 1) {

		mat4 modelTransform = glm::translate(identity<mat4>(),
			vec3(lampPos2))
			* glm::scale(identity<mat4>(), vec3(lampScale))
			* eulerAngleZ<float>(glm::radians<float>(lampRotZ))
			* eulerAngleY<float>(glm::radians<float>(lampRotY))
			* eulerAngleX<float>(glm::radians<float>(lampRotX));

		glUniformMatrix4fv(nMapDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		for (AIMesh* mesh : lampModel2)
		{
			mesh->setupTextures();
			mesh->render();
		}
	}



#pragma endregion


#pragma region Render transparant objects

	// Done with textured meshes - render transparent objects now (cylinder in this example)...

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (cylinderMesh) {

		mat4 T = cameraProjection * cameraView * glm::translate(identity<mat4>(), cylinderPos);

		cylinderMesh->setupTextures();
		cylinderMesh->render(T);
	}

	//Testing Rendering a transparent Object Effect
	if (test) {

		mat4 T = cameraProjection * cameraView * glm::translate(identity<mat4>(), cylinderPos);

		for (AIMesh* mesh : toriiModel)
		{
			mesh->setupTextures();
			mesh->render();
		}
	}


	glDisable(GL_BLEND);

#pragma endregion


	//
	//render directional light source
	//

	// Restore fixed-function pipeline
	glUseProgram(0);
	glBindVertexArray(0);
	glDisable(GL_TEXTURE_2D);

	mat4 cameraT = cameraProjection * cameraView;
	glLoadMatrixf((GLfloat*)&cameraT);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glColor3f(directLight.colour.r, directLight.colour.g, directLight.colour.b);
	glVertex3f(directLight.direction.x * 10.0f, directLight.direction.y * 10.0f, directLight.direction.z * 10.0f);
	glEnd();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// Demonstrate the use of a single point light source
void renderWithPointLight() {

	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get camera matrices
	mat4 cameraProjection = mainCamera->projectionTransform();
	mat4 cameraView = mainCamera->viewTransform() * translate(identity<mat4>(), -beastPos);

	// Plug-in texture-point light shader and setup relevant uniform variables
	// (keep this shader for all textured objects affected by the light source)
	glUseProgram(texPointLightShader);

	glUniformMatrix4fv(texPointLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(texPointLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(texPointLightShader_texture, 0); // set to point to texture unit 0 for AIMeshes
	glUniform3fv(texPointLightShader_lightPosition, 1, (GLfloat*)&(lights[0].pos));
	glUniform3fv(texPointLightShader_lightColour, 1, (GLfloat*)&(lights[0].colour));
	glUniform3fv(texPointLightShader_lightAttenuation, 1, (GLfloat*)&(lights[0].attenuation));
	
#pragma region Render opaque objects

	if (creatureMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), beastPos) * eulerAngleY<float>(glm::radians<float>(beastRotation));

		glUniformMatrix4fv(texPointLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		creatureMesh->setupTextures();
		creatureMesh->render();
	}

#pragma endregion

#pragma region Render transparant objects

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (cylinderMesh) {

		mat4 T = cameraProjection * cameraView * glm::translate(identity<mat4>(), cylinderPos);

		cylinderMesh->setupTextures();
		cylinderMesh->render(T);
	}

	glDisable(GL_BLEND);

#pragma endregion


	//
	// For demo purposes, render point light source
	//

	// Restore fixed-function
	glUseProgram(0);
	glBindVertexArray(0);
	glDisable(GL_TEXTURE_2D);

	mat4 cameraT = cameraProjection * cameraView;
	glLoadMatrixf((GLfloat*)&cameraT);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glColor3f(lights[0].colour.r, lights[0].colour.g, lights[0].colour.b);
	glVertex3f(lights[0].pos.x, lights[0].pos.y, lights[0].pos.z);
	glEnd();
}


void renderWithMultipleLights() {

	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get camera matrices
	mat4 cameraProjection = mainCamera->projectionTransform();
	mat4 cameraView = mainCamera->viewTransform() * translate(identity<mat4>(), -beastPos);


#pragma region Render all opaque objects with directional light

	glUseProgram(texDirLightShader);

	glUniformMatrix4fv(texDirLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(texDirLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(texDirLightShader_texture, 0); // set to point to texture unit 0 for AIMeshes
	glUniform3fv(texDirLightShader_lightDirection, 1, (GLfloat*)&(directLight.direction));
	glUniform3fv(texDirLightShader_lightColour, 1, (GLfloat*)&(directLight.colour));

	if (creatureMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), beastPos) * eulerAngleY<float>(glm::radians<float>(beastRotation));

		glUniformMatrix4fv(texDirLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		creatureMesh->setupTextures();
		creatureMesh->render();
	}

#pragma endregion



	// Enable additive blending for ***subsequent*** light sources!!!
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);



#pragma region Render all opaque objects with point light

	glUseProgram(texPointLightShader);

	glUniformMatrix4fv(texPointLightShader_viewMatrix, 1, GL_FALSE, (GLfloat*)&cameraView);
	glUniformMatrix4fv(texPointLightShader_projMatrix, 1, GL_FALSE, (GLfloat*)&cameraProjection);
	glUniform1i(texPointLightShader_texture, 0); // set to point to texture unit 0 for AIMeshes
	glUniform3fv(texPointLightShader_lightPosition, 1, (GLfloat*)&(lights[0].pos));
	glUniform3fv(texPointLightShader_lightColour, 1, (GLfloat*)&(lights[0].colour));
	glUniform3fv(texPointLightShader_lightAttenuation, 1, (GLfloat*)&(lights[0].attenuation));


	if (creatureMesh) {

		mat4 modelTransform = glm::translate(identity<mat4>(), beastPos) * eulerAngleY<float>(glm::radians<float>(beastRotation));

		glUniformMatrix4fv(texPointLightShader_modelMatrix, 1, GL_FALSE, (GLfloat*)&modelTransform);

		creatureMesh->setupTextures();
		creatureMesh->render();
	}

#pragma endregion


#pragma region Render transparant objects

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (cylinderMesh) {

		mat4 T = cameraProjection * cameraView * glm::translate(identity<mat4>(), cylinderPos);

		cylinderMesh->setupTextures();
		cylinderMesh->render(T);
	}

	glDisable(GL_BLEND);

#pragma endregion


	//
	// For demo purposes, render light sources
	//

	// Restore fixed-function
	glUseProgram(0);
	glBindVertexArray(0);
	glDisable(GL_TEXTURE_2D);

	mat4 cameraT = cameraProjection * cameraView;
	glLoadMatrixf((GLfloat*)&cameraT);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.0f);
	
	glBegin(GL_POINTS);

	glColor3f(directLight.colour.r, directLight.colour.g, directLight.colour.b);
	glVertex3f(directLight.direction.x * 10.0f, directLight.direction.y * 10.0f, directLight.direction.z * 10.0f);


	

	glColor3f(lights[0].colour.r, lights[0].colour.g, lights[0].colour.b);
	glVertex3f(lights[0].pos.x, lights[0].pos.y, lights[0].pos.z);
	
	glEnd();
}


// Animate elements
void updateScene() {

	float tDelta = 0.0f;

	if (gameClock) {

		gameClock->tick();
		tDelta = (float)gameClock->gameTimeDelta();
	}

	cylinderMesh->update(tDelta);

	// update main light source
	if (rotateDirectionalLight) {

		directLightTheta += glm::radians(30.0f) * tDelta;
		directLight.direction = vec3(cosf(directLightTheta), sinf(directLightTheta), 0.0f);
	}
	

	//
	// Handle movement based on user input
	//

	float moveSpeed = 3.0f; // movement displacement per second
	float rotateSpeed = 90.0f; // degrees rotation per second

	if (forwardPressed) {

		mat4 R = eulerAngleY<float>(glm::radians<float>(beastRotation)); // local coord space / basis vectors - move along z
		float dPos = moveSpeed * tDelta; // calc movement based on time elapsed
		beastPos += vec3(R[2].x * dPos, R[2].y * dPos, R[2].z * dPos); // add displacement to position vector
	}
	else if (backPressed) {

		mat4 R = eulerAngleY<float>(glm::radians<float>(beastRotation)); // local coord space / basis vectors - move along z
		float dPos = -moveSpeed * tDelta; // calc movement based on time elapswdisplacement to position vector
	}

	if (rotateLeftPressed) {
		
		beastRotation += rotateSpeed * tDelta;
	}
	else if (rotateRightPressed) {
		
		beastRotation -= rotateSpeed * tDelta;
	}

}


#pragma region Event handler functions

// Function to call when window resized
void resizeWindow(GLFWwindow* window, int width, int height)
{
	if (mainCamera) {

		mainCamera->setAspect((float)width / (float)height);
	}

	// Update viewport to cover the entire window
	glViewport(0, 0, width, height);

	windowWidth = width;
	windowHeight = height;
}


// Function to call to handle keyboard input
void keyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {

		// check which key was pressed...
		switch (key)
		{
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, true);
				break;
			
			case GLFW_KEY_W:
				forwardPressed = true;
				break;

			case GLFW_KEY_S:
				backPressed = true;
				break;

			case GLFW_KEY_A:
				rotateLeftPressed = true;
				break;

			case GLFW_KEY_D:
				rotateRightPressed = true;
				break;

			case GLFW_KEY_SPACE:
				rotateDirectionalLight = !rotateDirectionalLight;
				break;

			default:
			{
			}
		}
	}
	else if (action == GLFW_RELEASE) {
		// handle key release events
		switch (key)
		{
			case GLFW_KEY_W:
				forwardPressed = false;
				break;

			case GLFW_KEY_S:
				backPressed = false;
				break;

			case GLFW_KEY_A:
				rotateLeftPressed = false;
				break;

			case GLFW_KEY_D:
				rotateRightPressed = false;
				break;

			default:
			{
			}
		}
	}
}


void mouseMoveHandler(GLFWwindow* window, double xpos, double ypos) {

	if (mouseDown) {

		float dx = float(xpos - prevMouseX);
		float dy = float(ypos - prevMouseY);

		if (mainCamera)
			mainCamera->rotateCamera(-dy, -dx);

		prevMouseX = xpos;
		prevMouseY = ypos;
	}

}

void mouseButtonHandler(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_LEFT) {

		if (action == GLFW_PRESS) {

			mouseDown = true;
			glfwGetCursorPos(window, &prevMouseX, &prevMouseY);
		}
		else if (action == GLFW_RELEASE) {

			mouseDown = false;
		}
	}
}

void mouseScrollHandler(GLFWwindow* window, double xoffset, double yoffset) {

	if (mainCamera) {

		if (yoffset < 0.0)
			mainCamera->scaleRadius(1.1f);
		else if (yoffset > 0.0)
			mainCamera->scaleRadius(0.9f);
	}
}

void mouseEnterHandler(GLFWwindow* window, int entered) {
}

#pragma endregion