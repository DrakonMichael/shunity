//
//  engine_base.hpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 8/27/21.
//

#ifndef engine_base_hpp
#define engine_base_hpp

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
// Include GLEW

#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "loadShader.hpp"
#include "util_buffer.hpp"
#include "physicsEnvironment.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"



using namespace glm;



// REIMPLEMENT
class GameObject;
class Game;

class Component {
    public:
    GameObject* parent;
    
    virtual char* getName() {return "BASE_COMPONENT";}
    
    virtual void start() {}; // Runs on Component initialization
    virtual void update(float deltaTime) {}; // Runs every frame
    virtual void fixedUpdate() {}; // Runs 30/s
    virtual void lateUpdate() {}; // Runs immediately after fixedUpdate but before next update.
    virtual void removed() {}; // Runs when the component is about to be removed.
    
    virtual void reset() {};
    
    virtual bool shouldAlwaysDrawUI() {return false;};
    virtual void drawComponentUI() {};
    
    template <class X>
    Component* require() {
      return new X;
    }
    
    virtual std::vector<Component*> getDependencies() {
        std::vector<Component*> deps;
        // Require components here..
        return deps;
    }
    
    void setParent(GameObject* object);
};

// A rename of the Component class to make it easier to distinguish engine-provided components and user-made scripts.
class Script : public Component {
    virtual void start() override {}; // Runs on Component initialization
    virtual void update(float deltaTime) override {}; // Runs every frame
    virtual void fixedUpdate() override {}; // Runs 30/s
    virtual void lateUpdate() override {}; // Runs immediately after fixedUpdate but before next update.
    virtual void removed() override {}; // Runs when the component is about to be removed.
    virtual void drawComponentUI() override {};
    
    public:
    virtual char* getName() override {return "Script";};
};

// Functionally identical to Script, but is GE-generated
class SimulatedComponent : public Component {
    virtual void start() override {}; // Runs on Component initialization
    virtual void update(float deltaTime) override {}; // Runs every frame
    virtual void fixedUpdate() override {}; // Runs 30/s
    virtual void lateUpdate() override {}; // Runs immediately after fixedUpdate but before next update.
    virtual void removed() override {}; // Runs when the component is about to be removed.
    virtual void drawComponentUI() override {};
    
    public:
    virtual char* getName() override {return "SimulatedComponent";};
};

class PMetric : public Component {
    public:
    virtual char* getName() override {return "Performance Metrics";};
    float fps;
    float fpsValues[200];
    int lastValue = 0;
    float avg = 0.0f;
    float mem = 28.2f;
    
    virtual bool shouldAlwaysDrawUI() override {return true;};
    virtual void update(float deltaTime) override;
    virtual void drawComponentUI() override;
};

class Transform : public Component {
    public:
    
    virtual char* getName() override {return "Transform";};
    
    vec3 position;
    glm::quat orientation;
    vec3 scale;
    Transform();
    Transform(vec3 pos);
    Transform(vec3 pos, glm::quat orient);
    Transform(vec3 pos, glm::quat orient, vec3 scle);
    
    virtual void start() override {};
    virtual void update(float deltaTime) override {};
    virtual void drawComponentUI() override;
    virtual void reset() override;
};

class Inspector {
    public:
    GameObject* selectedObject;
    
    void draw();
};



class GameObject {
    char* UID; // MUST BE UNIQUE
    GameObject* SHU_INDEXED_PARENT;
    char* SHU_INDEXED_NAME;
    
    public:
    GameObject* parent;
    bool disabled = false;
    std::vector<GameObject*> children;
    std::vector<Component*> components;
    char * Name;
    
    // ENGINE SPECIFIC
    Game* SHU_GAME_INSTANCE;
    // END ENGINE SPECIFIC
    
    // Constrtuctor
    GameObject();
    GameObject(GameObject* other);
    
