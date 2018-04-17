//
//  FloatVectors.cpp
//  NYUCodebase
//
//  Created by Wilfredo Cueto on 4/12/18.
//  Copyright © 2018 Ivan Safrin. All rights reserved.
//

#include "FloatVectors.hpp"

vec3::vec3(float X, float Y, float Z): x(X), y(Y), z(Z) {}

vec3& vec3::operator=(const vec3 &other) {
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

vec3::vec3(const vec3 &other) {
    x = other.x;
    y = other.y;
    z = other.z;
}
