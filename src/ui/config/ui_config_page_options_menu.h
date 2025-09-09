#pragma once

#include "ui_config_common.h"
#include "ui_config_option.h"
#include "../elements/ui_config_page.h"
#include "../elements/ui_modal.h"
#include "../elements/ui_button.h"
#include "librecomp/config.hpp"
#include "librecomp/mods.hpp"

namespace recompui {
    class ConfigPageOptionsMenu : public ConfigPage {
    protected:
        recomp::config::Config *config;
        ConfigOptionPropertyCallbacks callbacks;
        set_option_value_callback set_option_value;
        std::function<void()> on_apply_callback;
        bool requires_confirmation = false;
        bool is_internal = false;

        check_page_dirty_callback check_page_dirty = nullptr;
        get_config_option_updates_callback get_config_option_updates = nullptr;
        clear_config_option_updates_callback clear_config_option_updates = nullptr;
        
        std::vector<NovaConfigOptionElement*> config_option_elements;
        Element *description_container = nullptr;
        std::string description_option_id = "";
        Button *apply_button = nullptr;

        virtual void process_event(const Event &e) override;
        std::string_view get_type_name() override { return "ConfigPageOptionsMenu"; }
    private:
        on_option_hover_callback bound_on_option_hover;
        set_option_value_callback bound_set_option_value;

        void render_config_options();
        void render_confirmation_footer();
        void on_option_hover(const std::string &option_id);
        void on_set_option_value(const std::string &option_id, recomp::config::ConfigValueVariant value);
        NovaConfigOptionElement* get_element_from_option_id(const std::string &option_id);
        void perform_option_render_updates();
        void apply_option_navigation();
        Element* get_navigation_element(int cur_index, int direction);
        void set_description_text(const std::string &text);
    public:
        ConfigPageOptionsMenu(
            Element *parent,
            recomp::config::Config *config,
            const ConfigOptionPropertyCallbacks &callbacks,
            set_option_value_callback set_option_value,
            bool requires_confirmation = false,
            bool is_internal = false
        );
        virtual ~ConfigPageOptionsMenu() = default;

        void set_check_page_dirty_callback(check_page_dirty_callback callback) { check_page_dirty = callback; }
        void set_on_apply_callback(std::function<void()> callback);
        void set_get_config_option_updates_callback(get_config_option_updates_callback callback) {
            get_config_option_updates = callback;
            if (clear_config_option_updates != nullptr) {
                queue_update();
            }
        }
        void set_clear_config_option_updates_callback(clear_config_option_updates_callback callback) {
            clear_config_option_updates = callback;
            if (get_config_option_updates != nullptr) {
                queue_update();
            }
        }
    };
} // namespace recompui
