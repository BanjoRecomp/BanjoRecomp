#include "ui_toggle.h"
#include "ui_pseudo_border.h"
#include "../ui_utils.h"

#include <cassert>

#include <ultramodern/ultramodern.hpp>

namespace recompui {

    struct ToggleSizing {
        float width;
        float height;
        float floater_width;
        float floater_height;
        float floater_margin;

        ToggleSizing(float width, float height, float floater_margin)
            : width(width), height(height),
              floater_width(height - floater_margin * 2.0f),
              floater_height(height - floater_margin * 2.0f),
              floater_margin(floater_margin) {}

        float left_offset(bool checked) const {
            return checked ? width - floater_width - floater_margin : floater_margin;
        }
    };

    static const ToggleSizing medium_toggle_sizing = ToggleSizing(64.0f, 36.0f, 6.0f);
    static const ToggleSizing large_toggle_sizing = ToggleSizing(144.0f, 72.0f, 8.0f);

    static const ToggleSizing &get_toggle_sizing(ToggleSize size) {
        switch (size) {
            case ToggleSize::Medium:
                return medium_toggle_sizing;
            case ToggleSize::Large:
                return large_toggle_sizing;
            default:
                assert(false && "Invalid toggle size");
        }
    }

    Toggle::Toggle(Element *parent, ToggleSize size) : Element(parent, Events(EventType::Click, EventType::Focus, EventType::Hover, EventType::Enable), "button") {
        this->size = size;
        const ToggleSizing &sizing = get_toggle_sizing(size);
        enable_focus();

        set_position(Position::Relative);

        set_width(sizing.width);
        set_height(sizing.height);
        set_border_radius(sizing.height * 0.5f);
        set_opacity(0.9f);
        set_cursor(Cursor::Pointer);
        // set_border_width(theme::border::width);
        set_border_color(theme::color::BW50);
        set_background_color(theme::color::Transparent);

        checked_style.set_border_color(theme::color::Primary);
        inner_border_styles.checked.set_border_color(theme::color::Primary);

        hover_style.set_border_color(theme::color::BW50);
        inner_border_styles.hover.set_border_color(theme::color::BW50);
        hover_style.set_background_color(theme::color::WhiteA5);
    
        focus_style.set_border_color(theme::color::BW50);
        inner_border_styles.focus.set_border_color(theme::color::BW50);
        focus_style.set_background_color(theme::color::WhiteA5);
    
        checked_hover_style.set_border_color(theme::color::Primary);
        inner_border_styles.checked_hover.set_border_color(theme::color::Primary);
        checked_hover_style.set_background_color(theme::color::PrimaryA30);

        checked_focus_style.set_border_color(theme::color::Primary);
        inner_border_styles.checked_focus.set_border_color(theme::color::Primary);
        checked_focus_style.set_background_color(theme::color::PrimaryA30);

        disabled_style.set_border_color(theme::color::BW50, 128);
        inner_border_styles.disabled.set_border_color(theme::color::BW50, 128);

        checked_disabled_style.set_border_color(theme::color::PrimaryD, 128);
        inner_border_styles.checked_disabled.set_border_color(theme::color::PrimaryD, 128);

        add_style(&checked_style, checked_state);
        add_style(&hover_style, hover_state);
        add_style(&focus_style, focus_state);
        add_style(&checked_hover_style, { checked_state, hover_state });
        add_style(&checked_focus_style, { checked_state, focus_state });
        add_style(&disabled_style, disabled_state);
        add_style(&checked_disabled_style, { checked_state, disabled_state });

        ContextId context = get_current_context();

        focus_border = context.create_element<FocusBorder>(this);
        focus_border->set_border_radius((sizing.height + theme::border::width * 4.0f + theme::border::width * 4.0f) * 0.5f);

        inner_border = context.create_element<PseudoBorder>(this);
        inner_border->set_border_radius((sizing.height - theme::border::width * 2.0f) * 0.5f);
        inner_border->set_border_color(theme::color::BW50);
        inner_border->add_style(&inner_border_styles.checked, checked_state);
        inner_border->add_style(&inner_border_styles.hover, hover_state);
        inner_border->add_style(&inner_border_styles.focus, focus_state);
        inner_border->add_style(&inner_border_styles.checked_hover, { checked_state, hover_state });
        inner_border->add_style(&inner_border_styles.checked_focus, { checked_state, focus_state });
        inner_border->add_style(&inner_border_styles.disabled, disabled_state);
        inner_border->add_style(&inner_border_styles.checked_disabled, { checked_state, disabled_state });

        floater = context.create_element<Element>(this);
        floater->set_position(Position::Absolute);
        floater->set_top(50.0f, Unit::Percent);
        floater->set_translate_2D(0, -50.0f, Unit::Percent);
        floater->set_width(sizing.floater_width);
        floater->set_height(sizing.floater_height);
        floater->set_border_radius(sizing.floater_height * 0.5f);
        floater->set_background_color(theme::color::TextDim);
        floater_checked_style.set_background_color(theme::color::Primary);
        floater_disabled_style.set_background_color(theme::color::TextDim, 128);
        floater_disabled_checked_style.set_background_color(theme::color::PrimaryD, 128);
        floater->add_style(&floater_checked_style, checked_state);
        floater->add_style(&floater_disabled_style, disabled_state);
        floater->add_style(&floater_disabled_checked_style, { checked_state, disabled_state });

        set_checked_internal(false, false, true, false);
    }

