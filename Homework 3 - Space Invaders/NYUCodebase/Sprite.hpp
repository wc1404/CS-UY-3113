//
//  Sprite.hpp
//  NYUCodebase
//
//  Created by Wilfredo Cueto on 4/16/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#ifndef Sprite_hpp
#define Sprite_hpp

#include <stdio.h>
#include <math.h>
#include "ShaderProgram.h"
#include "FloatVectors.hpp"
#include "Matrix.h"

class Sprite {
public:
    Sprite();
    
    Sprite(unsigned int textureID, float u, float v, float width, float height, float theSize);
    
    Sprite(unsigned int textureID, vec3 texPos, vec3 texDimen, float theSize);
    
    void draw(ShaderProgram *program) const;
    
    //Pass in/calculate true width/height in sprite
    vec3 getDimen() const;
    
private:
    unsigned int spriteTex;
    
    Matrix personalMatrix;
    
    float u;
    float v;
    
    float width;
    float height;
    
    float texCoords[12];
    float vertices[12];
    
    float size;
    vec3 trueDimen;
    
    void setVertxs();
    
};

#endif /* Sprite_hpp */
