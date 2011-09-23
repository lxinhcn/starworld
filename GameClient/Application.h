#pragma once
#include "singleton.h"
#include "hgefontmanager.h"
#include "hgefontex.h"

class b2Render;
class CGameLevel;
class CApplication
{
	friend struct CreateStatic< CApplication >;
	friend static bool RenderFunc();
	friend static bool FrameFunc();

private:
	CApplication(void);
	virtual ~CApplication(void);

public:
	bool Initialize();
	void Run();
	void UnInitialize();

	HGE* operator->(){ return m_hge; }
	HGE* getEngine(){ return m_hge; }

	hgeFontManagerStub& getFontManager(){ return m_FontManager; }

protected:
	bool UpdateLogic( float fDelta );

private:
	bool RenderFunc();
	bool FrameFunc();

private:
	HGE			*m_hge;
	b2World		*m_World;
	b2Render	*m_Render;
	static hgeFontManagerStub m_FontManager;
	float		m_zoomView;
	fPoint		m_ptOffset;

	fPoint		m_ptMonseDown;
	fPoint		m_ptMouseUp;

	bool		m_bDebug;
	bool		m_bEdit;
	CGameLevel*	m_pLevel;
};

typedef SingletonHolder< CApplication, CreateStatic > Application;