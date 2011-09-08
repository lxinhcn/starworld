#pragma once
#include "singleton.h"

class CApplication
{
	friend struct CreateStatic< CApplication >;
private:
	CApplication(void);
	virtual ~CApplication(void);

public:
	bool Initialize();
	void Run();
	void UnInitialize();

	HGE* operator->(){ return m_hge; }
	HGE* getEngine(){ return m_hge; }

protected:
	bool UpdateLogic( float fDelta );
	void Render();

private:
	static
	bool RenderFunc();

	static
	bool FrameFunc();

private:

	HGE			*m_hge;
};

typedef SingletonHolder< CApplication, CreateStatic > Application;