#pragma once

#include "ui_element.h"

namespace recompui {

    class PseudoBorder : public Element {
    protected:
        std::string_view get_type_name() override { return "PseudoBorder"; }
    public:
        PseudoBorder(Element *parent, bool inset = true, float border_width = 0, float border_outset_distance = 0);
    };

    class FocusBorder : public PseudoBorder {
    protected:
        std::string_view get_type_name() override { return "FocusBorder"; }
        virtual void process_event(const Event &e) override;
    public:
        FocusBorder(Element *parent, bool extra_dist = false);
    };

} // namespace recompui
