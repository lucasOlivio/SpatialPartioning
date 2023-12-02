#include "EngineScripting/commands/OrientTo.h"
#include "common/utils.h"
#include "common/utilsMat.h"
#include "common/ParserJSON.h"
#include "scene/SceneView.h"

OrientTo::OrientTo() :
    m_elapsedTime(0), m_currPhase(ePhase::STARTUP)
{
}

bool OrientTo::Initialize(rapidjson::Value& document)
{
    using namespace rapidjson;
    using namespace myutils;
    using namespace glm;

    std::string entity;
    vec3 orientation;
    vec3 position;
    bool isValid = true;

    // Initialize default command variables
    isValid &= this->Command::Initialize();
    m_name = "OrientTo";

    ParserJSON parser = ParserJSON();

    Value& objEntt = document["entity"];
    isValid &= parser.GetString(objEntt, entity);
    Value& objTime = document["time"];
    isValid &= parser.GetFloat(objTime, m_time);
    Value& objIn = document["easyIn"];
    isValid &= parser.GetFloat(objIn, m_easyInRatio);
    Value& objOut = document["easyOut"];
    isValid &= parser.GetFloat(objOut, m_easyOutRatio);
    Value& objStop = document["stopAtEnd"];
    isValid &= parser.GetBool(objStop, m_stopAtEnd);

    if (!isValid)
    {
        // Invalid arguments
        return false;
    }

    m_pTransform = SceneView::Get()->GetComponentByTag<TransformComponent>(entity, "transform");
    m_pForce = SceneView::Get()->GetComponentByTag<ForceComponent>(entity, "force");

    // Orientation given by a vector of degrees to rotate
    // Or by a given position to orientate torwards
    if (document.HasMember("orientation"))
    {
        Value& objLoc = document["orientation"];
        isValid &= parser.GetVec3(objLoc, m_finalRotation);
    }
    else
    {
        Value& objLoc = document["position"];
        isValid &= parser.GetVec3(objLoc, position);
        m_finalRotation = CalculateRotation(m_pTransform->GetPosition(), position);
    }

    return true;
}

void OrientTo::Initialize(TransformComponent* pTransform, ForceComponent* pForce, 
                          bool isPositionOriented, glm::vec3 orientTo, glm::vec3 maxSpeed)
{
    m_pTransform = pTransform;
    m_pForce = pForce;

    if (isPositionOriented)
    {
        m_finalRotation = myutils::CalculateRotation(m_pTransform->GetPosition(), orientTo);
    }
    else
    {
        m_finalRotation = orientTo;
    }

    m_stopAtEnd = false;

    m_easyInTime = 0.0;
    m_easyOutTime = 0.0;
    m_constantTime = 0;
    m_time = 0;

    m_maxVelocity = maxSpeed;
}

void OrientTo::Initialize(TransformComponent* pTransform, ForceComponent* pForce, 
                          bool isPositionOriented, glm::vec3 orientTo, float time)
{
    m_pTransform = pTransform;
    m_pForce = pForce;

    if (isPositionOriented)
    {
        m_finalRotation = myutils::CalculateRotation(m_pTransform->GetPosition(), orientTo);
    }
    else
    {
        m_finalRotation = orientTo;
    }

    m_stopAtEnd = false;

    m_easyInTime = 0.0;
    m_easyOutTime = 0.0;
    m_constantTime = time;
    m_time = time;
}

bool OrientTo::Update(double deltaTime)
{
    using namespace glm;

    m_elapsedTime += deltaTime;
    vec3 currOrientation = m_pTransform->GetOrientation();
    vec3 currCentrVelocity = m_pForce->GetCentrifugalVelocity();

    if (m_time > 0)
    {
        m_UpdateRotation(currOrientation, currCentrVelocity);
    }
    else
    {
        m_SetRotationMaxVel(currOrientation, currCentrVelocity);
    }

    bool isDone = IsDone();

    return isDone;
}

bool OrientTo::IsDone(void)
{
    float distanceToFinal = glm::length(m_pTransform->GetOrientation() - m_finalRotation);

    // Reached destination
    if (distanceToFinal <= 0.1)
    {
        // Reached or passed destination, so stop object and clamp to right orientation
        m_pTransform->SetOrientation(m_finalRotation);
        return true;
    }

    if (m_elapsedTime >= m_time)
    {
        // Did not reach destination but ran out of time so probably can't reach it
        return true;
    }

    return false;
}

bool OrientTo::PreStart(void)
{
    using namespace myutils;

    // Calculate orientation phases
    m_easyInPos = CalculateVector(m_pTransform->GetOrientation(),
                                  m_finalRotation, m_easyInRatio);  // Orientation it will stop accelerating
    m_easyOutPos = CalculateVector(m_pTransform->GetPosition(),
                                   m_finalRotation, 1.0f - m_easyOutRatio); // Orientation it will start decelerating

    // Calculate time phases
    m_easyInTime = m_easyInRatio * m_time;
    m_easyOutTime = m_easyOutRatio * m_time;
    m_constantTime = m_time - m_easyInTime - m_easyOutTime;

    return true;
}

bool OrientTo::PostEnd(void)
{
    if (m_stopAtEnd)
    {
        m_pForce->SetCentrifugalAcceleration(glm::vec3(0));
        m_pForce->SetCentrifugalVelocity(glm::vec3(0));
    }

    return true;
}

void OrientTo::m_UpdateRotation(glm::vec3 currOrientation, glm::vec3 currCentrVelocity)
{
    using namespace glm;

    // Check which phase of the velocity curve its in
    if (m_elapsedTime <= m_easyInTime &&
        m_currPhase == ePhase::STARTUP) {
        // Acceleration phase
        m_currPhase = ePhase::EASYIN;
        vec3 acceleration = myutils::CalculateAcceleration(currOrientation, m_easyInPos,
            m_elapsedTime, m_easyInTime);

        m_pForce->SetCentrifugalVelocity(vec3(0));
        m_pForce->SetCentrifugalAcceleration(acceleration);
    }
    else if (m_elapsedTime >= m_easyInTime &&
        m_elapsedTime <= m_easyInTime + m_constantTime &&
        m_currPhase != ePhase::CONSTANT) {
        // Constant speed phase
        m_currPhase = ePhase::CONSTANT;
        // Setting velocity again for cases when easyin = 0
        vec3 velocity = myutils::CalculateVelocity(currOrientation, m_easyOutPos,
            m_easyInTime,
            m_time - m_easyOutTime);

        m_pForce->SetCentrifugalAcceleration(vec3(0));
        m_pForce->SetCentrifugalVelocity(velocity);
    }
    else if (m_elapsedTime >= m_easyInTime + m_constantTime &&
        m_elapsedTime <= m_time &&
        m_currPhase != ePhase::EASYOUT) {
        // Deceleration phase
        m_currPhase = ePhase::EASYOUT;
        vec3 acceleration = myutils::CalculateAcceleration(currOrientation, m_finalRotation,
            currCentrVelocity,
            m_elapsedTime, m_time);

        m_pForce->SetCentrifugalAcceleration(-acceleration);
    }
}

void OrientTo::m_SetRotationMaxVel(glm::vec3 currOrientation, glm::vec3 currCentrVelocity)
{
    using namespace glm;
    using namespace myutils;

    vec3 normal = GetNormal(m_finalRotation, currOrientation);

    m_pForce->SetCentrifugalVelocity(m_maxVelocity * normal);
}
