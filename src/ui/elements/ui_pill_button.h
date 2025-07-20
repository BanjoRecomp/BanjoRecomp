#pragma once

#include "ui_element.h"
#include "ui_button.h"
#include "ui_svg.h"

namespace recompui {
    enum class PillButtonSize {
        Mini = 20,   // 20x20 (Inline with body text)
        Small = 32,  // 32x32
        Medium = 48, // 48x48
        Large = 56,  // 56x56
        Default = PillButtonSize::Large,
        XLarge = 72,  // 72x72
    };

    class PillButton : public Element {
    protected:
        ButtonStyle style = ButtonStyle::Primary;
        PillButtonSize size = PillButtonSize::Default;
        Style hover_style;
        Style focus_style;
        Style disabled_style;
        Style hover_disabled_style;
        std::list<std::function<void()>> pressed_callbacks;
        Svg *svg = nullptr;
        bool has_override_text_color = false;

        // Element overrides.
        virtual void process_event(const Event &e) override;
        std::string_view get_type_name() override { return "PillButton"; }
    public:
        PillButton(Element *parent, const std::string &text, const std::string &svg_src, ButtonStyle style, PillButtonSize size = PillButtonSize::Default);
        void add_pressed_callback(std::function<void()> callback);
        Style* get_hover_style() { return &hover_style; }
        Style* get_focus_style() { return &focus_style; }
        Style* get_disabled_style() { return &disabled_style; }
        Style* get_hover_disabled_style() { return &hover_disabled_style; }
        void apply_button_style(ButtonStyle new_style);
        void apply_theme_style(recompui::theme::color color, bool is_basic = false, bool with_text = false);
    };

} // namespace recompui
