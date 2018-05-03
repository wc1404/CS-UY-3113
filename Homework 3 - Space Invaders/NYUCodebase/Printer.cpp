//
//  Printer.cpp
//  NYUCodebase
//
//  Created by Wilfredo Cueto on 4/16/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "Printer.hpp"

Printer::Printer(unsigned int theFontTex, vec3 textDimen, float theSize, float theSpacing): fontTex(theFontTex), texDimen(textDimen), size(theSize), spacing(theSpacing) {
    
    texture_size = 1.0/(textDimen.x);
}

void Printer::print(ShaderProgram* program, Matrix &modelMatrix, const std::string text, const vec3* const pos, const vec3* const scale, const int rot) {
    
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    
    for(int i=0; i < text.size(); i++) {
        
        int spriteIndex = (int)text[i];
        float texture_x = (float)(spriteIndex % (int)texDimen.x) / texDimen.x;
        float texture_y = (float)(spriteIndex / (int)texDimen.y) / texDimen.y;
        
        vertexData.insert(vertexData.end(), {
            //Top Triangle
            ((size + spacing) * i) + (-0.5f * size), 0.5f * size,  //Top left
            ((size + spacing) * i) + (-0.5f * size), -0.5f * size, //Bottom left
            ((size + spacing) * i) + (0.5f * size), 0.5f * size,   //Top Right
            
            //Bottom Triangle
            ((size + spacing) * i) + (0.5f * size), -0.5f * size,  //Bottom Right
            ((size + spacing) * i) + (0.5f * size), 0.5f * size,   //Top Right
            ((size + spacing) * i) + (-0.5f * size), -0.5f * size, //Bottom Left
        });
        
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            
            texture_x + texture_size, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x, texture_y + texture_size,
        }); }
    
    modelMatrix.Identity();
        
    if (pos != nullptr) {
        modelMatrix.Translate(pos->x, pos->y, pos->z);
    }
    
    if (scale != nullptr) {
        modelMatrix.Scale(scale->x, scale->y, scale->z);
    }
    
    if (rot > 0) {
        modelMatrix.Rotate(rot);
    }
    
    program->SetModelMatrix(modelMatrix);
    
    glBindTexture(GL_TEXTURE_2D, fontTex);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, ((int)(text.size()) * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
