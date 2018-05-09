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
#include <SDL_mixer.h>

// 60 FPS (1.0f/60.0f) (update sixty times a second)
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

SDL_Window* displayWindow;

//Game States
enum GameMode {MENU, LEVEL, GAME_OVER};

//Texture Loader
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

//Setup for the screen and Shader Program
void setUp(ShaderProgram &program, Matrix &projectionMatrix, const vec3 &bound, int scaleFactor = 1) {
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
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

//Resets the board
void resetBoard(std::vector<GamePiece> &bullets, GamePiece &player, GamePiece &UFO, std::vector<std::vector<GamePiece>> &enemies, int &enemyAmount, int &score) {
    
    UFO.reset();
    
    //Reset All Bullets
    for (int i = 0; i < bullets.size(); i++) {
        bullets[i].reset();
    }
    
    //if the player died then reset the player and score, if not then add a life to the player and reset the enemy amount
    if (player.dead) {
        player.reset();
    } else {
        player.lives += 1;
        enemyAmount = 60;
    }
    
    //Reset all enemies
    for (int i = 0; i < enemies.size(); i++) {
        for (int j = 0; j < enemies[i].size(); j++) {
            enemies[i][j].reset();
        }
    }
    
}

//Updates GamePieces when in Play
void updateBoard(float time, int &score, std::vector<GamePiece> &bullets, GamePiece &player, GamePiece &UFO, std::vector<std::vector<GamePiece>> &enemies, int &enemyAmount, std::vector<Mix_Chunk*> sounds) {
    
    //------Scan for Player Movement------//
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    //Move Player Right
    if (keys[SDL_SCANCODE_RIGHT]) {
        if (player.pos.x >= 8 - (player.D.x/2)) {
            player.pos.x = 8 - (player.D.x/2);
        } else {
            player.move(time);
        }
    }
    
    //Move player Left
    if (keys[SDL_SCANCODE_LEFT]) {
        if (player.pos.x <= -8 + player.D.x/2) {
            player.pos.x = -8 + player.D.x/2;
        } else {
            player.move(-time);
        }
    }
    
    //If the player shot the bullet then move the bullet
    if (!bullets[0].dead) {
        bool playerBulletInBounds = (bullets[0].pos.y - bullets[0].D.y/2) > -8 && (bullets[0].pos.y + bullets[0].D.y/2) < 8;
        if (playerBulletInBounds) {
            bullets[0].move(time);
            
        } else {
            bullets[0].dead = true;
        }
    }
    
    //If all the enemies are killed then reset the game
    if (enemyAmount == 0) {
        resetBoard(bullets, player, UFO, enemies, enemyAmount, score);
    }
    
    //-------------UFO Update Logic
    
    //Generate random numbers to randomize UFO appearance
    int launchUFO = rand() % 800;
    int target = rand() % 400;
    
    //Random number reached a random target then launch the UFO
    if (launchUFO == target && UFO.dead) {
        UFO.dead = false;
    }
    
    //If the UFO is not dead then move within bounds
    if (!UFO.dead) {
        bool UFOInBounds = (UFO.pos.x - UFO.D.x/2) > -8 && (UFO.pos.x + UFO.D.x/2) < 8;

        if (UFOInBounds) {
            UFO.move(-time);
            Mix_PlayChannel(-1, sounds[3], 0);
            
        } else { //If UFO moves offscreen then kill it
            UFO.dead = true;
            UFO.reset();
        }
        
        //if the player bullet has hit the UFO then update score, play sound and kill both
        if (bullets[0].collision(UFO)) {
            bullets[0].dead = true;
            UFO.dead = true;
            score += 50;
            Mix_PlayChannel(-1, sounds[1], 0);
        }

    }
    
    //-------------Enemy Update Logic
    for (int i = 0; i < enemies.size(); i++) {
        for (int j = 0; j < enemies[i].size(); j++) {
            
            if (!enemies[i][j].dead) {
                
                //Check if the enemies are within the screen bounds
                if ((enemies[i][j].pos.x < -8 + (enemies[i][j].D.x/2)) || (enemies[i][j].pos.x > 8 - (enemies[i][j].D.x/2))) {
                    float penetration;
                    
                    //Find if there is another enemy that has penetrated the screen further than the one caught first and set penetration according to it
                    for (int k = 0; k < enemies[i].size(); k++) {
                        if (enemies[i][j].vel.x > 0) {
                            if (enemies[i][k].pos.x + enemies[i][k].D.x/2 > enemies[i][j].pos.x + enemies[i][j].D.x/2) {
                                j = k;
                            }
                        } else {
                            if (enemies[i][k].pos.x - enemies[i][k].D.x/2 < enemies[i][j].pos.x - enemies[i][j].D.x/2) {
                                j = k;
                            }
                        }
                        
                    }
                    
                    //Adjust the enemies into their positions
                    if (enemies[i][j].vel.x > 0) {
                        penetration = (enemies[i][j].pos.x + enemies[i][j].D.x/2) - 8;
                    } else {
                        penetration = -(enemies[i][j].pos.x - enemies[i][j].D.x/2) - 8;
                    }
                    
                    //enemy is out of bounds shift and reverse all enemies as well as move them down and increase the vel slightly
                    for (int i = 0; i < enemies.size(); i++) {
                        for (int j = 0; j < enemies[i].size(); j++) {
                            
                            if (enemies[i][j].vel.x > 0) {
                                enemies[i][j].pos.x -= (penetration);
                                enemies[i][j].vel.x = -(enemies[i][j].vel.x + 0.12);
                                
                            } else if (enemies[i][j].vel.x < 0) {
                                enemies[i][j].pos.x += (penetration);
                                enemies[i][j].vel.x = -(enemies[i][j].vel.x - 0.12);
                            }
                            
                            enemies[i][j].pos.y = enemies[i][j].pos.y - 0.05;
                        
                        }
                    }
                    
                } else { //If enemies are in screen move them
                    enemies[i][j].move(time);
                    
                    //if the players bullet has collided with an enemy kill both and make hit sound
                    if (bullets[0].collision(enemies[i][j])) {
                        bullets[0].dead = true;
                        enemies[i][j].dead = true;
                        score += 10;
                        enemyAmount -= 1;
                        
                        Mix_PlayChannel(-1, sounds[1], 0);
                    }
                    
                    //Generate random number to randoize shots
                    int shootBack = rand() % 500;
                    
                    //if the bullet can be shot and the random number hits this random target then shoot with a sound
                    if (shootBack == target && bullets[1].dead){
                        bullets[1].dead = false;
                        bullets[1].pos.x = enemies[i][j].pos.x;
                        bullets[1].pos.y = (enemies[i][j].pos.y - enemies[i][j].D.y);
                        
                        Mix_PlayChannel(-1, sounds[0], 0);
                        
                    }
                    
                    //if the enemy has reached the player then the game is over
                    if ((enemies[i][j].pos.y < (player.pos.y + player.D.y)) && (!enemies[i][j].dead)) {
                        player.dead = true;
                    }
                    
                }
                
            }
            
        }
    }
    
    //if the enemy shot a bullet then move the bullet and check for collision with player
    if (!bullets[1].dead) {
        bool enemyBulletInBounds = (bullets[1].pos.y - bullets[1].D.y/2) > -8 && (bullets[1].pos.y + bullets[1].D.y/2) < 8;
        
        //if the bullet is in the screen then move it, if not kill it
        if (enemyBulletInBounds) {
            bullets[1].move(time);
            
            //if the bullet collides then kill the bullet and the players lives are reduced and play a hit sound
            if (bullets[1].collision(player)) {
                bullets[1].dead = true;
                player.lives -= 1;
                Mix_PlayChannel(-1, sounds[2], 0);
                
                if (!player.lives) {
                    player.dead = true;
                }
            }

        } else {
            bullets[1].dead = true;
        }
    }
    
}

void processEvents(SDL_Event &event, bool &done, int &score, GameMode &mode, GamePiece &player, std::vector<GamePiece> &bullets, const std::vector<Mix_Chunk*> &sounds) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
            
        //if space is pressed and you are in the Game and the bullet can be shot then shoot a bullet and play a shot sound
        } else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && bullets[0].dead && mode == LEVEL) {
            bullets[0].dead = false;
            bullets[0].pos.x = player.pos.x;
            bullets[0].pos.y = (player.pos.y + player.D.y);
            Mix_PlayChannel(-1, sounds[0], 0);
        
        //if Space is pressed and the game is over or you are at the menu, then reset and go back to the game fresh
        } else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && (mode == MENU || mode == GAME_OVER)) {
            mode = LEVEL;
            player.reset();
            score = 0;
        }
    }
    
    if (player.dead) {
        mode = GAME_OVER;
    }
    
}

