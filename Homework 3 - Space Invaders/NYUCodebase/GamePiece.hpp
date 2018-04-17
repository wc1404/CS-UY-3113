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
#include "FloatVectors.hpp"
#include <SDL.h>
#include <SDL_opengl.h>
#include "Sprite.hpp"

class GamePiece {
public:
    GamePiece(vec3 thePos, vec3 theD, vec3 theVel);
    
    void draw(ShaderProgram &program, Matrix &personalMatrix) const;
    
    bool collision(const GamePiece &opp);
    
    void move(vec3 nextPos);
    
    bool overlap(float otherX, float otherY) const;
    
    float getXPos () const;
    
private:
    vec3 pos, vel, D;
    
    void setVertxs();
    
    float vertxs[12];
};

#endif /* GamePiece_hpp */
