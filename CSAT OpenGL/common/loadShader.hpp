//
//  loadShader.hpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 8/23/21.
//

#ifndef loadShader_hpp
#define loadShader_hpp

#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// Include glew and glfw
#include <glew.h>
#include <GLFW/glfw3.h>

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

#endif /* loadShader_hpp */
