#pragma once

#include "ui_element.h"
#include "ui_button.h"
#include "ui_svg.h"

namespace recompui {
    enum class IconButtonSize {
        Mini = 20,   // 20x20 (Inline with body text)
        Small = 32,  // 32x32
        Medium = 48, // 48x48
        Large = 56,  // 56x56
        Default = IconButtonSize::Large,
        XLarge = 72,  // 72x72
    };

    class IconButton : public Element {
    protected:
        ButtonStyle style = ButtonStyle::Primary;
        IconButtonSize size = IconButtonSize::Default;
        Style hover_style;
        Style focus_style;
        Style disabled_style;
        Style hover_disabled_style;
        std::list<std::function<void()>> pressed_callbacks;
        Svg *svg;

        // Element overrides.
        virtual void process_event(const Event &e) override;
        std::string_view get_type_name() override { return "IconButton"; }
    public:
        IconButton(Element *parent, const std::string &svg_src, ButtonStyle style, IconButtonSize size = IconButtonSize::Default);
        void add_pressed_callback(std::function<void()> callback);
        Style* get_hover_style() { return &hover_style; }
        Style* get_focus_style() { return &focus_style; }
        Style* get_disabled_style() { return &disabled_style; }
        Style* get_hover_disabled_style() { return &hover_disabled_style; }
        void apply_button_style(ButtonStyle new_style);
    private:
        void apply_theme_style(recompui::ThemeColor color, bool is_basic = false);
    };

} // namespace recompui
