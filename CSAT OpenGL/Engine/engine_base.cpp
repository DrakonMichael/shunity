//
//  engine_base.cpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 8/27/21.
//

#include "engine_base.hpp"

void GameObject::addChild(GameObject *child) {
    // Add to our child hierarchy..
    children.push_back(child);

    // Set child parent
    child->parent = this;
    child->SHU_INDEXED_PARENT = this;
    
    //!
    child->SHU_GAME_INSTANCE = this->SHU_GAME_INSTANCE;
    //!
}


void PMetric::update(float deltaTime) {
    fps = deltaTime;
    
    fpsValues[lastValue] = fps;
    lastValue++;
    if (lastValue > sizeof(fpsValues)/sizeof(float) - 1) {
        lastValue = 0;
    }
}


void PMetric::drawComponentUI() {
    
    ImVec4 color = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
    if((float)1/fps < 30) {
        color = ImVec4(1.0f, 1.0f, 0.2f, 1.0f);
    }
    if((float)1/fps < 10) {
        color = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
    }
    
    char c[100];
    sprintf(c, "%f", fps);
    ImGui::Separator();
    ImGui::Text("Performance Metrics");
    ImGui::Text("s/frame:"); ImGui::SameLine(); ImGui::TextColored(color, "%s", c);
    sprintf(c, "%f", (float)1/fps);
    ImGui::TextColored(color, "%s", c); ImGui::SameLine(); ImGui::Text(" FPS");
    if(fpsValues[sizeof(fpsValues)/sizeof(float) - 1] == NULL) {
        ImGui::Text("Average FPS:"); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "(Gathering values...)");
        ImGui::Separator();
    } else {
        
        
        float total;
        for(int i = 0; i < sizeof(fpsValues)/sizeof(float); i++) {
            total += 1/(fpsValues[i]);
        }

        avg = total/(float)(sizeof(fpsValues)/sizeof(float));
        
        sprintf(c, "%.0f", avg);
        ImGui::Text("Average FPS:"); ImGui::SameLine(); ImGui::TextColored(color, "%s", c);
        
        ImGui::Separator();
    }

}

void GameObject::addComponent(Component *component) {
    std::vector<Component*> deps = component->getDependencies();
    bool dependenciesMet = true;
    for(int n = 0; n < deps.size(); n++) {
        bool hasComponent = false;
        for(int j = 0; j < components.size(); j++) {
            if(components.at(j)->getName() == deps.at(n)->getName()) {
                hasComponent = true;
            }
        }
        
        if(!hasComponent) {
            dependenciesMet = false;
            std::string missing = std::string(deps.at(n)->getName());
            std::string newComp = std::string(component->getName());
            missing = "Unable to resolve peer dependency \"" + missing + "\" for new component \"" + newComp + "\" (GameObject \"" + Name + "\")";
            SHU_GAME_INSTANCE->Debug.error(missing.c_str());
            
        }
    }
    
    if(!dependenciesMet)
        return;
    
    
    components.push_back(component);
    component->parent = this;
}

void GameObject::setParent(GameObject *other) {
    other->addChild(this);
    this->SHU_INDEXED_PARENT = other;
}

std::vector<GameObject*> GameObject::getChildren() {
    return children;
}


GameObject* GameObject::getChild(char * name) {
    for(int n = 0; n < children.size(); n++) {
        GameObject* child = children.at(n);
        if(child->Name == name) {
            return child;
        }
    }
    return nullptr;
}

void GameObject::SHU_LIFECYCLE_UPDATE() {
    if(SHU_INDEXED_PARENT != parent) {
        this->setParent(parent);
        SHU_INDEXED_PARENT->removeChild(UID);
        SHU_INDEXED_PARENT = parent;
    }
}

