//
//  GamePiece.cpp
//  NYUCodebase
//
//  Created by Wilfredo Cueto on 2/22/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "GamePiece.hpp"

GamePiece::GamePiece(vec3 thePos, vec3 theD, vec3 theVel): pos(thePos), D(theD), vel(theVel) {
    setVertxs();
}

//Render the Piece with Textures
void GamePiece::draw(ShaderProgram &program, Matrix &personalMatrix) const {
    
    personalMatrix.Identity();
    program.SetModelMatrix(personalMatrix);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertxs);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

//Collision Logic
bool GamePiece::collision(const GamePiece &opp) {
    if ((opp.pos.x >= vertxs[0]/2 && opp.pos.x <= vertxs[4]/2) && (opp.pos.y <= vertxs[1]/2 && opp.pos.y >= vertxs[5]/2)) {
        return true;
    }
    return false;
}

//Move Pieces
void GamePiece::move(vec3 nexPos) {
    //Horizontal Update
    pos = nexPos;
    
    //Move Piece
    setVertxs();
}

bool GamePiece::overlap(float otherX, float otherY) const {
    if ((otherX >= vertxs[0]/2 && otherX <= vertxs[4]/2) && (otherY <= vertxs[1]/2 && otherY >= vertxs[5]/2)) {
        return true;
    }
    return false;
}

//Set Verticies for Piece
void GamePiece::setVertxs() {
    float inVertxs[12] =
    {
        //Top Triangle
        (pos.x + (D.x/2)), (pos.y + (D.y/2)), //Top Left
        (pos.x + (D.x/2)), (pos.y + (D.y/2)), //Top Right
        (pos.x + (D.x/2)), (pos.y - (D.y/2)), //Bottom Right
        
        //Bottom Triangle
        (pos.x - (D.x/2)), (pos.y + (D.y/2)), //Top Left
        (pos.x + (D.x/2)), (pos.y - (D.y/2)), //Bottom Right
        (pos.x - (D.x/2)), (pos.y - (D.y/2)) //Bottom Left
    };
    
    memcpy(vertxs, inVertxs, sizeof(float) * 12);
}
