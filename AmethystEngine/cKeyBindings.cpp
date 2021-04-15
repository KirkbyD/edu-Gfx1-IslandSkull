#include "GLCommon.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "cCallbacks.hpp"
#include "cCommandManager.hpp"
#include "Globals.hpp"

#include <stdio.h>
#include <iostream>o

bool isShiftKeyDownByAlone(int mods);
bool isCtrlKeyDownByAlone(int mods);
float devSpeed = 180.0f;
float maxSpeed = 2.0;

// ADDITIONALLY CHANGE THE DEVELOPER CONSOLE TO USE A SECONDARY WINDOW WITH THE STUFF FROM OSCARS CLASS
// FUCNTION FOR ACTUALLY ISSUING THE COMMAND (THIS IS ESSENTIALLY THE MEDIATOR)
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	cCommandManager* pCmdMngr = cCommandManager::GetCommandManager();
	float accelSpeed = 180.0f * deltaTime;
	float rotationSpeed = 1.0f * deltaTime; //assuming 60 fps, move 3 unit a second
	float devMoveSpeed = devSpeed * deltaTime;

	if (!isDevCon && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		if (key == GLFW_KEY_W)
			mainCamera.MoveForBack(devSpeed * deltaTime);
		if (key == GLFW_KEY_S)
			mainCamera.MoveForBack(-devSpeed * deltaTime);
		if (key == GLFW_KEY_A)
			mainCamera.MoveStrafe(devSpeed * deltaTime);
		if (key == GLFW_KEY_D)
			mainCamera.MoveStrafe(-devSpeed * deltaTime);
		if (key == GLFW_KEY_Q)
			mainCamera.MoveUpDown(devSpeed * deltaTime);
		if (key == GLFW_KEY_E)
			mainCamera.MoveUpDown(-devSpeed * deltaTime);
	}

	if (!isDevCon && action == GLFW_PRESS) {
		if (key == GLFW_KEY_1) {
			if (mainCamera.GetLookAtTarget() != pFindObjectByFriendlyName("TheSkull")) {
				mainCamera.SetTarget(pFindObjectByFriendlyName("TheSkull"));
				mainCamera.SetEye(glm::vec3(75.0f, 190.0f, 60.0f));
			}
			MIDTERMDRONECAM = false;
		}
		if (key == GLFW_KEY_2) {
			if (mainCamera.GetLookAtTarget() != pFindObjectByFriendlyName("BeachCave01")) {
				mainCamera.SetTarget(pFindObjectByFriendlyName("BeachCave01"));
				mainCamera.SetEye(glm::vec3(-153.0f, 21.0f, 178.0f));
			}
			MIDTERMDRONECAM = false;
		}
		if (key == GLFW_KEY_3) {
			if (mainCamera.GetLookAtTarget() != pFindObjectByFriendlyName("TheIsland"))	{
				mainCamera.SetTarget(pFindObjectByFriendlyName("TheIsland"));
				mainCamera.SetEye(mpLight["SceneLight1"]->getPosition());
			}
			MIDTERMDRONECAM = false;
		}
		if (key == GLFW_KEY_4) {
			if (MIDTERMDRONECAM != true) {
				mainCamera.SetEye(glm::vec3(droneRadius, droneHeight, droneRadius));
				mainCamera.SetTarget(pFindObjectByFriendlyName("TheIsland"));
				MIDTERMDRONECAM = true;
			}
		}
		if (key == GLFW_KEY_5) {
			if (mainCamera.GetLookAtTarget() != pFindObjectByFriendlyName("WaterCave01")) {
				mainCamera.SetEye(glm::vec3(-25.0f, 11.0f, -155.0f));
				mainCamera.SetTarget(pFindObjectByFriendlyName("WaterCave01"));
			}
			MIDTERMDRONECAM = false;
		}
		if (key == GLFW_KEY_0) {
			if (mainCamera.GetLookAtTarget() != NULL) {
				mainCamera.SetTarget(NULL);
			}
			MIDTERMDRONECAM = false;
		}
		if (key == GLFW_KEY_T) {
			//toggle lights for Q2
			cLightObject* bigLight = mpLight["SceneLight1"];
			if (MIDTERMDAYLIGHT) {
				//turn on lights / dim overhead
				for (std::pair<std::string, cLightObject*> lightPair : mpLight) {
					lightPair.second->param2 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
				}
				bigLight->atten = glm::vec4(0.0f, 0.00008, 0.0000073, 0.06f);
			}
			else {
				//turn off lights and brighten overhead.
				for (std::pair<std::string, cLightObject*> lightPair : mpLight) {
					lightPair.second->param2 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				}
				bigLight->atten = glm::vec4(0.0f, 0.00008, 0.0000018, 0.06f);
				bigLight->param2 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			}
			MIDTERMDAYLIGHT = !MIDTERMDAYLIGHT;
		}
		if (key == GLFW_KEY_F) {
			MIDTERMFIREFLYDANCE = !MIDTERMFIREFLYDANCE;
		}
		if (key == GLFW_KEY_R) {
			MIDTERMSKULLFLICKER = !MIDTERMSKULLFLICKER;
		}
	}

	//if (isDevCon && !isShiftKeyDownByAlone(mods) && isCtrlKeyDownByAlone(mods) && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
	//	// Position
	//	if (key == GLFW_KEY_PERIOD) {
	//		devSpeed *= 1.1;
	//	}
	//	if (key == GLFW_KEY_COMMA) {
	//		devSpeed *= 0.9;
	//	}
	//	if (isModelSelect) {
	//		if (key == GLFW_KEY_A)
	//			g_vec_pGameObjects[vpGameObjectsCurrentPos]->positionXYZ += mainCamera.GetRight() * devMoveSpeed;
	//		if (key == GLFW_KEY_D) 
	//			g_vec_pGameObjects[vpGameObjectsCurrentPos]->positionXYZ -= mainCamera.GetRight() * devMoveSpeed;
	//		if (key == GLFW_KEY_Q)
	//			g_vec_pGameObjects[vpGameObjectsCurrentPos]->positionXYZ += mainCamera.GetUp() * devMoveSpeed;
	//		if (key == GLFW_KEY_E)
	//			g_vec_pGameObjects[vpGameObjectsCurrentPos]->positionXYZ -= mainCamera.GetUp() * devMoveSpeed;
	//		if (key == GLFW_KEY_W)
	//			g_vec_pGameObjects[vpGameObjectsCurrentPos]->positionXYZ += mainCamera.GetFront() * devMoveSpeed;
	//		if (key == GLFW_KEY_S)
	//			g_vec_pGameObjects[vpGameObjectsCurrentPos]->positionXYZ -= mainCamera.GetFront() * devMoveSpeed;
	//		if (key == GLFW_KEY_R)
	//			g_vec_pGameObjects[vpGameObjectsCurrentPos]->rotationXYZ.y += 0.02;
	//		if (key == GLFW_KEY_T)
	//			g_vec_pGameObjects[vpGameObjectsCurrentPos]->rotationXYZ.y -= 0.02;
	//		if (key == GLFW_KEY_F)
	//			g_vec_pGameObjects[vpGameObjectsCurrentPos]->rotationXYZ.x += 0.02;
	//		if (key == GLFW_KEY_G)
	//			g_vec_pGameObjects[vpGameObjectsCurrentPos]->rotationXYZ.x -= 0.02;
	//		if (key == GLFW_KEY_V)
	//			g_vec_pGameObjects[vpGameObjectsCurrentPos]->rotationXYZ.z += 0.02;
	//		if (key == GLFW_KEY_B)
	//			g_vec_pGameObjects[vpGameObjectsCurrentPos]->rotationXYZ.z -= 0.02;
	//	}
	//	if (isLightSelect) {
	//		if (key == GLFW_KEY_A) {
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.x += mainCamera.GetRight().x * devMoveSpeed;
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.y += mainCamera.GetRight().y * devMoveSpeed;
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.z += mainCamera.GetRight().z * devMoveSpeed;
	//		}
	//		if (key == GLFW_KEY_D) {
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.x -= mainCamera.GetRight().x * devMoveSpeed;
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.y -= mainCamera.GetRight().y * devMoveSpeed;
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.z -= mainCamera.GetRight().z * devMoveSpeed;
	//		}
	//		if (key == GLFW_KEY_Q) {
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.x += mainCamera.GetUp().x * devMoveSpeed;
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.y += mainCamera.GetUp().y * devMoveSpeed;
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.z += mainCamera.GetUp().z * devMoveSpeed;
	//		}
	//		if (key == GLFW_KEY_E) {
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.x -= mainCamera.GetUp().x * devMoveSpeed;
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.y -= mainCamera.GetUp().y * devMoveSpeed;
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.z -= mainCamera.GetUp().z * devMoveSpeed;
	//		}
	//		if (key == GLFW_KEY_W) {
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.x += mainCamera.GetFront().x * devMoveSpeed;
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.y += mainCamera.GetFront().y * devMoveSpeed;
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.z += mainCamera.GetFront().z * devMoveSpeed;
	//		}
	//		if (key == GLFW_KEY_S) {
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.x -= mainCamera.GetFront().x * devMoveSpeed;
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.y -= mainCamera.GetFront().y * devMoveSpeed;
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->position.z -= mainCamera.GetFront().z * devMoveSpeed;
	//		}
	//		if (key == GLFW_KEY_1)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.x *= 0.99f;			// 99% of what it was
	//		if (key == GLFW_KEY_2)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.x *= 1.01f;			// 1% more of what it was
	//		if (key == GLFW_KEY_3)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.y *= 0.99f;			// 99% of what it was
	//		if (key == GLFW_KEY_4)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.y *= 1.01f;			// 1% more of what it was
	//		if (key == GLFW_KEY_5)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.z *= 0.99f;			// 99% of what it was
	//		if (key == GLFW_KEY_6)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.z *= 1.01f;			// 1% more of what it was
	//		if (key == GLFW_KEY_H) {
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->param1.y -= 0.1f;
	//			if(g_vec_pLightObjects[vpLightObjectsCurrentPos]->param1.y <= 0.0f)
	//				g_vec_pLightObjects[vpLightObjectsCurrentPos]->param1.y = 0.0f;
	//		}
	//		if (key == GLFW_KEY_J)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->param1.y += 0.1f;
	//		if (key == GLFW_KEY_N) {
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->param1.z -= 0.1f;
	//			if (g_vec_pLightObjects[vpLightObjectsCurrentPos]->param1.z <= 0.0f)
	//				g_vec_pLightObjects[vpLightObjectsCurrentPos]->param1.z = 0.0f;
	//		}
	//		if (key == GLFW_KEY_M)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->param1.z += glm::radians(0.1f);
	//		if (key == GLFW_KEY_R)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->direction.y += glm::radians(0.2);
	//		if (key == GLFW_KEY_T)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->direction.y -= glm::radians(0.2);
	//		if (key == GLFW_KEY_F)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->direction.x += glm::radians(0.2);
	//		if (key == GLFW_KEY_G)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->direction.x -= glm::radians(0.2);
	//		if (key == GLFW_KEY_V)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->direction.z += glm::radians(0.2);
	//		if (key == GLFW_KEY_B)
	//			g_vec_pLightObjects[vpLightObjectsCurrentPos]->direction.z -= glm::radians(0.2);
	//	}
	//	if (isComplexSelect) {
	//		if (key == GLFW_KEY_A)
	//			g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetPosition(	g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetPosition()
	//																			+ mainCamera.GetRight() * devMoveSpeed);
	//		if (key == GLFW_KEY_D)
	//			g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetPosition(	g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetPosition()
	//																			- mainCamera.GetRight() * devMoveSpeed);
	//		if (key == GLFW_KEY_Q)
	//			g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetPosition(	g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetPosition()
	//																			+ mainCamera.GetUp() * devMoveSpeed);
	//		if (key == GLFW_KEY_E)
	//			g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetPosition(	g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetPosition()
	//																			- mainCamera.GetUp() * devMoveSpeed);
	//		if (key == GLFW_KEY_W)
	//			g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetPosition(	g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetPosition()
	//																			+ mainCamera.GetFront() * devMoveSpeed);
	//		if (key == GLFW_KEY_S)
	//			g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetPosition(	g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetPosition()
	//																			- mainCamera.GetFront() * devMoveSpeed);
	//	}
	//}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool isShiftKeyDownByAlone(int mods) {
	if (mods == GLFW_MOD_SHIFT)
		return true;
	return false;
}

bool isCtrlKeyDownByAlone(int mods) {
	if (mods == GLFW_MOD_CONTROL)
		return true;
	return false;
}

void character_callback(GLFWwindow* window, unsigned codepoint) {
	// STUFF FOR THE DEVELOPER CONSOLE
	if ((char)codepoint != '`')
		cmdstr.push_back((char)codepoint);
}