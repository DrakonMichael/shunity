//
//  myScript.cpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 8/29/21.
//

#include "myScript.hpp"

std::vector<Component*> mover::getDependencies() {
   std::vector<Component*> deps;
   deps.push_back(require<Transform>());
   return deps;
}

void mover::update(float deltaTime) {
    this->parent->getComponent<Transform>()->position = vec3(sin(glfwGetTime()) * 7.5f, 0.0f, cos(glfwGetTime()) * 7.5f);
}

void mover::drawComponentUI() {
        float vel_f3[] = {vel.x, vel.y, vel.z};
        if (ImGui::DragFloat3("vel", vel_f3)) {
            vel = glm::make_vec3(vel_f3);
        }
        
        float spd = speed;
        if (ImGui::SliderFloat("Timespeed", &spd, -1.0f, 1.0f)) {
            speed = spd;
        }
}
