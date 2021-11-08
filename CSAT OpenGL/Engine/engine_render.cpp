//
//  engine_render.cpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 8/26/21.
//

#define engine_render_implementation
#include "engine_render.hpp"

Mesh::Mesh(const char * objPath) {
    std::vector<vec3> vertices;
    std::vector<vec2> UVs;
    std::vector<vec3> normals;
    loadOBJ(objPath, vertices, UVs, normals);
    
    vertexBuffer = buffer_vec3(vertices);
    UVBuffer = buffer_vec2(UVs);
    normalBuffer = buffer_vec3(normals);
}

void Texture::load_bmp(const char * imgPath) {
    texture = loadBMP_custom(imgPath);
}

Shader::Shader(const char * vertexFilePath, const char * fragFilePath) {
    shaderProgram = LoadShaders(vertexFilePath, fragFilePath);
}







Texture::Texture(const char * texturePath) {
    texture = loadBMP_custom(texturePath);
}

Shader::Shader() {
    shaderProgram = LoadShaders("../Sample/v_projection.glsl", "../Sample/unlit.glsl");
}

void LitTransformShader::instantiate() {
    shaderProgram = LoadShaders("../Sample/v_lit.glsl", "../Sample/f_lit.glsl");
    setAttribute("MVP");
    setAttribute("V");
    setAttribute("M");
    setAttribute("textureSampler");
    setAttribute("LightPosition_worldspace");
    setAttribute("lightColor");
    setAttribute("LightPower");
}

