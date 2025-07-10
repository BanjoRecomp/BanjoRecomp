#pragma once

#include "ui_element.h"

namespace recompui {

    class Svg : public Element {
    protected:
        std::string_view get_type_name() override { return "Svg"; }
    public:
        Svg(Element *parent, std::string_view src);
    };

} // namespace recompui