    void Toggle::set_all_style_enabled(std::string_view style_name, bool enabled) {
        set_style_enabled(style_name, enabled);
        inner_border->set_style_enabled(style_name, enabled);
        floater->set_style_enabled(style_name, enabled);
    }

    void Toggle::set_checked_internal(bool checked, bool animate, bool setup, bool trigger_callbacks) {
        if (this->checked != checked || setup) {
            this->checked = checked;

            if (animate) {
                last_time = ultramodern::time_since_start();
                queue_update();
            }
            else {
                floater_left = floater_left_target();
            }

            floater->set_left(floater_left, Unit::Dp);

            if (trigger_callbacks) {
                for (const auto &function : checked_callbacks) {
                    function(checked);
                }
            }

            set_all_style_enabled(checked_state, checked);
        }
    }

    float Toggle::floater_left_target() const {
        return get_toggle_sizing(size).left_offset(checked);
    }

    void Toggle::process_event(const Event &e) {
        switch (e.type) {
        case EventType::Click:
            if (is_enabled()) {
                set_checked_internal(!checked, true, false, true);
            }

            break;
        case EventType::Hover: {
            bool hover_active = std::get<EventHover>(e.variant).active && is_enabled();
            set_all_style_enabled(hover_state, hover_active);
            break;
        }
        case EventType::Focus: {
            bool focus_active = std::get<EventFocus>(e.variant).active;
            set_all_style_enabled(focus_state, focus_active);
            queue_update();
            break;
        }
        case EventType::Enable: {
            bool enable_active = std::get<EventEnable>(e.variant).active;
            set_all_style_enabled(disabled_state, !enable_active);
            if (enable_active) {
                set_cursor(Cursor::Pointer);
                set_focusable(true);
            }
            else {
                set_cursor(Cursor::None);
                set_focusable(false);
            }
            break;
        }
        case EventType::Update: {
            std::chrono::high_resolution_clock::duration now = ultramodern::time_since_start();
            float delta_time = std::max(std::chrono::duration<float>(now - last_time).count(), 0.0f);
            last_time = now;
            bool should_queue_update = false;

            constexpr float dp_speed = 740.0f;
            const float target = floater_left_target();
            if (target < floater_left) {
                floater_left += std::max(-dp_speed * delta_time, target - floater_left);
            }
            else {
                floater_left += std::min(dp_speed * delta_time, target - floater_left);
            }

            if (abs(target - floater_left) < 1e-4f) {
                floater_left = target;
            }
            else {
                should_queue_update = true;
            }

            floater->set_left(floater_left, Unit::Dp);

            if (should_queue_update) {
                queue_update();
            }

            break;
        }
        default:
            break;
        }
    }

    void Toggle::set_checked(bool checked) {
        set_checked_internal(checked, false, false, false);
    }

    bool Toggle::is_checked() const {
        return checked;
    }

    void Toggle::add_checked_callback(std::function<void(bool)> callback) {
        checked_callbacks.emplace_back(callback);
    }
};
