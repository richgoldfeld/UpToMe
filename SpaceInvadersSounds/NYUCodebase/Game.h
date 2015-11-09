#pragma once
#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <vector>
#include <string>
#include "Entity.h"
#include <SDL_mixer.h>

enum GameState {STATE_MENU, STATE_GAME, STATE_END};
GLuint LoadTexture(const char *image_path);
class Game {
public:
	Game();
	~Game();
	void Setup();
	void ProcessEvents();
	bool UpdateAndRender();
	void Render();
	void Update(float elapsed);
	void DrawText(int fontTexture, std::string text, float size, float spacing);
private:
	bool done;
	float lastFrameTicks;
	SDL_Window* displayWindow;
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	ShaderProgram *program;
	SDL_Event event;
	int state;
	int score;
	int damage;
	GLuint fontTexture;
	GLuint spriteSheet;
	bool alive;
	Entity* player;
	float sincelastfire;
	std::vector<Entity*> aliens;
	std::vector<Entity*> bullets;
	std::vector<Entity*> lasers;
	void RenderMenu();
	void RenderGame();
	void Victory();
	void Loss();
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	Mix_Chunk *gunSound;
	Mix_Chunk *enemyDeath;
	Mix_Music *music;
};