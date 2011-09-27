/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeAnimation helper class implementation
*/


#include "..\..\include\hgeanim.h"


hgeAnimation::hgeAnimation(HTEXTURE tex, int row, int col, int mode, float FPS, float x, float y, float w, float h)
	: hgeSprite(tex, x, y, w, h)
{
	orig_width = hge->Texture_GetWidth(tex, true);

	nRow = row;
	nCol = col;
	fSinceLastFrame=-1.0f;
	fSpeed=1.0f/FPS;
	bPlaying=false;

	Mode = mode; //HGEANIM_FWD | HGEANIM_LOOP | HGEANIM_ROW;
	nDelta=1;
	SetFrame(0);
}

hgeAnimation::hgeAnimation(const hgeAnimation & anim)
: hgeSprite(anim)
{ 
	// Copy hgeAnimation parameters: 
	this->orig_width	  = anim.orig_width;
	this->bPlaying        = anim.bPlaying; 
	this->fSpeed          = anim.fSpeed; 
	this->fSinceLastFrame = anim.fSinceLastFrame;
	this->Mode            = anim.Mode;
	this->nDelta          = anim.nDelta;
	this->nRow			  = anim.nRow;
	this->nCol			  = anim.nCol;
	this->nCurFrame       = anim.nCurFrame;
}

void hgeAnimation::SetMode(int mode)
{
	Mode=mode;

	if(mode & HGEANIM_REV)
	{
		nDelta = -1;
		SetFrame(nCol*nRow-1);
	}
	else
	{
		nDelta = 1;
		SetFrame(0);
	}
}


void hgeAnimation::Play()
{
	bPlaying=true;
	fSinceLastFrame=-1.0f;
	if(Mode & HGEANIM_REV)
	{
		nDelta = -1;
		SetFrame(nCol*nRow-1);
	}
	else
	{
		nDelta = 1;
		SetFrame(0);
	}
}


void hgeAnimation::Update(float fDeltaTime)
{
	if(!bPlaying) return;

	if(fSinceLastFrame == -1.0f)
		fSinceLastFrame=0.0f;
	else
		fSinceLastFrame += fDeltaTime;

	while(fSinceLastFrame >= fSpeed)
	{
		fSinceLastFrame -= fSpeed;

		if(nCurFrame + nDelta == nRow*nCol)
		{
			switch(Mode)
			{
				case HGEANIM_FWD:
				case HGEANIM_REV | HGEANIM_PINGPONG:
					bPlaying = false;
					break;

				case HGEANIM_FWD | HGEANIM_PINGPONG:
				case HGEANIM_FWD | HGEANIM_PINGPONG | HGEANIM_LOOP:
				case HGEANIM_REV | HGEANIM_PINGPONG | HGEANIM_LOOP:
					nDelta = -nDelta;
					break;
			}
		}
		else if(nCurFrame + nDelta < 0)
		{
			switch(Mode)
			{
				case HGEANIM_REV:
				case HGEANIM_FWD | HGEANIM_PINGPONG:
					bPlaying = false;
					break;

				case HGEANIM_REV | HGEANIM_PINGPONG:
				case HGEANIM_REV | HGEANIM_PINGPONG | HGEANIM_LOOP:
				case HGEANIM_FWD | HGEANIM_PINGPONG | HGEANIM_LOOP:
					nDelta = -nDelta;
					break;
			}
		}

		if(bPlaying) SetFrame(nCurFrame+nDelta);
	}
}

void hgeAnimation::SetFrame(int n)
{
	float tx1, ty1, tx2, ty2;
	bool bX, bY, bHS;

	n = n % (nRow * nCol);
	if(n < 0) n = nRow * nCol + n;
	nCurFrame = n;

	// calculate texture coords for frame n
	if( Mode | HGEANIM_ROW )
	{
		ty1 = ty + nCurFrame/nCol;
		tx1 = tx + nCurFrame%nCol * width;
	}
	else
	{
		ty1 = ty + nCurFrame%nRow * height;
		tx1 = tx + nCurFrame/nRow;
	}

	tx2 = tx1 + width;
	ty2 = ty1 + height;

	tx1 /= tex_width;
	ty1 /= tex_height;
	tx2 /= tex_width;
	ty2 /= tex_height;

	quad.v[0].tx=tx1; quad.v[0].ty=ty1;
	quad.v[1].tx=tx2; quad.v[1].ty=ty1;
	quad.v[2].tx=tx2; quad.v[2].ty=ty2;
	quad.v[3].tx=tx1; quad.v[3].ty=ty2;

	bX=bXFlip; bY=bYFlip; bHS=bHSFlip;
	bXFlip=false; bYFlip=false;
	SetFlip(bX,bY,bHS);
}

