#pragma region Includes
#include "GLCommon.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include "cModelLoader.hpp"			
#include "cVAOManager.hpp"
#include "cModelObject.hpp"
#include "cShaderManager.hpp"
#include <sstream>
#include <limits.h>
#include <float.h>
#include "cPhysics.hpp"
#include "DebugRenderer/cDebugRenderer.hpp"
#include "cLightHelper.hpp"
#include "cLightManager.hpp"
#include "cFileManager.hpp"
#include "cError.hpp"
#include "cCallbacks.hpp"
#include "cFirstPersonCamera.hpp"
#include "cThirdPersonCamera.hpp"
#include "Globals.hpp"
#include "cModelManager.hpp"
#include "cCommandManager.hpp"
#include "cMediator.hpp"
#include "cComplexObject.hpp"
#include <random>
#pragma endregion

void DrawObject(glm::mat4 m, cModelObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager);

std::map<std::string, cMesh*> mpMesh;
std::map<std::string, cLightObject*> mpLight;
std::map<std::string, cModelObject*> mpModel;
cFirstPersonCamera mainCamera;
//cThirdPersonCamera mainCamera;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

//RANDOMIZER
std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<unsigned int>leftyQuad(159303, 320000); //
std::uniform_int_distribution<unsigned int>RightyQuad(345548, 700000); //
std::uniform_int_distribution<int>MovePos(25, 100);
std::uniform_int_distribution<int>MoveNeg(-100, -25);
std::uniform_int_distribution<int>Move(-50, 50);

bool MIDTERMSKULLFLICKER = false;
bool MIDTERMFIREFLYDANCE = false;
bool MIDTERMDAYLIGHT = false;
bool MIDTERMDRONECAM = false;
float droneAngle = 0.0f;
float droneSpeed = (2 * 3.14159) / 15;
float droneRadius = 450.0f;
float droneHeight = 500.0f;

//bool bLightDebugSheresOn = true;

int vpGameObjectsCurrentPos = 0;
int vpLightObjectsCurrentPos = 0;
int vpComplexObjectsCurrentPos = 0;

// VARIABLES FOR SELECTING AND CONTROLING VARIOUS ASPECTS OF THE ENGINE AT RUNTIME
//		isDevCons		=		Is Developer Console Enabled?
//		isLightSelect	=		Is Lights Selectable Enabled?
//		isModelSelect	=		Is Models Selectable Enabled?
//		isObjCtrl		=		Is Objects Controllable Enabled?
bool isDevCon = false;
bool isModelSelect = false;
bool isLightSelect = false;
bool isComplexSelect = false;
bool isObjCtrl = false;

std::string cmdstr;

// Load up my "scene"  (now global)
// OBJECT VECTORS
std::vector<cModelObject*> g_vec_pGameObjects;
std::vector<cLightObject*> g_vec_pLightObjects;
std::vector<cComplexObject*> g_vec_pComplexObjects;
std::vector<cMesh*> g_vec_pMeshObjects;

// MAPS
//std::map<std::string /*FriendlyName*/, cModelObject*> g_map_GameObjectsByFriendlyName;


// returns NULL (0) if we didn't find it.
cModelObject* pFindObjectByFriendlyName(std::string name);
cModelObject* pFindObjectByFriendlyNameMap(std::string name);


// MOVED USER INPUT TO AN EXTERNAL FILE.

// command to target different object types...
// model = meshes
// lighting = lights
// ...
// command = target parm ||| target lighting as an example
// commands should handle no parms, 1 parm, or two parms.

std::map<std::string, eShapeTypes> g_mp_String_to_ShapeType;

void PopulateShapeTypeMap() {
	g_mp_String_to_ShapeType["CUBE"] = eShapeTypes::CUBE;
	g_mp_String_to_ShapeType["MESH"] = eShapeTypes::MESH;
	g_mp_String_to_ShapeType["PLANE"] = eShapeTypes::PLANE;
	g_mp_String_to_ShapeType["SPHERE"] = eShapeTypes::SPHERE;
	g_mp_String_to_ShapeType["UNKNOWN"] = eShapeTypes::UNKNOWN;
}



