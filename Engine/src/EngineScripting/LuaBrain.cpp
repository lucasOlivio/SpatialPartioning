#include "EngineScripting/LuaBrain.h"
#include "components/Script.h"
#include "common/utils.h"
#include "EngineScripting/luaBindings/LuaCommandDispatcher.h"
#include "EngineScripting/luaBindings/LuaGetInfo.h"
#include "scene/SceneView.h"
#include <iostream>
#include <fstream>
#include <sstream>

LuaBrain* LuaBrain::m_pInstance = nullptr;

typedef std::map< EntityID, sScriptData*> mapEnttScript;
typedef std::pair< EntityID, sScriptData*> pairEnttScript;
typedef mapEnttScript::iterator itEnttScript;

LuaBrain::~LuaBrain()
{
	return;
}

LuaBrain* LuaBrain::Get()
{
	if (LuaBrain::m_pInstance == nullptr)
	{
		LuaBrain::m_pInstance = new LuaBrain();
	}
	return LuaBrain::m_pInstance;
}

bool LuaBrain::Initialize(std::string baseScriptsPath)
{
	m_baseScriptsPath = baseScriptsPath;

	// Create new Lua state.
	m_pLuaState = luaL_newstate();

	luaL_openlibs(m_pLuaState);

	// Bind Lua integration functions
	lua_pushcfunction(m_pLuaState, lua_SendCommands);
	lua_setglobal(m_pLuaState, "SendCommands");
	lua_pushcfunction(m_pLuaState, lua_GetFrontDirection);
	lua_setglobal(m_pLuaState, "GetFrontDirection");
	lua_pushcfunction(m_pLuaState, lua_GetTransform);
	lua_setglobal(m_pLuaState, "GetTransform");
	lua_pushcfunction(m_pLuaState, lua_GetKey);
	lua_setglobal(m_pLuaState, "GetKey");

	return true;
}

void LuaBrain::Destroy()
{
	for (pairEnttScript pairData : m_mapScriptsData)
	{
		// Release table of functions from registry
		luaL_unref(m_pLuaState, LUA_REGISTRYINDEX, pairData.second->tableFunctions);

		delete pairData.second;
	}

	lua_close(m_pLuaState);
	return;
}

bool LuaBrain::LoadScene()
{
	m_mapScriptsData.clear();
	printf("Loading scripts...\n");
	
	// Lua functions bindings
	for (SceneView::Get()->First("script"); !SceneView::Get()->IsDone(); SceneView::Get()->Next())
	{
		EntityID entityId = SceneView::Get()->CurrentKey();
		ScriptComponent* pScript = SceneView::Get()->CurrentValue<ScriptComponent>();

		if (!pScript->IsActive())
		{
			continue;
		}

		bool isLoaded = LoadScript(entityId, pScript->scriptName);
		if (!isLoaded)
		{
			CheckEngineError(("Not able to load " + pScript->scriptName).c_str());
			return false;
		}
	}

	return true;
}


// Saves (and overwrites) any script
// scriptName is just so we can delete them later
bool LuaBrain::LoadScript(EntityID entityID, std::string scriptName)
{
	std::string scriptSource = m_ReadLuaScriptFile(scriptName);
	if (scriptSource == "")
	{
		return false;
	}
	sScriptData* pScriptData = new sScriptData();

	// Run script to load functions
	RunScriptImmediately(scriptSource);

	// Load all functions into registry
	pScriptData->tableFunctions = m_CreateTableRegistry();
	pScriptData->onstart = m_LoadFunctionRegistry(pScriptData->tableFunctions, "onstart");
	pScriptData->onupdate = m_LoadFunctionRegistry(pScriptData->tableFunctions, "onupdate");
	pScriptData->oncollision = m_LoadFunctionRegistry(pScriptData->tableFunctions, "oncollision");
	pScriptData->onkeyinput = m_LoadFunctionRegistry(pScriptData->tableFunctions, "onkeyinput");

	m_mapScriptsData[entityID] = pScriptData;

	return true;
}


void LuaBrain::DeleteScript(EntityID entityID)
{
	sScriptData* pScriptData = m_GetScriptData(entityID);
	if (pScriptData)
	{
		delete pScriptData;
	}

	m_mapScriptsData.erase(entityID);
	return;
}

void LuaBrain::OnStart()
{
	bool sceneLoaded = LoadScene();
	if (!sceneLoaded)
	{
		return;
	}

	for ( itEnttScript itScript = m_mapScriptsData.begin(); itScript != m_mapScriptsData.end(); itScript++)
	{
		EntityID entityID = itScript->first;
		sScriptData* pScriptData = itScript->second;

		if (pScriptData->onstart == LUA_REFNIL)
		{
			// Script doesn`t have an onstart function
			continue;
		}

		m_GetFunctionRegistry(pScriptData->tableFunctions, pScriptData->onstart);

		// Call the onstart function for each object
		m_CallFunction(0, 0);
	}

	return;
}

// Call all the active scripts that are loaded
void LuaBrain::Update(float deltaTime)
{
	for (itEnttScript itScript = m_mapScriptsData.begin(); 
		 itScript != m_mapScriptsData.end(); 
		 itScript++)
	{
		EntityID entityID = itScript->first;
		sScriptData* pScriptData = itScript->second;

		if (pScriptData->onupdate == LUA_REFNIL)
		{
			// Script doesn`t have an onstart function
			continue;
		}

		m_GetFunctionRegistry(pScriptData->tableFunctions, pScriptData->onupdate);

		lua_pushnumber(m_pLuaState, deltaTime);

		m_CallFunction(1, 0);
	}

	return;
}

