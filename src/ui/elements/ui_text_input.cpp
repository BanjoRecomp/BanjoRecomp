#include "ui_text_input.h"

#include <cassert>

namespace recompui {

    void TextInput::process_event(const Event &e) {
        switch (e.type) {
        case EventType::Text: {
            const EventText &event = std::get<EventText>(e.variant);
            text = event.text;

            for (const auto &function : text_changed_callbacks) {
                function(text);
            }

            break;
        }
        case EventType::Focus: {
            const EventFocus &event = std::get<EventFocus>(e.variant);
            if (focus_callback != nullptr) {
                focus_callback(event.active);
            }
            set_style_enabled(focus_state, event.active);
            break;
        }
        default:
            break;
        }
    }
    
    TextInput::TextInput(Element *parent, bool text_visible) : Element(parent, Events(EventType::Text, EventType::Focus), "input") {
        if (!text_visible) {
            set_attribute("type", "password");
        }
        set_min_width(60.0f);
        set_border_color(theme::color::TextA50);
        set_border_width(theme::border::width);
        set_padding(8.0f - theme::border::width);
        set_border_radius(theme::border::radius_sm);
        set_focusable(true);
        set_nav_auto(NavDirection::Up);
        set_nav_auto(NavDirection::Down);
        set_tab_index_auto();
        focus_style.set_border_color(theme::color::Primary);
        add_style(&focus_style, focus_state);
    }

    void TextInput::set_text(std::string_view text) {
        this->text = std::string(text);
        set_attribute("value", this->text);
    }

    const std::string &TextInput::get_text() {
        return text;
    }

    void TextInput::add_text_changed_callback(std::function<void(const std::string &)> callback) {
        text_changed_callbacks.emplace_back(callback);
    }

    void TextInput::set_focus_callback(std::function<void(bool)> callback) {
        focus_callback = callback;
    }
};
