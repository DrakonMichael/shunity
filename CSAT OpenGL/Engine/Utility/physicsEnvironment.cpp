//
//  physicsEnvironment.cpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 11/3/21.
//

#include "physicsEnvironment.hpp"

PhysicsEnvironment::PhysicsEnvironment() {
    _broadphase = new btDbvtBroadphase();
    _collisionConfiguration = new btDefaultCollisionConfiguration();
    _dispatcher = new btCollisionDispatcher(_collisionConfiguration);
    _solver = new btSequentialImpulseConstraintSolver();
    _world = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _collisionConfiguration);
    _world->setGravity(btVector3(0, -9.81, 0));
}

void PhysicsEnvironment::step(float dt) {
    _world->stepSimulation(dt);
}