void GameObject::start() {
    SHU_INDEXED_PARENT = parent;
    for(int n = 0; n < components.size(); n++) {
        try {
            if(SHU_GAME_INSTANCE->getRuntime() == 0) {
                if(!dynamic_cast<Script*>(components.at(n)) && !dynamic_cast<SimulatedComponent*>(components.at(n))) {
                    this->components.at(n)->start();
                }
            } else if (SHU_GAME_INSTANCE->getRuntime() == 1) {
                if(dynamic_cast<Script*>(components.at(n)) || dynamic_cast<SimulatedComponent*>(components.at(n))) {
                    this->components.at(n)->start();
                }
            }
        } catch(const char * except) {
            this->SHU_GAME_INSTANCE->Debug.error(except);
        }
    }
    
    std::vector<GameObject*> children = getChildren();
    for(int n = 0; n < children.size(); n++) {
        children.at(n)->start();
    }
}

void GameObject::update(float deltaTime) {
    SHU_LIFECYCLE_UPDATE();
    for(int n = 0; n < components.size(); n++) {
        try {
            
            if(SHU_GAME_INSTANCE->getRuntime() == 1) {
                this->components.at(n)->update(deltaTime);
            } else {
                if(!dynamic_cast<Script*>(components.at(n)) && !dynamic_cast<SimulatedComponent*>(components.at(n))) {
                    this->components.at(n)->update(deltaTime);
                }
            }
            
        } catch(const char * except) {
            this->SHU_GAME_INSTANCE->Debug.error(except);
        }
    }
    
    std::vector<GameObject*> children = getChildren();
    for(int n = 0; n < children.size(); n++) {
        children.at(n)->update(deltaTime);
    }
}

void Console::draw() {
    ImGui::Begin("Console");
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 1.0f, 0.9f));
    ImGui::Button("ERR"); ImGui::PopStyleColor();
    
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.1f, 1.0f, 0.6f));
    ImGui::SameLine(); ImGui::Button("WARN"); ImGui::PopStyleColor();
    
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.66f, 0.85f, 0.9f));
    ImGui::SameLine(); ImGui::Button("LOG"); ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5f, 0.35f, 0.3f));
    ImGui::SameLine(); ImGui::Button("ENG"); ImGui::PopStyleColor();
    ImGui::SameLine();
    if(ImGui::Button("CLEAR")) {
        clear();
    }
    ImGui::Separator();
    ImGui::BeginChild("CONSOLE_MSGS");

    for(int n = 0; n < events.size(); n++) {
        if(std::string(events.at(n)).find(std::string("[LOG]")) != std::string::npos) {
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.66f, 0.65f, 0.85f));
            ImGui::Text(events.at(n).c_str());
            ImGui::PopStyleColor();
        } else if(std::string(events.at(n)).find(std::string("[ENG]")) != std::string::npos) {
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.5f, 0.2f, 0.85f));
            ImGui::Text(events.at(n).c_str());
            ImGui::PopStyleColor();
        } else if(std::string(events.at(n)).find(std::string("[WARN]")) != std::string::npos) {
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.25f, 1.0f, 1.0f));
            ImGui::Text(events.at(n).c_str());
            ImGui::PopStyleColor();
        } else if(std::string(events.at(n)).find(std::string("[ERR]")) != std::string::npos) {
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(1.0f, 1.0f, 1.0f));
            ImGui::Text(events.at(n).c_str());
            ImGui::PopStyleColor();
        } else {
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 0.6f), events.at(n).c_str());
        }
        
        int count = 0;
        while(n+count+1 < events.size()) {
            if(events.at(n+count+1) == events.at(n)) {
                count += 1;
            } else {
                break;
            }
        }
        if(count > 0) {
            ImGui::SameLine();
            
            std::string x = "(x";
            std::string ct = std::to_string(count + 1);
            std::string full = x + ct + ")";
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(1.0f, 0.0f, 0.6f));
            ImGui::Text(full.c_str()); ImGui::PopStyleColor();
            n += count;
        }
        
        
    }
    
    ImGui::EndChild();
    
    ImGui::End();
}

void Console::log(const char * msg) {
    events.push_back("[LOG] " + std::string(msg));
}

void Console::warn(const char * msg) {
    events.push_back("[WARN] " + std::string(msg));
}

