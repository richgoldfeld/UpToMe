#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include "Object.h"
#include "ShaderProgram.h"
#include "Matrix.h"
#include <string>

class Game{
public:
	Game();
	~Game();
	void First();
	bool Both();
	void Render();
	void Update(float elapsed);
	void DrawText(int fontTexture, std::string text, float size, float spacing);
	Matrix modelMatrix; 
private:
	int state;
	int score;
	int damage;
	bool alive;
	bool done;
	float lastFrameTicks;
	float timePassed;
	SDL_Window* displayWindow;
	GLuint fontTexture;
	GLuint spriteSheet;
	ShaderProgram* program;
	Matrix projectionMatrix;
	Matrix viewMatrix;
	Object* player;
	std::vector<Object*> aliens;
	std::vector<Object*> bullets;
	std::vector<Object*> lasers;

	void UpdateProjectiles(float elapsed, std::vector<Object*>& projectiles);
	void UpdateAliensPoints(float elapsed, std::vector<Object*>& aliens, std::vector<Object*>& projectiles);
	void UpdateDamage(float elapsed, std::vector<Object*>& projectiles);
	void UpdateMenu();
	void UpdateGame();
	void RenderMenu();
	void RenderGame();
	void Victory();
	void Loss();

};

