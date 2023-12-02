#pragma once

#include "Component.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class CameraComponent : public Component
{
public:
	glm::vec3 upVector;
	float distance;

	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);
};