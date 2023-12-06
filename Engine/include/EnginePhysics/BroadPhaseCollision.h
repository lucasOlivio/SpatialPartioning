#pragma once

#include "cAABB.h"
#include <map>

class BroadPhaseCollision
{
public:
	BroadPhaseCollision();
	~BroadPhaseCollision();

	void LoadScene();
	void ClearAABBs();

	// Calculate in which AABB index this points is
	uint LocatePoint(glm::vec3 point);

	glm::vec3 ReverseLocatePoint(uint theIndex);

	// Returns the relative AABB to this location
	cAABB* GetAABB(glm::vec3 point);
	// Returns the relative AABB to this index
	cAABB* GetAABB(uint idxAABB);
private:
	std::map< uint /*index*/, cAABB* > m_mapAABBs;

	glm::vec3 m_lengthPerBox;

	// If aabb for this location is not on map then creates one
	cAABB* m_GetOrCreateAABB(uint idxAABB);

	// Go through each mesh triangle to insert into AABB
	// Only should use when collision type is mesh triangles indirect
	void m_InsertMeshTrianglesIndirect(EntityID entityID);

	// Calculate triangle location index and insert in cAABB
	// Insert triangle in every aabb thta collided
	void m_InsertTriangle(int indexTriangle, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);

	// TODO: Temporarily to debug broadphase creation
	friend class DebugSystem;
};