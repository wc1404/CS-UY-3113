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
#include "ShaderProgram.h"
#include "FloatVectors.hpp"

class Sprite {
public:
    Sprite(unsigned int textureID, float u, float v, float width, float height, float size);
    
    void drawSprite(ShaderProgram &program, Matrix personalMatrix);
    
private:
    vec3 pos, dimen;
    
    float size;
    unsigned int textureID;
    
    float texCoords[12];
    float vertxs[12];
    
    void setVertxs();
    void setTexCoords();

};

#endif /* Sprite_hpp */