void Console::error(const char * msg) {
    events.push_back("[ERR] " + std::string(msg));
}

void Console::engine(const char * msg) {
    events.push_back("[ENG] " + std::string(msg));
}

void Console::cont(const char * msg) {
    events.push_back("  >> " + std::string(msg));
}

void Inspector::draw() {
    std::string targetName(selectedObject->Name);
    
    if(selectedObject->disabled) { ImGui::BeginDisabled(); }

    
    std::string flavor = "? ";
    
    if(selectedObject->parent == nullptr) {
        flavor.append("(Game Root)");
    } else {
        if(selectedObject->components.size() == 0) {
            flavor.append("(Folder)");
        } else {
            flavor.append("(Object)");
        }
    }

    
    ImGui::Begin("Inspector");
    
    ImGui::Text("%s", targetName.c_str());
    ImGui::SameLine(); ImGui::TextColored((ImVec4)ImColor::HSV(1.0f, 0.0f, 0.6f), "%s", flavor.c_str());
    ImGui::Separator();
    selectedObject->drawUI();
    if(selectedObject->disabled) { ImGui::EndDisabled();
        ImGui::Separator(); ImGui::Spacing();
        ImGui::Text("This object is disabled.");
        ImGui::TextWrapped("Re-enable property editing by setting the 'GameObject.disabled' flag to FALSE");
    }
    
    /*
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Add Component >"))
        {
            selectedObject->addComponent(new Transform());
        }

        ImGui::EndPopup();
    }
     */
    

    
    ImGui::End();
}

void Hierarchy::updateSelection(GameObject * newObject) {
    gameInstance->SHU_INSPECTOR.selectedObject = newObject;
}

void GameObject::drawHierarchyObject(bool passRender) {
    for(int n = 0; n < children.size(); n++) {
            children.at(n)->drawHierarchyObject();
    }
}

void GameObject::drawHierarchyObject() {
    if(children.size() > 0) {
        bool nodeOpen = ImGui::TreeNodeEx(Name, ImGuiTreeNodeFlags_OpenOnArrow);
        if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(GLFW_MOUSE_BUTTON_1)) {
            SHU_GAME_INSTANCE->SHU_HIERARCHY.updateSelection(this);
        }
        
        if(nodeOpen) {
            for(int n = 0; n < children.size(); n++) {
                    children.at(n)->drawHierarchyObject();
            }
            
            ImGui::TreePop();
        }
    } else {
        ImGui::Indent();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushItemWidth(-1.0f);
        ImGui::Selectable(Name, false);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor();
        
        if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(GLFW_MOUSE_BUTTON_1)) {
            SHU_GAME_INSTANCE->SHU_HIERARCHY.updateSelection(this);
        }
        
        ImGui::Unindent();
    }
    
}