//Render Game Screen
void renderGame(ShaderProgram &program, std::vector<Matrix*> &matricies, int &score, std::vector<GamePiece> &bullets, GamePiece &player, const std::vector<GamePiece> &lives, GamePiece &UFO,  const std::vector<std::vector<GamePiece>> &enemies, std::vector<Printer*> &printers, const std::vector<vec3*> &printerPos) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetProjectionMatrix(*matricies[0]);
    program.SetViewMatrix(*matricies[1]);
    
    //Print Score
    printers[2]->print(&program, *matricies[2], std::to_string(score), printerPos[2]);
    
    //Rener player
    player.draw(program, *matricies[2]);
    
    //Render Bullets
    bullets[0].draw(program, *matricies[2]);
    bullets[1].draw(program, *matricies[2]);
    
    //Render Player Lives
    for (int i = 0; i < player.lives; i++) {
        lives[i].draw(program, *matricies[2]);
    }
    
    UFO.draw(program, *matricies[2]);
    
    //Render Enemies
    for (int i = 0; i < enemies.size(); i++) {
        for (int j = 0; j < enemies[i].size(); j++) {
            enemies[i][j].draw(program, *matricies[2]);
        }
    }
    
}

//Render Game Over Screen
void renderGameOver(ShaderProgram &program, std::vector<Matrix*> &matricies, int &score, GamePiece &player, std::vector<Printer*> &printers) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetProjectionMatrix(*matricies[0]);
    program.SetViewMatrix(*matricies[1]);
    
    vec3 gameOverPos(-3.0f, 3.0f);
    vec3 finalScorePos(-3.4f, -0.8f);
    vec3 menuPos(-4.0f, -4.0f);
    
    //Print Game over Status
    printers[0]->print(&program, *matricies[2], "GAME OVER", &gameOverPos);
    
    //Print Ressurecton Menu
    printers[1]->print(&program, *matricies[2], "Press [Space] to Rise Again!", &menuPos);
    
    //Print Final Score
    printers[2]->print(&program, *matricies[2], "Final Score: " + std::to_string(score), &finalScorePos);
    
    player.dead = false;
    player.draw(program, *matricies[2]);
    player.dead = true;
    
}

