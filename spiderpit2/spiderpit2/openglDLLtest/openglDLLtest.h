// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OPENGLDLLTEST_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OPENGLDLLTEST_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef OPENGLDLLTEST_EXPORTS
#define OPENGLDLLTEST_API __declspec(dllexport)
#else
#define OPENGLDLLTEST_API __declspec(dllimport)
#endif

// This class is exported from the openglDLLtest.dll
class OPENGLDLLTEST_API CopenglDLLtest {
public:
	CopenglDLLtest(void);
	// TODO: add your methods here.
};

extern OPENGLDLLTEST_API int nopenglDLLtest;

OPENGLDLLTEST_API int fnopenglDLLtest(void);
