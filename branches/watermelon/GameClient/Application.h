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

	bool IsDebug()const{ return mDebug; }
	bool IsEdit()const{ return mEdit; }
protected:
	bool UpdateLogic( float fTime, float fDelta );

private:
	bool RenderFunc();
	bool FrameFunc();

private:
	HGE			*hge;
	b2Render	*mRender;
	float		mZoom;
	fPoint		ptOffset;

	fPoint		ptMonseDown;
	fPoint		ptMouseUp;

	bool		mDebug;
	bool		mEdit;

	CGameLevel*		mLevel;
	hgeAnimation*	mCursor;
};

typedef SingletonHolder< CApplication, CreateStatic > Application;