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

protected:
	void Destroy();

	void Draw( b2Transform transform );
	void DrawShape(b2Fixture* fixture, const b2Transform& xf, const b2Color& color);

	LuaObject	getTexture(){ return mTexture; }
	LuaObject	getSprites(){ return mSprites; }
	LuaObject	getAnimation(){ return mAnimation; }

private:
	HGE*		hge;
	b2World*	mWorld;

	b2Render*	mRender;
	Script		mScript;
	hgeSprite*	mBackground;

	b2Transform		mWorldTransform;

	LuaObject	mTexture;
	LuaObject	mSprites;
	LuaObject	mAnimation;

	float		mZoom;
	float		mLastTime;
};
