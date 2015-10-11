#include "Object.h"

Object::Object(int texture, float xpos, float ypos, float turn, float w, float h){
	textureID = texture;
	x = xpos;
	y = ypos;
	rotation = turn;
	width = w;
	height = h;
}
Object::Object(int spriteSheet, float xpos, float ypos, float xchange, float ychange, float turn, float uSprite, float vSprite, float w, float h){
	textureID = spriteSheet;
	x = xpos;
	y = ypos;
	direction_x = xchange;
	direction_y = ychange;
	speed = 1.0f;
	rotation = turn;
	u = uSprite;
	v = vSprite;
	width = w;
	height = h;
}
Object::~Object(){}

void Object::Draw(){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	//Game.modelMatrix.identity();
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glScalef(width, height, 1.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);
	float verticies[] = { -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f };
	glVertexPointer(2, GL_FLOAT, 0, verticies);
	glEnableClientState(GL_VERTEX_ARRAY);
	GLfloat quadUVs[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_TEXTURE_2D);
}

void Object::DrawSprite(float scale)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);
	GLfloat quad[] = { -width * scale, height * 2 * scale, -width * scale, -height * 2 * scale,
		width * scale, -height * 2 * scale, width * scale, height * 2 * scale };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);
	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
}