void GameObject::drawUI() {
    if (ImGui::CollapsingHeader("OBJECT_INFO", ImGuiTreeNodeFlags_None))
    {
        std::string objName(Name);
        std::string fullName = "Name: " + objName;
        ImGui::Text(fullName.c_str());
        if (ImGui::TreeNode("Children")) {
            for(int n = 0; n < children.size(); n++) {
                if(ImGui::Selectable(children.at(n)->Name)) {
                    SHU_GAME_INSTANCE->SHU_HIERARCHY.updateSelection(children.at(n));
                }
            }

            ImGui::TreePop();
        }
        
        if (ImGui::TreeNode("Components")) {
            for(int n = 0; n < components.size(); n++) {
                std::string index = std::to_string(n);
                index = "(" + index + ")";
                ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 0.8f), index.c_str());
                ImGui::SameLine(); ImGui::Selectable(components.at(n)->getName());
                if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
                {
                    ImGui::Text(components.at(n)->getName());
                    ImGui::Separator();
                    if(ImGui::MenuItem("Delete")) {
                        if(SHU_deletedDependencyCheck(this, components.at(n)->getName())) {
                            removeComponent(components.at(n)->getName());
                        } else {
                            std::string text = std::string("Unable to remove component \"");
                            text = text + components.at(n)->getName() + "\" due to unresolved dependency conflicts.";
                            SHU_GAME_INSTANCE->Debug.error(text.c_str());
                            
                            for(Component* comp : SHU_getDependencyConflictsOnDelete(this, components.at(n)->getName())) {
                                std::string text = std::string(comp->getName());
                                text = "Component \"" + text + "\" is dependant on " + components.at(n)->getName();
                                SHU_GAME_INSTANCE->Debug.cont(text.c_str());
                            }
                            
                        }
                        ImGui::CloseCurrentPopup();
                    }
                    if(ImGui::MenuItem("Reset")) {
                        components.at(n)->reset();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Right-click to open options");
                
            }

            ImGui::TreePop();
        }
    }
    
    for(int n = 0; n < components.size(); n++) {
        
        if(components.at(n)->shouldAlwaysDrawUI() == false) {
            bool open = ImGui::CollapsingHeader(components.at(n)->getName(), ImGuiTreeNodeFlags_None);
            if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
            {
                ImGui::Text(components.at(n)->getName());
                ImGui::Separator();
                if(ImGui::MenuItem("Delete")) {
                    if(SHU_deletedDependencyCheck(this, components.at(n)->getName())) {
                        removeComponent(components.at(n)->getName());
                    } else {
                        std::string text = std::string("Unable to remove component \"");
                        text = text + components.at(n)->getName() + "\" due to unresolved dependency conflicts.";
                        SHU_GAME_INSTANCE->Debug.error(text.c_str());
                        
                        for(Component* comp : SHU_getDependencyConflictsOnDelete(this, components.at(n)->getName())) {
                            std::string text = std::string(comp->getName());
                            text = "Component \"" + text + "\" is dependant on " + components.at(n)->getName();
                            SHU_GAME_INSTANCE->Debug.cont(text.c_str());
                        }
                        
                    }
                    ImGui::CloseCurrentPopup();
                }
                if(ImGui::MenuItem("Reset")) {
                    components.at(n)->reset();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        
        
            if(dynamic_cast<Script*>(components.at(n))) {
                ImGui::SameLine(); ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 0.8f), "(Script)");
                if(ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.35f, 1.0f), "Scripts ");
                    ImGui::SameLine(); ImGui::Text("are a special subclass of ");
                    ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.35f, 1.0f), "Components");
                    
                    ImGui::TextWrapped("which usually contain user-written code. They execute only when the project is in the RUN state.");
                    ImGui::Separator();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.7f), "\"RUN\" hotkey: (CTRL + F5)");
                    ImGui::EndTooltip();
                }
            }
            
            if(dynamic_cast<SimulatedComponent*>(components.at(n))) {
                ImGui::SameLine(); ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 0.8f), "(SC)");
                if(ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.35f, 1.0f), "SCs (Simulated Components) ");
                    ImGui::SameLine(); ImGui::Text("are a special subclass of ");
                    ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.35f, 1.0f), "Components");
                    
                    ImGui::TextWrapped("generated by the engine, and which have behavior that only executes during engine runtime.");
                    ImGui::Separator();
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.7f), "\"RUN\" hotkey: (CTRL + F5)");
                    ImGui::EndTooltip();
                }
            }
            
            if (open) {
                components.at(n)->drawComponentUI();
            }
        } else {
            components.at(n)->drawComponentUI();
        }
            
    }
}


GameObject::GameObject() {
    Name = "GameObject";
}

GameObject::GameObject(GameObject* other) {
    // !
    SHU_GAME_INSTANCE = other->SHU_GAME_INSTANCE;
    SHU_INDEXED_PARENT = other;
    // !
    Name = "GameObject";
    setParent(other);
}

void GameObject::removeChild(char* UID) {
    for(unsigned int n = 0; n < children.size(); n++) {
        if(children.at(n)->UID == UID) {
            children.erase(children.begin() + n);
            break;
        }
    }
}

