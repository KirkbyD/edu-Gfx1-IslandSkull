#include "cPhysics.hpp"
#include "cComplexObject.hpp"
#include <time.h>

cPhysics::cPhysics()
{
	this->m_Gravity = glm::vec3(0.0f, -9.81f, 0.0f);
	this->m_Friction = glm::vec3(0.99f, 0.8f, 0.99f);
	this->b_cubeSwap = false;
	srand(time(NULL));
	return;
}


void cPhysics::setGravity(glm::vec3 newGravityValue)
{
	this->m_Gravity = newGravityValue;
	return;
}

glm::vec3 cPhysics::getGravity(void)
{
	return this->m_Gravity;
}


void cPhysics::IntegrationStep(std::vector<cModelObject*>& vec_pGameObjects, float deltaTime)
{
	for (unsigned int index = 0;
		index != vec_pGameObjects.size(); index++)
	{
		cModelObject* pCurObj = vec_pGameObjects[index];

		if (pCurObj->inverseMass != 0.0f && pCurObj->parentObject == NULL)
		{
			// Forward Explicit Euler Inetegration
			//NewVelocty += Velocity + ( Ax * DeltaTime )

			pCurObj->velocity.x += pCurObj->accel.x * deltaTime;
			pCurObj->velocity.y += (pCurObj->accel.y -9.81f) * deltaTime;
			pCurObj->velocity.z += pCurObj->accel.z * deltaTime;

			pCurObj->prevPositionXYZ.x = pCurObj->positionXYZ.x;
			pCurObj->prevPositionXYZ.y = pCurObj->positionXYZ.y;
			pCurObj->prevPositionXYZ.z = pCurObj->positionXYZ.z;

			pCurObj->positionXYZ.x += pCurObj->velocity.x * deltaTime;
			pCurObj->positionXYZ.y += pCurObj->velocity.y * deltaTime;
			pCurObj->positionXYZ.z += pCurObj->velocity.z * deltaTime;

			//Respawning spheres
			if (pCurObj->positionXYZ.y < -5) {
				pCurObj->positionXYZ = glm::vec3(rand() % 25 - 25, 20.0f, rand() % 25 - 25);
				pCurObj->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
				pCurObj->accel = glm::vec3(0.0f, 0.0f, 0.0f);
			}

			/*if ((pCurObj->positionXYZ.x <= -5.0f || pCurObj->positionXYZ.x >= 5.0f) || (pCurObj->positionXYZ.z <= 0.0f || pCurObj->positionXYZ.z >= 10.0f)) {
				if (pCurObj->friendlyName == "FallingSphere")
				{
					pCurObj->positionXYZ = glm::vec3(0.0f, 5.0f, 5.0f);
					pCurObj->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
				}
			}*/

			if ((pCurObj->positionXYZ.x <= -15.0f || pCurObj->positionXYZ.x >= 15.0f) || (pCurObj->positionXYZ.z <= -30.0f || pCurObj->positionXYZ.z >= 0.0f)) {
				if (pCurObj->friendlyName == "SphereForAnnulus")
				{
					pCurObj->positionXYZ = glm::vec3(0.0f, 20.0f, -15.0f);
					pCurObj->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
				}
			}
		}

		// Dynamic hack	
		if (pCurObj->friendlyName == "MovingCube" && pCurObj->parentObject == NULL)
		{
			if (b_cubeSwap)	{
				if (pCurObj->positionXYZ.z <= 5.0f)	{
					b_cubeSwap = false;
				}
				pCurObj->positionXYZ.z -= deltaTime;
			}
			else	{
				if (pCurObj->positionXYZ.z >= 15.0f) {
					b_cubeSwap = true;
				}
				pCurObj->positionXYZ.z += deltaTime;
			}			
		}

		if (pCurObj->friendlyName == "NotAxisAlignedAnnulus" && pCurObj->parentObject == NULL)
		{
			if (pCurObj->rotationXYZ.z >= 6.28319f)
			{
				pCurObj->rotationXYZ.z = 0.0f;
			}
			pCurObj->rotationXYZ.z += deltaTime;
		}
	}

	for (unsigned int index = 0;
		index != g_vec_pComplexObjects.size(); index++)
	{
		cComplexObject* pCurObj = g_vec_pComplexObjects[index];

		if (pCurObj->GetInverseMass() != 0.0f && pCurObj->parentObject == NULL)
		{
			// Forward Explicit Euler Inetegration
			//NewVelocty += Velocity + ( Ax * DeltaTime )

			glm::vec3 vel = pCurObj->GetVelocity();
			glm::vec3 accel = pCurObj->GetAcceleration();

			vel.x += accel.x * deltaTime;
			vel.y += (accel.y + this->m_Gravity.y) * deltaTime;
			vel.z += accel.z * deltaTime;
			
			glm::vec3 pos = pCurObj->getPosition();
			pCurObj->SetPreviousPosition(pos);

			pos.x += vel.x * deltaTime;
			pos.y += vel.y * deltaTime;
			pos.z += vel.z * deltaTime;

			pCurObj->SetPosition(pos);

			if (pCurObj->GetPosition().y < -5) {
				pCurObj->SetPosition(glm::vec3(0.0f, 50.0f, 0.0f));
				pCurObj->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			}
		}
	}

	return;
}

