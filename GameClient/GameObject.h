#pragma once

struct GameObject
{
	int type;

	float hp;
	float st;

	hgeSprite*		mSprite;
	hgeAnimation*	mAnimation;
};
