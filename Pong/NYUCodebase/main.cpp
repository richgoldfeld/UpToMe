#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include <time.h>
#include <math.h>
#include <iostream>
#define RAD (3.14159265 / 180)

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

int getAngle(){
	int angle;
	angle = rand() % 359 + 1;
	
	if (angle == 180){
		angle = getAngle();
	}
	else if (angle > 45 && angle < 135){
		angle = getAngle();
	}
	else if (angle > 225 && angle < 315){
		angle = getAngle();
	}
	return angle;
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	glViewport(0, 0, 640, 360);
	ShaderProgram program(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
	float lastFrameTicks = 0.0f;
	float distance = 1.0f;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	float ypos1 = 0.4f;
	float ypos2 = 0.4f;
	float ybp = 0.05f;
	float xbp = 0.05f;
	srand(time(NULL));
	int angle = getAngle();

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		glUseProgram(program.programID);
		glClear(GL_COLOR_BUFFER_BIT);

#pragma region draw centerline, topline, bottomline
		glEnableVertexAttribArray(program.positionAttribute);
		float topbottom[] = { 3.5f, 2.0f, -3.5f, 2.0f, -3.5f, 1.9f, -3.5f, 1.9f, 3.5f, 1.9f, 3.5f, 2.0f, 
			3.5f, -2.0f, -3.5f, -2.0f, -3.5f, -1.9f, -3.5f, -1.9f, 3.5f, -1.9f, 3.5f, -2.0f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, topbottom);
		glDrawArrays(GL_TRIANGLES, 0, 12);
		glDisableVertexAttribArray(program.positionAttribute);

		glEnableVertexAttribArray(program.positionAttribute);
		float centerline[] = { .01f, 2.0f, -.01f, 2.0f, -.01f, -2.0f, -.01f, -2.0f, .01f, -2.0f, .01f, 2.0f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, centerline);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
#pragma endregion

		glEnableVertexAttribArray(program.positionAttribute);
		float p1[] = { -3.4f, ypos1, -3.4f, (ypos1 - 0.8f), -3.3f, (ypos1 - 0.8f), -3.3f, (ypos1 - 0.8f), -3.3f, ypos1, -3.4f, ypos1 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, p1);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);

		glEnableVertexAttribArray(program.positionAttribute);
		float p2[] = { 3.4f, ypos2, 3.4f, (ypos2 - 0.8f), 3.3f, (ypos2 - 0.8f), 3.3f, (ypos2 - 0.8f), 3.3f, ypos2, 3.4f, ypos2 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, p2);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);

		glEnableVertexAttribArray(program.positionAttribute);
		float ball[] = {xbp, ybp, xbp - .1, ybp, xbp - .1, ybp - .1, xbp, ybp, xbp - .1, ybp - .1, xbp, ybp - .1};
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ball);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);

		if (keys[SDL_SCANCODE_W]){
			if (ypos1 < 2) {ypos1 += elapsed * distance;}
		}
		else if (keys[SDL_SCANCODE_S]){
			if ((ypos1 - 0.8) > -2){ypos1 -= elapsed * distance;}
		}

		if (keys[SDL_SCANCODE_UP]){
			if (ypos2 < 2){ ypos2 += elapsed * distance; }
		}
		else if (keys[SDL_SCANCODE_DOWN]){
			if ((ypos2 - 0.8) > -2){ ypos2 -= elapsed *	distance; }
		}

		xbp += cos(angle * RAD) * elapsed * distance * 5;
		ybp += sin(angle * RAD) * elapsed * distance * 5;

		if ((xbp - .1) <= -3.5){
			xbp = .05f;
			ybp = .05f;
			angle = getAngle();
		}
		if (xbp >= 3.5){
			xbp = .05f;
			ybp = .05f;
			angle = getAngle();
		}
		if (ybp >= 1.88){
			if (angle > 90){
				angle += 90;
			}
			else{
				angle -= 90;
			}
			ybp = 1.87;
		}
		if ((ybp - .1) <= -1.88){
			if (angle < 270){
				angle -= 90;
			}
			else{
				angle += 90;
			}
			ybp = -1.77;
		}

		if (xbp <= -3.2){
			if (ybp >= (ypos1 - .8) && (ybp - .1) <= ypos1){
				if (angle < 180){ angle -= 90; }
				else{ angle += 90; }
				xbp = -3.19;
			}
		}

		if (xbp >= 3.3){
			if (ybp >= (ypos2 - .8) && (ybp - .1) <= ypos2){
				if (angle < 45){ angle += 270; }
				else{ angle -= 90; }
				xbp = 3.29;
			}
		}

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
