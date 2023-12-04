#pragma once

#include "Component.h"
#include "EngineScripting/ScriptProperties.h"
#include <map>

class ScriptComponent : public Component
{
public:
	ScriptComponent();
	virtual ~ScriptComponent() {};

	std::string scriptName;

	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);

	void SetTableRegistry(int tbIdx);
	void SetOnStart(int fncIdx);
	void SetOnUpdate(int fncIdx);
	void SetOnCollision(int fncIdx);
	void SetOnKeyInput(int fncIdx);

	int GetTableRegistry();
	int GetOnStart();
	int GetOnUpdate();
	int GetOnCollision();
	int GetOnKeyInput();

private:
	sScriptFunctions m_sFunctions;
	sScriptState     m_sState;
};
