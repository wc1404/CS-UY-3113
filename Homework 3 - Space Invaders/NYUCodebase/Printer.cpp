//
//  Printer.cpp
//  NYUCodebase
//
//  Created by Wilfredo Cueto on 4/16/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "Printer.hpp"

Printer::Printer(unsigned int theFontTex, vec3 printDimen, vec3 textDimen, float theSize, float theSpacing): fontTex(theFontTex), dimen(printDimen), texDimen(textDimen), size(theSize), spacing(theSpacing) {
    
    texture_size = 1.0/(textDimen.x);
    
}

void Printer::print(ShaderProgram &program, vec3 pos, const std::string text) {
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    for(int i=0; i < text.size(); i++) {
        
        int spriteIndex = (int)text[i];
        float texture_x = (float)(spriteIndex % (int)texDimen.x) / texDimen.x;
        float texture_y = (float)(spriteIndex / texDimen.y) / texDimen.y;
        
        vertexData.insert(vertexData.end(), {
            ((size + spacing) * i) + (-0.5f * size), 0.5f * size,
            ((size + spacing) * i) + (-0.5f * size), -0.5f * size,
            ((size + spacing) * i) + (0.5f * size), 0.5f * size,
            
            ((size + spacing) * i) + (0.5f * size), -0.5f * size,
            ((size + spacing) * i) + (0.5f * size), 0.5f * size,
            ((size + spacing) * i) + (-0.5f * size), -0.5f * size,
        });
        
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            
            texture_x + texture_size, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x, texture_y + texture_size,
        }); }
    
    glBindTexture(GL_TEXTURE_2D, fontTex);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, ((int)(text.size()) * 6));
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}