void renderTitle(ShaderProgram &program, std::vector<Matrix*> &matricies, std::vector<Printer*> &printers, const std::vector<vec3*> &printerPos) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetProjectionMatrix(*matricies[0]);
    program.SetViewMatrix(*matricies[1]);
    
    //Print Title
    printers[0]->print(&program, *matricies[2], "Space Invaders", printerPos[0]);
    
    //Print Menu Item
    printers[1]->print(&program, *matricies[2], "Press [Space] to Fight!", printerPos[1]);
    
}

int main(int argc, char *argv[]) {
    //----------------Shader Programs----------------//
    ShaderProgram program;
    
    //----------------MATRICIES----------------//
    Matrix projectionMatrix, viewMatrix, modelMatrix;
    std::vector<Matrix*> matricies { &projectionMatrix, &viewMatrix, &modelMatrix };
    
    //----------------SETUP----------------//
    vec3 bounds(720, 720);
    setUp(program, projectionMatrix, bounds, 8);
    
    //Zero Vectors
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
    vec3 menuPos(-3.2f);
    vec3 scorePos(-7.6f, 7.6f);
    
    std::vector<Printer*> printers { &titlePrinter, &menuPrinter, &scorePrinter };
    std::vector<vec3*> printerPos { &titlePos, &menuPos, &scorePos };
    
    //----------------SPRITES----------------//
    
    //-----Player
    vec3 playerPos(0.0f, -6.5f);
    vec3 playerVel(8.0f);
    Sprite playerShipSkin(spriteSheetTexture, 224.0f/1024.0f, 832.0f/1024.0f, 99.0f/1024.0f, 75.0f/1024.0f, 1.0);
    GamePiece playerShip(&playerShipSkin, playerPos, playerVel, 3);
    
    //-----PlayerLives
    vec3 livesPos(7.6f, 7.65f);
    Sprite playerLivesSkin(spriteSheetTexture, 775.0f/1024.0f, 301.0f/1024.0f, 33.0f/1024.0f, 26.0f/1024.0f, 0.5);
    
    std::vector<GamePiece> lives;
    
    int originalLives = playerShip.lives;
    
    for (int i = 0; i < playerShip.lives; i++) {
        GamePiece playerLives(&playerLivesSkin, livesPos, zeroVel);
        lives.push_back(playerLives);
        livesPos.x -= 0.65;
    }
    
    livesPos.x = 7.6f;
    
    //----Enemies
    vec3 enemyPos(-5.0f, -1.0f);
    vec3 enemyVel(2.0f);
    vec3 enemyPosUFO(7.5f, 5.5f);
    Sprite enemyShipUFOSkin(spriteSheetTexture, 505.0f/1024.0f, 898.0f/1024.0f, 91.0f/1024.0f, 91.0f/1024.0f, 0.8);
    GamePiece enemyShipUFO(&enemyShipUFOSkin, enemyPosUFO, enemyVel, 0);
    
    Sprite enemyShipBlueSkin1(spriteSheetTexture, 425.0f/1024.0f, 468.0f/1024.0f, 92.7f/1024.0f, 84.0f/1024.0f, 0.8);
    Sprite enemyShipBlueSkin2(spriteSheetTexture, 222.5f/1024.0f, 0.0f/1024.0f, 102.4f/1024.0f, 84.0f/1024.0f, 0.8);
    Sprite enemyShipGreenSkin(spriteSheetTexture, 133.0f/1024.0f, 412.0f/1024.0f, 103.8f/1024.0f, 84.0f/1024.0f, 0.8);
    Sprite enemyShipRedSkin(spriteSheetTexture, 520.0f/1024.0f, 577.0f/1024.0f, 82.0f/1024.0f, 84.0f/1024.0f, 0.8);
    Sprite enemyShipBlackSkin(spriteSheetTexture, 346.0f/1024.0f, 150.0f/1024.0f, 97.0f/1024.0f, 84.0f/1024.0f, 0.8);
    
    std::vector<std::vector<GamePiece>> enemies;
    
    int enemyRowAmt = 10; //Amount of enemies in Row
    
    for (int i = 0; i < enemyRowAmt; i++) {
        std::vector<GamePiece> enemyLine;
        
        float originalY = enemyPos.y;
        
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
        
        enemyPos.x += 1;
        enemies.push_back(enemyLine);
    }
    
    //-----Projectiles
    vec3 playerProjVel(0.0f,5.0f);
    vec3 enemyProjVel(0.0f,-5.0f);
    
    Sprite playerProjSkin(spriteSheetTexture, 858.0f/1024.0f, 230.0f/1024.0f, 9.0f/1024.0f, 54.0f/1024.0f, 0.9);
    GamePiece playerProj(&playerProjSkin, zeroPos, playerProjVel, 0);
    
    Sprite enemyProjSkin(spriteSheetTexture, 856.0f/1024.0f, 421.0f/1024.0f, 9.0f/1024.0f, 54.0f/1024.0f, 0.4);
    GamePiece enemyProj(&enemyProjSkin, zeroPos, enemyProjVel, 0);
    
    std::vector<GamePiece> bullets {playerProj, enemyProj};
    
    //----------------Game Mechanics----------------//
    int score = 0;
    int enemyAmount = 60;
    
    GameMode mode = MENU;
    
    float accumulator = 0.0f;
    float lastFrameTicks = 0.0f;
    
    //----------------Sounds----------------//
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    Mix_Chunk* shotSound;
    Mix_Chunk* hitSound;
    Mix_Chunk* hurtSound;
    Mix_Chunk* UFOSound;
    
    shotSound = Mix_LoadWAV("shot.wav");
    hitSound = Mix_LoadWAV("hit.wav");
    hurtSound = Mix_LoadWAV("hurt.wav");
    UFOSound = Mix_LoadWAV("UFO.wav");
    
    std::vector<Mix_Chunk*> sounds {shotSound, hitSound, hurtSound, UFOSound};
    
    //----------------Music----------------//
    Mix_Music *music;
    
    music = Mix_LoadMUS("Musica.mp3");
    
    Mix_PlayMusic(music, -1);
    
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
        
        if (true) {
            
            //Time Keeping
            elapsed += accumulator;
            if(elapsed < FIXED_TIMESTEP) {
                accumulator = elapsed;
                continue;
            }
            
            //Ensure Proper TimeStep
            while(elapsed >= FIXED_TIMESTEP) {
                
                if (originalLives != playerShip.lives) {
                    lives.clear();
                    
                    for (int i = 0; i < playerShip.lives; i++) {
                        GamePiece playerLives(&playerLivesSkin, livesPos, zeroVel);
                        lives.push_back(playerLives);
                        livesPos.x -= 0.65;
                    }
                    
                    livesPos.x = 7.6f;
                    
                    originalLives = playerShip.lives;
                }
                
                processEvents(event, done, score, mode, playerShip, bullets, sounds);
                
                //Game Mode Switch
                switch (mode) {
                    case MENU:
                        renderTitle(program, matricies, printers, printerPos);
                        break;
                        
                    case LEVEL:
                        updateBoard(FIXED_TIMESTEP, score, bullets, playerShip, enemyShipUFO, enemies, enemyAmount, sounds);
                        renderGame(program, matricies, score, bullets, playerShip, lives, enemyShipUFO, enemies, printers, printerPos);
                        break;
                        
                    case GAME_OVER:
                        renderGameOver(program, matricies, score, playerShip, printers);
                        resetBoard(bullets, playerShip, enemyShipUFO, enemies, enemyAmount, score);
                        break;
                        
                    default:
                        break;
                }
                
                elapsed -= FIXED_TIMESTEP;
            }
            
            accumulator = elapsed;
            
        }
        SDL_GL_SwapWindow(displayWindow);
    }
    
    //Free Space
    Mix_FreeChunk(shotSound);
    Mix_FreeChunk(hitSound);
    Mix_FreeChunk(hurtSound);
    Mix_FreeChunk(UFOSound);
    Mix_FreeMusic(music);
    SDL_Quit();
}
