#pragma once
#include "Game.h"
using namespace std;
#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);
	return textureID;
}
Game::Game() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("GoldfeldFinal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
	glewInit();
	#endif
	glViewport(0, 0, 400, 600);
	program = new ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Setup();
}
void Game::Setup() {
	// SDL and OpenGL initialization
	lastFrameTicks = 0.0f;
	projectionMatrix.setOrthoProjection(-1.0f, 1.0f, -1.5f, 1.5f, -1.0f, 1.0f);
	bool done = false;
	currentOrthTop = 1.5f;
	currentOrthBottom = -1.5f;
	//spriteSheet = LoadTexture("sprites.png");
	player = new Entity(spriteSheet, 0.2f, -1.3f, 0.12f, 0.18f, 0, 86, 99, 75);
	player2 = new Entity(spriteSheet, -0.2f, -1.3f, 0.12f, 0.18f, 0, 86, 99, 75);
	sinceLastSpawn = 0.0f;
	srand(static_cast <unsigned> (time(0)));
}
Game::~Game() {
	// SDL and OpenGL cleanup (joysticks, textures, etc).
	delete player;
	for (GLuint i = 0; i < enemies.size(); i++) { delete enemies[i]; }
	for (GLuint i = 0; i < bullets.size(); i++) { delete bullets[i]; }
	SDL_Quit();
}
void Game::Render() {
	program->setModelMatrix(modelMatrix);
	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);
	glClear(GL_COLOR_BUFFER_BIT);
	
	player->Render(program);
	player2->Render(program);

	for (Entity* e : enemies){
		e->Render(program);
	}
	for (Entity* e : bullets){
		e->Render(program);
	}
	SDL_GL_SwapWindow(displayWindow);
	// clear, render and swap the window
}
void Game::ProcessEvents() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
	}
}
void Game::Update(float elapsed) {
	// move things based on time passed
	// check for collisions and respond to them

	move = .3f;
	
	viewMatrix.Translate(0.0f, -move * elapsed, 0.0f);
	player->ypos += move * elapsed;
	player2->ypos += move * elapsed;
	currentOrthTop += elapsed * move; 
	currentOrthBottom += elapsed * move;
	sinceLastSpawn += elapsed;
	//ENEMY SPAWN
	if (sinceLastSpawn > 2){
		sinceLastSpawn = 0;
		float randX = -.9 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (.9 - -.9)));
		enemies.push_back(new Entity(spriteSheet, randX, currentOrthTop, 0.15f, 0.2f, 0, 0, 104, 84));
		enemies[enemies.size() - 1]->sinceLastFire = 0.0f;
	}
	//ENEMY OR BULLET OFF SCREEN
	for (size_t i = 0; i < enemies.size(); i++){
		if (enemies[i]->ypos < currentOrthBottom - .1){
			delete enemies[i];
			enemies.erase(enemies.begin() + i);
		}
	}
	for (size_t i = 0; i < bullets.size(); i++){
		if (bullets[i]->ypos < currentOrthBottom - .1){
			delete bullets[i];
			bullets.erase(bullets.begin() + i);
		}
	}
	//QUIT METHOD
	if (keys[SDL_SCANCODE_ESCAPE]){
		SDL_Quit();
		done = true;
	}
	//PLAYER 1 MOVEMENT
	if (keys[SDL_SCANCODE_LEFT] && player->xpos > -.94){
		player->xpos -= elapsed * .6f;
	}
	else if (keys[SDL_SCANCODE_RIGHT] && player->xpos < .94){
		player->xpos += elapsed * .6f;
	}
	if (keys[SDL_SCANCODE_UP] && player->ypos < currentOrthTop - .09f){
		player->ypos += elapsed * .9f;
	}
	else if (keys[SDL_SCANCODE_DOWN] && player->ypos > currentOrthBottom + .09f){
		player->ypos -= elapsed * .9f;
	}
	//PLAYER 2 MOVEMENT
	if (keys[SDL_SCANCODE_A] && player2->xpos > -.94){
		player2->xpos -= elapsed * .6f;
	}
	else if (keys[SDL_SCANCODE_D] && player2->xpos < .94){
		player2->xpos += elapsed * .6f;
	}
	if (keys[SDL_SCANCODE_W] && player2->ypos < currentOrthTop - .09f){
		player2->ypos += elapsed * .9f;
	}
	else if (keys[SDL_SCANCODE_S] && player2->ypos > currentOrthBottom + .09f){
		player2->ypos -= elapsed * .9f;
	}
	//ENEMIES SHOOT
	for (Entity* e : enemies){
		e->Update(elapsed);
		if (e->sinceLastFire > 4){
			e->sinceLastFire = 0;
			bullets.push_back(new Entity(spriteSheet, e->xpos, (e->ypos - e->iheight), .03f, e->iheight, 0, 163, 13, 37));
			bullets[bullets.size() - 1]->velocity_y = 0.6f;
		}
	}
	for (Entity* e : enemies){
		e->Update(elapsed);
	}
	for (Entity* e : bullets){
		e->ypos -= elapsed * .25;
		if (e->xpos > 0){ e->xpos -= elapsed * .08; }
		else { e->xpos += elapsed * .08; }
	}
}
bool Game::UpdateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	ProcessEvents();
	Update(elapsed);
	Render();
	return done;
}