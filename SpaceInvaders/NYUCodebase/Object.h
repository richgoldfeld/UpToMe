#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
class Object
{
public:
	Object(int texture, float xpos, float ypos, float turn, float w, float h);
	Object(int spriteSheet, float xpos, float ypos, float xchange, float ychange, float turn, float uSprite, float vSprite, float w, float h);
	~Object();
	void Draw();
	void DrawSprite(float scale);

	float x;
	float y;
	float rotation;
	int textureID;
	float u;
	float v;
	float width;
	float height;
	float speed;
	float direction_x;
	float direction_y;
};

