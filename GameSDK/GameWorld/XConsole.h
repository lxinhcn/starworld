#pragma once
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS

#define BUFSIZE 4096 

HANDLE hChildStdinRd, hChildStdinWr, hChildStdinWrDup, 
hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup, 
hInputFile, hSaveStdin, hSaveStdout; 

CORE_API BOOL CreateChildProcess(VOID); 
CORE_API VOID WriteToPipe(VOID); 
CORE_API VOID ReadFromPipe(VOID); 
CORE_API VOID ErrorExit(LPTSTR); 
CORE_API VOID ErrMsg(LPTSTR, BOOL); 

class CORE_API CXConsole
{
public:
	CXConsole(void);
	~CXConsole(void);
};