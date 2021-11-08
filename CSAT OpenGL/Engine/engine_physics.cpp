//
//  engine_physics.cpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 10/28/21.
//

#include "engine_physics.hpp"

std::vector<Component*> Rigidbody::getDependencies() {
   std::vector<Component*> deps;
   deps.push_back(require<Transform>());
    deps.push_back(require<Collider>());
   return deps;
}

void Rigidbody::update(float deltaTime) {
    this->parent->getComponent<Transform>()->position += vec3(0, -1*deltaTime, 0);
}

void Rigidbody::drawComponentUI() {
    ImGui::Checkbox("Is Kinematic?", &isKinematic);
    ImGui::Checkbox("Freeze Position", &freezePosition);
    ImGui::Checkbox("Freeze Rotation", &freezeRotation);
    ImGui::InputFloat("Mass", &mass);
}

PhysicsMaterial::PhysicsMaterial(float res, float sf) {
    restitution = res;
    surfaceFriction = sf;
}

PhysicsMaterial::PhysicsMaterial() {
    restitution = 0.5f;
    surfaceFriction = 0.25f;
}

void Rigidbody::start() {
    Transform* transform = this->parent->getComponent<Transform>();
    
    //1
    btQuaternion rotation;
    rotation.setEulerZYX(0, 0, 0);
    
    //2
    btVector3 position = btVector3(transform->position.x, transform->position.y, transform->position.z);
    
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(rotation, position));
}

Rigidbody::Rigidbody() {
    isKinematic = false;
    freezeRotation = false;
    freezePosition = false;
    mat = PhysicsMaterial(0.75f, 0.5f);
    

}


