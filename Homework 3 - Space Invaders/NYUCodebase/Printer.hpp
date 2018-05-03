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
#include "Matrix.h"

class Printer {
public:
    Printer(unsigned int theFontTex, vec3 textDimen, float theSize, float theSpacing);
    
    void print(ShaderProgram* program, Matrix &modelMatrix, const std::string text = "Default Text", const vec3* const pos = nullptr, const vec3* const scale = nullptr, const int rot = 0);
    
private:
    vec3 texDimen;
    
    float size;
    float texture_size;
    bool center;
    
    float spacing;
    unsigned int fontTex;
};

#endif /* Printer_hpp */
