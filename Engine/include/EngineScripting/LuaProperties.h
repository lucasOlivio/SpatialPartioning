#pragma once

struct sScriptData
{
	int tableFunctions;
	// References to loaded functions (-1 if function not implemented on script)
	int onstart;
	int onupdate;
	int oncollision;
	int onkeyinput;
};
