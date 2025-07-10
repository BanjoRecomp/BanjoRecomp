#include "ui_svg.h"

#include <cassert>

namespace recompui {

    Svg::Svg(Element *parent, std::string_view src) : Element(parent, 0, "svg") {
        set_src(src);
    }

};
