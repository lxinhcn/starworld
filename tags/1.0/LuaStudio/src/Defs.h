/*-----------------------------------------------------------------------------
	Lua Studio

Copyright (c) 1996-2008 Michal Kowalski
-----------------------------------------------------------------------------*/

#ifndef _asm_h_
#define _asm_h_

typedef unsigned int UINT32;		// liczba ca³kowita 32-bitowa bez znaku
typedef signed int SINT32;		// liczba ca³kowita 32-bitowa ze znakiem
typedef unsigned char UINT8;		// liczba ca³kowita 8-bitowa bez znaku
typedef unsigned short int UINT16;	// liczba ca³kowita 16-bitowa bez znaku


namespace Defs
{
	enum Breakpoint
	{
		BPT_NONE	= 0x00,		// nie ma przerwania
		BPT_EXECUTE	= 0x01,		// przerwanie przy wykonaniu
		BPT_READ	= 0x02,		// przerwanie przy odczycie
		BPT_WRITE	= 0x04,		// przerwanie przy zapisie
		BPT_MASK	= 0x07,
		BPT_NO_CODE	= 0x08,		// wiersz nie zawiera kodu - przerwanie nie mo¿e by?ustawione
		BPT_TEMP_EXEC=0x10,		// przerwanie tymczasowe do zatrzymania programu
		BPT_DISABLED= 0x80		// przerwanie wy³¹czone
	};

	typedef UINT16 FileUID;
};


//CAsm::DeasmFmt inline operator | (CAsm::DeasmFmt f1, CAsm::DeasmFmt f2)
//{
//	return static_cast<CAsm::DeasmFmt>(DWORD(f1) | DWORD(f2));
//}


#endif
