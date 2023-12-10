#pragma once

#include "components/Camera.h"
#include "components/Transform.h"
#include <glm/mat4x4.hpp>

class CameraSystem
{
public:
	// ctors & dtors
	CameraSystem();
	~CameraSystem();

	void Initialize();

	glm::mat4 GetViewMat();

	glm::vec3 GetCameraPosition();

	// Change the active camera in the scene
	void ChangeCamera(CameraComponent* pCamera, TransformComponent* pTransform);

private:

	CameraComponent* m_pCamera;
	TransformComponent* m_pTransform;
};