//
//  GamePiece.cpp
//  NYUCodebase
//
//  Created by Wilfredo Cueto on 2/22/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "GamePiece.hpp"

GamePiece::GamePiece(const Sprite* const theSkin, vec3 thePos, vec3 theVel, int livesAmt): skin(theSkin), pos(thePos), originalPos(thePos), originalLives(livesAmt), originalVel(theVel), lives(livesAmt) {
    D.x = skin->getDimen().x;
    D.y = skin->getDimen().y;
    
    vel = theVel;
    
    if (!lives) {
        dead = true;
    }
    
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
bool GamePiece::collision(GamePiece &opp) {
    if (!dead) {
        if (( ((pos.x + D.x/2) >= (opp.pos.x - opp.D.x/2)) && ((pos.x - D.x/2) <= (opp.pos.x + opp.D.x/2)) ) && ( ((pos.y + D.y/2) >= (opp.pos.y - opp.D.y/2)) && ((pos.y - D.y/2) <= (opp.pos.y + opp.D.y/2)) )) {
            return true;
        }
    }
    return false;
}

//Move Pieces
void GamePiece::move(float elapsed) {
    pos.x += vel.x * elapsed;
    pos.y += vel.y * elapsed;
}

void GamePiece::reset() {
    pos = originalPos;
    lives = originalLives;
    vel = originalVel;
    if (!lives) {
        dead = true;
    } else {
        dead = false;
    }
}