void cPhysics::GetClosestTriangleToPoint(Point pointXYZ, cMesh& mesh, glm::vec3& closestPoint, sPhysicsTriangle& closestTriangle)
{
	float closestDistanceSoFar = FLT_MAX;

	for (unsigned int triIndex = 0;
		triIndex != mesh.vecTriangles.size();
		triIndex++)
	{
		sPlyTriangle& curTriangle = mesh.vecTriangles[triIndex];

		sPlyVertexXYZ_N triVert1 = mesh.vecVertices[curTriangle.vert_index_1];
		sPlyVertexXYZ_N triVert2 = mesh.vecVertices[curTriangle.vert_index_2];
		sPlyVertexXYZ_N triVert3 = mesh.vecVertices[curTriangle.vert_index_3];

		Point triVertPoint1;
		triVertPoint1.x = triVert1.x;
		triVertPoint1.y = triVert1.y;
		triVertPoint1.z = triVert1.z;

		Point triVertPoint2;
		triVertPoint2.x = triVert2.x;
		triVertPoint2.y = triVert2.y;
		triVertPoint2.z = triVert2.z;

		Point triVertPoint3;
		triVertPoint3.x = triVert3.x;
		triVertPoint3.y = triVert3.y;
		triVertPoint3.z = triVert3.z;

		glm::vec3 curClosetPoint = ClosestPtPointTriangle(pointXYZ,
			triVertPoint1, triVertPoint2, triVertPoint3);

		float distanceNow = glm::distance(curClosetPoint, pointXYZ);

		if (distanceNow <= closestDistanceSoFar)
		{
			closestDistanceSoFar = distanceNow;

			closestPoint = curClosetPoint;

			closestTriangle.verts[0].x = triVert1.x;
			closestTriangle.verts[0].y = triVert1.y;
			closestTriangle.verts[0].z = triVert1.z;
			closestTriangle.verts[1].x = triVert2.x;
			closestTriangle.verts[1].y = triVert2.y;
			closestTriangle.verts[1].z = triVert2.z;
			closestTriangle.verts[2].x = triVert3.x;
			closestTriangle.verts[2].y = triVert3.y;
			closestTriangle.verts[2].z = triVert3.z;

			glm::vec3 triVert1Norm = glm::vec3(triVert1.nx, triVert1.ny, triVert1.nz);
			glm::vec3 triVert2Norm = glm::vec3(triVert2.nx, triVert2.ny, triVert2.nz);
			glm::vec3 triVert3Norm = glm::vec3(triVert3.nx, triVert3.ny, triVert3.nz);

			closestTriangle.normal = (triVert1Norm + triVert2Norm + triVert3Norm) / 3.0f;
		}

	}

	return;
}

