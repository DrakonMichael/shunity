//
//  util_buffer.cpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 8/26/21.
//

#include "util_buffer.hpp"

GLuint buffer_generic(const GLfloat* data, int size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat), data, GL_STATIC_DRAW);
    
    return buffer;
}

GLuint buffer_vec3(std::vector<vec3> &vertices) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    return buffer;
}

GLuint buffer_vec2(std::vector<vec2> &vertices) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    return buffer;
}