int main() {
	PopulateShapeTypeMap();

	cError errmngr;
	cFileManager* pFileManager = cFileManager::GetFileManager();
	cLightManager* pLightManager = cLightManager::GetLightManager();
	cModelManager* pModelManager = cModelManager::GetModelManager();
	cCommandManager* pCommandManager = cCommandManager::GetCommandManager();

	cMediator* pMediator = new cMediator();
	pMediator->ConnectEndpointClients();

	GLFWwindow* window;

	std::map<std::string, cShaderManager::cShader> mpVertexShader;
	std::map<std::string, cShaderManager::cShader> mpFragmentShader;



	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);



	cDebugRenderer* pDebugRenderer = new cDebugRenderer();
	pDebugRenderer->initialize();

	cModelLoader* pTheModelLoader = new cModelLoader();	// Heap
	cShaderManager* pTheShaderManager = new cShaderManager();

	pFileManager->LoadShaders(pFileManager->GetShadersFile(), mpVertexShader, mpFragmentShader);

	// change LoadShaders to bool and return true or false based off of success, then change below to return -1 if false.
	// error will display from the LoadShaders function.
	if (!pTheShaderManager->createProgramFromFile("SimpleShader", mpVertexShader["exampleVertexShader01"], mpFragmentShader["exampleFragmentShader01"])) {
		errmngr.DisplayError("Main", __func__, "shader_compile_failure", "Could not compile shader", pTheShaderManager->getLastError());
		return -1;
	}

	GLuint shaderProgID = pTheShaderManager->getIDFromFriendlyName("SimpleShader");

	// Create a VAO Manager...
	cVAOManager* pTheVAOManager = new cVAOManager();



	// move this configuration into a seperate file that loads from a configuration file.
	// Note, the "filename" here is really the "model name" that we can look up later (i.e. it doesn't have to be the file name)

	pFileManager->LoadModelsOntoGPU(pTheModelLoader, pTheVAOManager, shaderProgID, pFileManager->GetMeshFile(), mpMesh);

	//move the physics stuff into a physics.ini so that it can be loaded in the physics engine instead of the graphics engine portion of the game engine file loading.
	// At this point, the model is loaded into the GPU

	// make a call to the filemanager to create the gameobjects from the loaded file.
	//pFileManager->CreateModelsFromData(g_vec_pGameObjects, "models/modelsTest", g_mp_String_to_ShapeType);
	pFileManager->BuildObjects(pFileManager->GetModelFile());
	
	glEnable(GL_DEPTH);			// Write to the depth buffer
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing


	cPhysics* pPhsyics = new cPhysics();
	cLightHelper* pLightHelper = new cLightHelper();

	//pFileManager->CreateLights("lights/lights", g_vec_pLightObjects, mpLight);
	pFileManager->BuildObjects(pFileManager->GetLightsFile());

	pFileManager->BuildObjects(pFileManager->GetComplexObjFile());

	mainCamera.SetTarget(pFindObjectByFriendlyName("TheIsland"));
	mainCamera.SetEye(mpLight["SceneLight1"]->getPosition());

	glm::vec3 flashlightTarget = pFindObjectByFriendlyName("BeachCave01")->getPosition();
	flashlightTarget.y += 5.0f;
	flashlightTarget.z -= 1.0f;
	mpLight["FlashLight"]->direction = glm::vec4(glm::normalize(flashlightTarget - mpLight["FlashLight"]->getPosition()), 0.0f);

	glm::vec3 ffTarget = pFindObjectByFriendlyName("mangrove1-1")->positionXYZ;

	while (!glfwWindowShouldClose(window)) {
		float ratio;
		int width, height;
		glm::mat4 p, v;

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		// Projection matrix
		p = glm::perspective(0.6f,		// FOV
			ratio,			// Aspect ratio
			0.1f,			// Near clipping plane
			1000.0f);		// Far clipping plane

		// View matrix
		mainCamera.MoveFront();
		v = mainCamera.GetViewMatrix();

		glViewport(0, 0, width, height);

		// Clear both the colour buffer (what we see) and the 
		//  depth (or z) buffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// move to a lighting file and call load lights or something of the like.
		// move to a light manager class..
		pLightManager->LoadLightsToGPU(g_vec_pLightObjects, shaderProgID);

		// Also set the position of my "eye" (the camera)
		GLint eyeLocation_UL = glGetUniformLocation(shaderProgID, "eyeLocation");

		glUniform4f(eyeLocation_UL,
			mainCamera.GetEye().x,
			mainCamera.GetEye().y,
			mainCamera.GetEye().z,
			1.0f);

		// DEBUG TITLE CURRENT OBJECT SELECTED
		if (isComplexSelect)
			glfwSetWindowTitle(window, g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->friendlyName.c_str());
		else if (isModelSelect)
			glfwSetWindowTitle(window, g_vec_pGameObjects[vpGameObjectsCurrentPos]->friendlyName.c_str());
		else if (isLightSelect)
			glfwSetWindowTitle(window, g_vec_pLightObjects[vpLightObjectsCurrentPos]->friendlyName.c_str());


		GLint matView_UL = glGetUniformLocation(shaderProgID, "matView");
		GLint matProj_UL = glGetUniformLocation(shaderProgID, "matProj");

		glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(v));
		glUniformMatrix4fv(matProj_UL, 1, GL_FALSE, glm::value_ptr(p));


		// **************************************************
		// **************************************************
		// Loop to draw everything in the scene

		// move into a render function / class
		for (int index = 0; index != ::g_vec_pGameObjects.size(); index++) {
			glm::mat4 matModel = glm::mat4(1.0f);
			cModelObject* pCurrentObject = ::g_vec_pGameObjects[index];
			if (pCurrentObject->isVisible) { DrawObject(matModel, pCurrentObject, shaderProgID, pTheVAOManager); }
		}//for (int index...

		// **************************************************
		//	Pause updates if the dev console is open.
		if (!isDevCon) {
			//RANDOMIZE LIGHT ATTENUATION
			if (MIDTERMSKULLFLICKER) {
				mpLight["LeftEyeLight"]->atten.z = leftyQuad(mt) * 0.000005;
				mpLight["RightEyeLight"]->atten.z = RightyQuad(mt) * 0.000005;
			}
			
			if (MIDTERMDRONECAM) {
				//DO MATH TO MOVE CAMERA!
				glm::vec3 tmp;

				droneAngle -= droneSpeed * deltaTime;

				tmp.x = glm::cos(droneAngle) * droneRadius;
				tmp.y = droneHeight;
				tmp.z = glm::sin(droneAngle) * droneRadius;

				mainCamera.SetEye(tmp);
			}

			if (MIDTERMFIREFLYDANCE)
			{
				for (iGameObject* firefly : g_vec_pComplexObjects) {
					//random movement
					glm::vec3 curLoc = firefly->getPosition();
					glm::vec3 move;
					float deltatimeDiv = deltaTime * 0.25;
					if (ffTarget.x + 5 <= curLoc.x)
						move.x = MoveNeg(mt) * deltatimeDiv;
					else if (ffTarget.x - 5 >= curLoc.x)
						move.x = MovePos(mt) * deltatimeDiv;
					else
						move.x = Move(mt) * deltatimeDiv;

					if (curLoc.x >= ffTarget.y + 15 )
						move.y = MoveNeg(mt) * deltatimeDiv;
					else if (curLoc.y <= 5.0f)
						move.y = MovePos(mt) * deltatimeDiv;
					else
						move.y = Move(mt) * deltatimeDiv;
										
					if (ffTarget.z + 5 <= curLoc.z)
						move.z = MoveNeg(mt) * deltatimeDiv;
					else if (ffTarget.z - 5 >= curLoc.z)
						move.z = MovePos(mt) * deltatimeDiv;
					else
						move.z = Move(mt) * deltatimeDiv;

					((cComplexObject*)firefly)->Move(move);
				}
			}			
		}

		// **************************************************

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();

	// Delete everything
	delete pTheModelLoader;
	delete pDebugRenderer;
	delete pLightHelper;
	exit(EXIT_SUCCESS);
}



