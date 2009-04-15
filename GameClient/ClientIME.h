#pragma once

class CClientIME :	public XUI_IME
{
public:
	CClientIME(void);
	~CClientIME(void);

	static bool Initialize();

	static HINSTANCE	m_hDllImm32;	// IMM32 DLL handle
	static HINSTANCE	m_hDllVer;		// Version DLL handle
	static HIMC			m_hImcDef;		// Default input context
	static HKL			m_hklCurrent;	// Current keyboard layout of the process
};
