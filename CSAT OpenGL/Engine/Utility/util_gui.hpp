//
//  util_gui.hpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 9/27/21.
//

#ifndef util_gui_hpp
#define util_gui_hpp

#include <stdio.h>
#include "engine_base.hpp"
#include "engine_render.hpp"

class IMCanvas;

/**
 Base class for all Imgui elements that will be rendered as part of the scene
 */
class IMElement {
    public:
    IMCanvas* canvas;
    IMElement();
    IMElement(IMCanvas* other);
    virtual void render() {};
};

/**
 Handles the rendering of GUI elements using ImGui. Other Gui solutions will be avaliable later
 */
class IMCanvas : public Component {
    std::vector<IMElement*> elements;
    int renderTarget;
    
    public:
    IMCanvas();
    virtual void update(float deltaTime) override;
    virtual void start() override;
    
    void setRenderTarget(int t) {renderTarget = t;};
    void addElement(IMElement* element);
};



#endif /* util_gui_hpp */
