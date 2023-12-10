#include "EngineScripting/commands/CreateEntity.h"
#include "common/ParserJSON.h"
#include "components/Transform.h"
#include "scene/Scene.h"
#include "scene/SceneView.h"
#include "Engine.h"

CreateEntity::CreateEntity()
{
}

bool CreateEntity::Initialize(rapidjson::Value& document)
{
    using namespace rapidjson;

    std::string entity;

    bool isValid = true;

    // Initialize default command variables
    isValid &= this->Command::Initialize();
    m_name = "CreateEntity";

    ParserJSON parser = ParserJSON();

    Value& objEntt = document["entity"];
    isValid &= parser.GetString(objEntt, entity);
    Value& objPos = document["position"];
    isValid &= parser.GetVec3(objPos, m_position);
    Value& objOrient = document["orientation"];
    isValid &= parser.GetVec3(objOrient, m_orientation);

    m_entityID = SceneView::Get()->GetEntityByTag(entity);

    return isValid;
}

bool CreateEntity::Update(double deltaTime)
{
    SceneView* pScene = SceneView::Get();

    // Create the entity
    EntityID newEntity = Scene::Get()->CreateEntity(m_entityID);

    // Set transform parameters
    TransformComponent* pTransform = pScene->GetComponent<TransformComponent>(newEntity, "transform");

    pTransform->SetPosition(m_position);
    pTransform->SetOrientation(m_orientation);

    iComponent* pComp = pScene->GetComponent(newEntity, "camera");
    if (pComp)
    {
        // new component have camera then we update the active camera
        Engine::Get()->ChangeCamera((CameraComponent*)pComp, pTransform);
    }

    return true;
}

bool CreateEntity::IsDone(void)
{
    // Only needs to run once
    return true;
}

bool CreateEntity::PreStart(void)
{
    return true;
}

bool CreateEntity::PostEnd(void)
{
    return true;
}