void LuaBrain::OnCollision(EntityID entityID, std::string tagCollided)
{
	for (itEnttScript itScript = m_mapScriptsData.begin();
		itScript != m_mapScriptsData.end();
		itScript++)
	{
		EntityID entityID = itScript->first;
		sScriptData* pScriptData = itScript->second;

		if (pScriptData->onstart == LUA_REFNIL)
		{
			// Script doesn`t have this function
			continue;
		}

		m_GetFunctionRegistry(pScriptData->tableFunctions, pScriptData->onupdate);

		lua_pushstring(m_pLuaState, tagCollided.c_str());

		m_CallFunction(1, 0);
	}

	return;
}

void LuaBrain::OnKeyInput(sKeyInfo keyInfo)
{
	for (itEnttScript itScript = m_mapScriptsData.begin();
		itScript != m_mapScriptsData.end();
		itScript++)
	{
		EntityID entityID = itScript->first;
		sScriptData* pScriptData = itScript->second;

		if (pScriptData->onkeyinput == LUA_REFNIL)
		{
			// Script doesn`t have this function
			continue;
		}

		m_GetFunctionRegistry(pScriptData->tableFunctions, pScriptData->onkeyinput);

		lua_pushnumber(m_pLuaState, keyInfo.pressedKey);
		lua_pushnumber(m_pLuaState, keyInfo.action);
		lua_pushnumber(m_pLuaState, keyInfo.mods);
		lua_pushnumber(m_pLuaState, keyInfo.scanCode);

		m_CallFunction(4, 0);
	}

	return;
}

// Runs a script, but doesn't save it
void LuaBrain::RunScriptImmediately(std::string script)
{
	int error = luaL_loadstring(m_pLuaState,
		script.c_str());

	if (error != 0 /*no error*/)
	{
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << m_DecodeLuaErrorToString(error) << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		return;
	}

	// execute funtion in "protected mode", where problems are 
	//  caught and placed on the stack for investigation
	error = lua_pcall(m_pLuaState,	/* lua state */
	                  0,	/* nargs: number of arguments pushed onto the lua stack */
	                  0,	/* nresults: number of results that should be on stack at end*/
	                  0);	/* errfunc: location, in stack, of error function.
								if 0, results are on top of stack. */
	if (error != 0 /*no error*/)
	{
		std::cout << "Lua: There was an error..." << std::endl;
		std::cout << m_DecodeLuaErrorToString(error) << std::endl;

		std::string luaError;
		// Get error information from top of stack (-1 is top)
		luaError.append(lua_tostring(m_pLuaState, -1));

		// Make error message a little more clear
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << luaError << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		// We passed zero (0) as errfunc, so error is on stack)
		lua_pop(m_pLuaState, 1);  /* pop error message from the stack */
	}

	return;
}

std::string LuaBrain::m_DecodeLuaErrorToString(int error)
{
	switch (error)
	{
	case 0:
		return "Lua: no error";
		break;
	case LUA_YIELD:
		return "Lua: Lua yield";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error";
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	}//switch ( error )

	// Who knows what this error is?
	return "Lua: UNKNOWN error";
}

std::string LuaBrain::m_ReadLuaScriptFile(std::string scriptName)
{
	// Open the file
	std::ifstream file(m_baseScriptsPath + scriptName);

	// Check if the file is opened successfully
	if (!file.is_open()) {
		CheckEngineError(("Error opening file: " + scriptName).c_str());
		return std::string(); // Return an empty string to indicate an error
	}

	// Read the contents of the file into a string
	std::stringstream buffer;
	buffer << file.rdbuf();

	// Close the file
	file.close();

	// Return the contents as a string
	return buffer.str();
}

sScriptData* LuaBrain::m_GetScriptData(EntityID entityId)
{
	using namespace std;

	itEnttScript it = m_mapScriptsData.find(entityId);
	if (it == m_mapScriptsData.end())
	{
		return nullptr;
	}

	return it->second;
}

int LuaBrain::m_CreateTableRegistry()
{
	lua_newtable(m_pLuaState);
	return luaL_ref(m_pLuaState, LUA_REGISTRYINDEX);
}

int LuaBrain::m_LoadFunctionRegistry(int tableIdx, const char* funcName)
{
	// Retrieve table registry to store function
	lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, tableIdx);
	// Retrieve function named "funcName" to store
	lua_getglobal(m_pLuaState, funcName);
	// store function in the function table
	// (-2 table is 2 places up the current stack)
	int funcIdx = luaL_ref(m_pLuaState, -2); 

	// Done with the function table, so pop it
	lua_pop(m_pLuaState, 1); 

	return funcIdx;
}

void LuaBrain::m_GetFunctionRegistry(int tableIdx, int funcIdx)
{
	lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, tableIdx); // retrieve function table
	lua_rawgeti(m_pLuaState, -1, funcIdx);				   // retrieve function

	return;
}

int LuaBrain::m_CallFunction(int numParameters, int numReturns)
{
	using namespace std;

	// Call the onstart function for each object
	int result = lua_pcall(m_pLuaState, numParameters, numReturns, 0);
	if (result != LUA_OK) 
	{
		std::cout << "Lua: There was an error..." << std::endl;
		std::cout << m_DecodeLuaErrorToString(result) << std::endl;

		string luaError;
		// Get error information from top of stack (-1 is top)
		luaError.append(lua_tostring(m_pLuaState, -1));

		cout << "-------------------------------------------------------" << endl;
		cout << "Error running function: ";
		cout << luaError << endl;
		cout << "-------------------------------------------------------" << endl;
		lua_pop(m_pLuaState, 1);
	}

	return result;
}
