//
//  Sprite.cpp
//  NYUCodebase
//
//  Created by Wilfredo Cueto on 4/16/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "Sprite.hpp"

Sprite::Sprite() {}

Sprite::Sprite(unsigned int textureID, float u, float v, float width, float height, float theSize): spriteTex(textureID), u(u), v(v), width(width), height(height), size(theSize) { setVertxs(); }

Sprite::Sprite(unsigned int textureID, vec3 texPos, vec3 texDimen, float theSize):spriteTex(textureID), u(texPos.x), v(texPos.y), width(texDimen.x), height(texDimen.y), size(theSize) { setVertxs(); }

void Sprite::draw(ShaderProgram* program) const {
    
    glBindTexture(GL_TEXTURE_2D, spriteTex);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

vec3 Sprite::getDimen() const {
    return trueDimen;
}

void Sprite::setVertxs() {
    
    GLfloat textCoords[] = {
        u, v + height,
        u + width, v,
        u, v,
        u + width, v,
        u, v + height,
        u + width, v + height
    };
    
    float aspect = width / height;
    
    float vertxs[] = {
        -0.5f * size * aspect, -0.5f * size, //Bottom Left
         0.5f * size * aspect,  0.5f * size, //Top Right
        -0.5f * size * aspect,  0.5f * size, //Top Left
        
         0.5f * size * aspect,  0.5f * size, //Top Right
        -0.5f * size * aspect, -0.5f * size, //Bottom Left
         0.5f * size * aspect, -0.5f * size  //Bottom Right
    };
    
    for(int i = 0; i < 12; i++) {
        vertices[i] = vertxs[i];
    }
    
    //set height and width
    trueDimen.x = fabs(vertices[0] * 2);
    trueDimen.y = fabs(vertices[1] * 2);
    
    for(int i = 0; i < 12; i++) {
        texCoords[i] = textCoords[i];
    }
}
