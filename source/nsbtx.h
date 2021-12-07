#pragma once

#include "nsmb.h"

class NSBTX
{
public:
	Vec2 size;
	Vec2 center;

	void setup(void* file, const Vec2& size, const Vec2& center, int texID = 0, int palID = 0);
	void setTexture(int texID);
	void setPalette(int palID);
	void draw(Vec3& position, Vec3& scale);

private:
	Texture tex;
	TextureInfo texInfo;
};
