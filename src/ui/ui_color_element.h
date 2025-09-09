#pragma once

#include "recomp_input.h"
#include "elements/ui_element.h"
#include "elements/ui_theme.h"

namespace recompui {

class ElementColor : public Rml::Element {
private:
    recompui::Element *element;
public:
    ElementColor(const Rml::String& tag);
    void OnAttributeChange(const Rml::ElementAttributes& changed_attributes) override;
    virtual ~ElementColor();
    void check_color_attribute(const std::string &attr, const std::string &check, theme::color color);
};

} // namespace recompui
