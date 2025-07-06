#include "ui_button.h"

#include <cassert>

namespace recompui {

    Button::Button(Element *parent, const std::string &text, ButtonStyle style) : Element(parent, Events(EventType::Click, EventType::Hover, EventType::Enable, EventType::Focus), "button", true) {
        this->style = style;

        enable_focus();

        set_text(text);
        set_display(Display::Block);
        set_padding(23.0f);
        set_border_width(1.1f);
        set_border_radius(12.0f);
        set_font_size(28.0f);
        set_letter_spacing(3.08f);
        set_line_height(28.0f);
        set_font_style(FontStyle::Normal);
        set_font_weight(700);
        set_cursor(Cursor::Pointer);
        set_color(ThemeColor::Text);
        set_tab_index(TabIndex::Auto);
        hover_style.set_color(ThemeColor::Text);
        focus_style.set_color(ThemeColor::Text);
        disabled_style.set_color(ThemeColor::TextDim, 128);
        hover_disabled_style.set_color(ThemeColor::Text, 128);

        apply_button_style(style);

        // transition: color 0.05s linear-in-out, background-color 0.05s linear-in-out;
    }

    void Button::process_event(const Event &e) {
        switch (e.type) {
        case EventType::Click:
            if (is_enabled()) {
                for (const auto &function : pressed_callbacks) {
                    function();
                }
            }
            break;
        case EventType::Hover: 
            set_style_enabled(hover_state, std::get<EventHover>(e.variant).active && is_enabled());
            break;
        case EventType::Enable:
            {
                bool enable_active = std::get<EventEnable>(e.variant).active;
                set_style_enabled(disabled_state, !enable_active);
                if (enable_active) {
                    set_cursor(Cursor::Pointer);
                    set_focusable(true);
                }
                else {
                    set_cursor(Cursor::None);
                    set_focusable(false);
                }
            }
            break;
        case EventType::Focus:
            set_style_enabled(focus_state, std::get<EventFocus>(e.variant).active);
            break;
        case EventType::Update:
            break;
        default:
            assert(false && "Unknown event type.");
            break;
        }
    }

    void Button::add_pressed_callback(std::function<void()> callback) {
        pressed_callbacks.emplace_back(callback);
    }

    void Button::apply_button_style(ButtonStyle new_style) {
        style = new_style;
        switch (style) {
        case ButtonStyle::Primary: {
            apply_theme_style(ThemeColor::Primary);
            break;
        }
        case ButtonStyle::Secondary: {
            apply_theme_style(ThemeColor::Secondary);
            break;
        }
        case ButtonStyle::Tertiary: {
            apply_theme_style(ThemeColor::Text);
            break;
        }
        case ButtonStyle::Success: {
            apply_theme_style(ThemeColor::Success);
            break;
        }
        case ButtonStyle::Warning: {
            apply_theme_style(ThemeColor::Warning);
            break;
        }
        case ButtonStyle::Danger: {
            apply_theme_style(ThemeColor::Danger);
            break;
        }
        default:
            assert(false && "Unknown button style.");
            break;
        }
    }

    void Button::apply_theme_style(recompui::ThemeColor color) {
        const uint8_t border_opacity = 204;
        const uint8_t background_opacity = 13;
        const uint8_t border_hover_opacity = 255;
        const uint8_t background_hover_opacity = 77;

        set_border_color(color, border_opacity);
        set_background_color(color, background_opacity);
        hover_style.set_border_color(color, border_hover_opacity);
        hover_style.set_background_color(color, background_hover_opacity);
        focus_style.set_border_color(color, border_hover_opacity);
        focus_style.set_background_color(color, background_hover_opacity);
        disabled_style.set_border_color(color, border_opacity / 4);
        disabled_style.set_background_color(color, background_opacity / 4);
        hover_disabled_style.set_border_color(color, border_hover_opacity / 4);
        hover_disabled_style.set_background_color(color, background_hover_opacity / 4);

        add_style(&hover_style, hover_state);
        add_style(&focus_style, focus_state);
        add_style(&disabled_style, disabled_state);
        add_style(&hover_disabled_style, { hover_state, disabled_state });
    }
};