void DrawObject(glm::mat4 m, cModelObject* pCurrentObject, GLint shaderProgID, cVAOManager* pVAOManager) {
	m = glm::mat4(1.0f);


	// ******* TRANSLATION TRANSFORM *********
	glm::mat4 matTrans
		= glm::translate(glm::mat4(1.0f),
			glm::vec3(pCurrentObject->positionXYZ.x,
				pCurrentObject->positionXYZ.y,
				pCurrentObject->positionXYZ.z));
	m = m * matTrans;
	// ******* TRANSLATION TRANSFORM *********



	// ******* ROTATION TRANSFORM *********
	//mat4x4_rotate_Z(m, m, (float) glfwGetTime());
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
		pCurrentObject->rotationXYZ.z,					// Angle 
		glm::vec3(0.0f, 0.0f, 1.0f));
	m = m * rotateZ;

	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
		pCurrentObject->rotationXYZ.y,	//(float)glfwGetTime(),					// Angle 
		glm::vec3(0.0f, 1.0f, 0.0f));
	m = m * rotateY;

	glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
		pCurrentObject->rotationXYZ.x,	// (float)glfwGetTime(),					// Angle 
		glm::vec3(1.0f, 0.0f, 0.0f));
	m = m * rotateX;
	// ******* ROTATION TRANSFORM *********



	// ******* SCALE TRANSFORM *********
	glm::mat4 scale = glm::scale(glm::mat4(1.0f),
		glm::vec3(pCurrentObject->scale,
			pCurrentObject->scale,
			pCurrentObject->scale));
	m = m * scale;
	// ******* SCALE TRANSFORM *********


	// Choose which shader to use
	//glUseProgram(program);
	glUseProgram(shaderProgID);



	GLint matModel_UL = glGetUniformLocation(shaderProgID, "matModel");

	glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(m));



	// Find the location of the uniform variable newColour
	GLint newColour_location = glGetUniformLocation(shaderProgID, "newColour");

	glUniform3f(newColour_location,
		pCurrentObject->objectColourRGBA.r,
		pCurrentObject->objectColourRGBA.g,
		pCurrentObject->objectColourRGBA.b);


	GLint diffuseColour_UL = glGetUniformLocation(shaderProgID, "diffuseColour");
	glUniform4f(diffuseColour_UL,
		pCurrentObject->objectColourRGBA.r,
		pCurrentObject->objectColourRGBA.g,
		pCurrentObject->objectColourRGBA.b,
		pCurrentObject->objectColourRGBA.a);	// 

	GLint specularColour_UL = glGetUniformLocation(shaderProgID, "specularColour");
	glUniform4f(specularColour_UL,
		pCurrentObject->specularColour.r,
		pCurrentObject->specularColour.g,
		pCurrentObject->specularColour.b,
		pCurrentObject->specularColour.a);
	//glUniform4f(specularColour_UL,
	//	1.0f,	// R
	//	1.0f,	// G
	//	1.0f,	// B
	//	1000.0f);	// Specular "power" (how shinny the object is)
	//				// 1.0 to really big (10000.0f)

	GLint debugColour_UL = glGetUniformLocation(shaderProgID, "debugColour");
	GLint bDoNotLight_UL = glGetUniformLocation(shaderProgID, "bDoNotLight");

	if (pCurrentObject->isWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);		// LINES
		glUniform4f(debugColour_UL,
			pCurrentObject->debugColour.r,
			pCurrentObject->debugColour.g,
			pCurrentObject->debugColour.b,
			pCurrentObject->debugColour.a);
		glUniform1f(bDoNotLight_UL, (float)GL_TRUE);
	}
	else {	// Regular object (lit and not wireframe)
		glUniform1f(bDoNotLight_UL, (float)GL_FALSE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		// SOLID
	}
	//glPointSize(15.0f);

	if (pCurrentObject->disableDepthBufferTest)
		glDisable(GL_DEPTH_TEST);					// DEPTH Test OFF

	else
		glEnable(GL_DEPTH_TEST);						// Turn ON depth test

	if (pCurrentObject->disableDepthBufferWrite)
		glDisable(GL_DEPTH);						// DON'T Write to depth buffer

	else
		glEnable(GL_DEPTH);								// Write to depth buffer


	sModelDrawInfo drawInfo;
	if (pVAOManager->FindDrawInfoByModelName(pCurrentObject->meshName, drawInfo)) {
		glBindVertexArray(drawInfo.VAO_ID);
		glDrawElements(GL_TRIANGLES,
			drawInfo.numberOfIndices,
			GL_UNSIGNED_INT,
			0);
		glBindVertexArray(0);
	}

	return;
} // DrawObject;


// returns NULL (0) if we didn't find it.
cModelObject* pFindObjectByFriendlyName(std::string name) {
	// Do a linear search 
	for (unsigned int index = 0; index != g_vec_pGameObjects.size(); index++) {
		if (::g_vec_pGameObjects[index]->friendlyName == name) {
			// Found it!!
			return ::g_vec_pGameObjects[index];
		}
	}
	// Didn't find it
	return NULL;
}

// returns NULL (0) if we didn't find it.
cModelObject* pFindObjectByFriendlyNameMap(std::string name) {
	return ::mpModel[name];
}
