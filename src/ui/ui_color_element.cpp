#include "ui_color_element.h"

namespace recompui {

ElementColor::ElementColor(const Rml::String& tag) : Rml::Element(tag) {
    SetProperty(Rml::PropertyId::Display, Rml::Style::Display::Inline);
}

void ElementColor::check_color_attribute(const std::string &attr, const std::string &check, theme::color color) {
    if (attr == check) {
        recompui::Element this_compat(this);
        this_compat.set_color(color);
    }
}

void ElementColor::OnAttributeChange(const Rml::ElementAttributes& changed_attributes) {
    Rml::Element::OnAttributeChange(changed_attributes);

    for (auto &attr : changed_attributes) {
        std::string name = attr.first;
        check_color_attribute(name, "primary", theme::color::Primary);
        check_color_attribute(name, "secondary", theme::color::Secondary);
        check_color_attribute(name, "warning", theme::color::Warning);
        check_color_attribute(name, "danger", theme::color::Danger);
        check_color_attribute(name, "success", theme::color::Success);
    }
}

ElementColor::~ElementColor() {
}

} // namespace recompui
