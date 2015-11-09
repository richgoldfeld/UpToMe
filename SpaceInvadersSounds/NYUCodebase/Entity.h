#pragma once
#include "ShaderProgram.h"
#include <iostream>
class Entity {
public:
	Entity(GLuint texture, float x, float y, float width, float height, 
		float spritex, float spritey, float spritewidth, float spriteheight);
	void Update(float elapsed);
	void Render(ShaderProgram *program);
	float xpos;
	float ypos;
	float u;
	float v;
	float iwidth;
	float iheight;
	float swidth;
	float sheight;
	float velocity_x;
	float velocity_y;
	GLuint textureId;
};