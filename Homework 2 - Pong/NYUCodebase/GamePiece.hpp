//
//  GamePiece.hpp
//  NYUCodebase
//
//  Created by Wilfredo Cueto on 2/22/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#ifndef GamePiece_hpp
#define GamePiece_hpp

#include "ShaderProgram.h"
#include "Matrix.h"
#include <SDL.h>
#include <SDL_opengl.h>

class GamePiece {
public:
    GamePiece(float posX, float posY, float theWidth, float theHeight, float velX = 0.0f, float velY = 0.0f);
    
    void Draw(ShaderProgram &program, Matrix &personalMatrix) const;
    
    bool colRight(const GamePiece &opp);
    
    bool colLeft(const GamePiece &opp);
    
    bool colUp(const GamePiece &opp);
    
    bool colDown(const GamePiece &opp);
    
    bool padColUp(const GamePiece &opp);
    
    bool padColDown(const GamePiece &opp);
    
    void move(float elapsed, bool down = false);
    
    void reset(bool negX = false);
    
    float getXPos() const;
    
private:
    float xVel;
    float yVel;
    
    float xPos;
    float yPos;
    
    const float width;
    const float height;
    
    void setVertxs();
    
    void increaseVel();
    
    float vertxs[12];
};

#endif /* GamePiece_hpp */
