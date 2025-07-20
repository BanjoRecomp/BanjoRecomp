#include "ui_binding_button.h"
#include "ui_theme.h"
#include <ultramodern/ultramodern.hpp>

namespace recompui {
    static const float padding = 8.0f;

    BindingButton::BindingButton(Element *parent, const std::string &mapped_binding) : Element(parent, Events(EventType::Click, EventType::Hover, EventType::Enable, EventType::Focus), "button") {
        this->mapped_binding = mapped_binding;

        enable_focus();
        apply_sizing_styling(this);

        set_border_color(theme::color::WhiteA5);
        set_background_color(theme::color::WhiteA5);
        set_color(theme::color::TextDim);

        set_cursor(Cursor::Pointer);

        focus_style.set_border_color(theme::color::White);
        focus_style.set_background_color(theme::color::WhiteA30);
        focus_style.set_color(theme::color::TextActive);
        hover_style.set_border_color(theme::color::WhiteA80);
        hover_style.set_background_color(theme::color::WhiteA20);
        hover_style.set_color(theme::color::Text);

        disabled_style.set_color(theme::color::TextDim);
        disabled_style.set_opacity(0.5f);
        disabled_style.set_cursor(Cursor::None);

        add_style(&hover_style, hover_state);
        add_style(&focus_style, focus_state);
        add_style(&disabled_style, disabled_state);
        add_style(&hover_disabled_style, { hover_state, disabled_state });

        ContextId context = get_current_context();

        bound_text_el = context.create_element<Element>(this, 0, "div", true);
        bound_text_el->set_text(mapped_binding);
        apply_binding_style();

        recording_parent = context.create_element<Element>(this);
        recording_circle = context.create_element<Element>(recording_parent);
        recording_edge = context.create_element<Element>(recording_parent);
        recording_svg = context.create_element<Svg>(recording_edge, "icons/RecordBorder.svg");
        apply_recording_style();
    }

    void BindingButton::apply_sizing_styling(Element *el) {
        const float height = 56.0f - (theme::border::width * 2.0f);
        el->set_display(Display::Flex);
        el->set_position(Position::Relative);

        el->set_flex_grow(1.0f);
        el->set_flex_shrink(1.0f);
        el->set_flex_basis(100.0f, recompui::Unit::Percent);

        el->set_align_items(AlignItems::Center);
        el->set_justify_content(JustifyContent::Center);

        el->set_width(100.0f, recompui::Unit::Percent);
        el->set_height(height);
        el->set_padding(padding);

        el->set_border_width(theme::border::width);
        el->set_border_radius(theme::border::radius_sm);
        el->set_border_color(theme::color::Transparent);
        el->set_background_color(theme::color::Transparent);
    }

    void BindingButton::apply_recording_style() {
        recording_parent->set_display(Display::Flex);
        recording_parent->set_position(Position::Absolute);

        recording_parent->set_top(0.0f);
        recording_parent->set_left(0.0f);
        recording_parent->set_right(0.0f);
        recording_parent->set_bottom(0.0f);

        recording_parent->set_align_items(AlignItems::Center);
        recording_parent->set_justify_content(JustifyContent::Center);
        recording_parent->set_opacity(0);

        const float circle_size = 24;
        recording_circle->set_width(circle_size);
        recording_circle->set_height(circle_size);
        recording_circle->set_border_radius(circle_size * 0.5f);
        recording_circle->set_background_color(theme::color::Danger);

        const float edge_size = 36;
        recording_edge->set_position(Position::Absolute);
        recording_edge->set_top(50.0f, recompui::Unit::Percent);
        recording_edge->set_left(50.0f, recompui::Unit::Percent);
        recording_edge->set_width(edge_size);
        recording_edge->set_height(edge_size);
        recording_edge->set_translate_2D(-50.0f, -50.0f, recompui::Unit::Percent);

        recording_svg->set_width(edge_size);
        recording_svg->set_height(edge_size);
        recording_svg->set_image_color(theme::color::Danger);
    }

    void BindingButton::apply_binding_style() {
        bound_text_el->set_font_family("promptfont");
        bound_text_el->set_font_size(40.0f);
        bound_text_el->set_font_style(FontStyle::Normal);
        bound_text_el->set_font_weight(400);
        bound_text_el->set_line_height(40.0f);
        bound_text_el->set_opacity(1);
    }

    void BindingButton::add_pressed_callback(std::function<void()> callback) {
        pressed_callbacks.push_back(std::move(callback));
    }

    void BindingButton::set_binding(const std::string &binding) {
        this->mapped_binding = binding;
        bound_text_el->set_text(mapped_binding);
    }

    void BindingButton::set_is_binding(bool is_binding) {
        this->is_binding = is_binding;

        if (is_binding) {
            bound_text_el->set_opacity(0);
            recording_parent->set_opacity(1);
            queue_update();
        } else {
            bound_text_el->set_opacity(1);
            recording_parent->set_opacity(0);
        }
    }

    void BindingButton::process_event(const Event &e) {
        switch (e.type) {
        case EventType::Click:
            if (is_enabled()) {
                for (const auto &function : pressed_callbacks) {
                    function();
                }
                set_is_binding(!is_binding);
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
            {
                if (is_binding == false) {
                    break;
                }
                queue_update();
                std::chrono::high_resolution_clock::duration since_start = ultramodern::time_since_start();
                auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_start).count();
                const float loop_length_seconds = 1.5f;
                float t = static_cast<float>(millis) / (loop_length_seconds * 1000.0f);
                float sine_time = sinf(t * 2.0f * 3.14159f);
                float scale = 1.0f + ((sine_time * 0.15f / 2.0f) - 0.15f);
                recording_circle->set_scale_2D(scale, scale);
            }
            break;
        default:
            assert(false && "Unknown event type.");
            break;
        }
    }

} // namespace recompui
