//
//  util_gui.cpp
//  CSAT OpenGL
//
//  Created by Michael Karpov on 9/27/21.
//

#include "util_gui.hpp"

IMElement::IMElement() {
    canvas = nullptr;
}

IMElement::IMElement(IMCanvas* other) {
    canvas = other;
}

void IMCanvas::addElement(IMElement* element) {
    elements.push_back(element);
}
