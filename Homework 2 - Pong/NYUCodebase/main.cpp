#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include "GamePiece.hpp"

SDL_Window* displayWindow;

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Assignment 2 - Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    glViewport(0, 0, 640, 360);
    
    ShaderProgram program;
    program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    projectionMatrix.SetOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
    
    //Initialize Game Pieces
    GamePiece ball(0.0f, 0.0f, 0.2f, 0.2f, 1.85f, 1.75f);
    
    GamePiece top(-3.55f, 2.0f, 7.1f, 0.15f);
    GamePiece bottom(-3.55f, -1.85f, 7.1f, 0.15f);
    
    GamePiece rPad(3.0f, 0.0f, 0.2f, 0.85f, 0.0f, 7.0f);
    GamePiece lPad(-3.2f, 0.0f, 0.2f, 0.85f, 0.0f, 7.0f);
    
    //Score and Time Keeping
    float lastFrameTicks = 0.0f;
    int rightScore = 0;
    int leftScore = 0;
    
    //Start Game
    bool start = false;
    
    glUseProgram(program.programID);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        
        //Time Counting
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    start = true;
                }
            }
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Scan for key presses
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_UP] && !rPad.padColUp(top)){
            rPad.move(elapsed);
        }
        if (keys[SDL_SCANCODE_DOWN] && !rPad.padColDown(bottom)) {
            rPad.move(elapsed, true);
        }
        if (keys[SDL_SCANCODE_W] && !lPad.padColUp(top)) {
            lPad.move(elapsed);
        }
        if (keys[SDL_SCANCODE_S] && !lPad.padColDown(bottom)) {
            lPad.move(elapsed, true);
        }
        
        
        //Set Matricies
        program.SetProjectionMatrix(projectionMatrix);
        program.SetViewMatrix(viewMatrix);
        
        //Ball Logic
        if (start) {
            if (!ball.colUp(top) && !ball.colDown(bottom) && !ball.colRight(rPad) && !ball.colLeft(lPad)) {
                ball.move(elapsed);
            } else {
                ball.move(elapsed);
            }
        } else {
            std::cout << "Press SPACE to start" << std::endl;
        }
        
        //Ball Logic: Win logic
        if (ball.getXPos() >= 3.55) {
            leftScore += 1;
            ball.reset(true);
            std::cout << "Left Point: " << leftScore << std::endl;
        }
        
        if (ball.getXPos() <= -3.55) {
            rightScore += 1;
            ball.reset();
            std::cout << "Right Point: " << rightScore << std::endl;
        }
        
        //Draw Everything
        top.Draw(program, modelMatrix);
        bottom.Draw(program, modelMatrix);
        
        ball.Draw(program, modelMatrix);
        
        rPad.Draw(program, modelMatrix);
        lPad.Draw(program, modelMatrix);
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
}
