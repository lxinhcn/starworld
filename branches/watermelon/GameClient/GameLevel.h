#pragma once

class CGameLevel : public b2ContactListener
{
public:
	CGameLevel( b2World* world );
	~CGameLevel(void);

	bool Load( const char* filename );
	bool UpdateLogic( float fDelta );
	void Render();

protected:
	void Destroy();

	LuaObject	getTexture(){ return mTexture; }
	LuaObject	getSprites(){ return mSprites; }
	LuaObject	getAnimation(){ return mAnimation; }

private:
	HGE*		hge;
	b2World*	mWorld;

	Script		mScript;
	hgeSprite*	mBackground;

	LuaObject	mTexture;
	LuaObject	mSprites;
	LuaObject	mAnimation;

};
