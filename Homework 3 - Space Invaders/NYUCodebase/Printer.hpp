//
//  Printer.hpp
//  NYUCodebase
//
//  Created by Wilfredo Cueto on 4/16/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#ifndef Printer_hpp
#define Printer_hpp

#include <stdio.h>
#include "ShaderProgram.h"
#include "FloatVectors.hpp"
#include <vector>

class Printer {
public:
    Printer(unsigned int theFontTex, vec3 printDimen, vec3 textDimen, float theSize, float theSpacing);
    
    void print(ShaderProgram &program, vec3 pos, const std::string text = "Default Text");
    
private:
    vec3 texDimen, dimen;
    
    float size;
    float texture_size;
    
    float spacing;
    unsigned int fontTex;
};

#endif /* Printer_hpp */
