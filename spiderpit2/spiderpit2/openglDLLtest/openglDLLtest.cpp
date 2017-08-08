// openglDLLtest.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "openglDLLtest.h"
#include <gl\gl.h>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// This is an example of an exported variable
OPENGLDLLTEST_API int nopenglDLLtest=0;

// This is an example of an exported function.
OPENGLDLLTEST_API int fnopenglDLLtest(void)
{
	glClearColor(0,1,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	return 42;
}

// This is the constructor of a class that has been exported.
// see openglDLLtest.h for the class definition
CopenglDLLtest::CopenglDLLtest()
{
	return;
}
