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
    program.SetModelMatrix(personalMatrix);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertxs);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
}

//Collision Logic
bool GamePiece::colRight(const GamePiece &opp) {
    //Right Collision
    if ((xPos + width) > opp.xPos && (yPos <= opp.yPos && (yPos-height) >= (opp.yPos - opp.height))) {
        xVel = -xVel;
        increaseVel();
        return true;
    }
    return false;
}

bool GamePiece::colLeft(const GamePiece &opp) {
    //Left Collision
    if (xPos < (opp.xPos + opp.width) && (yPos <= opp.yPos && (yPos-height) >= (opp.yPos - opp.height))) {
        xVel = -xVel;
        increaseVel();
        return true;
    }
    return false;
}

bool GamePiece::colUp(const GamePiece &opp) {
    //Upper Collision
    if (yPos > (opp.yPos - opp.height)) {
        yVel = -yVel;
        increaseVel();
        return true;
    }
    return false;
}

bool GamePiece::colDown(const GamePiece &opp) {
    //Down Collision
    if ((yPos - height) < opp.yPos) {
        yVel = -yVel;
        increaseVel();
        return true;
    }
    return false;
}

//Paddle Colision Logic Funtions
bool GamePiece::padColUp(const GamePiece &opp) {
    //Upper Collision
    if (yPos > (opp.yPos - opp.height)) {
        return true;
    }
    return false;
}

bool GamePiece::padColDown(const GamePiece &opp) {
    //Down Collision
    if ((yPos - height) < opp.yPos) {
        return true;
    }
    return false;
}

//Reset Piece
void GamePiece::reset(bool negX) {
    
    if (negX) {
        xVel = -1.85f;
    } else {
        xVel = 1.85f;
    }
    
    yVel = 1.75f;
    xPos = 0;
    yPos = 0;
}

//Move Pieces
void GamePiece::move(float elapsed, bool down) {
    
    xPos += (elapsed * xVel); //Horizontal Update
    //Vertical Update
    if (down) {
        yPos += (elapsed * -yVel);
    } else {
        yPos += (elapsed * yVel);
    }
    
    //Move Piece
    setVertxs();
}

//Win Query
float GamePiece::getXPos() const {
    return xPos;
}

//Set Verticies for Piece
void GamePiece::setVertxs() {
    float inVertxs[12] =
        {
        xPos, yPos,
        (xPos + width), yPos,
        (xPos + width),  (yPos - height),
        
        xPos, yPos,
        (xPos + width), (yPos - height),
        xPos, (yPos - height)
        };
    
    memcpy(vertxs, inVertxs, sizeof(float) * 12);
}

//Speed Up or slow down object
void GamePiece::increaseVel() {
    xVel += 0.07f;
    yVel += 0.07f;
}