void cPhysics::GetClosestTrianglesToSphere(Point pointXYZ, float distanceRange, cMesh& mesh,
	std::vector<glm::vec3>& vecClosestPoints, std::vector<sPhysicsTriangle>& vecClosestTriangles)
{
	float closestDistanceSoFar = FLT_MAX;
	sPhysicsTriangle closestTriangle;

	for (unsigned int triIndex = 0;
		triIndex != mesh.vecTriangles.size();
		triIndex++)
	{
		sPlyTriangle& curTriangle = mesh.vecTriangles[triIndex];

		sPlyVertexXYZ_N triVert1 = mesh.vecVertices[curTriangle.vert_index_1];
		sPlyVertexXYZ_N triVert2 = mesh.vecVertices[curTriangle.vert_index_2];
		sPlyVertexXYZ_N triVert3 = mesh.vecVertices[curTriangle.vert_index_3];

		Point triVertPoint1;
		triVertPoint1.x = triVert1.x;
		triVertPoint1.y = triVert1.y;
		triVertPoint1.z = triVert1.z;

		Point triVertPoint2;
		triVertPoint2.x = triVert2.x;
		triVertPoint2.y = triVert2.y;
		triVertPoint2.z = triVert2.z;

		Point triVertPoint3;
		triVertPoint3.x = triVert3.x;
		triVertPoint3.y = triVert3.y;
		triVertPoint3.z = triVert3.z;

		glm::vec3 curClosetPoint = ClosestPtPointTriangle(pointXYZ,
			triVertPoint1, triVertPoint2, triVertPoint3);

		float distanceNow = glm::distance(curClosetPoint, pointXYZ);

		if (distanceNow <= closestDistanceSoFar)
		{
			closestDistanceSoFar = distanceNow;

			closestTriangle.verts[0].x = triVert1.x;
			closestTriangle.verts[0].y = triVert1.y;
			closestTriangle.verts[0].z = triVert1.z;
			closestTriangle.verts[1].x = triVert2.x;
			closestTriangle.verts[1].y = triVert2.y;
			closestTriangle.verts[1].z = triVert2.z;
			closestTriangle.verts[2].x = triVert3.x;
			closestTriangle.verts[2].y = triVert3.y;
			closestTriangle.verts[2].z = triVert3.z;

			glm::vec3 triVert1Norm = glm::vec3(triVert1.nx, triVert1.ny, triVert1.nz);
			glm::vec3 triVert2Norm = glm::vec3(triVert2.nx, triVert2.ny, triVert2.nz);
			glm::vec3 triVert3Norm = glm::vec3(triVert3.nx, triVert3.ny, triVert3.nz);

			closestTriangle.normal = (triVert1Norm + triVert2Norm + triVert3Norm) / 3.0f;

			if (distanceNow <= distanceRange)
			{
				vecClosestPoints.push_back(curClosetPoint);
				vecClosestTriangles.push_back(closestTriangle);
			}

		}

	}

	return;
}


void cPhysics::TestForCollisions(std::vector<cModelObject*>& vec_pGameObjects)
{
	// This will store all the collisions in this frame
	//std::vector<sCollisionInfo> vecCollisions;

	//sCollisionInfo collisionInfo;

	for (unsigned int outerLoopIndex = 0;
		outerLoopIndex != vec_pGameObjects.size(); outerLoopIndex++)
	{
		for (unsigned int innerLoopIndex = 0;
			innerLoopIndex != vec_pGameObjects.size(); innerLoopIndex++)
		{
			cModelObject* pA = vec_pGameObjects[outerLoopIndex];
			cModelObject* pB = vec_pGameObjects[innerLoopIndex];

			// Note that if you don't respond to the 
			// collision here, then you will get the same
			// result twice (Object "A" with "B" and later, 
			// object "B" with "A" - but it's the same collison

			//if (pA->getUniqueID() == pB->getUniqueID())
			if (pA->friendlyName == pB->friendlyName)
			{
				// It's the same object
			}
			else if (pA->physicsShapeType == SPHERE && pB->physicsShapeType == SPHERE)
			{
				if (DoSphereSphereCollisionTest(pA, pB/*, collisionInfo*/))
				{
					//Nice!
					//vecCollisions.push_back(collisionInfo);
				}
			}
			else if (pA->physicsShapeType == SPHERE && pB->physicsShapeType == MESH)
			{
				if (DoSphereMeshCollisionTest(pA, pB/*, collisionInfo*/))
				{
					//Nice!
					//vecCollisions.push_back(collisionInfo);
				}
			}
		}
	}

}

