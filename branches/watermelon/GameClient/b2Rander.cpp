#include "StdAfx.h"
#include "b2Rander.h"
#include "hgecolor.h"
#include "hgefontex.h"
#include "hgefontmanager.h"

#include <cstdio>
#include <cstdarg>
#include <cstring>
using namespace std;

b2Render::b2Render()
: hge( hgeCreate(HGE_VERSION) )
, font( hgeFontHelper( "..\\Resource\\ARIALN.ttf", 12 ).getFont() )
{
}

b2Render::~b2Render()
{
	if(hge)
	{
		hge->Release();
		hge = NULL;
	}
}

void b2Render::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	for (int32 i = 1; i < vertexCount; ++i)
	{
		hge->Gfx_RenderLine( vertices[i-1].x, vertices[i-1].y, vertices[i].x, vertices[i].y, hgeColor(1.0f, color.r, color.g, color.b) );
	}

	hge->Gfx_RenderLine( vertices[vertexCount-1].x, vertices[vertexCount-1].y, vertices[0].x, vertices[0].y, hgeColor(1.0f, color.r, color.g, color.b) );

}

void b2Render::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	for (int32 i = 1; i < vertexCount; ++i)
	{
		hge->Gfx_RenderLine( vertices[i-1].x, vertices[i-1].y, vertices[i].x, vertices[i].y, hgeColor(1.0f, color.r, color.g, color.b) );
	}

	hge->Gfx_RenderLine( vertices[vertexCount-1].x, vertices[vertexCount-1].y, vertices[0].x, vertices[0].y, hgeColor(1.0f, color.r, color.g, color.b) );
}

void b2Render::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
	b2Vec2 p(center);

	for (int32 i = 0; i <= k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		hge->Gfx_RenderLine( p.x, p.y, v.x, v.y, hgeColor(1.0f, color.r, color.g, color.b) );

		p = v;

		theta += k_increment;
	}
}

void b2Render::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
	b2Vec2 p(center);

	for (int32 i = 0; i <= k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		hge->Gfx_RenderLine( p.x, p.y, v.x, v.y, hgeColor(1.0f, color.r, color.g, color.b) );

		p = v;

		theta += k_increment;
	}
}

void b2Render::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	hge->Gfx_RenderLine( p1.x, p1.y, p2.x, p2.y, hgeColor(1.0f, color.r, color.g, color.b) );
}

void b2Render::DrawTransform(const b2Transform& xf)
{
	//b2Vec2 p1 = xf.p, p2;
	//const float32 k_axisScale = 0.4f;
	//glBegin(GL_LINES);

	//glColor3f(1.0f, 0.0f, 0.0f);
	//glVertex2f(p1.x, p1.y);
	//p2 = p1 + k_axisScale * xf.q.GetXAxis();
	//glVertex2f(p2.x, p2.y);

	//glColor3f(0.0f, 1.0f, 0.0f);
	//glVertex2f(p1.x, p1.y);
	//p2 = p1 + k_axisScale * xf.q.GetYAxis();
	//glVertex2f(p2.x, p2.y);

	//glEnd();
}

void b2Render::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
	hgeQuad q;
	q.v[0].col = hgeColor( 1.0f, color.r, color.g, color.b );
	q.v[0].x = p.x - size/2.0f;
	q.v[0].y = p.y - size/2.0f;

	q.v[1].col = hgeColor( 1.0f, color.r, color.g, color.b );
	q.v[1].x = p.x + size/2.0f;
	q.v[1].y = p.y - size/2.0f;

	q.v[2].col = hgeColor( 1.0f, color.r, color.g, color.b );
	q.v[2].x = p.x - size/2.0f;
	q.v[2].y = p.y + size/2.0f;

	q.v[3].col = hgeColor( 1.0f, color.r, color.g, color.b );
	q.v[3].x = p.x + size/2.0f;
	q.v[3].y = p.y + size/2.0f;

	hge->Gfx_RenderQuad( &q );
}

void b2Render::DrawString(int x, int y, const char *string, ...)
{
	char buffer[1024];

	va_list arg;
	va_start(arg, string);
	vsprintf(buffer, string, arg);
	va_end(arg);

	font->Render( (float)x, (float)y, HGETEXT_LEFT, XA2W(buffer) );
}

void b2Render::DrawAABB(b2AABB* aabb, const b2Color& c)
{
	hgeQuad q;
	q.v[0].col = hgeColor( 0.5f, c.r, c.g, c.b );
	q.v[0].x = aabb->lowerBound.x;
	q.v[0].y = aabb->lowerBound.y;

	q.v[1].col = hgeColor( 0.5f, c.r, c.g, c.b );
	q.v[1].x = aabb->upperBound.x;
	q.v[1].y = aabb->lowerBound.y;

	q.v[2].col = hgeColor( 0.5f, c.r, c.g, c.b );
	q.v[2].x = aabb->lowerBound.x;
	q.v[2].y = aabb->upperBound.y;

	q.v[3].col = hgeColor( 0.5f, c.r, c.g, c.b );
	q.v[3].x = aabb->upperBound.x;
	q.v[3].y = aabb->upperBound.y;

	hge->Gfx_RenderQuad( &q );
}
