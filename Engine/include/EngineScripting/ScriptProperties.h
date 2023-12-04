#pragma once

struct sScriptState
{

};

struct sScriptFunctions
{
	sScriptFunctions()
	{
		tableFunctions = -1;
		onstart = -1;
		onupdate = -1;
		oncollision = -1;
		onkeyinput = -1;
	}

	int tableFunctions;
	// References to loaded functions (-1 if function not implemented on script)
	int onstart;
	int onupdate;
	int oncollision;
	int onkeyinput;
};
