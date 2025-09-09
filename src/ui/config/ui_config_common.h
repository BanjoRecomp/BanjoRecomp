#pragma once
#include "librecomp/config.hpp"
#include "librecomp/mods.hpp"

namespace recompui {
    // Callbacks for render updates relating to config option changes.
    using get_config_option_updates_callback = std::function<std::vector<recomp::config::ConfigOptionUpdateContext>()>;
    using clear_config_option_updates_callback = std::function<void()>;
    using check_page_dirty_callback = std::function<bool()>;

    using set_option_value_callback = std::function<void(const std::string &option_id, recomp::config::ConfigValueVariant value)>;
    // Callbacks for various config option properties
    using get_option_value_callback = std::function<const recomp::config::ConfigValueVariant(const std::string &option_id)>;
    using get_option_name_callback = std::function<std::string(const std::string &option_id)>;
    using get_option_description_callback = std::function<std::string(const std::string &option_id)>;
    using get_option_disabled_callback = std::function<bool(const std::string &option_id)>;
    using get_option_hidden_callback = std::function<bool(const std::string &option_id)>;
    using get_option_configuration_callback = std::function<recomp::config::ConfigOptionVariant(const std::string &option_id)>;

    // Text that displays on the right side of the options
    using get_enum_option_details_callback = std::function<std::string(const std::string &option_id)>;
    // Fetches if an individual enum option is disabled
    using get_enum_option_disabled_callback = std::function<bool(const std::string &option_id, uint32_t enum_index)>;

    using on_option_hover_callback = std::function<void(const std::string &option_id)>;

    class ConfigOptionPropertyCallbacks {
    // private:
    //     const get_option_configuration_callback get_configuration;
    public:
        const get_option_configuration_callback get_configuration;
        const get_option_value_callback get_value;
        const get_option_name_callback get_name;
        const get_option_description_callback get_description;
        const get_option_hidden_callback get_hidden;
        const get_option_disabled_callback get_disabled = nullptr;

        const get_enum_option_details_callback get_enum_option_details = nullptr;
        const get_enum_option_disabled_callback get_enum_option_disabled = nullptr;

        ConfigOptionPropertyCallbacks(
            get_option_configuration_callback get_configuration,
            get_option_value_callback get_value,
            get_option_name_callback get_name,
            get_option_description_callback get_description,
            get_option_hidden_callback get_hidden,
            get_option_disabled_callback get_disabled = nullptr,
            get_enum_option_details_callback get_enum_option_details = nullptr,
            get_enum_option_disabled_callback get_enum_option_disabled = nullptr
        ) : 
            get_configuration(get_configuration),
            get_value(get_value),
            get_name(get_name),
            get_description(get_description),
            get_hidden(get_hidden),
            get_disabled(get_disabled),
            get_enum_option_details(get_enum_option_details),
            get_enum_option_disabled(get_enum_option_disabled) {}

        ConfigOptionPropertyCallbacks(const ConfigOptionPropertyCallbacks&) = default;

        recomp::config::ConfigOptionEnum get_enum_configuration(const std::string &option_id) {
            return std::get<recomp::config::ConfigOptionEnum>(get_configuration(option_id));
        }
        recomp::config::ConfigOptionNumber get_number_configuration(const std::string &option_id) {
            return std::get<recomp::config::ConfigOptionNumber>(get_configuration(option_id));
        }
        recomp::config::ConfigOptionString get_string_configuration(const std::string &option_id) {
            return std::get<recomp::config::ConfigOptionString>(get_configuration(option_id));
        }
    };
};
