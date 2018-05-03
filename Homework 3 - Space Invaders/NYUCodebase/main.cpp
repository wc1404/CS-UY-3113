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

#include "FloatVectors.hpp"
#include "Printer.hpp"
#include "Sprite.hpp"
#include "GamePiece.hpp"
#include <vector>

// 60 FPS (1.0f/60.0f) (update sixty times a second)
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

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

void setUp(ShaderProgram &program, Matrix &projectionMatrix, const vec3 &bound, int scaleFactor = 1) {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("El Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, bound.x, bound.y, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    glViewport(0, 0, bound.x, bound.y);
    
    program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    glUseProgram(program.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    projectionMatrix.SetOrthoProjection((-bound.x/bound.y)*scaleFactor, (bound.x/bound.y)*scaleFactor, (-bound.x/bound.y)*scaleFactor, (bound.x/bound.y)*scaleFactor, -1.0f, 1.0f);
}

void update(float time, int &score, const vec3 &bound, GamePiece &player, std::vector<GamePiece> &bullets, std::vector<std::vector<GamePiece>> &enemies) {
    //------Scan for Player Movement------//
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    vec3 normalizedBounds( (((bound.x/bound.y)*8) - 0.01), (((bound.x/bound.y)*8) - 0.01) );
    
    bool playerValid = player.inBounds(normalizedBounds);
    
    if (playerValid) {
        if (keys[SDL_SCANCODE_LEFT]){
            player.move(-time);
            bullets[1].move(-time);
        }
        
        if (keys[SDL_SCANCODE_RIGHT]){
            player.move(time);
            bullets[1].move(time);
        }
        
        if (keys[SDL_SCANCODE_SPACE]) {
            player.shoot();
            
        } else if (player.inquiryShot()) {
            
            if (bullets[1].inBounds(normalizedBounds)) {
                bullets[1].move(time);
            } else {
                player.shoot(false);
                bullets[1].reset();
            }
            
        }
    }
    
    for (int i = 0; i < enemies.size(); i++) {
        for (int j = 0; j < enemies[i].size(); j++) {
            
            bool enemyValid = enemies[i][j].inBounds(normalizedBounds);
            
            if (enemyValid) {
                enemies[i][j].move(time);
            } else {
                enemies[i][j].affectV("x", 0);
            }
            
            if (player.inquiryShot()) {
                std::cout << bullets[1].collision(enemies[i][j]) << std::endl;
            }
            
        }
    }
    
}

void processEvents(SDL_Event &event, bool &done, bool &title) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        } else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
            title = false;
        }
    }
}

void render(ShaderProgram &program, std::vector<Matrix*> &matricies, int &score, bool title, GamePiece &player, const std::vector<GamePiece> &lives, GamePiece &UFO, std::vector<GamePiece> &bullets, const std::vector<std::vector<GamePiece>> &enemies, std::vector<Printer*> &printers, const std::vector<vec3*> &printerPos) {
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetProjectionMatrix(*matricies[0]);
    program.SetViewMatrix(*matricies[1]);
    
    if (title) {
        printers[0]->print(&program, *matricies[2], "Space Invaders", printerPos[0]);
        
        printers[1]->print(&program, *matricies[2], "Press Space to Fight!", printerPos[1]);
        
    } else {
        printers[2]->print(&program, *matricies[2], std::to_string(score), printerPos[2]);
        
        player.draw(program, *matricies[2]);
        
        
        for (int i = 0; i < player.lifeForce(); i++) {
            lives[i].draw(program, *matricies[2]);
        }
        
        UFO.draw(program, *matricies[2]);
        
        for (int i = 0; i < enemies.size(); i++) {
            for (int j = 0; j < enemies[i].size(); j++) {
                enemies[i][j].draw(program, *matricies[2]);
            }
        }
        
        if (player.inquiryShot()) {
            bullets[1].draw(program, *matricies[2]);
        }
        
    }
}

