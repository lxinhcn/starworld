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

private:
	HGE*	 hge;
	b2World* m_world;

	hgeSprite	mBackground;
	hgeSprite	*mBody;
	HTEXTURE m_hBackground;
};
