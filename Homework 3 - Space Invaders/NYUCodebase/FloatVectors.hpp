//
//  FloatVectors.hpp
//  NYUCodebase
//
//  Created by Wilfredo Cueto on 4/12/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#ifndef FloatVectors_hpp
#define FloatVectors_hpp

#include <stdio.h>

class vec3 {
public:
    vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
    
    vec3(const vec3 &other);
    
    vec3& operator=(const vec3 &other);
    
    float x;
    float y;
    float z;
};

#endif /* FloatVectors_hpp */