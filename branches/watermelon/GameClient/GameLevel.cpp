#include "StdAfx.h"
#include "GameLevel.h"
#include "Setting.h"
#include "b2Rander.h"
#include "Application.h"
#include "GameObject.h"

CGameLevel::CGameLevel( b2Render* Render, b2Transform transform )
: mWorld( NULL )
, hge( hgeCreate(HGE_VERSION) )
, mBackground( NULL )
, mRender( Render )
, mWorldTransform( transform )
, mLastTime( 0.0f )
, mZoom( 20.0f )
{
}

CGameLevel::~CGameLevel(void)
{
	hge->Release();
}

bool CGameLevel::Load( const char* filename )
{
	try
	{
		mScript.doFile( filename );

		mWorld = new b2World( mScript.get< const b2Vec2& >("gravity"), true );
		mZoom = mScript.get< float >( "zoom" );

		mWorldTransform.q.c *= mZoom;
		mWorldTransform.q.s *= mZoom;

		mTexture = mScript.get< LuaObject >( "texture" );
		if( !mTexture.isvalid() )
			return false;

		mSprites = mScript.get< LuaObject >( "sprites" );
		if( !mSprites.isvalid() )
			return false;

		mAnimation = mScript.get< LuaObject >( "animation" );
		if( !mAnimation.isvalid() )
			return false;

		mBackground = mSprites.get< hgeSprite* >( "background" );

		LuaCall< void (b2World*) > create( &mScript, "CreateLevel" );

		create( mWorld );
	}
	catch( std::runtime_error e )
	{
		XGC_ASSERT_MSGA( false, e.what() );
		return false;
	}

	return true;
}

bool CGameLevel::UpdateLogic( float fTime, float fDelta )
{
	if( mLastTime == 0.0f )
		mLastTime = fTime;

	while( fTime - mLastTime > 0.01f )
	{
		mWorld->Step( 0.01f, 6, 2 );
		mLastTime += 0.01f;
	}
	return true;
}

void CGameLevel::Render()
{
	if( mBackground )
	{
		mBackground->RenderStretch( 0.0f, 0.0f, (float)hge->System_GetState( HGE_SCREENWIDTH ), (float)hge->System_GetState( HGE_SCREENHEIGHT ) );
	}

	Draw(mWorldTransform);
}

void CGameLevel::Destroy()
{

}

void CGameLevel::Draw( b2Transform transform )
{
	for( b2Body* b = mWorld->GetBodyList(); b; b= b->GetNext() )
	{
		b2Transform xf = b->GetTransform();
		xf.p.y = -xf.p.y;
		xf.q.c = -xf.q.c;
		xf = b2Mul( transform, xf );
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			if (b->IsActive() == false)
			{
				DrawShape(f, xf, b2Color(0.5f, 0.5f, 0.3f));
			}
			else if (b->GetType() == b2_staticBody)
			{
				DrawShape(f, xf, b2Color(0.5f, 0.9f, 0.5f));
			}
			else if (b->GetType() == b2_kinematicBody)
			{
				DrawShape(f, xf, b2Color(0.5f, 0.5f, 0.9f));
			}
			else if (b->IsAwake() == false)
			{
				DrawShape(f, xf, b2Color(0.6f, 0.6f, 0.6f));
			}
			else
			{
				DrawShape(f, xf, b2Color(0.9f, 0.7f, 0.7f));
			}
		}
	}
}

void CGameLevel::DrawShape(b2Fixture* fixture, const b2Transform& xf, const b2Color& color)
{
	GameObject* pObject = (GameObject*)fixture->GetUserData();
	
	if( pObject )
	{
		switch (fixture->GetType())
		{
		case b2Shape::e_circle:
			{
				b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

				b2Vec2 center = b2Mul( xf, circle->m_p );
				float32 radius = circle->m_radius * mZoom;

				b2Vec2 v1 = center - b2Vec2( radius, radius );
				b2Vec2 v2 = center + b2Vec2( radius, radius );
				if( pObject->mSprite )
					pObject->mSprite->RenderStretch( v1.x, v1.y, v2.x, v2.y );
				else if( pObject->mAnimation )
					pObject->mAnimation->RenderStretch( v1.x, v1.y, v2.x, v2.y );
			}
			break;

		case b2Shape::e_polygon:
			{
				b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
				int32 vertexCount = poly->m_vertexCount;
				b2Assert(vertexCount <= b2_maxPolygonVertices);
				b2Vec2 vertices[b2_maxPolygonVertices];

				for (int32 i = 0; i < vertexCount; ++i)
				{
					vertices[i] = b2Mul(xf, poly->m_vertices[i]);
				}

				if( vertexCount == 4 )
				{
					if( pObject->mSprite )
						pObject->mSprite->Render4V( vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y, vertices[2].x, vertices[2].y, vertices[3].x, vertices[3].y );
					else if( pObject->mAnimation )
						pObject->mAnimation->Render4V( vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y, vertices[2].x, vertices[2].y, vertices[3].x, vertices[3].y );
				}
			}
			break;

		default:
			break;
		}
	}
	if( Application::Instance().IsDebug() )
	{
		switch (fixture->GetType())
		{
		case b2Shape::e_circle:
			{
				b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

				b2Vec2 center = b2Mul(xf, circle->m_p );
				float32 radius = circle->m_radius * mZoom;
				b2Vec2 axis = b2Mul(xf.q, b2Vec2(1.0f, 0.0f));

				mRender->DrawSolidCircle(center, radius, axis, color);
			}
			break;

		case b2Shape::e_edge:
			{
				b2EdgeShape* edge = (b2EdgeShape*)fixture->GetShape();
				b2Vec2 v1 = b2Mul(xf, edge->m_vertex1);
				b2Vec2 v2 = b2Mul(xf, edge->m_vertex2);
				mRender->DrawSegment(v1, v2, color);
			}
			break;

		case b2Shape::e_chain:
			{
				b2ChainShape* chain = (b2ChainShape*)fixture->GetShape();
				int32 count = chain->GetVertexCount();
				const b2Vec2* vertices = chain->GetVertices();

				b2Vec2 v1 = b2Mul(xf, vertices[0]);
				for (int32 i = 1; i < count; ++i)
				{
					b2Vec2 v2 = b2Mul(xf, vertices[i]);
					mRender->DrawSegment(v1, v2, color);
					mRender->DrawCircle(v1, 0.05f, color);
					v1 = v2;
				}
			}
			break;

		case b2Shape::e_polygon:
			{
				b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
				int32 vertexCount = poly->m_vertexCount;
				b2Assert(vertexCount <= b2_maxPolygonVertices);
				b2Vec2 vertices[b2_maxPolygonVertices];

				for (int32 i = 0; i < vertexCount; ++i)
				{
					vertices[i] = b2Mul(xf, poly->m_vertices[i]);
				}

				mRender->DrawSolidPolygon(vertices, vertexCount, color);
			}
			break;

		default:
			break;
		}
	}
}
