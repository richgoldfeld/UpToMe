#include "Game.h"
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
	Setup();
}
void Game::Setup() {
	// SDL and OpenGL initialization
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif
	lastFrameTicks = 0.0f;
	glViewport(0, 0, 800, 600);
	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	projectionMatrix.setOrthoProjection(-1.33f, 1.33f, -1.0f, 1.0f, -1.0f, 1.0f);
	bool done = false;
	fontTexture = LoadTexture("pixel_font.png");
	spriteSheet = LoadTexture("sprites.png");
	state = 0;
	alive = true;
	score = 0;
	damage = 0;
	sincelastfire = 0.0f;
	player = new Entity(spriteSheet, 0.0f, -0.85f, 0.15f, 0.1f, 0, 86, 99, 75);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float initialy = 0.7f;
	for (float i = -0.88f; i < .88f; i += .16f){
		aliens.push_back(new Entity(spriteSheet, i, initialy, 0.15f, 0.1f, 0, 0, 104, 84));
		aliens.push_back(new Entity(spriteSheet, i, initialy - .11f, 0.15f, 0.1f, 0, 0, 104, 84));
		aliens.push_back(new Entity(spriteSheet, i, initialy - .22f, 0.15f, 0.1f, 0, 0, 104, 84));
		aliens.push_back(new Entity(spriteSheet, i, initialy - .33f, 0.15f, 0.1f, 0, 0, 104, 84));
		aliens.push_back(new Entity(spriteSheet, i, initialy - .44f, 0.15f, 0.1f, 0, 0, 104, 84));
	}
	for (Entity* e : aliens){
		e->velocity_x = 0.3f;
	}
}
Game::~Game() {
	// SDL and OpenGL cleanup (joysticks, textures, etc).
	delete player;
	for (GLuint i = 0; i < bullets.size(); i++) { delete bullets[i]; }
	for (GLuint i = 0; i < aliens.size(); i++) { delete aliens[i]; }
	SDL_Quit();
}
void Game::DrawText(int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (size_t i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
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
	case STATE_GAME:
		RenderGame();
		break;
	case STATE_END:
		if (alive){ Victory(); }
		else{ Loss(); }
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
	if (keys[SDL_SCANCODE_SPACE] && state == 0){
		state = 1;
	}
	else if (keys[SDL_SCANCODE_SPACE] && state == 2){
		state = 0;
		Setup();
	}
	if (state == 1){
		if (keys[SDL_SCANCODE_LEFT] && player->xpos > -1.25){
			player->xpos -= elapsed * .5f;
		}
		else if (keys[SDL_SCANCODE_RIGHT] && player->xpos < 1.25){
			player->xpos += elapsed * .5f;
		}
		Entity* relevant = NULL;
		for (size_t i = 0; i < aliens.size(); i++){
			if (aliens[i] != NULL){
				relevant = aliens[i];
				break;
			}
		}
		if (relevant == NULL){
			state++;
		}
		else{
			if (relevant->velocity_x < 0 && (relevant->xpos < -1.255)){
				for (Entity* e : aliens){
					e->velocity_x = 0.3f;
					e->ypos -= .05f;
				}
			}
			else if (relevant->velocity_x > 0){
				for (size_t i = 0; i < aliens.size(); i++){
					if (aliens[i] != NULL){
						relevant = aliens[i];
					}
				}
				if (relevant->xpos > 1.255){
					for (Entity* e : aliens){
						e->velocity_x = -0.3f;
						e->ypos -= .05f;
					}
				}
			}
		}
		sincelastfire += elapsed;
		if (keys[SDL_SCANCODE_SPACE]){
			if (sincelastfire > 0.7){
				bullets.push_back(new Entity(spriteSheet, player->xpos, (player->ypos + player->iheight), .03f, player->iheight, 0, 163, 13, 37));
				bullets[bullets.size() - 1]->velocity_y = 0.6f;
				sincelastfire = 0.0f;
			}
		}
		for (size_t i = 0; i < bullets.size(); i++){
			if (bullets[i]->ypos > 1.3){
				delete bullets[i];
				bullets.erase(bullets.begin() + i);
			}
		}
		for (size_t i = 0; i < bullets.size(); i++){
			Entity* e = bullets[i];
			for (size_t j = 0; j < aliens.size(); j++){
				Entity* a = aliens[j];
				if (
					(e->ypos - (e->iheight / 2)) >(a->ypos + (a->iheight / 2)) ||
					(e->ypos + (e->iheight / 2)) < (a->ypos - (a->iheight / 2)) ||
					(e->xpos - (e->iwidth / 2)) > (a->xpos + (a->iwidth / 2)) ||
					(e->xpos + (e->iwidth / 2)) < (a->xpos - (a->iwidth / 2))
					)
				{}
				else{
					delete bullets[i];
					bullets.erase(bullets.begin() + i);
					delete aliens[j];
					aliens.erase(aliens.begin() + j);
					score += 10;
					break;
				}			
			}
		}
		for (Entity* e : aliens){
			e->Update(elapsed);
		}
		for (Entity* e : bullets){
			e->Update(elapsed);
		}		
	}
	// move things based on time passed
	// check for collisions and respond to them
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

void Game::RenderMenu(){
	modelMatrix.identity();
	modelMatrix.Translate(-0.69f, 0.7f, 0.0);
	DrawText(fontTexture, "SpaceInvaders", 0.13f, 0.0f);
	/*
	modelMatrix.identity();
	modelMatrix.Translate(-0.0f, 0.8f, 0.0);
	DrawText(fontTexture, "Click To Play", 0.05f, 0.0f);
	*/
}
void Game::RenderGame(){
	modelMatrix.identity();
	for (size_t i = 0; i < aliens.size(); i++) {
		aliens[i]->Render(program);
	}
	for (Entity* e : bullets){
		e->Render(program);
	}
	player->Render(program);
}
void Game::Victory(){
	modelMatrix.identity();
	modelMatrix.Translate(-0.6f, 0.4f, 0.0);
	DrawText(fontTexture, "You win", 0.13f, 0.0f);
}
void Game::Loss(){
	modelMatrix.identity();
	modelMatrix.Translate(-0.6f, 0.4f, 0.0);
	DrawText(fontTexture, "You lose", 0.13f, 0.0f);
}