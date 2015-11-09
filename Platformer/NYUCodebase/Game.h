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
using namespace std;

GLuint LoadTexture(const char *image_path);
class Game {
public:
	Game();
	~Game();
	void Setup();
	void ProcessEvents();
	bool UpdateAndRender();
	void Render();
	void RenderTiles();
	void Update(float elapsed);
	bool readHeader(ifstream &input);
	bool readLayerData(ifstream &input);
	
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
	Entity* player;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	int mapWidth;
	int mapHeight;
	unsigned char** levelData;
	float TILE_SIZE;
};