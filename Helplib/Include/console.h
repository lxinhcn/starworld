#pragma once
#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS


class TOOLLIB_API CConsole
{
public:
	CConsole();
	CConsole(LPCTSTR lpszTitle, SHORT ConsoleHeight = 300, SHORT ConsoleWidth = 80);
	~CConsole();

private:
	void Attach(SHORT ConsoleHeight, SHORT ConsoleWidth);
	static BOOL IsExistent;
};

void TOOLLIB_API gotoxy( WORD x, WORD y );
void TOOLLIB_API cls( HANDLE hConsole );
