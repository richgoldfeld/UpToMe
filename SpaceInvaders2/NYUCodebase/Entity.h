#pragma once
#include "ShaderProgram.h"
#include "SheetSprite.h"
enum EntityType { ENTITY_PLAYER, ENTITY_ENEMY, ENTITY_COIN };
class Entity {
public:
	Entity();
	void Update(float elapsed);
	void Render(ShaderProgram *program);
	//bool collidesWith(Entity *entity);
	SheetSprite sprite;
	float x;
	float y;
	float width;
	float height;
	float velocity_x;
	float velocity_y;
	//bool isStatic;
	EntityType entityType;
};