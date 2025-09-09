#include "ui_config_tab_manifest.h"

namespace recompui {

ConfigTab::ConfigTab(recomp::config::Config *config) : TabContext(config->name),
    config(config),
    callbacks(ConfigOptionPropertyCallbacks(
        // get_configuration
        [this](const std::string &option_id) {
            auto schema = this->config->get_config_schema();
            size_t option_index = schema.options_by_id.at(option_id);
            auto &option = schema.options[option_index];
            return option.variant;
        },
        // get_value
        [this](const std::string &option_id) {
            if (this->config->requires_confirmation) {
                return this->config->get_temp_option_value(option_id);
            }
            return this->config->get_option_value(option_id);
        },
        // get_name
        [this](const std::string &option_id) {
            auto schema = this->config->get_config_schema();
            size_t option_index = schema.options_by_id.at(option_id);
            return schema.options[option_index].name;
        },
        // get_description
        [this](const std::string &option_id) {
            auto schema = this->config->get_config_schema();
            size_t option_index = schema.options_by_id.at(option_id);
            return schema.options[option_index].description;
        },
        // get_hidden
        [this](const std::string &option_id) {
            auto schema = this->config->get_config_schema();
            size_t option_index = schema.options_by_id.at(option_id);
            return this->config->is_config_option_hidden(option_index);
        },
        // get_disabled
        [this](const std::string &option_id) {
            auto schema = this->config->get_config_schema();
            size_t option_index = schema.options_by_id.at(option_id);
            return this->config->is_config_option_disabled(option_index);
        },
        // get_enum_option_details
        [this](const std::string &option_id) {
            auto schema = this->config->get_config_schema();
            size_t option_index = schema.options_by_id.at(option_id);
            return this->config->get_enum_option_details(option_index);
        },
        // get_enum_option_disabled
        [this](const std::string &option_id, uint32_t enum_index) {
            auto schema = this->config->get_config_schema();
            size_t option_index = schema.options_by_id.at(option_id);
            return this->config->get_enum_option_disabled(option_index, enum_index);
        }
    )) {};

Element* ConfigTab::create_tab_contents(recompui::ContextId context, Element* parent) {
    ConfigPageOptionsMenu *conf_page = context.create_element<ConfigPageOptionsMenu>(
        parent,
        config,
        callbacks,
        [this](const std::string &option_id, recomp::config::ConfigValueVariant value) {
            this->config->set_option_value(option_id, value);
        },
        config->requires_confirmation,
        true
    );

    conf_page->set_get_config_option_updates_callback([this]() {
        return this->config->get_config_option_updates();
    });

    conf_page->set_clear_config_option_updates_callback([this]() {
        this->config->clear_config_option_updates();
    });

    if (config->requires_confirmation) {
        conf_page->set_on_apply_callback([this]() {
            this->config->save_config();
        });
        conf_page->set_check_page_dirty_callback([this]() {
            return this->config->is_dirty();
        });
    }

    return conf_page;
};

bool ConfigTab::can_be_closed() {
    if (!config->requires_confirmation) {
        return true;
    }

    if (config->is_dirty()) {
        // TODO: show prompt to apply/cancel changes
        return false;
    }

    return true;
}

void ConfigTab::on_tab_close() {
    config->save_config();
}

} // namespace recompui
