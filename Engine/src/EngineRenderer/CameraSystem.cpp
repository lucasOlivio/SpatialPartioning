#include "EngineRenderer/CameraSystem.h"
#include "common/opengl.h"
#include "scene/SceneView.h"
#include "common/utilsMat.h"
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtx/string_cast.hpp>

CameraSystem::CameraSystem() : 
    m_pCamera(nullptr), m_pTransform(nullptr)
{
}

CameraSystem::~CameraSystem()
{
}

void CameraSystem::Initialize()
{
    m_pCamera = SceneView::Get()->GetComponentByTag<CameraComponent>("camera", "camera");
    m_pTransform = SceneView::Get()->GetComponentByTag<TransformComponent>("camera", "transform");
}

glm::mat4 CameraSystem::GetViewMat()
{
    using namespace glm;
    using namespace myutils;

    // Calculating camera view
    vec3 cameraPosition = GetCameraPosition();
    quat transfRotation = m_pTransform->GetQuatOrientation();
    float scale = m_pTransform->GetScale();

    // Get Camera transform based on offset camera position
    mat4 mat = mat4(1.0);
    ApplyTranslation(cameraPosition, mat);
    ApplyRotation(transfRotation, mat);

    return inverse(mat);
}

glm::vec3 CameraSystem::GetCameraPosition()
{
    using namespace glm;

    vec3 transfForward = m_pTransform->GetForwardVector();
    vec3 transfPosition = m_pTransform->GetPosition();
    float distance = m_pCamera->distance;

    vec3 offsetPos = -distance * transfForward;

    vec3 camFinalPos = transfPosition + offsetPos;

    return camFinalPos;
}

