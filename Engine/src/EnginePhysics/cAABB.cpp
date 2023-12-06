#include "EnginePhysics/cAABB.h"

glm::vec3 cAABB::GetCentreXYZ(void)
{
    return (minXYZ + maxXYZ) * 0.5f;
}

glm::vec3 cAABB::GetExtentsXYZ(void)
{
	return maxXYZ - minXYZ;
}
