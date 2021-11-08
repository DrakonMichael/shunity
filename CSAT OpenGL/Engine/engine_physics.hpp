//
//  engine_physics.hpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 10/28/21.
//

#ifndef engine_physics_hpp
#define engine_physics_hpp

#include <stdio.h>

// Physics imports
#include "btBulletDynamicsCommon.h"
#include "engine_base.hpp"

// Uh oh.

class Collider : public Component {
    // This class literally just holds the collider, nothing else.
    virtual char* getName() override {return "Collider3D";};
    public:
    btCollisionShape* shape;
};

class PhysicsMaterial {
    public:
    float restitution;
    float surfaceFriction;
    PhysicsMaterial(float res, float sf);
    PhysicsMaterial();
};

class Rigidbody : public SimulatedComponent {
    virtual char* getName() override {return "Rigidbody3D";};
    virtual void start() override;
    virtual void update(float deltaTime) override;
    virtual void drawComponentUI() override;
    virtual std::vector<Component*> getDependencies() override;
    PhysicsMaterial mat;
    
    public:
    Rigidbody();
    
    bool isKinematic = false;
    bool freezeRotation = false;
    bool freezePosition = false;
    float mass = 1.0f;
};



#endif /* engine_physics_hpp */
