//
//  physicsEnvironment.hpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 11/3/21.
//

#ifndef physicsEnvironment_hpp
#define physicsEnvironment_hpp

#include "btBulletDynamicsCommon.h"

#include <stdio.h>

class PhysicsEnvironment {
    btBroadphaseInterface*                  _broadphase;
    btDefaultCollisionConfiguration*        _collisionConfiguration;
    btCollisionDispatcher*                  _dispatcher;
    btSequentialImpulseConstraintSolver*    _solver;
    btDiscreteDynamicsWorld*                _world;
    
    public:
    PhysicsEnvironment();
    void step(float dt);
};

#endif /* physicsEnvironment_hpp */
