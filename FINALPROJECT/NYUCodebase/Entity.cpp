#include "Entity.h"

void Entity::Render(ShaderProgram *program){
		float vertex[] = { xpos - (.5 * iwidth), ypos + (.5 * iheight), 
		xpos - (.5 * iwidth), ypos - (.5 * iheight),
		xpos + (.5 * iwidth), ypos - (.5 * iheight),
		xpos + (.5 * iwidth), ypos - (.5 * iheight),
		xpos + (.5 * iwidth), ypos + (.5 * iheight),
		xpos - (.5 * iwidth), ypos + (.5 * iheight)
	};
	float texCoordData[] = {u, v, u, (v + sheight), (u + swidth), (v + sheight),
		(u + swidth), (v + sheight), (u + swidth), v, u, v};
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertex);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData);
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	
}
Entity::Entity(GLuint texture, float x, float y, float width, float height,
	float spritex, float spritey, float spritewidth, float spriteheight){
	textureId = texture;
	xpos = x;
	ypos = y;
	iwidth = width;
	iheight = height;
	u = spritex;
	v = spritey;
	swidth = spritewidth;
	sheight = spriteheight;
	velocity_x = 0.0f;
	velocity_y = 0.0f;
	sinceLastFire = 0.0f;
}
void Entity::Update(float elapsed){
	//xpos += velocity_x * elapsed;
	//ypos += velocity_y * elapsed;
	sinceLastFire += elapsed;
}