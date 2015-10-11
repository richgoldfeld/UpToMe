#include "Game.h"
#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
//using namespace std;
enum GameState {MENU = 1, GAME = 2, GAME_OVER = 3};
GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	if (!surface){ return -1; }
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	SDL_FreeSurface(surface);
	return textureID;
}
void Game::DrawText(int fontTexture, std::string text, float size, float spacing){ 
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
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
	ShaderProgram program1(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
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
Game::Game(){
#ifdef _WINDOWS
	glewInit();
#endif
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	ShaderProgram program1(RESOURCE_FOLDER"fragment.glsl", RESOURCE_FOLDER"fragment.glsl");
	program = &program1;
	glViewport(0, 0, 800, 600);
	projectionMatrix.setOrthoProjection(-1.33f, 1.33f, -1.0f, 1.0f, -1.0f, 1.0f);
	fontTexture = LoadTexture("pixel_font.png");
	spriteSheet = LoadTexture("sprites.png");
	player = new Object(spriteSheet, 0.0f, -0.9f, 0.0f, 0.0f, 0.0f, 0.0f / 256.0f, 156.0f / 512.0f, 112.0f / 256.0f, 75.0f / 512.0f);

	First();
	done = false;
	lastFrameTicks = 0.0f;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
}
Game::~Game(){
	delete program;
	delete player;
	for (GLuint i = 0; i < bullets.size(); i++) { delete bullets[i]; }
	for (GLuint i = 0; i < lasers.size(); i++) { delete lasers[i]; }
	for (GLuint i = 0; i < aliens.size(); i++) { delete aliens[i]; }
	SDL_Quit();
}
void Game::First(){
	for (GLuint i = 0; i < bullets.size(); i++) { delete bullets[i]; }
	for (GLuint i = 0; i < lasers.size(); i++) { delete lasers[i]; }
	for (GLuint i = 0; i < aliens.size(); i++) { delete aliens[i]; }
	bullets.clear();
	lasers.clear();
	aliens.clear();
	for (float i = 0.3f; i < 1.0f; i += 0.3f){
		aliens.push_back(new Object(spriteSheet, -0.8f, i, 0.2f, -0.05f, 0.0f, 114.0f / 256.0f, 0.0f / 512.0f, 93.0f / 256.0f, 84.0f / 512.0f));
		aliens.push_back(new Object(spriteSheet, -0.4f, i, 0.2f, -0.05f, 0.0f, 0.0f / 256.0f, 311.0f / 512.0f, 104.0f / 256.0f, 84.0f / 512.0f));
		aliens.push_back(new Object(spriteSheet, 0.0f, i, 0.2f, -0.05f, 0.0f, 0.0f / 256.0f, 397.0f / 512.0f, 103.0f / 256.0f, 84.0f / 512.0f));
		aliens.push_back(new Object(spriteSheet, 0.4f, i, 0.2f, -0.05f, 0.0f, 114.0f / 256.0f, 86.0f / 512.0f, 82.0f / 256.0f, 84.0f / 512.0f));
		aliens.push_back(new Object(spriteSheet, 0.8f, i, 0.2f, -0.05f, 0.0f, 106.0f / 256.0f, 311.0f / 512.0f, 97.0f / 256.0f, 84.0f / 512.0f));
	}
	state = 1;
	score = 0;
	alive = true;
	damage = 0;
	timePassed = 0.0f;
}
bool Game::Both()
{
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	Update(elapsed);
	Render();
	return done;
}
void Game::UpdateProjectiles(float elapsed, std::vector<Object*>& projectiles){
	for (GLuint i = 0; i < projectiles.size(); i++) {
		if (projectiles[i]->y > 1.1 || projectiles[i]->y < -1.1){
			if (projectiles[i]){
				delete projectiles[i];
				projectiles.erase(projectiles.begin() + i);
			}
		}
		else{projectiles[i]->y += projectiles[i]->direction_y*projectiles[i]->speed*elapsed;}

	}
}
void Game::UpdateAliensPoints(float elapsed, std::vector<Object*>& aliens, std::vector<Object*>& projectiles){
	timePassed += elapsed;
	for (GLuint i = 0; i < aliens.size(); i++) {
		aliens[i]->x += aliens[i]->direction_x*aliens[i]->speed*elapsed;
		if (aliens[i]->y < -0.75f){ state = 3; alive = false; }
		if (aliens[i]->x>1.2f || aliens[i]->x<-1.2f){
			for (GLuint k = 0; k < aliens.size(); k++) {
				aliens[k]->direction_x = -(aliens[k]->direction_x);
				aliens[k]->x += aliens[k]->direction_x*aliens[k]->speed*elapsed;
				aliens[k]->y += aliens[k]->direction_y;
				aliens[k]->speed += 0.1f;
			}
		}
		if (aliens[i]->x > player->x - 0.01f && aliens[i]->x< player->x + 0.01f&&timePassed>0.2f){
			lasers.push_back(new Object(spriteSheet, aliens[i]->x, aliens[i]->y, 0.0f, -1.0f, 0.0f, 173.0f / 256.0f, 211.0f / 512.0f, 13.0f / 256.0f, 57.0f / 512.0f));
			timePassed = 0.0f;
		}
		for (GLuint j = 0; j < projectiles.size(); j++) {
			if ((aliens[i]->x + (aliens[i]->width*0.3f) > projectiles[j]->x) &&
				(aliens[i]->x - (aliens[i]->width*0.3f) < projectiles[j]->x) &&
				(aliens[i]->y + (aliens[i]->height*0.3f) > projectiles[j]->y) &&
				(aliens[i]->y - (aliens[i]->height*0.3f) < projectiles[j]->y)){
				if (projectiles[j]){
					delete projectiles[j];
					projectiles.erase(projectiles.begin() + j);
				}
				if (aliens[i]){
					delete aliens[i];
					aliens.erase(aliens.begin() + i);
					score += 100;
				}

				if (!aliens.size()) state = 3;
				break;
			}
		}
	}
}
void Game::UpdateDamage(float elapsed, std::vector<Object*>& projectiles){
	for (GLuint j = 0; j < projectiles.size(); j++) {
		if ((player->x + (player->width*0.3f) > projectiles[j]->x) &&
			(player->x - (player->width*0.3f) < projectiles[j]->x) &&
			(player->y + (player->height*0.3f) > projectiles[j]->y) &&
			(player->y - (player->height*0.3f) < projectiles[j]->y)){
			damage++;
			if (projectiles[j]){
				delete projectiles[j];
				projectiles.erase(projectiles.begin() + j);
			}
		}
	}
	if (damage > 3){
		alive = false;
		state = 3;
	}
}
void Game::Update(float elapsed){
	UpdateProjectiles(elapsed, bullets);
	UpdateProjectiles(elapsed, lasers);
	UpdateAliensPoints(elapsed, aliens, bullets);
	UpdateDamage(elapsed, lasers);
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (event.button.button == 1){
				if (state == 2){
					bullets.push_back(new Object(spriteSheet, player->x, -0.8f, 0.0f, 1.0f, 0.0f, 158.0f / 256.0f, 211.0f / 512.0f, 13.0f / 256.0f, 57.0f / 512.0f));
				}
				else if (state == 3){
					First();
				}
				else state = 2;
			}
		}
		else if (event.type == SDL_MOUSEMOTION) {
			player->x = (((float)event.motion.x / 800.0f) * 2.666f) - 1.333f;//the new x position from mouse
			if (player->x > 1.2f){ player->x = 1.2f; }
			else if (player->x < -1.2f){ player->x = -1.2f; }
		}
	}
	program->setModelMatrix(modelMatrix);
	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);
}
void Game::Render(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	switch (state) {
	case MENU:
		RenderMenu();
		break;
	case GAME:
		RenderGame();
		break;
	case GAME_OVER:
		if (alive) Victory();
		else Loss();
		break;
	}
	SDL_GL_SwapWindow(displayWindow);
}
void Game::RenderMenu(){
	modelMatrix.identity();
	modelMatrix.Translate(-0.8f, 0.5f, 0.0);
	DrawText(fontTexture, "SpaceInvaders", 0.13f, 0.0f);
	modelMatrix.identity();
	modelMatrix.Translate(-0.3f, -0.5f, 0.0);
	DrawText(fontTexture, "Click To Play", 0.05f, 0.0f);
}
void Game::RenderGame(){
	modelMatrix.identity();
	modelMatrix.Translate(-1.2f, 0.85f, 0.0);
	DrawText(fontTexture, std::to_string(score), 0.05f, 0.0f);
	modelMatrix.Translate(0.0f, 0.1f, 0.0);
	for (GLuint i = damage; i < 3; i++){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, spriteSheet);

		GLfloat verticies[] = { -112.0f / 256.0f * 0.1, 75.0f / 512.0f * 0.2, -112.0f / 256.0f * 0.1, -75.0f / 512.0f * 0.2,
			112.0f / 256.0f * 0.1, -75.0f / 512.0f * 0.2, 112.0f / 256.0f * 0.1, 75.0f / 512.0f * 0.2 };//spritesheet is 1:2 so height needs to be multiplied to keep proportion
		glVertexPointer(2, GL_FLOAT, 0, verticies);
		glEnableClientState(GL_VERTEX_ARRAY);
		GLfloat quadUVs[] = { 0.0f / 256.0f, 156.0f / 512.0f, 0.0f / 256.0f, 156.0f / 512.0f + 75.0f / 512.0f, 0.0f / 256.0f + 112.0f / 256.0f, 156.0f / 512.0f + 75.0f / 512.0f, 0.0f / 256.0f + 112.0f / 256.0f, 156.0f / 512.0f };
		glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisable(GL_TEXTURE_2D);
		modelMatrix.Translate(0.1f, 0.0f, 0.0);
	}
	glLoadIdentity();
	player->DrawSprite(0.3f);
	float u, v, w, h;
	if (damage){
		if (damage == 1){
			u = 0.0f;
			v = 233.0f;
			w = 111.0f;
			h = 76.0f;
		}
		else if (damage == 2){
			u = 0.0f;
			v = 78.0f;
			w = 112.0f;
			h = 76.0f;
		}
		else if (damage == 3){
			u = 0.0f;
			v = 0.0f;
			w = 112.0f;
			h = 76.0f;
		}
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, spriteSheet);
		glMatrixMode(GL_MODELVIEW);
		GLfloat quad[] = { -w / 256.0f * 0.3, h / 512.0f * 0.6, -w / 256.0f * 0.3, -h / 512.0f * 0.6,
			w / 256.0f * 0.3, -h / 512.0f * 0.6, w / 256.0f * 0.3, h / 512.0f * 0.6 };//spritesheet is 1:2 so height needs to be multiplied to keep proportion
		glVertexPointer(2, GL_FLOAT, 0, quad);
		glEnableClientState(GL_VERTEX_ARRAY);
		GLfloat quadUVs[] = { u / 256.0f, v / 512.0f, u / 256.0f, v / 512.0f + h / 512.0f, u / 256.0f + w / 256.0f, v / 512.0f + h / 512.0f, u / 256.0f + w / 256.0f, v / 512.0f };
		glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_QUADS, 0, 4);
		glDisable(GL_TEXTURE_2D);
	}
	for (GLuint i = 0; i < aliens.size(); i++) {aliens[i]->DrawSprite(0.3f);}
	for (GLuint i = 0; i < bullets.size(); i++) {bullets[i]->DrawSprite(0.3f);}
	for (GLuint i = 0; i < lasers.size(); i++) {lasers[i]->DrawSprite(0.3f);}
}
void Game::Victory(){
	modelMatrix.identity();
	modelMatrix.Translate(-0.85f, 0.5f, 0.0);
	DrawText(fontTexture, "YOU WIN!", 0.25f, 0.0f);
	modelMatrix.Translate(0.35f, -1.0f, 0.0);
	DrawText(fontTexture, "Click To Play Again", 0.05f, 0.0f);
}
void Game::Loss(){
	modelMatrix.identity();
	modelMatrix.Translate(-0.5f, 0.5f, 0.0);
	DrawText(fontTexture, "YOU LOSE!", 0.13f, 0.0f);
	modelMatrix.Translate(0.05f, -1.0f, 0.0);
	DrawText(fontTexture, "Click To Play Again", 0.05f, 0.0f);
}