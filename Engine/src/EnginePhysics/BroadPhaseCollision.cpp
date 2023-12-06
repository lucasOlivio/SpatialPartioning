#include "EnginePhysics/BroadPhaseCollision.h"
#include "scene/SceneView.h"
#include "components/Collision.h"
#include "components/Transform.h"
#include "components/Model.h"
#include <math.h>

typedef std::pair< uint, cAABB* > pairAABBs;
typedef std::map< uint, cAABB* >::iterator itAABBs;

BroadPhaseCollision::BroadPhaseCollision()
{
	// TODO: For non-static grid aabbs this should be dynamic, comming from component size or map
	m_lengthPerBox = glm::vec3(3'000, 3'000, 3'000);
}

BroadPhaseCollision::~BroadPhaseCollision()
{
	ClearAABBs();
}

void BroadPhaseCollision::LoadScene()
{
	ClearAABBs();

	SceneView* pScene = SceneView::Get();

	for (pScene->First("collision"); !pScene->IsDone(); pScene->Next())
	{
		EntityID entityID = pScene->CurrentKey();
		CollisionComponent* pCollision = pScene->CurrentValue<CollisionComponent>();

		if (!pCollision->IsActive())
		{
			continue;
		}

		if (pCollision->Get_eShape() == eShape::MESH_OF_TRIANGLES_INDIRECT)
		{
			m_InsertMeshTrianglesIndirect(entityID);
		}
	}
}

void BroadPhaseCollision::ClearAABBs()
{
	for (pairAABBs pairAABB : m_mapAABBs)
	{
		delete pairAABB.second;
	}

	m_mapAABBs.clear();
}

uint BroadPhaseCollision::LocatePoint(glm::vec3 point)
{
	unsigned int theX = (unsigned int)floor(point.x / m_lengthPerBox.x);
	unsigned int theY = (unsigned int)floor(point.y / m_lengthPerBox.y);
	unsigned int theZ = (unsigned int)floor(point.z / m_lengthPerBox.z);

	// Shift the numbers into the columns we want:
	// xxx,yyy,zzz
	// For 32 bits, we'll multiply by 1000
	unsigned int theIndex = (theX * 1000 * 1000) +
							       (theY * 1000) +
							              (theZ);

	return theIndex;
}

glm::vec3 BroadPhaseCollision::ReverseLocatePoint(uint theIndex)
{
	unsigned int theZ = theIndex % 1000;
	unsigned int theY = (theIndex / 1000) % 1000;
	unsigned int theX = theIndex / (1000 * 1000);

	glm::vec3 minPoint;
	minPoint.x = (float)theX * m_lengthPerBox.x;
	minPoint.y = (float)theY * m_lengthPerBox.y;
	minPoint.z = (float)theZ * m_lengthPerBox.z;

	return minPoint;
}

cAABB* BroadPhaseCollision::GetAABB(uint idxAABB)
{
	itAABBs itAABB = m_mapAABBs.find(idxAABB);
	if (itAABB == m_mapAABBs.end())
	{
		return nullptr;
	}

	return itAABB->second;
}

cAABB* BroadPhaseCollision::GetAABB(glm::vec3 point)
{
	uint idxAABB = LocatePoint(point);
	
	return GetAABB(idxAABB);
}

cAABB* BroadPhaseCollision::m_GetOrCreateAABB(uint idxAABB)
{
	cAABB* pAABB = GetAABB(idxAABB);
	if (pAABB)
	{
		return pAABB;
	}

	pAABB = new cAABB();
	pAABB->minXYZ = ReverseLocatePoint(idxAABB);
	pAABB->maxXYZ = pAABB->minXYZ + m_lengthPerBox;

	m_mapAABBs[idxAABB] = pAABB;

	return pAABB;
}

void BroadPhaseCollision::m_InsertTriangle(int indexTriangle, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
	// Locate each vertex
	uint idxV1 = LocatePoint(v1);
	uint idxV2 = LocatePoint(v2);
	uint idxV3 = LocatePoint(v3);

	// TODO: When a triangle edge passes 3 or more aabbs it should be in those aabbs too
	// Insert triangle into all AABBs that it intersects
	cAABB* pAABB = m_GetOrCreateAABB(idxV1);
	pAABB->vecIdxTriangles.push_back(indexTriangle);

	if (idxV2 != idxV1)
	{
		cAABB* pAABB2 = m_GetOrCreateAABB(idxV2);
		pAABB->vecIdxTriangles.push_back(indexTriangle);
	}

	if (idxV3 != idxV1 && idxV3 != idxV2)
	{
		cAABB* pAABB3 = m_GetOrCreateAABB(idxV3);
		pAABB->vecIdxTriangles.push_back(indexTriangle);
	}

	return;
}

void BroadPhaseCollision::m_InsertMeshTrianglesIndirect(EntityID entityID)
{
	SceneView* pScene = SceneView::Get();

	TransformComponent* pTransform = pScene->GetComponent<TransformComponent>(entityID, "transform");
	// TODO: Should get for each mesh? Then would have to update on animation change
	// Get first mesh only from array
	ModelComponent* pModel = pScene->GetComponent<ModelComponent>(entityID, "model");
	
	sMesh* pMesh       = pModel->GetMesh(0);
	glm::mat4 matModel = pTransform->GetTransform();

	// for every triangle in the mesh insert into aabb
	for (uint i = 0; i < pMesh->numberOfTriangles; i++)
	{
		// Get vertices converting to worldposition
		glm::vec3 v1 = (matModel * glm::vec4(pMesh->pTriangles[i].vertices[0], 1.0f));
		glm::vec3 v2 = (matModel * glm::vec4(pMesh->pTriangles[i].vertices[1], 1.0f));
		glm::vec3 v3 = (matModel * glm::vec4(pMesh->pTriangles[i].vertices[2], 1.0f));

		m_InsertTriangle(i, v1, v2, v3);
	}
}

