#include "ui_config_page_options_menu.h"


namespace recompui {

ConfigPageOptionsMenu::ConfigPageOptionsMenu(
    Element *parent,
    recomp::config::Config *config,
    const ConfigOptionPropertyCallbacks &callbacks,
    set_option_value_callback set_option_value,
    bool requires_confirmation,
    bool is_internal
) : ConfigPage(parent, Events(EventType::Hover, EventType::Update, EventType::MenuAction)),
    config(config),
    callbacks(ConfigOptionPropertyCallbacks(callbacks)),
    set_option_value(set_option_value),
    requires_confirmation(requires_confirmation),
    is_internal(is_internal)
{
    ContextId context = recompui::get_current_context();
    set_as_navigation_container(NavigationType::Vertical);

    render_config_options();
    description_container = context.create_element<Element>(body->get_right(), 0, "p", true);
    description_container->set_typography(theme::Typography::Body);
    description_container->set_line_height(28.0f);
    description_container->set_padding(8.0f);
    set_description_text("");

    if (requires_confirmation) {
        add_footer();
        render_confirmation_footer();
    }
}

void ConfigPageOptionsMenu::set_description_text(const std::string &text) {
    if (description_container) {
        if (is_internal) {
            description_container->set_text_unsafe(text);
        } else {
            description_container->set_text(text);
        }
    }
}

NovaConfigOptionElement* ConfigPageOptionsMenu::get_element_from_option_id(const std::string &option_id) {
    for (auto *element : config_option_elements) {
        if (element->get_option_id() == option_id) {
            return element;
        }
    }
    return nullptr;
}

void ConfigPageOptionsMenu::perform_option_render_updates() {
    using ConfigOptionUpdateType = recomp::config::ConfigOptionUpdateType;
    if (get_config_option_updates == nullptr || clear_config_option_updates == nullptr) {
        return;
    }
    queue_update();

    auto options_updates = get_config_option_updates();
    bool has_updates = !options_updates.empty();
    for (auto &option_update : options_updates) {
        size_t option_index = option_update.option_index;
        auto schema = config->get_config_schema();
        std::string &option_id = schema.options[option_index].id;
        NovaConfigOptionElement* element = get_element_from_option_id(option_id);
        if (element == nullptr) {
            printf("Failed to update conf option: '%s'\n", option_id.c_str());
            continue;
        }
        for (auto &update_type :option_update.updates) {
            switch (update_type) {
                case ConfigOptionUpdateType::Disabled: {
                    element->update_disabled();
                    break;
                }
                case ConfigOptionUpdateType::Hidden: {
                    element->update_hidden();
                    break;
                }
                case ConfigOptionUpdateType::EnumDetails: {
                    NovaConfigOptionEnum *enum_element = static_cast<NovaConfigOptionEnum*>(element);
                    enum_element->update_enum_details();
                    break;
                }
                case ConfigOptionUpdateType::EnumDisabled: {
                    NovaConfigOptionEnum *enum_element = static_cast<NovaConfigOptionEnum*>(element);
                    enum_element->update_enum_disabled();
                    break;
                }
                case ConfigOptionUpdateType::Value: {
                    element->update_value();
                    break;
                }
                case ConfigOptionUpdateType::Description: {
                    if (description_container && description_option_id == option_id) {
                        set_description_text(callbacks.get_description(option_id));
                    }
                    break;
                }
            }
        }
    }
    clear_config_option_updates();
    if (has_updates) {
        // apply_option_navigation();
    }
}

void ConfigPageOptionsMenu::process_event(const Event &e) {
    switch (e.type) {
    case EventType::Hover: {
        bool active = std::get<EventHover>(e.variant).active;
        if (!active) {
            set_description_text("");
        }
        break;
    }
    case EventType::Update: {
        perform_option_render_updates();
        break;
    }
    case EventType::MenuAction: {
        auto action = std::get<EventMenuAction>(e.variant).action;
        if (action == MenuAction::Apply) {
            if (on_apply_callback != nullptr) {
                on_apply_callback();
            }
        }
        break;
    }
    default:
        assert(false && "Unknown event type.");
        break;
    }
}

void ConfigPageOptionsMenu::render_confirmation_footer() {
    ContextId context = recompui::get_current_context();
    footer->set_as_navigation_container(NavigationType::Horizontal);

    apply_button = context.create_element<Button>(footer->get_right(), "Apply", ButtonStyle::Secondary);
    apply_button->set_enabled(false);
    apply_button->set_as_primary_focus();
}

void ConfigPageOptionsMenu::on_set_option_value(const std::string &option_id, recomp::config::ConfigValueVariant value) {
    set_option_value(option_id, value);
    if (check_page_dirty && apply_button != nullptr) {
        apply_button->set_enabled(check_page_dirty());
    }
}

void ConfigPageOptionsMenu::render_config_options() {
    ContextId context = recompui::get_current_context();
    Element *body_left = get_body()->get_left();
    body_left->clear_children();
    body_left->set_padding(0.0f);

    body_left->set_display(Display::Block);
    body_left->set_position(Position::Relative);
    body_left->set_height(100.0f, Unit::Percent);
    {
        auto body_left_scroll = context.create_element<Element>(body_left, 0, "div", false);
        body_left_scroll->set_display(Display::Block);
        body_left_scroll->set_width(100.0f, Unit::Percent);
        body_left_scroll->set_min_height(100.0f, Unit::Percent);
        body_left_scroll->set_max_height(100.0f, Unit::Percent);
        body_left_scroll->set_padding(16.0f);
        body_left_scroll->set_overflow_y(Overflow::Auto);

        config_option_elements.clear();
        bound_on_option_hover = [this](const std::string &option_id) {
            this->on_option_hover(option_id);
        };
        bound_set_option_value = [this](const std::string &option_id, recomp::config::ConfigValueVariant value) {
            this->on_set_option_value(option_id, value);
        };
        auto schema = config->get_config_schema();
        for (size_t i = 0; i < schema.options.size(); i++) {
            auto &config_option = schema.options[i];
            NovaConfigOptionElement *element = nullptr;
            switch (config_option.type) {
                case recomp::config::ConfigOptionType::Enum: {
                    element = context.create_element<NovaConfigOptionEnum>(
                        body_left_scroll,
                        config_option.id,
                        i,
                        &callbacks,
                        bound_set_option_value,
                        bound_on_option_hover
                    );
                    break;
                }
                case recomp::config::ConfigOptionType::Number: {
                    element = context.create_element<NovaConfigOptionNumber>(
                        body_left_scroll,
                        config_option.id,
                        i,
                        &callbacks,
                        bound_set_option_value,
                        bound_on_option_hover
                    );
                    break;
                }
                case recomp::config::ConfigOptionType::String: {
                    element = context.create_element<NovaConfigOptionString>(
                        body_left_scroll,
                        config_option.id,
                        i,
                        &callbacks,
                        bound_set_option_value,
                        bound_on_option_hover
                    );
                    break;
                }
                case recomp::config::ConfigOptionType::Bool: {
                    element = context.create_element<NovaConfigOptionBool>(
                        body_left_scroll,
                        config_option.id,
                        i,
                        &callbacks,
                        bound_set_option_value,
                        bound_on_option_hover
                    );
                    break;
                }
                default: {
                    assert(false && "Unknown config option type");
                }
            };
            element->update_disabled();
            element->update_hidden();
            config_option_elements.push_back(element);
        }

        apply_option_navigation();
    }
}

Element* ConfigPageOptionsMenu::get_navigation_element(int cur_index, int direction) {
    int new_index = cur_index + direction;
    if (new_index < 0) {
        // need to handle getting above element
        return nullptr;
    }
    auto schema = config->get_config_schema();
    if (
        new_index >= static_cast<int>(config_option_elements.size()) ||
        new_index >= static_cast<int>(schema.options.size())) {
        // need to handle getting below element
        return nullptr;
    }
    auto option = schema.options[new_index];
    if (callbacks.get_disabled(option.id) || callbacks.get_hidden(option.id)) {
        return get_navigation_element(new_index, direction);
    }

    return config_option_elements[new_index]->get_focus_element();
}

void ConfigPageOptionsMenu::apply_option_navigation() {
    Element *first_enabled = nullptr;
    auto schema = config->get_config_schema();
    for (size_t i = 0; i < schema.options.size(); i++) {
        auto option = schema.options[i];
        bool this_is_fully_enabled = !callbacks.get_disabled(option.id) && !callbacks.get_hidden(option.id);
        if (first_enabled == nullptr && this_is_fully_enabled) {
            first_enabled = config_option_elements[i];
            config_option_elements[i]->set_as_primary_focus(true);
        } else {
            config_option_elements[i]->set_as_primary_focus(false);
        }

        auto *prev_element = get_navigation_element(i, -1);
        if (prev_element != nullptr) {
            config_option_elements[i]->set_nav(NavDirection::Up, prev_element);
        }
        auto *next_element = get_navigation_element(i, 1);
        if (next_element != nullptr) {
            config_option_elements[i]->set_nav(NavDirection::Down, next_element);
        }
    }
}

void ConfigPageOptionsMenu::on_option_hover(const std::string &option_id) {
    if (description_container) {
        description_option_id = option_id;
        set_description_text(callbacks.get_description(option_id));
    }
}

void ConfigPageOptionsMenu::set_on_apply_callback(std::function<void()> callback) {
    if (apply_button) {
        on_apply_callback = [this, callback]{
            callback();
            if (this->apply_button && this->check_page_dirty) {
                this->apply_button->set_enabled(this->check_page_dirty());
            }
        };
        apply_button->add_pressed_callback(on_apply_callback);
    }
}

} // namespace recompui
