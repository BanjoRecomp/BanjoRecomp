#include "overloaded.h"
#include "ui_radio.h"
#include "../ui_utils.h"

namespace recompui {

    // RadioOption

    RadioOption::RadioOption(Element *parent, std::string_view name, uint32_t index) : Element(parent, Events(EventType::MouseButton, EventType::Click, EventType::Focus, EventType::Hover, EventType::Enable, EventType::Update), "label", true) {
        this->index = index;

        enable_focus();
        set_text(name);
        set_cursor(Cursor::Pointer);
        set_typography(theme::Typography::LabelSM);
        set_border_color(theme::color::Text, 0);
        set_border_bottom_width(theme::border::width);
        set_color(theme::color::TextInactive);
        set_padding_top(8.0f);
        set_padding_bottom(8.0f - theme::border::width);
        set_text_transform(TextTransform::Uppercase);
        set_height_auto();
        set_opacity(1.0f);
        hover_style.set_color(theme::color::WhiteA80);
        checked_style.set_color(theme::color::White);
        checked_style.set_border_color(theme::color::Text);
        pulsing_style.set_border_color(theme::color::SecondaryA80);
        disabled_style.set_color(theme::color::TextInactive);
        disabled_style.set_opacity(0.5f);
        disabled_style.set_cursor(Cursor::None);

        add_style(&hover_style, { hover_state });
        add_style(&checked_style, { checked_state });
        add_style(&pulsing_style, { focus_state });
        add_style(&disabled_style, { disabled_state });
    }

    void RadioOption::set_pressed_callback(std::function<void(uint32_t)> callback) {
        pressed_callback = callback;
    }

    void RadioOption::set_focus_callback(std::function<void(bool)> callback) {
        focus_callback = callback;
    }

    void RadioOption::set_selected_state(bool enable) {
        set_style_enabled(checked_state, enable);
        set_as_primary_focus(enable);
    }

    void RadioOption::process_event(const Event &e) {
        switch (e.type) {
        case EventType::Click:
            pressed_callback(index);
            break;
        case EventType::Hover:
            set_style_enabled(hover_state, std::get<EventHover>(e.variant).active);
            break;
        case EventType::Enable: {
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
            break;
        }
        case EventType::Focus:
            {
                bool active = std::get<EventFocus>(e.variant).active;
                set_style_enabled(focus_state, active);
                if (active) {
                    queue_update();
                }
                if (focus_callback != nullptr) {
                    focus_callback(active);
                }
            }
            break;
        case EventType::Update:
            if (is_style_enabled(focus_state)) {
                pulsing_style.set_color(recompui::get_pulse_color(750));
                apply_styles();
                queue_update();
            }
            break;
        default:
            break;
        }
    }

    // Radio

    void Radio::set_index_internal(uint32_t index, bool setup, bool trigger_callbacks) {
        if (this->index != index || setup) {
            if (this->index < static_cast<uint32_t>(options.size())) {
                options[this->index]->set_selected_state(false);
            }
            if (index < static_cast<uint32_t>(options.size())) {
                options[index]->set_selected_state(true);
            }
            this->index = index;

            if (trigger_callbacks) {
                for (const auto &function : index_changed_callbacks) {
                    function(index);
                }
            }
        }
    }

    void Radio::option_selected(uint32_t index) {
        set_index_internal(index, false, true);
    }
    
    void Radio::set_input_value(const ElementValue& val) {
        std::visit(overloaded {
            [this](uint32_t u) { set_index(u); }, 
            [this](float f) { set_index(f); }, 
            [this](double d) { set_index(d); },
            [](std::monostate) {}
        }, val);
    }

    Radio::Radio(Element *parent) : Container(parent, FlexDirection::Row, JustifyContent::FlexStart, Events(EventType::Focus, EventType::Update, EventType::Enable)) {
        set_gap(24.0f);
        set_align_items(AlignItems::FlexStart);
        // enable_focus();
        set_as_navigation_container(NavigationType::Horizontal);
    }