    // Hierarchy
    void addChild(GameObject* child);
    void addComponent(Component* component);
    void setParent(GameObject* other);
    void updateChildNames();
    void removeChild(char* UID);
    void drawUI();
    void drawHierarchyObject();
    void drawHierarchyObject(bool passRender);
    
    // Lifecycle
    void SHU_LIFECYCLE_UPDATE();
    void start();
    void update(float deltaTime);
    
    // Getters
    std::vector<GameObject*> getChildren();
    char* getUID() { return UID; }
    GameObject* getChild(char * name);
    
    template<typename T>
    void removeComponent() {
        for (int n = 0; n < components.size(); n++) {
            Component* c = components.at(n);
            if (T* cmp = dynamic_cast<T*>(c)) {
                components.erase(components.begin() + n);
                return;
            }
        }
    }
    
    void removeComponent(char* name) {
        for (int n = 0; n < components.size(); n++) {
            char* c = components.at(n)->getName();
            if (c == name) {
                components.erase(components.begin() + n);
                return;
            }
        }
    }
    
    template<typename T>
    T* getComponent() {
        for (Component* c : components) {
            if (T* cmp = dynamic_cast<T*>(c)) {
                return (T*)c;
            }
        }

        return nullptr;
    }
    
    template<typename T>
    std::vector<T*> getComponentsInDescendants(int depth) {
        std::vector<T*> c;
        T* r = getComponent<T>();
        if(r != nullptr)
            c.push_back(r);
        
        if(depth == 0) // -1 means infinite depth
            return c;
        

        
        if(children.size() > 0) {
            for (GameObject * child : children) {
                std::vector<T*> res = child->getComponentsInDescendants<T>(depth-1);
                // merge 2 vectors
                c.reserve( res.size() );
                c.insert( c.end(), res.begin(), res.end() );
            }
        } else {
            return c;
        }
        
        return components;
    }
    
    template<typename T>
    T* getComponentInDescendants(int depth) {
        T* r = getComponent<T>();
        if(r != nullptr)
            return r;
        
        if(depth == 0) // -1 means infinite depth
            return nullptr;
        
        if(children.size() > 0) {
            for (GameObject * child : children) {
                T* res = child->getComponentInDescendants<T>(depth-1);
                if(res != nullptr) {
                    return res;
                }
            }
        } else {
            return nullptr;
        }
        
        return nullptr;
    }
};

class Hierarchy {
    public:
    Game* gameInstance;
    GameObject* rootObject;
    GameObject* selectedObject;
    
    Hierarchy() {};
    Hierarchy(GameObject* inst) { rootObject = inst; };
    
    void updateSelection(GameObject * newObject);
    bool draw();
};

class Console {
    public:
    std::vector<std::string> events;
    
    void draw();
    void log(const char * msg);
    void warn(const char * msg);
    void error(const char * msg);
    void engine(const char * msg);
    void cont(const char * msg);
    
    void clear() {events.clear();};
};

class Game {
    GLFWwindow* window;
    bool run = false;
    int runtime = 0; // 0 -- Engine, 1 -- Compiled
    void update(float deltaTime);
    void drawComponentUIs();
    int lastID = 0;
    
    int width;
    int height;
    
    public:
    GameObject* root;
    Game();
    Game(int w, int h);
    
    int generateID();
    
    void start();
    void loop();
    void pause();
    
    int getWindowWidth() { return width; }
    int getWindowHeight() { return height; }
    GLFWwindow* getWindow() { return window; }
    int getRuntime() { return runtime; }
    
    //OpenGL methods
    void createWindow(int width, int height);
    
    
    
    // Engine props and methods
    Inspector SHU_INSPECTOR;
    Hierarchy SHU_HIERARCHY;
    Console Debug;
    
    // Game singletons
    PhysicsEnvironment Physics;
};

// FUNCTIONS

bool SHU_deletedDependencyCheck(GameObject* object, char* componentName);
std::vector<Component*> SHU_getDependencyConflictsOnDelete(GameObject* object, char* componentName);


#endif /* engine_base_hpp */
