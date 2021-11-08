//
//  modelUtil.hpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 8/26/21.
//

#ifndef modelUtil_hpp
#define modelUtil_hpp

#include <stdio.h>
#include <glm.hpp>

using namespace glm;

#include <vector>
#include <string>

void loadOBJ(const char * path, std::vector<vec3> &vertices, std::vector<vec2> &uvs, std::vector<vec3> &normals);

#endif /* modelUtil_hpp */
