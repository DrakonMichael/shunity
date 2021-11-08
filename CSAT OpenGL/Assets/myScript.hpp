//
//  myScript.hpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 8/29/21.
//

#ifndef myScript_hpp
#define myScript_hpp

#include "engine_base.hpp"
#include "engine_render.hpp"

#include "imgui.h"
#define DEBUG this->parent->SHU_GAME_INSTANCE->Debug

class mover : public Script {
    virtual char* getName() override {return "Rigidbody3D";};
    virtual void update(float deltaTime) override;
    virtual void drawComponentUI() override;
    
    virtual std::vector<Component*> getDependencies() override;
    
    vec3 vel = vec3(0, 0, 0);
    vec3 rvel = vec3(0, 0, 0);
    
    public:
    float speed = 1.0f;
};

#include <stdio.h>

#endif /* myScript_hpp */
