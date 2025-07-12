#pragma once

#include "ui_element.h"
#include "ui_svg.h"

namespace recompui {
    class BindingButton : public Element {
    protected:
        bool is_binding = false;
        std::string mapped_binding; // promptfont representation of the binding.
        
        Element *bound_text_el;
        Element *recording_parent;
        Element *recording_circle;
        Element *recording_edge;
        Svg *recording_svg;

        Style binding_style;
        Style hover_style;
        Style focus_style;
        Style disabled_style;
        Style hover_disabled_style;
        std::list<std::function<void()>> pressed_callbacks;

        // Element overrides.
        virtual void process_event(const Event &e) override;
        std::string_view get_type_name() override { return "BindingButton"; }
    public:
        BindingButton(Element *parent, const std::string &mapped_binding);
        void add_pressed_callback(std::function<void()> callback);
        void set_binding(const std::string &binding);
        void set_is_binding(bool is_binding);
        Style* get_hover_style() { return &hover_style; }
        Style* get_focus_style() { return &focus_style; }
        Style* get_disabled_style() { return &disabled_style; }
        Style* get_hover_disabled_style() { return &hover_disabled_style; }
    private:
        void apply_recording_style();
        void apply_binding_style();
    };

} // namespace recompui