int main(int argc, char *argv[]) {
    //----------------Shader Programs----------------//
    ShaderProgram program;
    
    
    //----------MATRICIES----------------//
    Matrix projectionMatrix, viewMatrix, modelMatrix;
    std::vector<Matrix*> matricies { &projectionMatrix, &viewMatrix, &modelMatrix };
    
    //----------------SETUP----------------//
    vec3 bounds(720, 720);
    setUp(program, projectionMatrix, bounds, 8);
    
    vec3 zeroPos;
    vec3 zeroVel;
    
    //----------------TEXTURES----------------//
    GLuint fontTex = LoadTexture(RESOURCE_FOLDER"pixel_font.png");
    GLuint spriteSheetTexture = LoadTexture(RESOURCE_FOLDER"sheet.png");
    
    
    //----------------PRINTERS----------------//
    vec3 textDimen(16.0f, 16.0f);
    
    Printer titlePrinter(fontTex, textDimen, 0.7f, 0.0001f);
    Printer menuPrinter(fontTex, textDimen, 0.3f, 0.00001f);
    Printer scorePrinter(fontTex, textDimen, 0.5, 0.00001f);
    
    vec3 titlePos(-4.3f, 3.0f);
    vec3 menuPos(-3.0f);
    vec3 scorePos(-7.6f, 7.6f);
    
    std::vector<Printer*> printers { &titlePrinter, &menuPrinter, &scorePrinter };
    std::vector<vec3*> printerPos { &titlePos, &menuPos, &scorePos };
    
    //----------------SPRITES----------------//
    
    //Enemies
    vec3 enemyPos(-5.0f, -1.0f);
    vec3 enemyVel(2.0f);
    vec3 enemyPosUFO(0.0f, 5.5f);
    Sprite enemyShipUFOSkin(spriteSheetTexture, 505.0f/1024.0f, 898.0f/1024.0f, 91.0f/1024.0f, 91.0f/1024.0f, 0.8);
    GamePiece enemyShipUFO(&enemyShipUFOSkin, enemyPosUFO, enemyVel);
    
    Sprite enemyShipBlueSkin1(spriteSheetTexture, 425.0f/1024.0f, 468.0f/1024.0f, 92.7f/1024.0f, 84.0f/1024.0f, 0.8);
    Sprite enemyShipBlueSkin2(spriteSheetTexture, 222.5f/1024.0f, 0.0f/1024.0f, 102.4f/1024.0f, 84.0f/1024.0f, 0.8);
    Sprite enemyShipGreenSkin(spriteSheetTexture, 133.0f/1024.0f, 412.0f/1024.0f, 103.8f/1024.0f, 84.0f/1024.0f, 0.8);
    Sprite enemyShipRedSkin(spriteSheetTexture, 520.0f/1024.0f, 577.0f/1024.0f, 82.0f/1024.0f, 84.0f/1024.0f, 0.8);
    Sprite enemyShipBlackSkin(spriteSheetTexture, 346.0f/1024.0f, 150.0f/1024.0f, 97.0f/1024.0f, 84.0f/1024.0f, 0.8);
    
    //Player
    vec3 playerPos(0.0f, -6.5f);
    vec3 playerVel(8.0f);
    Sprite playerShipSkin(spriteSheetTexture, 224.0f/1024.0f, 832.0f/1024.0f, 99.0f/1024.0f, 75.0f/1024.0f, 1.0);
    GamePiece playerShip(&playerShipSkin, playerPos, playerVel, 3);
    
    //PlayerLives
    vec3 livesPos(7.6f, 7.65f);
    vec3 livesVel;
    Sprite playerLivesSkin(spriteSheetTexture, 775.0f/1024.0f, 301.0f/1024.0f, 33.0f/1024.0f, 26.0f/1024.0f, 0.5);
    std::vector<GamePiece> lives;
    
    for (int i = 0; i < playerShip.lifeForce(); i++) {
        GamePiece playerLives(&playerLivesSkin, livesPos, livesVel);
        lives.push_back(playerLives);
        livesPos.x -= 0.65;
    }
    
    std::vector<std::vector<GamePiece>> enemies;
    
    int enemyColAmt = 11; //Amount of enemies in column
    int enemyRowAmt = 6; //Amount of enemies in Row
    
    for (int i = 0; i < enemyColAmt; i++) {
        std::vector<GamePiece> enemyLine;
        float originalY = enemyPos.y;
        for (int j = 0; j < enemyRowAmt; j++) {
            GamePiece enemyShipBlue1(&enemyShipBlueSkin1, enemyPos, enemyVel);
            
            enemyPos.y += 1;
            GamePiece enemyShipBlue2(&enemyShipBlueSkin2, enemyPos, enemyVel);
            
            enemyPos.y += 1;
            GamePiece enemyShipGreen(&enemyShipGreenSkin, enemyPos, enemyVel);
            
            enemyPos.y += 1;
            GamePiece enemyShipBlack(&enemyShipBlackSkin, enemyPos, enemyVel);
            
            enemyPos.y += 1;
            GamePiece enemyShipRed(&enemyShipRedSkin, enemyPos, enemyVel);
            
            enemyLine.push_back(enemyShipBlue1);
            enemyLine.push_back(enemyShipBlue2);
            enemyLine.push_back(enemyShipGreen);
            enemyLine.push_back(enemyShipBlack);
            enemyLine.push_back(enemyShipRed);
            
            enemyPos.y = originalY;
        }
        enemyPos.x += 1;
        enemies.push_back(enemyLine);
    }
    
    //Projectiles
    vec3 projVel(0.0f,4.0f);
    
    Sprite playerProjSkin(spriteSheetTexture, 856.0f/1024.0f, 421.0f/1024.0f, 9.0f/1024.0f, 54.0f/1024.0f, 0.4);
    GamePiece playerProj(&playerProjSkin, playerPos, projVel);
    
    Sprite enemyProjSkin(spriteSheetTexture, 858.0f/1024.0f, 230.0f/1024.0f, 9.0f/1024.0f, 54.0f/1024.0f, 0.4);
    GamePiece enemyProj(&enemyProjSkin, zeroPos, projVel);
    
    std::vector<GamePiece> bullets {playerProj, enemyProj};
    
    /*
     
     <SubTexture name="laserBlue01.png" x="856" y="421" width="9" height="54"/>
     <SubTexture name="laserBlue02.png" x="841" y="647" width="13" height="37"/>
     <SubTexture name="laserBlue03.png" x="856" y="57" width="9" height="37"/>
     <SubTexture name="laserBlue04.png" x="835" y="565" width="13" height="37"/>
     <SubTexture name="laserBlue05.png" x="858" y="475" width="9" height="37"/>
     <SubTexture name="laserBlue06.png" x="835" y="752" width="13" height="37"/>
     <SubTexture name="laserBlue07.png" x="856" y="775" width="9" height="37"/>
     <SubTexture name="laserBlue08.png" x="596" y="961" width="48" height="46"/>
     <SubTexture name="laserBlue09.png" x="434" y="325" width="48" height="46"/>
     <SubTexture name="laserBlue10.png" x="740" y="724" width="37" height="37"/>
     <SubTexture name="laserBlue11.png" x="698" y="795" width="38" height="37"/>
     <SubTexture name="laserBlue12.png" x="835" y="695" width="13" height="57"/>
     <SubTexture name="laserBlue13.png" x="856" y="869" width="9" height="57"/>
     <SubTexture name="laserBlue14.png" x="842" y="206" width="13" height="57"/>
     <SubTexture name="laserBlue15.png" x="849" y="480" width="9" height="57"/>
     <SubTexture name="laserBlue16.png" x="843" y="62" width="13" height="54"/>
     
     <SubTexture name="laserRed01.png" x="858" y="230" width="9" height="54"/>
     <SubTexture name="laserRed02.png" x="843" y="977" width="13" height="37"/>
     <SubTexture name="laserRed03.png" x="856" y="602" width="9" height="37"/>
     <SubTexture name="laserRed04.png" x="843" y="940" width="13" height="37"/>
     <SubTexture name="laserRed05.png" x="856" y="983" width="9" height="37"/>
     <SubTexture name="laserRed06.png" x="843" y="903" width="13" height="37"/>
     <SubTexture name="laserRed07.png" x="856" y="131" width="9" height="37"/>
     <SubTexture name="laserRed08.png" x="580" y="661" width="48" height="46"/>
     <SubTexture name="laserRed09.png" x="602" y="600" width="48" height="46"/>
     <SubTexture name="laserRed10.png" x="738" y="650" width="37" height="36"/>
     <SubTexture name="laserRed11.png" x="737" y="613" width="37" height="37"/>
     <SubTexture name="laserRed12.png" x="843" y="846" width="13" height="57"/>
     <SubTexture name="laserRed13.png" x="856" y="812" width="9" height="57"/>
     <SubTexture name="laserRed14.png" x="843" y="789" width="13" height="57"/>
     <SubTexture name="laserRed15.png" x="856" y="926" width="9" height="57"/>
     <SubTexture name="laserRed16.png" x="848" y="684" width="13" height="54"/>
     */
    
    
    //----------------Game Mechanics----------------//
    int score = 0;
    bool title = true;
    
    float accumulator = 0.0f;
    float lastFrameTicks = 0.0f;
    
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
        
        processEvents(event, done, title);
        
        render(program, matricies, score, title, playerShip, lives, enemyShipUFO, bullets, enemies, printers, printerPos);
        
        if (!title) {
            //Time Keeping
            elapsed += accumulator; // get elapsed time
            
            if(elapsed < FIXED_TIMESTEP) {
                accumulator = elapsed;
                continue;
            }
            while(elapsed >= FIXED_TIMESTEP) {
                update(FIXED_TIMESTEP, score, bounds, playerShip, bullets, enemies);
                elapsed -= FIXED_TIMESTEP;
            }
            accumulator = elapsed;
        }
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
}