bool cPhysics::DoSphereSphereCollisionTest(cModelObject* pA, cModelObject* pB)//, sCollisionInfo& collisionInfo)
{
	if (glm::length(pA->positionXYZ - pB->positionXYZ) <= pA->scale + pB->scale)
	{

		float m1, m2, x1, x2;
		glm::vec3 v1temp, v1, v2, v1x, v2x, v1y, v2y, x(pA->positionXYZ - pB->positionXYZ);

		glm::normalize(x);
		v1 = pA->velocity;
		x1 = dot(x, v1);
		v1x = x * x1;
		v1y = v1 - v1x;
		m1 = pA->inverseMass;

		x = x * -1.0f;
		v2 = pB->velocity;
		x2 = dot(x, v2);
		v2x = x * x2;
		v2y = v2 - v2x;
		m2 = pB->inverseMass;

		pA->positionXYZ = pA->prevPositionXYZ;
		pA->velocity = glm::vec3(v1x * (m1 - m2) / (m1 + m2) + v2x * (2 * m2) / (m1 + m2) + v1y) / 4.0f;

		if (pB->parentObject == NULL) {
			pB->positionXYZ = pB->prevPositionXYZ;
			pB->velocity = glm::vec3(v1x * (2 * m1) / (m1 + m2) + v2x * (m2 - m1) / (m1 + m2) + v2y) / 4.0f;
		}
		else {
			((cComplexObject*)pB->parentObject)->SetPosition(((cComplexObject*)pB->parentObject)->GetPosition());
			((cComplexObject*)pB->parentObject)->SetVelocity(glm::vec3(v1x * (2 * m1) / (m1 + m2) + v2x * (m2 - m1) / (m1 + m2) + v2y) / 4.0f);
		}
		
		return true;
	}

	return false;
}

bool cPhysics::DoSphereMeshCollisionTest(cModelObject* pA, cModelObject* pB)//, sCollisionInfo& collisionInfo)
{

	glm::vec3 closestPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	cPhysics::sPhysicsTriangle closestTriangle;

	//std::vector<glm::vec3> vecClosestPoints;
	//std::vector<sPhysicsTriangle> vecClosestTriangles;

	glm::mat4 matWorld = calculateWorldMatrix(pB);

	cMesh newMesh;
	if (mpMesh.find(pB->meshName) != mpMesh.end())
	{
		CalculateTransformedMesh(*mpMesh[pB->meshName], matWorld, newMesh);
	}
	else
	{
		return false;
	}

	GetClosestTriangleToPoint(pA->positionXYZ, newMesh, closestPoint, closestTriangle);
	//GetClosestTrianglesToSphere(pA->positionXYZ, pA->scale, newMesh, vecClosestPoints, vecClosestTriangles);


	float distance = glm::length(pA->positionXYZ - closestPoint);

	if (distance <= pA->scale && pA->parentObject == NULL)
	{
		pA->positionXYZ.y = pA->prevPositionXYZ.y;

		float distanceAfter = glm::length(pA->positionXYZ - closestPoint);

		glm::vec3 velocityVector = glm::normalize(pA->velocity);

		glm::vec3 reflectionVec = glm::reflect(velocityVector, closestTriangle.normal);
		reflectionVec = glm::normalize(reflectionVec);

		reflectionVec *= m_Friction;

		float speed = glm::length(pA->velocity);

		pA->velocity = reflectionVec * speed;

		// Dynamic hack
		//if (pB->friendlyName != "TheGround")
		if (pB->friendlyName == "MovingCube" || pB->friendlyName == "NotAxisAlignedAnnulus")
		{
			Vector newVec = glm::normalize(pA->positionXYZ - closestPoint);
			pA->velocity += newVec * 5.0f;
		}

		return true;
	}

	else if (pA->parentObject != NULL && distance <= pA->scale * ((cComplexObject*)pA->parentObject)->GetScale())
	{
		((cComplexObject*)pA->parentObject)->SetPosition(((cComplexObject*)pA->parentObject)->GetPreviousPosition());

		glm::vec3 velocityVector = glm::normalize(((cComplexObject*)pA->parentObject)->GetVelocity());

		glm::vec3 reflectionVec = glm::reflect(velocityVector, closestTriangle.normal);
		reflectionVec = glm::normalize(reflectionVec);

		reflectionVec *= m_Friction;

		float speed = glm::length(((cComplexObject*)pA->parentObject)->GetVelocity());

		((cComplexObject*)pA->parentObject)->SetVelocity(reflectionVec * speed);

		return true;
	}
		
	return false;
}