void LitTransformShader::bindData() {
    glUseProgram(shaderProgram);
    
    GameObject* cur = renderer->parent;
    mat4 res = mat4(1.0f);
    while(cur->parent != NULL) {
        res = translate(res, cur->getComponent<Transform>()->position);
        res *= toMat4(quat(cur->getComponent<Transform>()->orientation));
        res = scale(res, cur->getComponent<Transform>()->scale);
        cur = cur->parent;
    }
    
    Camera* Cam = renderer->parent->SHU_GAME_INSTANCE->root->getComponentInDescendants<Camera>(-1);

    
    mat4 proj = Cam->getCameraProjectionMatrix();
    mat4 view = Cam->getCameraViewMatrix();
    mat4 mvp = proj * view * res; // Generate the final
    
    PointLight* light = renderer->parent->SHU_GAME_INSTANCE->root->getComponentInDescendants<PointLight>(-1);
    vec3 lightPos = light->parent->getComponent<Transform>()->position;
    vec3 lightColor = light->color;
    glUniform3f(getAttribute("LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(getAttribute("lightColor"), lightColor.x, lightColor.y, lightColor.z);
    glUniform1f(getAttribute("LightPower"), light->strength);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->texture->texture);
    glUniform1i(getAttribute("textureSampler"), 0);
    
    glUniformMatrix4fv(getAttribute("MVP"), 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(getAttribute("M"), 1, GL_FALSE, &res[0][0]);
    glUniformMatrix4fv(getAttribute("V"), 1, GL_FALSE, &view[0][0]);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->UVBuffer);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->normalBuffer);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    renderer->draw();
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

void LitShader::instantiate() {
    shaderProgram = LoadShaders("../Sample/v_lit.glsl", "../Sample/f_lit.glsl");
    setAttribute("MVP");
    setAttribute("V");
    setAttribute("M");
    setAttribute("textureSampler");
    setAttribute("LightPosition_worldspace");
    setAttribute("lightColor");
    setAttribute("LightPower");
}

void PointLight::drawComponentUI() {
        ImGui::SliderFloat("strength", &strength, 0.0f, 100.0f);
        float c[4] = {color.x, color.y, color.z};
        if(ImGui::ColorEdit4("Color", c)) {
            color = vec3(c[0], c[1], c[2]);
        }
}

std::vector<Component*> PointLight::getDependencies() {
   std::vector<Component*> deps;
   deps.push_back(require<Transform>());
   return deps;
}

void LitShader::bindData() {
    glUseProgram(shaderProgram);
    
    
    
    quat rot = quat(renderer->parent->getComponent<Transform>()->orientation);
    mat4 RotationMatrix = toMat4(rot);
    

    
    mat4 modelMatrix = mat4(1.0f);
    modelMatrix = translate(modelMatrix, renderer->parent->getComponent<Transform>()->position);
    modelMatrix = modelMatrix * RotationMatrix;
    modelMatrix = scale(modelMatrix, renderer->parent->getComponent<Transform>()->scale);
    
    Camera* Cam = renderer->parent->SHU_GAME_INSTANCE->root->getComponentInDescendants<Camera>(-1);
    PointLight* light = renderer->parent->SHU_GAME_INSTANCE->root->getComponentInDescendants<PointLight>(-1);
    
    if(Cam == NULL || light == NULL)
        return;
    
    
    mat4 proj = Cam->getCameraProjectionMatrix();
    mat4 view = Cam->getCameraViewMatrix();
    mat4 mvp = proj * view * modelMatrix; // Generate the final
    
    vec3 lightPos = light->parent->getComponent<Transform>()->position;
    vec3 lightColor = light->color;
    glUniform3f(getAttribute("LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(getAttribute("lightColor"), lightColor.x, lightColor.y, lightColor.z);
    glUniform1f(getAttribute("LightPower"), light->strength);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->texture->texture);
    glUniform1i(getAttribute("textureSampler"), 0);
    
    glUniformMatrix4fv(getAttribute("MVP"), 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(getAttribute("M"), 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(getAttribute("V"), 1, GL_FALSE, &view[0][0]);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->UVBuffer);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->normalBuffer);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    renderer->draw();
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

void UnlitShader::instantiate() {
    shaderProgram = LoadShaders("../Sample/v_projection.glsl", "../Sample/unlit.glsl");
    setAttribute("MVP");
    setAttribute("texSampler");
}

void UnlitShader::bindData() {
    glUseProgram(shaderProgram);
    
    quat rot = quat(renderer->parent->getComponent<Transform>()->orientation);
    mat4 RotationMatrix = toMat4(rot);
    
    
    mat4 modelMatrix = mat4(1.0f);
    modelMatrix = translate(modelMatrix, renderer->parent->getComponent<Transform>()->position);
    modelMatrix = modelMatrix * RotationMatrix;
    modelMatrix = scale(modelMatrix, renderer->parent->getComponent<Transform>()->scale);
    
    Camera* Cam = renderer->parent->SHU_GAME_INSTANCE->root->getComponentInDescendants<Camera>(-1);
    
    if(Cam == NULL)
        return;
    
    mat4 proj = Cam->getCameraProjectionMatrix();
    mat4 view = Cam->getCameraViewMatrix();
    mat4 mvp = proj * view * modelMatrix; // Generate the final
    
    glUniformMatrix4fv(getAttribute("MVP"), 1, GL_FALSE, &mvp[0][0]);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->texture->texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(getAttribute("texSampler"), 0);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->mesh->UVBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    renderer->draw();
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
}

void Shader::setAttribute(const char * attribName) {
    attributeMap[attribName] = glGetUniformLocation(shaderProgram, attribName);
}

GLuint Shader::getAttribute(const char * attribName) {
    return attributeMap[attribName];
}

MeshRenderer::MeshRenderer(char * meshPath, char * texPath, Shader* imp_shader) {
    Mesh* m = new Mesh(meshPath);
    Texture* t = new Texture(texPath);
    Shader* s = imp_shader;
    
    setMesh(m);
    setTexture(t);
    setShader(s);
}

std::vector<Component*> MeshRenderer::getDependencies() {
   std::vector<Component*> deps;
   deps.push_back(require<Transform>());
   return deps;
}

void MeshRenderer::start() {
    shader->instantiate();
}


void MeshRenderer::update(float deltaTime) {
    shader->bindData();
 }

void MeshRenderer::drawComponentUI() {

        if(ImGui::TreeNode("Mesh")) {
            ImGui::Text("Mesh data");
            ImGui::TreePop();
        }
        
        if(ImGui::TreeNode("Texture")) {
            ImGui::Text("Tex data");
            ImGui::TreePop();
        }
        
        if(ImGui::TreeNode("Shader")) {
            ImGui::Text("Shader data");
            ImGui::TreePop();
        }

}

void CameraReset::start() {
    this->parent->getComponent<Camera>()->position = this->parent->getComponent<Transform>()->position;
}

std::vector<Component*> CameraReset::getDependencies() {
   std::vector<Component*> deps;
   deps.push_back(require<Transform>());
   deps.push_back(require<Camera>());
   return deps;
}

void CameraReset::drawComponentUI() {
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.1f, 1.0f), "Camera reset enabled");
    if(ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("The camera will reset to the position of its");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.35f, 1.0f), "Transform");
        ImGui::TextWrapped("component when the project is run.");
        ImGui::EndTooltip();
    }
}

