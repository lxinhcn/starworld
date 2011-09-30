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

	HGE* operator->(){ return hge; }
	HGE* getEngine(){ return hge; }

protected:
	bool UpdateLogic( float fDelta );

private:
	bool RenderFunc();
	bool FrameFunc();

	void DrawShape(b2Fixture* fixture, const b2Transform& xf, const b2Color& color);

private:
	HGE			*hge;
	b2World		*mWorld;
	b2Render	*mRender;
	float		mZoom;
	fPoint		ptOffset;

	fPoint		ptMonseDown;
	fPoint		ptMouseUp;

	bool		mDebug;
	bool		mEdit;

	b2Transform		mWorldTransform;
	CGameLevel*		mLevel;
	hgeAnimation*	mCursor;
};

typedef SingletonHolder< CApplication, CreateStatic > Application;