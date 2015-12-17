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
	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Setup();
}
void Game::Setup() {
	// SDL and OpenGL initialization
	lastFrameTicks = 0.0f;
	projectionMatrix.setOrthoProjection(-1.0f, 1.0f, -1.5f, 1.5f, -1.0f, 1.0f);
	viewMatrix.identity();
	bool done = false;
	currentOrthTop = 1.5f;
	currentOrthBottom = -1.5f;
	spriteSheet = LoadTexture("sprites.png");
	healthImage = LoadTexture("health.png");
	backgroundImage = LoadTexture("background.png");
	otherSheet = LoadTexture("sprites2.png");
	state = 0;
	menu = new Entity(otherSheet, 0, (currentOrthTop - 1.5f), 2.0f, 2.0f, 0, 0, (623.0f / 1024.0f), (460.0f / 512.0f));
	background = new Entity(backgroundImage, 0, 0, 2.0f, 400.0f, 0.0f, 0.0f, (600.0f / 2048.0f), (800.0f / 1024.0f * 80.0f));
	player = new Entity(spriteSheet, 0.2f, -1.3f, 0.12f, 0.18f, 
		(472.0f / 1024.0f), 0.0f, (159.0f / 1024.0f), (276.0f / 512.0f));
	player2 = new Entity(spriteSheet, -0.2f, -1.3f, 0.12f, 0.18f, 
		(633.0f / 1024.0f), 0.0f, (158.0f / 1024.0f), (275.0f / 512.0f));
	p1health = new Entity(healthImage, -0.8f, -1.425f, 0.4f, 0.15f, 0.0f, 0.0f, 1.0f, 1.0f);
	p2health = new Entity(healthImage, 0.8f, -1.425f, 0.4f, 0.15f, 0.0f, 0.0f, 1.0f, 1.0f);
	endGame = new Entity(otherSheet, 0, 0, 1.0f, 0.2f,
		0.0f, (478.0f / 512.0f), (90.0f / 1024.0f), (14.0f / 512.0f));
	sinceLastSpawn = 0.0f;
	srand(static_cast <unsigned> (time(0)));
	player1health = 3.0f;
	player2health = 3.0f;
}
Game::~Game() {
	// SDL and OpenGL cleanup (joysticks, textures, etc).
	delete player;
	delete endGame;
	delete player2;
	delete p1health;
	delete p2health;
	delete background;
	for (GLuint i = 0; i < enemies.size(); i++) { delete enemies[i]; }
	for (GLuint i = 0; i < bullets.size(); i++) { delete bullets[i]; }
	SDL_Quit();
}
void Game::Render() {
	program->setModelMatrix(modelMatrix);
	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);
	glClear(GL_COLOR_BUFFER_BIT);

	switch (state) {
	case STATE_MENU:
		RenderMenu();
		break;
	case STATE_LEVEL1:
		RenderGame();
		break;
	case STATE_LEVEL2:
		RenderGame();
		break;
	case STATE_LEVEL3:
		RenderGame();
		break;
	case STATE_1WIN:
		RenderEndGame();
		break;
	case STATE_2WIN:
		RenderEndGame();
		break;
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
	//QUIT METHOD
	if (keys[SDL_SCANCODE_ESCAPE]){
		SDL_Quit();
		done = true;
	}
	if (state == 0){
		if (keys[SDL_SCANCODE_1]){state = 1;}
		else if (keys[SDL_SCANCODE_2]){	state = 2;	}
		else if (keys[SDL_SCANCODE_3]){	state = 3;	}
	}
	else if (state == 4 || state == 5){
		if (keys[SDL_SCANCODE_SPACE]){
			state = 0;
			Setup();
		}
	}
	else if (state == 1 || state == 2 || state == 3){
		if (state == 2){
			background->u = (1204.0f / 2048.0f);
		}
		else if (state == 3){
			background->u = (602.0f / 2048.0f);
		}
		move = .3f;
		viewMatrix.Translate(0.0f, -move * elapsed, 0.0f);
		player->ypos += move * elapsed;
		player2->ypos += move * elapsed;
		p1health->ypos += move * elapsed;
		p2health->ypos += move * elapsed;
		//background->ypos += move * elapsed;
		currentOrthTop += elapsed * move;
		currentOrthBottom += elapsed * move;
		sinceLastSpawn += elapsed;
		//ENEMY SPAWN
		if (sinceLastSpawn > 2){
			sinceLastSpawn = 0;
			float randX = -.9 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (.9 - -.9)));
			enemies.push_back(new Entity(spriteSheet, randX, currentOrthTop, 0.15f, 0.2f, 0, 0, (470.0f / 1024.0f), (477.0f / 512.0f)));
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
		//PLAYER 1 MOVEMENT
		if (keys[SDL_SCANCODE_LEFT] && player->xpos > -.94){
			player->xpos -= elapsed * .5f;
		}
		else if (keys[SDL_SCANCODE_RIGHT] && player->xpos < .94){
			player->xpos += elapsed * .5f;
		}
		if (keys[SDL_SCANCODE_UP] && player->ypos < currentOrthTop - .09f){
			player->ypos += elapsed * .7f;
		}
		else if (keys[SDL_SCANCODE_DOWN] && player->ypos > currentOrthBottom + .09f){
			player->ypos -= elapsed * .6f;
		}
		//PLAYER 2 MOVEMENT
		if (keys[SDL_SCANCODE_A] && player2->xpos > -.94){
			player2->xpos -= elapsed * .5f;
		}
		else if (keys[SDL_SCANCODE_D] && player2->xpos < .94){
			player2->xpos += elapsed * .5f;
		}
		if (keys[SDL_SCANCODE_W] && player2->ypos < currentOrthTop - .09f){
			player2->ypos += elapsed * .7f;
		}
		else if (keys[SDL_SCANCODE_S] && player2->ypos > currentOrthBottom + .09f){
			player2->ypos -= elapsed * .6f;
		}
		//ENEMIES SHOOT
		for (Entity* e : enemies){
			e->Update(elapsed);
			if (e->sinceLastFire > 4){
				e->sinceLastFire = 0;
				bullets.push_back(new Entity(spriteSheet, e->xpos, (e->ypos - e->iheight), .06f, e->iheight,
					(472.0f / 1024.0f), (278.0f / 512.0f), (77.0f / 1024.0f), (123.0f / 512.0f)));
				bullets[bullets.size() - 1]->velocity_y = 0.6f;
			}
		}

		//Player collision with bullets
		for (size_t i = 0; i < bullets.size(); i++){
			Entity* e = bullets[i];
			if (player->xpos < e->xpos + e->iwidth &&
				player->xpos + player->iwidth > e->xpos &&
				player->ypos < e->ypos + e->iheight &&
				player->iheight + player->ypos > e->ypos)
			{
				delete bullets[i];
				bullets.erase(bullets.begin() + i);
				player1health--;
			}
			else if (player2->xpos < e->xpos + e->iwidth &&
				player2->xpos + player2->iwidth > e->xpos &&
				player2->ypos < e->ypos + e->iheight &&
				player2->iheight + player2->ypos > e->ypos)
			{
				delete bullets[i];
				bullets.erase(bullets.begin() + i);
				player2health--;
			}
		}
		//Enemy collision with player
		for (size_t i = 0; i < enemies.size(); i++){
			Entity* e = enemies[i];
			if (player->xpos < e->xpos + e->iwidth &&
				player->xpos + player->iwidth > e->xpos &&
				player->ypos < e->ypos + e->iheight &&
				player->iheight + player->ypos > e->ypos)
			{
				delete enemies[i];
				enemies.erase(enemies.begin() + i);
				player1health--;
			}
			else if (player2->xpos < e->xpos + e->iwidth &&
				player2->xpos + player2->iwidth > e->xpos &&
				player2->ypos < e->ypos + e->iheight &&
				player2->iheight + player2->ypos > e->ypos)
			{
				delete enemies[i];
				enemies.erase(enemies.begin() + i);
				player2health--;
			}
		}
		//Change players visual health based on damage
		if (player1health == 2){
			p1health->swidth = player1health / 3.0f;
		}
		else if (player1health == 1){
			p1health->swidth = player1health / 3.0f;
		}
		if (player2health == 2){
			p2health->swidth = player2health / 3.0f;
		}
		else if (player2health == 1){
			p2health->swidth = player2health / 3.0f;
		}

		if (player1health == 0){
			state = 5;
		}
		else if (player2health == 0){
			state = 4;
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

void Game::RenderMenu() {
	menu->ypos = (currentOrthTop - 1.5f);
	menu->Render(program);
}
void Game::RenderGame(){
	background->Render(program);
	player->Render(program);
	player2->Render(program);
	p1health->Render(program);
	p2health->Render(program);

	for (Entity* e : enemies){
		e->Render(program);
	}
	for (Entity* e : bullets){
		e->Render(program);
	}
}
void Game::RenderEndGame(){
	endGame->ypos = currentOrthTop - 1.5;
	if (state == 5){
		endGame->v = (462.0f / 512.0f);
		endGame = new Entity(otherSheet, 0, currentOrthTop - 1.5, 1.0f, 0.2f,
			0.0f, (462.0f / 512.0f), (90.0f / 1024.0f), (14.0f / 512.0f));
	}
	endGame->Render(program);
}