bool Hierarchy::draw() {
    ImGui::Begin("Hierarchy");
    rootObject->drawHierarchyObject(true);
    
    bool interrupt = false;
    
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("New Object"))
        {
            GameObject newObj = new GameObject();
            newObj.start();
            newObj.setParent(gameInstance->root);
            gameInstance->Debug.engine("Initialized a new GameObject");
            interrupt = true;
        }
        
        if (ImGui::MenuItem("Select Root")) {
            updateSelection(gameInstance->root);
        }

        ImGui::EndPopup();
    }
    
    ImGui::End();
    return interrupt;
}

Game::Game() {
    root = new GameObject();
    root->Name = "root";
    root->disabled = true;
    root->SHU_GAME_INSTANCE = this;
    this->SHU_HIERARCHY = Hierarchy(root);
    this->SHU_HIERARCHY.gameInstance = this;
    width = 800;
    height = 600;
    createWindow(width, height);
    Physics = PhysicsEnvironment();
    
    start();
}

Game::Game(int w, int h) {
    root = new GameObject();
    root->Name = "root";
    root->disabled = true;
    width = w;
    height = h;
    root->SHU_GAME_INSTANCE = this;
    this->SHU_HIERARCHY = Hierarchy(root);
    this->SHU_HIERARCHY.gameInstance = this;
    createWindow(w, h);
    start();
}

void Game::start() {
    root->start();
}

void Game::update(float deltaTime) {
    Physics.step(deltaTime);
    root->update(deltaTime);
}

void Game::loop() {
    run = true;
    double currentTime = 0;
    double lastTime = 0;
    float deltaTime = 0.0f;
    glfwSwapInterval(0);
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // FBO TEST
    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    
    // The texture we're going to render to
    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, getWindowWidth(), getWindowHeight(), 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // The depth buffer
    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, getWindowWidth(), getWindowHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw "Incomplete Frame Buffer";
    

    
    // END FBO TEST
    Debug.engine("Initial start() call");
    start();
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(this->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    SHU_INSPECTOR.selectedObject = root;
    
    Debug.engine("Initialized loop run");
    
    while (run) {
        currentTime = glfwGetTime();
        deltaTime = float(currentTime - lastTime);
        

        
        // Draw
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(runtime == 0) {
            // IMGUI
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0,0,getWindowWidth(),getWindowHeight()); // Render on the whole framebuffer, complete from the lower left corner to the upper right
            
            update(deltaTime);
            
            glBindFramebuffer(GL_FRAMEBUFFER,0);
            
            // BEGIN IMGUI TEST
            
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New", "CTRL+SHFT+N")) {
                        // New
                    }
                    if (ImGui::MenuItem("Open", "CTRL+SHFT+N")) {
                        // New
                    }
                    if (ImGui::MenuItem("Open Recent", "")) {
                        // New
                    }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Save", "CTRL+S")) {
                        // New
                    }
                    if (ImGui::MenuItem("Build", "CTRL+SHFT+B")) {
                        // New
                    }
                }
                if (ImGui::BeginMenu("Edit"))
                {
                    if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
                    ImGui::Separator();
                    if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                    if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                    if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                    ImGui::EndMenu();
                }
                
                if (ImGui::BeginMenu("Project"))
                {
                    if (ImGui::MenuItem("Run Project", "CTRL+F5")) {
                        runtime = 1;
                        root->start();
                    }
                    
                    if (ImGui::MenuItem("Run Project with Debug Tools", "CTRL+SHFT+F5")) {
                        runtime = 2;
                        root->start();
                    }
                    ImGui::EndMenu();
                }
                
                ImGui::EndMainMenuBar();
                
            }
            
            
            ImGui::Begin("Game");
            {
                ImGui::BeginChild("GameRender");
                ImVec2 wsize = ImGui::GetWindowSize();
                ImGui::Image((ImTextureID) renderedTexture, wsize, ImVec2(0, 1), ImVec2(1, 0));
                ImGui::EndChild();
            }
            ImGui::End();
            
            // Draw windows
            SHU_INSPECTOR.draw();
            if(SHU_HIERARCHY.draw()) {
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                continue;
            }

            Debug.draw();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                
        } else if (runtime == 1) { // COMPILE TIME
            glBindFramebuffer(GL_FRAMEBUFFER,0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            update(deltaTime);
        } else if (runtime == 2) { // Compile with debug
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0,0,getWindowWidth(),getWindowHeight()); // Render on the whole framebuffer, complete from the lower left corner to the upper right
            update(deltaTime);
            glBindFramebuffer(GL_FRAMEBUFFER,0);
            Debug.draw();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        // END IMGUI TEST
        
        // Swap buffers
        glViewport(0, 0, this->getWindowWidth(), this->getWindowHeight());
        
        glfwSwapBuffers(window);
        glfwPollEvents();
         
        // delta-post
        lastTime = currentTime;
        
        // Test for quit
        if(glfwGetKey(window, GLFW_KEY_ESCAPE ) == GLFW_PRESS || glfwWindowShouldClose(window) == 1) {
            run = false;
            glfwTerminate();
            break;
        }
    }
}

