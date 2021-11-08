// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>


GLFWwindow* window;
int win_width = 1420;
int win_height = 768;

// tex util
#include "bmpUtil.hpp"

// Shaders
#include "loadShader.hpp"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Models
#include "modelUtil.hpp"

#include "engine_base.hpp"
#include "engine_render.hpp"
#include "engine_physics.hpp"

// User assets
#include "myScript.hpp"




using namespace glm;

int main( void )
{
   
    
    Game game = Game(win_width, win_height);
    game.root->addComponent(new PMetric());
    // Start
    
    GameObject sample = GameObject(game.root);
    sample.Name = (char*) "Sample Assets";
    
    
    GameObject monke = GameObject(&sample);
    monke.addComponent(new Transform());
    monke.addComponent(new MeshRenderer("../suzanne.obj", "../uvtemplate.bmp", new LitShader()));
    monke.addComponent(new Collider());
    monke.addComponent(new Rigidbody());
    monke.disabled = false;
    
    monke.Name = (char*) "Sample Monkey";
    
    GameObject camera = GameObject(&sample);
    camera.Name = (char*) "Scene Camera";
    camera.addComponent(new Transform());
    camera.addComponent(new Camera());
    camera.addComponent(new CameraReset());
    
    GameObject light = GameObject(&sample);
    light.Name = (char*) "Point Light";
    light.addComponent(new Transform());
    light.addComponent(new PointLight());
    light.getComponent<PointLight>()->color = vec3(1.0f, 1.0f, 1.0f);
    light.getComponent<PointLight>()->strength = 20.0f;
    light.addComponent(new mover());
    light.addComponent(new MeshRenderer("../cube.obj", "../uvtemplate.bmp", new UnlitShader()));
    light.getComponent<Transform>()->scale = vec3(0.1f, 0.1f, 0.1f);
    
    monke.getComponent<Transform>()->position = vec3(0, 0, 4.5f);
    
    GameObject floor = GameObject(&sample);
    floor.Name = (char*) "Floor";
    floor.addComponent(new Transform());
    floor.addComponent(new MeshRenderer("../cube.obj", "../uvtemplate.bmp", new LitShader()));
    floor.getComponent<Transform>()->scale = vec3(50.0f, 0.1f, 50.0f);
    floor.getComponent<Transform>()->position = vec3(0.0f, -10.0f, 0.0f);
    
    GameObject ceiling = GameObject(&sample);
    ceiling.Name = (char*) "Ceiling";
    ceiling.addComponent(new Transform());
    ceiling.addComponent(new MeshRenderer("../cube.obj", "../uvtemplate.bmp", new LitShader()));
    ceiling.getComponent<Transform>()->scale = vec3(50.0f, 0.1f, 50.0f);
    ceiling.getComponent<Transform>()->position = vec3(0.0f, 2.0f, 0.0f);
    
    SHU_setFaceCulling(true);
    
    /*
     The fact that this loop unrolls as a single instruction to create 10 seperate objects but THEN to
     set ALL of their names to the SAME thing is unequivocally the most annoying bug i have
     ever encountered -- and I work in web development.
     
    for(int i = 0; i < 10; i++) {
        GameObject n = GameObject(&floor);
        n.Name = const_cast<char*>(std::to_string(i).c_str());
        n.addComponent(new Transform());
    }*/

    game.loop();
    
    return 0;
}

