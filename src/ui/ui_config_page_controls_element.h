#pragma once

#include "recomp_input.h"
#include "elements/ui_config_page.h"
#include "ui_config_page_controls.h"

namespace recompui {

extern ConfigPageControls *controls_page;

class ElementConfigPageControls : public Rml::Element {
public:
    ElementConfigPageControls(const Rml::String& tag);
    virtual ~ElementConfigPageControls();
};

} // namespace recompui
