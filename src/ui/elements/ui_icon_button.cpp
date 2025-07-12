#include "ui_icon_button.h"

#include <cassert>

namespace recompui {    
    IconButton::IconButton(Element *parent, const std::string &svg_src, ButtonStyle style, IconButtonSize size) : Element(parent, Events(EventType::Click, EventType::Hover, EventType::Enable, EventType::Focus), "button") {
        this->style = style;
        this->size = size;
        // Borders add width to the button, so this subtracts from the base size to bring it back to the expected size.
        float float_size_internal = static_cast<float>(size) - (theme::border::width * 2.0f);

        enable_focus();

        set_display(Display::Flex);
        set_align_items(AlignItems::Center);
        set_justify_content(JustifyContent::Center);
        set_width(float_size_internal);
        set_min_width(float_size_internal);
        set_max_width(float_size_internal);
        set_height(float_size_internal);
        set_min_height(float_size_internal);
        set_max_height(float_size_internal);
        set_border_width(theme::border::width);
        set_border_radius(float_size_internal * 0.5f);
        set_border_color(theme::color::Transparent);

        set_cursor(Cursor::Pointer);
        set_color(theme::color::TextDim);
        set_tab_index(TabIndex::Auto);

        hover_style.set_color(theme::color::Text);
        focus_style.set_color(theme::color::Text);
        disabled_style.set_color(theme::color::TextDim);
        disabled_style.set_cursor(Cursor::None);
        disabled_style.set_opacity(0.5f);
        hover_disabled_style.set_color(theme::color::TextDim);

        float icon_size = 0;
        switch (size) {
            case IconButtonSize::Mini:
                icon_size = 16.0f;
                break;
            case IconButtonSize::Small:
                icon_size = 24.0f;
                break;
            case IconButtonSize::Medium:
                icon_size = 32.0f;
                break;
            case IconButtonSize::Large:
            default:
                icon_size = 32.0f;
                break;
            case IconButtonSize::XLarge:
                icon_size = 40.0f;
                break;
        }

        ContextId context = get_current_context();

        svg = context.create_element<Svg>(this, svg_src);
        svg->set_width(icon_size);
        svg->set_color(theme::color::TextDim);

        apply_button_style(style);

        // transition: color 0.05s linear-in-out, background-color 0.05s linear-in-out;
    }

    void IconButton::process_event(const Event &e) {
        switch (e.type) {
        case EventType::Click:
            if (is_enabled()) {
                for (const auto &function : pressed_callbacks) {
                    function();
                }
            }
            break;
        case EventType::Hover:
            {
                bool hover_active = std::get<EventHover>(e.variant).active && is_enabled();
                set_style_enabled(hover_state, hover_active);
                svg->set_color(hover_active ? theme::color::Text : theme::color::TextDim);
            }
            break;
        case EventType::Enable:
            {
                bool enable_active = std::get<EventEnable>(e.variant).active;
                set_style_enabled(disabled_state, !enable_active);
                if (enable_active) {
                    set_cursor(Cursor::Pointer);
                    set_focusable(true);
                    svg->set_color(theme::color::TextDim);
                }
                else {
                    set_cursor(Cursor::None);
                    set_focusable(false);
                    svg->set_color(theme::color::TextDim);
                }
            }
            break;
        case EventType::Focus:
            {
                bool focus_active = std::get<EventFocus>(e.variant).active;
                set_style_enabled(focus_state, focus_active);
                svg->set_color(focus_active ? theme::color::Text : theme::color::TextDim);
            }
            break;
        case EventType::Update:
            break;
        default:
            assert(false && "Unknown event type.");
            break;
        }
    }

    void IconButton::add_pressed_callback(std::function<void()> callback) {
        pressed_callbacks.emplace_back(callback);
    }

    void IconButton::apply_button_style(ButtonStyle new_style) {
        style = new_style;
        switch (style) {
        case ButtonStyle::Primary: {
            apply_theme_style(theme::color::Primary);
            break;
        }
        case ButtonStyle::Secondary: {
            apply_theme_style(theme::color::Secondary);
            break;
        }
        case ButtonStyle::Tertiary: {
            apply_theme_style(theme::color::Text);
            break;
        }
        case ButtonStyle::Success: {
            apply_theme_style(theme::color::Success);
            break;
        }
        case ButtonStyle::Warning: {
            apply_theme_style(theme::color::Warning);
            break;
        }
        case ButtonStyle::Danger: {
            apply_theme_style(theme::color::Danger);
            break;
        }
        case ButtonStyle::Basic: {
            apply_theme_style(theme::color::Text, true);
            break;
        }
        default:
            assert(false && "Unknown button style.");
            break;
        }
    }

    void IconButton::apply_theme_style(recompui::theme::color color, bool is_basic) {
        const uint8_t background_opacity = is_basic ? 0 : 13;
        const uint8_t border_opacity = is_basic ? 0 : 204;
        const uint8_t background_hover_opacity = 77;
        const uint8_t border_hover_opacity = is_basic ? background_hover_opacity : 255;

        set_border_color(color, border_opacity);
        set_background_color(color, background_opacity);
        hover_style.set_border_color(color, border_hover_opacity);
        hover_style.set_background_color(color, background_hover_opacity);
        focus_style.set_border_color(color, border_hover_opacity);
        focus_style.set_background_color(color, background_hover_opacity);

        add_style(&hover_style, hover_state);
        add_style(&focus_style, focus_state);
        add_style(&disabled_style, disabled_state);
        add_style(&hover_disabled_style, { hover_state, disabled_state });
    }
};
