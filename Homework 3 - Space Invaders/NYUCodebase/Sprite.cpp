//
//  Sprite.cpp
//  NYUCodebase
//
//  Created by Wilfredo Cueto on 4/16/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "Sprite.hpp"

Sprite::Sprite(unsigned int textureID, float u, float v, float width, float height, float theSize): pos(u,v), dimen(width, height), size(theSize) {}

void Sprite::drawSprite(ShaderProgram &program, Matrix personalMatrix) {
    
    personalMatrix.Identity();
    program.SetModelMatrix(personalMatrix);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertxs);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
}

void Sprite::setVertxs() {
    
    float aspect = dimen.x / dimen.y;
    
    float vertices[] = {
        -0.5f * size * aspect, -0.5f * size,
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size * aspect, 0.5f * size,
        
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size * aspect, -0.5f * size ,
        0.5f * size * aspect, -0.5f * size
    };
    
    memcpy(vertices, vertxs, sizeof(float) * 12);
}

void Sprite::setTexCoords() {
    
    GLfloat rawTexCoords[] = {
        pos.x, (pos.y + dimen.y),
        (pos.x + dimen.x), pos.y,
        pos.x, pos.y,
        
        (pos.x + dimen.x), pos.y,
        pos.x, (pos.y + dimen.y),
        (pos.x + dimen.x), (pos.y + dimen.y)
    };
    
    memcpy(texCoords, rawTexCoords, sizeof(float) * 12);
}
