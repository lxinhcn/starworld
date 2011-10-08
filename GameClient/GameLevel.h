#pragma once

class b2Render;
class CGameLevel : public b2ContactListener
{
public:
	CGameLevel( b2Render* Render, b2Transform transform );
	~CGameLevel(void);

	bool Load( const char* filename );
	bool UpdateLogic( float fTime, float fDelta );
	void Render();

	void OnButtonDown( float x, float y, int key, int flags, bool editing );
	void OnButtonUp( float x, float y, int key, int flags, bool editing );
	void OnMouseWheel( float x, float y, int wheel, int flags, bool editing );
	void OnMouseMove( float x, float y, int key, int flags, bool editing );
protected:
	void Destroy();

	void Draw( b2Transform transform );
	void DrawShape(b2Fixture* fixture, const b2Transform& xf, const b2Color& color);

	LuaObject	getTexture(){ return mTexture; }
	LuaObject	getSprites(){ return mSprites; }
	LuaObject	getAnimation(){ return mAnimation; }

	b2Vec2 Screen2World( fPoint pt );
private:
	HGE*		hge;
	b2World*	mWorld;

	b2Render*	mRender;
	Script		mScript;
	hgeSprite*	mBackground;

	b2Transform	mWorldTransform;

	b2MouseJoint*	mMouseJoint;
	b2Body*			mGroundBody;

	LuaObject	mTexture;
	LuaObject	mSprites;
	LuaObject	mAnimation;

	float		mZoom;
	float		mLastTime;
};
