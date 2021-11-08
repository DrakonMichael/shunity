//
//  engine_render.hpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 8/26/21.
//

#ifndef engine_render_hpp
#define engine_render_hpp

#include <stdio.h>
#include <vector>
#include <map>

#include "engine_base.hpp"
#include "util_buffer.hpp"

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Include mesh utils
#include "modelUtil.hpp"
#include "util_buffer.hpp"

// Include shader utils
#include "loadShader.hpp"
#include "bmpUtil.hpp"

#include <stdio.h>

class Camera: public Component {
    public:
    vec3 position;
    vec2 orientation;
    vec3 up;
    float fov;
    float nearPlane;
    float farPlane;
    float speed;
    float sensitivity;
    
    virtual char* getName() override {return "Camera";};
    
    Camera() {
        position = vec3(0, 0, -10);
        orientation = vec2(radians(0.0f), radians(0.0f));
        up = vec3(0, 1, 0);
        fov = 45.0f;
        nearPlane = 0.1f;
        farPlane = 100.0f;
        speed = 3.0f;
        sensitivity = 0.05f;
    };
    
    glm::mat4 getCameraProjectionMatrix() {
        return perspective(radians(fov), (float)parent->SHU_GAME_INSTANCE->getWindowWidth() / (float)parent->SHU_GAME_INSTANCE->getWindowHeight(), nearPlane, farPlane);
    };
    glm::mat4 getCameraViewMatrix() {
        vec3 direction(cos(orientation.y) * sin(orientation.x), sin(orientation.y), cos(orientation.y) * cos(orientation.x));
        return lookAt(position, position + direction, up); //TEMP
    }
    
    virtual void start() override;
    virtual void update(float deltaTime) override;
    virtual void drawComponentUI() override;
};

class CameraReset : public Script {
    virtual char* getName() override {return "Camera Reset";};
    
    virtual void start() override;
    virtual void drawComponentUI() override;
    virtual std::vector<Component*> getDependencies() override;
};

class PointLight : public Component {
    public:
    virtual char* getName() override {return "Point Light";};
    vec3 color;
    float strength = 50.0f;
    
    virtual void drawComponentUI() override;
    virtual std::vector<Component*> getDependencies();
};


class MeshRenderer;

class Mesh {
    public:
    MeshRenderer* renderer;
    GLuint vertexBuffer;
    GLuint UVBuffer;
    GLuint normalBuffer;
    Mesh(const char * objPath);
    Mesh() {
        Mesh("../cube.obj");
    }
};

class Texture {
    
    public:
    MeshRenderer* renderer;
    GLuint texture;
    void load_bmp(const char * imgPath);
    Texture(const char * texturePath);
    Texture() {
        Texture("../uvtemplate.bmp");
    }
};

class Shader {
    protected:
    GLuint shaderProgram;
    std::map<const char *, GLuint> attributeMap;
    
    public:
    MeshRenderer* renderer;
    Shader(const char * vertexFilePath, const char * fragFilePath);
    Shader();

    virtual void instantiate() {};
    virtual void bindData() {};
    void setAttribute(const char * attribName);
    GLuint getAttribute(const char * attribName);
};

class UnlitShader : public Shader {
    virtual void instantiate() override;
    virtual void bindData() override;
};

class LitShader : public Shader {
    virtual void instantiate() override;
    virtual void bindData() override;
};

class LitTransformShader : public Shader {
    virtual void instantiate() override;
    virtual void bindData() override;
};

class MeshRenderer : public Component {
    public:
    Mesh* mesh;
    Texture* texture;
    Shader* shader;
    
    virtual char* getName() override {return "Mesh Renderer";};
    
    MeshRenderer(char * meshPath, char * texPath, Shader* imp_shader);
    
    virtual void start() override;
    virtual void update(float deltaTime) override;
    virtual void drawComponentUI() override;
    
    void draw() { glDrawArrays(GL_TRIANGLES, 0, 5000); }
    std::vector<Component*> getDependencies() override;
    
    void setMesh(Mesh* m) { mesh = m; mesh->renderer = this; };
    void setTexture(Texture* t) { texture = t; texture->renderer = this; };
    void setShader(Shader* s) { shader = s; shader->renderer = this; };

};

void SHU_setFaceCulling(bool enable);




#endif /* engine_render_hpp */
