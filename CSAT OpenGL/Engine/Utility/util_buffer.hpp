//
//  util_buffer.hpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 8/26/21.
//

#ifndef util_buffer_hpp
#define util_buffer_hpp

// Include GLEW
#include <GL/glew.h>


// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm.hpp>
using namespace glm;

#include <stdio.h>
#include <vector>

GLuint buffer_generic(const GLfloat* data, int size);
GLuint buffer_vec2(std::vector<vec2> &vertices);
GLuint buffer_vec3(std::vector<vec3> &vertices);


#endif /* util_buffer_hpp */