glm::mat4 cPhysics::calculateWorldMatrix(cModelObject* pCurrentObject)
{

	glm::mat4 matWorld = glm::mat4(1.0f);


	// TRANSLATION
	glm::mat4 matTrans = glm::translate(glm::mat4(1.0f),
		glm::vec3(pCurrentObject->positionXYZ.x,
			pCurrentObject->positionXYZ.y,
			pCurrentObject->positionXYZ.z));
	matWorld = matWorld * matTrans;


	// ROTATION
	glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
		pCurrentObject->rotationXYZ.x,			// Angle 
		glm::vec3(1.0f, 0.0f, 0.0f));
	matWorld = matWorld * rotateX;

	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
		pCurrentObject->rotationXYZ.y,			// Angle 
		glm::vec3(0.0f, 1.0f, 0.0f));
	matWorld = matWorld * rotateY;

	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
		pCurrentObject->rotationXYZ.z,			// Angle 
		glm::vec3(0.0f, 0.0f, 1.0f));
	matWorld = matWorld * rotateZ;


	// SCALE
	glm::mat4 scale = glm::scale(glm::mat4(1.0f),
		glm::vec3(pCurrentObject->scale,
			pCurrentObject->scale,
			pCurrentObject->scale));
	matWorld = matWorld * scale;


	return matWorld;
}

void cPhysics::CalculateTransformedMesh(cMesh& originalMesh, glm::mat4 matWorld,
	cMesh& mesh_transformedInWorld)
{
	mesh_transformedInWorld = originalMesh;

	for (std::vector<sPlyVertexXYZ_N>::iterator itVert = mesh_transformedInWorld.vecVertices.begin();
		itVert != mesh_transformedInWorld.vecVertices.end(); itVert++)
	{
		glm::vec4 vertex = glm::vec4(itVert->x, itVert->y, itVert->z, 1.0f);

		glm::vec4 vertexWorldTransformed = matWorld * vertex;

		itVert->x = vertexWorldTransformed.x;
		itVert->y = vertexWorldTransformed.y;
		itVert->z = vertexWorldTransformed.z;

		glm::mat4 matWorld_Inv_Transp = glm::inverse(glm::transpose(matWorld));

		glm::vec4 normal = glm::vec4(itVert->nx, itVert->ny, itVert->nz, 1.0f);

		glm::vec4 normalWorldTransformed = matWorld_Inv_Transp * normal;

		itVert->nx = normalWorldTransformed.x;
		itVert->ny = normalWorldTransformed.y;
		itVert->nz = normalWorldTransformed.z;
	}

	return;
}