void Camera::start() {
    glfwSetInputMode(this->parent->SHU_GAME_INSTANCE->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(this->parent->SHU_GAME_INSTANCE->getWindow(), this->parent->SHU_GAME_INSTANCE->getWindowWidth()/2, this->parent->SHU_GAME_INSTANCE->getWindowHeight()/2);
}

void Camera::drawComponentUI() {
    ImGui::SliderFloat("FOV", &fov, 0.0f, 180.0f);
    ImGui::SliderFloat("Near plane", &nearPlane, 0.0f, 100.0f);
    ImGui::SliderFloat("Far plane", &farPlane, 0.0f, 500.0f);
    ImGui::SliderFloat("Speed", &speed, 0.0f, 25.0f);
    ImGui::SliderFloat("Sensitivity", &sensitivity, 0.0f, 1.0f);
}

void Camera::update(float deltaTime) {
    if (glfwGetKey( this->parent->SHU_GAME_INSTANCE->getWindow(), GLFW_KEY_Q ) != GLFW_PRESS && this->parent->SHU_GAME_INSTANCE->getRuntime() == 0){
        glfwSetInputMode(this->parent->SHU_GAME_INSTANCE->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }
    
    
    glfwSetInputMode(this->parent->SHU_GAME_INSTANCE->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    double xpos, ypos;
    glfwGetCursorPos(this->parent->SHU_GAME_INSTANCE->getWindow(), &xpos, &ypos);
    
    // Hardcoding a small grace period otherwise the camera jumps as glfwSetCursorPos isn't instantaneous.
    if(glfwGetTime() > 1.0f) {
        orientation.x += sensitivity * deltaTime * float(this->parent->SHU_GAME_INSTANCE->getWindowWidth()/2 - xpos);
        orientation.y += sensitivity * deltaTime * float(this->parent->SHU_GAME_INSTANCE->getWindowHeight()/2 - ypos);
    }

    orientation.y = clamp(orientation.y, radians(-90.0f), radians(90.0f));
    
    glfwSetCursorPos(this->parent->SHU_GAME_INSTANCE->getWindow(), this->parent->SHU_GAME_INSTANCE->getWindowWidth()/2, this->parent->SHU_GAME_INSTANCE->getWindowHeight()/2);
    
    
    vec3 direction(
        cos(orientation.y) * sin(orientation.x),
        sin(orientation.y),
        cos(orientation.y) * cos(orientation.x)
    );
    
    vec3 right = glm::vec3(
        sin(orientation.x - 3.14f/2.0f),
        0,
        cos(orientation.x - 3.14f/2.0f)
    );
    
    up = glm::cross( right, direction );
    
    // Move forward
    
    if (glfwGetKey( this->parent->SHU_GAME_INSTANCE->getWindow(), GLFW_KEY_UP ) == GLFW_PRESS){
        position += direction * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey( this->parent->SHU_GAME_INSTANCE->getWindow(), GLFW_KEY_DOWN ) == GLFW_PRESS){
        position -= direction * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey( this->parent->SHU_GAME_INSTANCE->getWindow(), GLFW_KEY_RIGHT ) == GLFW_PRESS){
        position += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey( this->parent->SHU_GAME_INSTANCE->getWindow(), GLFW_KEY_LEFT ) == GLFW_PRESS){
        position -= right * deltaTime * speed;
    }
     
}

void SHU_setFaceCulling(bool enable) {
    if(enable) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}
