//
//  GamePiece.cpp
//  NYUCodebase
//
//  Created by Wilfredo Cueto on 2/22/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "GamePiece.hpp"

GamePiece::GamePiece(const Sprite* const theSkin, vec3 thePos, vec3 theVel, int livesAmt): skin(theSkin), pos(thePos), originalPos(thePos), vel(theVel), originalLives(livesAmt), lives(livesAmt) {
    D.x = skin->getDimen().x;
    D.y = skin->getDimen().y;
}

//Render the Piece with Textures
void GamePiece::draw(ShaderProgram &program, Matrix &personalMatrix) const {
    if (!dead) {
        personalMatrix.Identity();
        personalMatrix.Translate(pos.x, pos.y, pos.z);
        
        program.SetModelMatrix(personalMatrix);
        
        skin->draw(&program);
    }
}

//Collision Logic
bool GamePiece::collision(GamePiece &opp, bool harmless) {
    if (!dead) {
        if (( ((pos.x + D.x/2) >= (opp.pos.x - opp.D.x/2)) && ((pos.x - D.x/2) <= (opp.pos.x + opp.D.x/2)) ) && ( ((pos.y + D.y/2) >= (opp.pos.y - opp.D.y/2)) && ((pos.y - D.y/2) <= (opp.pos.y + opp.D.y/2)) )) {
            
            if (!harmless) {
                lifeForce(1);
                opp.lifeForce(1);
            }
            
            return true;
        }
    }
    return false;
}

//Check if the GamePiece is in the screen
bool GamePiece::inBounds(const vec3 &bound) {
    if (dead) { return false; }
    
    if ((pos.x + D.x/2) > bound.x) {
        pos.x = bound.x - D.x/2;
        return false;
    }
    
    if ((pos.x - D.x/2) < -bound.x) {
        pos.x = D.x/2 - bound.x;
        return false;
    }
    
    if ((pos.y + D.y/2) > bound.y) {
        pos.y -= bound.y - D.y/2;
        return false;
    }
    
    if ((pos.y - D.y/2) < -bound.y) {
        pos.y += D.y/2 - bound.y;
        return false;
    }
    return true;
}

//Move Pieces
void GamePiece::move(float elapsed) {
    pos.x += vel.x * elapsed;
    pos.y += vel.y * elapsed;
}

void GamePiece::shoot(bool didShoot) {
    shot = didShoot;
}

bool GamePiece::inquiryShot() const {
    return shot;
}

void GamePiece::affectV(const std::string &axis, float affect) {
    
    if (axis == "x") {
        
        if (affect == 0) {
            vel.x = -vel.x;
        } else {
            vel.x -= affect;
        }
        
    } else if (axis == "y") {
        
        if (affect == 0) {
            vel.y = -vel.y;
        } else {
            vel.y -= affect;
        }
        
    } else if (axis == "z") {
        
        if (affect == 0) {
            vel.z = -vel.z;
        } else {
            vel.z -= affect;
        }
        
    }
    
}

//Return and Affect the life of a GamePiece
int GamePiece::lifeForce(int affectLife) {
    if (affectLife) {
        lives -= affectLife;
        if (lives <= 0) {
            dead = true;
        }
    }
    return lives;
}

vec3 GamePiece::getPos() const {
    return pos;
}

void GamePiece::reset() {
    pos = originalPos;
    lives = originalLives;
}