    void Radio::process_event(const Event &e) {
        switch (e.type) {
        case EventType::Focus:
            if (!options.empty()) {
                // if (std::get<EventFocus>(e.variant).active) {
                //     blur();
                //     queue_child_focus();
                // }
                if (focus_callback != nullptr) {
                    focus_callback(std::get<EventFocus>(e.variant).active);
                }
            }
            break;
        case EventType::Update:
            if (child_focus_queued) {
                child_focus_queued = false;
                if (index < static_cast<uint32_t>(options.size())) {
                    options[index]->focus();
                } else if (!options.empty()) {
                    options.front()->focus();
                }
            }
            break;
        case EventType::Enable:
            {
                bool active = std::get<EventEnable>(e.variant).active;
                for (auto &option : options) {
                    option->set_enabled(active);
                }
            }
            break;
        }
    }

    Radio::~Radio() {

    }

    void Radio::add_option(std::string_view name) {
        RadioOption *option = get_current_context().create_element<RadioOption>(this, name, uint32_t(options.size()));
        option->set_pressed_callback([this](uint32_t index){ options[index]->focus(); option_selected(index); });
        option->set_focus_callback([this](bool active) {
            if (focus_callback != nullptr) {
                focus_callback(active);
            }
        });
        options.emplace_back(option);

        // The first option was added, select it.
        if (options.size() == 1) {
            set_index_internal(0, true, false);
        }
        // At least one other option already existed, so set up navigation.
        else {
            options[options.size() - 2]->set_nav(NavDirection::Right, options[options.size() - 1]);
            options[options.size() - 1]->set_nav(NavDirection::Left, options[options.size() - 2]);
        }
    }

    void Radio::set_index(uint32_t index) {
        set_index_internal(index, false, false);
    }

    uint32_t Radio::get_index() const {
        return index;
    }

    void Radio::add_index_changed_callback(std::function<void(uint32_t)> callback) {
        index_changed_callbacks.emplace_back(callback);
    }

    void Radio::set_focus_callback(std::function<void(bool)> callback) {
        focus_callback = callback;
    }
    
    void Radio::set_nav_auto(NavDirection dir) {
        Element::set_nav_auto(dir);
        if (!options.empty()) {
            switch (dir) {
                case NavDirection::Up:
                case NavDirection::Down:
                    for (Element* e : options) {
                        e->set_nav_auto(dir);
                    }
                    break;
                case NavDirection::Left:
                    options.front()->set_nav_auto(dir);
                    break;
                case NavDirection::Right:
                    options.back()->set_nav_auto(dir);
                    break;
            }
        }
    }

    void Radio::set_nav_none(NavDirection dir) {
        Element::set_nav_none(dir);
        if (!options.empty()) {
            switch (dir) {
                case NavDirection::Up:
                case NavDirection::Down:
                    for (Element* e : options) {
                        e->set_nav_none(dir);
                    }
                    break;
                case NavDirection::Left:
                    options.front()->set_nav_none(dir);
                    break;
                case NavDirection::Right:
                    options.back()->set_nav_none(dir);
                    break;
            }
        }
    }

    void Radio::set_nav(NavDirection dir, Element* element) {
        Element::set_nav(dir, element);
        if (!options.empty()) {
            switch (dir) {
                case NavDirection::Up:
                case NavDirection::Down:
                    for (Element* e : options) {
                        e->set_nav(dir, element);
                    }
                    break;
                case NavDirection::Left:
                    options.front()->set_nav(dir, element);
                    break;
                case NavDirection::Right:
                    options.back()->set_nav(dir, element);
                    break;
            }
        }
    }

    void Radio::set_nav_manual(NavDirection dir, const std::string& target) {
        Element::set_nav_manual(dir, target);
        if (!options.empty()) {
            switch (dir) {
                case NavDirection::Up:
                case NavDirection::Down:
                    for (Element* e : options) {
                        e->set_nav_manual(dir, target);
                    }
                    break;
                case NavDirection::Left:
                    options.front()->set_nav_manual(dir, target);
                    break;
                case NavDirection::Right:
                    options.back()->set_nav_manual(dir, target);
                    break;
            }
        }
    }

};
