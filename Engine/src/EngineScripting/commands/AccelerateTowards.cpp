#include "EngineScripting/commands/AccelerateTowards.h"
#include "common/ParserJSON.h"
#include "scene/SceneView.h"

AccelerateTowards::AccelerateTowards()
{
}

bool AccelerateTowards::Initialize(rapidjson::Value& document)
{
    using namespace rapidjson;

    std::string entity;
    bool isValid = true;

    // Initialize default command variables
    isValid &= this->Command::Initialize();

    ParserJSON parser = ParserJSON();

    Value& objEntt = document["entity"];
    isValid &= parser.GetString(objEntt, entity);

    Value& objDir = document["direction"];
    isValid &= parser.GetVec3(objDir, m_direction);

    Value& objAcceleration = document["acceleration"];
    isValid &= parser.GetFloat(objAcceleration, m_acceleration);

    Value& objMaxVel = document["maxSpeed"];
    isValid &= parser.GetFloat(objMaxVel, m_maxSpeed);

    if (!isValid)
    {
        // Invalid arguments
        return false;
    }

    m_pTransform = SceneView::Get()->GetComponentByTag<TransformComponent>(entity, "transform");
    m_pForce = SceneView::Get()->GetComponentByTag<ForceComponent>(entity, "force");

    return true;
}

bool AccelerateTowards::Update(double deltaTime)
{
    // Here we check when max speed is reached
    glm::vec3 currVel = m_pForce->GetVelocity();

    if (m_maxSpeed > 0 &&
        glm::length(currVel) > m_maxSpeed)
    {
        m_pForce->SetVelocity(m_vecMaxVel);
    }

    return false;
}

bool AccelerateTowards::IsDone(void)
{
    // Accelerates indefinitly
    return false;
}

bool AccelerateTowards::PreStart(void)
{
    m_direction = m_pTransform->GetRelativeVector(m_direction);
    m_direction = glm::normalize(m_direction);

    m_vecAcc    = m_direction * m_acceleration;
    m_vecMaxVel = m_direction * m_maxSpeed;

    // Here we set the acceleration force
    m_pForce->SetAcceleration(m_vecAcc);

    return true;
}

bool AccelerateTowards::PostEnd(void)
{
    return true;
}

void AccelerateTowards::m_Accelerate()
{
}