int Game::generateID() {
    lastID += 1;
    return lastID;
}

void Game::drawComponentUIs() {
    root->drawUI();
}

void Game::createWindow(int width, int height) {
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return;
    }
    
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
   
    
    window = glfwCreateWindow( width, height, "SHU Core a1.0", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
    
    
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return;
    }
}

void Component::setParent(GameObject *object) {
    object->addComponent(this);
}

void Transform::drawComponentUI() {

        float pos_float3[] = {position.x, position.y, position.z};
        if(ImGui::DragFloat3("position", pos_float3)) {
            position = glm::make_vec3(pos_float3);
        }
        
        float scl_float3[] = {scale.x, scale.y, scale.z};
        if(ImGui::DragFloat3("scale", scl_float3)) {
            scale = glm::make_vec3(scl_float3);
        }
        
        glm::vec3 euler = glm::eulerAngles(orientation);
        float rot_float3[] = {radians(euler.x), radians(euler.y), radians(euler.z)};
        if(ImGui::DragFloat3("rotation", rot_float3)) {
            orientation = quat(vec3(radians(rot_float3[0]), radians(rot_float3[1]), radians(rot_float3[2])));
        }
}

Transform::Transform() {
    position = vec3(0.0f, 0.0f, 0.0f);
    orientation = glm::quat(vec3(0.0f, 0.0f, 0.0f));
    scale = vec3(1.0f, 1.0f, 1.0f);
}

Transform::Transform(vec3 pos, glm::quat orient) {
    position = pos;
    orientation = orient;
    scale = vec3(1.0f, 1.0f, 1.0f);
}

Transform::Transform(vec3 pos, glm::quat orient, vec3 scle) {
    position = pos;
    orientation = orient;
    scale = scle;
}

void Transform::reset() {
    position = vec3(0, 0, 0);
    orientation = quat(vec3(0, 0, 0));
    scale = vec3(1, 1, 1);
}

// FUNCTIONS

/**
 Checks if it's OK to remove a component with name "componentName" from GameObject "object"
 @param {GameObject} object - Object to check
 @param {char*} componentName - Component to remove
 @returns {bool}
 */
bool SHU_deletedDependencyCheck(GameObject* object, char* componentName) {
    for(int n = 0; n < object->components.size(); n++) {
        std::vector<Component*> deps = object->components.at(n)->getDependencies();
        for(int j = 0; j < deps.size(); j++) {
            if(componentName == deps.at(j)->getName()) {
                return false;
            }
        }
    }
    return true;
}

std::vector<Component*> SHU_getDependencyConflictsOnDelete(GameObject* object, char* componentName) {
    std::vector<Component*> missingDeps;
    for(int n = 0; n < object->components.size(); n++) {
        std::vector<Component*> deps = object->components.at(n)->getDependencies();
        for(int j = 0; j < deps.size(); j++) {
            if(componentName == deps.at(j)->getName()) {
                missingDeps.push_back(object->components.at(n));
            }
        }
    }
    return missingDeps;
}
