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
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "Entity.h"
#include <SDL_mixer.h>
#include <ctime>
#include <cstdlib>
#include <math.h>
using namespace std;

enum GameState { STATE_MENU, STATE_LEVEL1, STATE_LEVEL2, STATE_LEVEL3, STATE_1WIN, STATE_2WIN};
GLuint LoadTexture(const char *image_path);
class Game {
public:
	Game();
	~Game();
	void Setup();
	void ProcessEvents();
	bool UpdateAndRender();
	void Render();
	void RenderMenu();
	void RenderGame();
	void RenderEndGame();
	void Update(float elapsed);
	bool readHeader(ifstream &stream);
	bool readLayerData(ifstream &stream);
	
private:
	bool done;
	float lastFrameTicks;
	SDL_Window* displayWindow;
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	ShaderProgram *program;
	SDL_Event event;
	GLuint fontTexture;
	GLuint spriteSheet;
	GLuint healthImage;
	GLuint backgroundImage;
	GLuint otherSheet;

	Entity* player;
	Entity* player2;
	Entity* p1health;
	Entity* p2health;
	Entity* background;
	Entity* endGame;
	Entity* menu;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	float currentOrthTop;
	float currentOrthBottom;
	int state;
	float move;
	float player1health;
	float player2health;

	float sinceLastSpawn;
	vector<Entity*> enemies;
	vector<Entity*> bullets;
};