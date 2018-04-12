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

SDL_Window* displayWindow;

GLuint LoadTexture(const char *filePath) {
    int w,h,comp;
    unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
    if(image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    GLuint retTexture;
    glGenTextures(1, &retTexture);
    glBindTexture(GL_TEXTURE_2D, retTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(image);
    return retTexture;
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Assignment 1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    glViewport(0, 0, 640, 360);
    
    ShaderProgram program;
    program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    
    ShaderProgram texProgram;
    texProgram.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    
    GLuint birdsTexture = LoadTexture(RESOURCE_FOLDER"birds.png");
    GLuint flowersTexture = LoadTexture(RESOURCE_FOLDER"Flowers.png");
    GLuint cloudsTexture = LoadTexture(RESOURCE_FOLDER"clouds.png");
    GLuint personTexture = LoadTexture(RESOURCE_FOLDER"person.png");
    GLuint jeepTexture = LoadTexture(RESOURCE_FOLDER"jeep.png");
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    projectionMatrix.SetOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    
    float lastFrameTicks = 0.0f;
    float move = -5.0;
    float pMove = -3.5;
    
    glClearColor(0.529, 0.808, 0.980, 1.0);
    
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(texProgram.programID);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        texProgram.SetProjectionMatrix(projectionMatrix);
        texProgram.SetViewMatrix(viewMatrix);
        
        program.SetProjectionMatrix(projectionMatrix);
        program.SetViewMatrix(viewMatrix);
        
        //Verticies
        float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5,  0.5, -0.5, -0.5, 0.5,  0.5, -0.5,  0.5 };
        float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
        
        //Time Counting
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        //Floor
        program.SetColor(0.804, 0.522, 0.247, 1.0);
        modelMatrix.Identity();
        modelMatrix.Scale(7.3, 1, 0.0);
        modelMatrix.Translate(0.0, -2.3, 0.0);
        program.SetModelMatrix(modelMatrix);
        
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Flowers
        glBindTexture(GL_TEXTURE_2D, flowersTexture);
        
        modelMatrix.Identity();
        modelMatrix.Translate(-0.045, -1.4, 0.0);
        modelMatrix.Scale(7.0, 1.3, 1.0);
        texProgram.SetModelMatrix(modelMatrix);
        
        
        glVertexAttribPointer(texProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(texProgram.positionAttribute);
        
        glVertexAttribPointer(texProgram.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(texProgram.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Clouds
        glBindTexture(GL_TEXTURE_2D, cloudsTexture);
        
        if (move >= 5.0) {
            move = -5.0;
        } else {
            move += elapsed;
        }
        
        modelMatrix.Identity();
        modelMatrix.Translate(move, 1.2, 0.0);
        modelMatrix.Scale(2.7, 2.0, 1.0);
        texProgram.SetModelMatrix(modelMatrix);
        
        glVertexAttribPointer(texProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(texProgram.positionAttribute);
        
        glVertexAttribPointer(texProgram.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(texProgram.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Birds
        glBindTexture(GL_TEXTURE_2D, birdsTexture);
        
        modelMatrix.Identity();
        modelMatrix.Translate(0.0, 1.35, 0.0);
        modelMatrix.Rotate(180.0f * (3.1415926f / 180.0f));
        modelMatrix.Scale(7.0, 1.5, 1.0);
        texProgram.SetModelMatrix(modelMatrix);
        
        glVertexAttribPointer(texProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(texProgram.positionAttribute);
        
        glVertexAttribPointer(texProgram.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(texProgram.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Person
        glBindTexture(GL_TEXTURE_2D, personTexture);
        
        if (elapsed >= 0.017) {
            pMove += elapsed;
        }
        
        if (pMove >= 0) {
            pMove = 0;
        }
        
        modelMatrix.Identity();
        modelMatrix.Scale(1.5, 1.5, 1.0);
        modelMatrix.Translate(0.0, pMove, 0.0);
        texProgram.SetModelMatrix(modelMatrix);
        
        glVertexAttribPointer(texProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(texProgram.positionAttribute);
        
        glVertexAttribPointer(texProgram.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(texProgram.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(texProgram.positionAttribute);
        glDisableVertexAttribArray(texProgram.texCoordAttribute);
        
        //Person
        glBindTexture(GL_TEXTURE_2D, jeepTexture);
        
        modelMatrix.Identity();
        modelMatrix.Scale(1.5, 1.5, 1.0);
        modelMatrix.Translate(-move, -1.0, 0.0);
        texProgram.SetModelMatrix(modelMatrix);
        
        glVertexAttribPointer(texProgram.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(texProgram.positionAttribute);
        
        glVertexAttribPointer(texProgram.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(texProgram.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(texProgram.positionAttribute);
        glDisableVertexAttribArray(texProgram.texCoordAttribute);
        
        //glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
}
