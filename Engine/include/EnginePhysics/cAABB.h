#pragma once

#include "common/types.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <math.h>
#include <vector>

class cAABB
{
public:
	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;

	// Collision type: sMeshOfTriangles_Indirect
	// Index to mesh triangles inside this AABB
	std::vector<uint> vecIdxTriangles;

	glm::vec3 GetCentreXYZ(void);

	glm::vec3 GetExtentsXYZ(void);
};


