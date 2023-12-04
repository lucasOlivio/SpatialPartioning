#include "components/Script.h"

ScriptComponent::ScriptComponent()
{
    m_sFunctions = sScriptFunctions();
    m_sState     = sScriptState();
}

void ScriptComponent::GetInfo(sComponentInfo& compInfoOut)
{
    compInfoOut.componentName = "script";
    compInfoOut.componentParameters.clear();

    AddCompParInfo("scriptName", "string", scriptName, compInfoOut);

    Component::GetInfo(compInfoOut);
}

void ScriptComponent::SetParameter(sParameterInfo& parameterIn)
{
    Component::SetParameter(parameterIn);

    if (parameterIn.parameterName == "scriptName") {
        scriptName = parameterIn.parameterStrValue;
    }

    return;
}

void ScriptComponent::SetTableRegistry(int tbIdx)
{
    m_sFunctions.tableFunctions = tbIdx;
}

void ScriptComponent::SetOnStart(int fncIdx)
{
    m_sFunctions.onstart = fncIdx;
}

void ScriptComponent::SetOnUpdate(int fncIdx)
{
    m_sFunctions.onupdate = fncIdx;
}

void ScriptComponent::SetOnCollision(int fncIdx)
{
    m_sFunctions.oncollision = fncIdx;
}

void ScriptComponent::SetOnKeyInput(int fncIdx)
{
    m_sFunctions.onkeyinput = fncIdx;
}

int ScriptComponent::GetTableRegistry()
{
    return m_sFunctions.tableFunctions;
}

int ScriptComponent::GetOnStart()
{
    return m_sFunctions.onstart;
}

int ScriptComponent::GetOnUpdate()
{
    return m_sFunctions.onupdate;
}

int ScriptComponent::GetOnCollision()
{
    return m_sFunctions.oncollision;
}

int ScriptComponent::GetOnKeyInput()
{
    return m_sFunctions.onkeyinput;
}
