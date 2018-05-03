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
    GamePiece(const Sprite* const theSkin, vec3 thePos, vec3 theVel, int livesAmt = 1);
    
    void draw(ShaderProgram &program, Matrix &personalMatrix) const;
    
    bool collision(GamePiece &opp, bool harmless = false);
    
    bool inBounds(const vec3 &bound);
    
    void move(float elapsed);
    
    void shoot(bool didShoot = true);
    
    bool inquiryShot() const;
    
    void affectV(const std::string &axis, float affect);
    
    int lifeForce(int affectLife = 0);
    
    vec3 getPos() const;
    
    void reset();
    
private:
    vec3 pos, originalPos, vel, D;
    
    const Sprite* const skin;
    
    bool shot = false;
    
    int lives, originalLives;
    bool dead = false;
};

#endif /* GamePiece_hpp */
