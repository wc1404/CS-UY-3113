//
//  GamePiece.cpp
//  NYUCodebase
//
//  Created by Wilfredo Cueto on 2/22/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "GamePiece.hpp"

GamePiece::GamePiece(float posX, float posY, float theWidth, float theHeight, float velX, float velY): width(theWidth), height(theHeight), xVel(velX), yVel(velY), xPos(posX), yPos(posY) {
    setVertxs();
}

//Render the Piece
void GamePiece::Draw(ShaderProgram &program, Matrix &personalMatrix) const {
    
    personalMatrix.Identity();
    personalMatrix.Translate(xPos, yPos, 0.0f);
    program.SetModelMatrix(personalMatrix);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertxs);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
}

//Collision Logic
bool GamePiece::collision(const GamePiece &opp) {
    if ((opp.xPos >= vertxs[0]/2 && opp.xPos <= vertxs[4]/2) && (opp.yPos <= vertxs[1]/2 && opp.yPos >= vertxs[5]/2)) {
        return true;
    }
    return false;
}

//Move Pieces
void GamePiece::move(float nextPosX, float nextPosY) {
    //Horizontal Update
    xPos = nextPosX;
    
    //Vertical Update
    yPos = nextPosY;
    
    //Move Piece
    setVertxs();
}

bool GamePiece::overlap(float otherX, float otherY) const {
    if ((otherX >= vertxs[0]/2 && otherX <= vertxs[4]/2) && (otherY <= vertxs[1]/2 && otherY >= vertxs[5]/2)) {
        return true;
    }
    return false;
}

float GamePiece::getXPos () const {
    return vertxs[0];
}

//Set Verticies for Piece
void GamePiece::setVertxs() {
    float inVertxs[12] =
        {
        //Top Triangle
        (xPos - (width/2)), (yPos + (height/2)), //Top Left
        (xPos + (width/2)), (yPos + (height/2)), //Top Right
        (xPos + (width/2)),  (yPos - (height/2)), //Bottom Right
        
        //Bottom Triangle
        (xPos - (width/2)), (yPos + (height/2)), //Top Left
        (xPos + (width/2)), (yPos - (height/2)), //Bottom Right
        (xPos - (width/2)), (yPos - (height/2)) //Bottom Left
        };
    
    memcpy(vertxs, inVertxs, sizeof(float) * 12);
}

