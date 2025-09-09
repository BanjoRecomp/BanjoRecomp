#pragma once

#include "ui_config_common.h"
#include "../elements/ui_element.h"
#include "../elements/ui_label.h"
#include "../elements/ui_radio.h"
#include "../elements/ui_text_input.h"
#include "../elements/ui_slider.h"
#include "../elements/ui_toggle.h"

namespace recompui {

    class NovaConfigOptionElement : public Element {
    protected:
        std::string option_id;
        size_t option_index;
        ConfigOptionPropertyCallbacks *callbacks;
        on_option_hover_callback on_hover;
        set_option_value_callback set_option_value;
        Label *name_label = nullptr;

        virtual void process_event(const Event &e) override;
        std::string_view get_type_name() override { return "NovaConfigOptionElement"; }

    public:
        NovaConfigOptionElement(
            Element *parent,
            std::string option_id,
            size_t option_index,
            ConfigOptionPropertyCallbacks *callbacks,
            set_option_value_callback set_option_value,
            on_option_hover_callback on_hover
        );
        virtual ~NovaConfigOptionElement() = default;
        // required overrides
        virtual void update_value() = 0;
        virtual void update_disabled() = 0;
        virtual Element *get_focus_element() = 0;
        void update_hidden();
        const std::string &get_option_id() { return option_id; }
        void set_nav_auto(NavDirection dir) override { get_focus_element()->set_nav_auto(dir); }
        void set_nav_none(NavDirection dir) override { get_focus_element()->set_nav_none(dir); }
        void set_nav(NavDirection dir, Element* element) override { get_focus_element()->set_nav(dir, element); }
        void set_nav_manual(NavDirection dir, const std::string& target) override { get_focus_element()->set_nav_manual(dir, target); }
    };

class NovaConfigOptionEnum : public NovaConfigOptionElement {
protected:
    Radio *radio = nullptr;
    Label *details_label = nullptr;

    std::string_view get_type_name() override { return "NovaConfigOptionEnum"; }
public:
    NovaConfigOptionEnum(
        Element *parent,
        std::string option_id,
        size_t option_index,
        ConfigOptionPropertyCallbacks *callbacks,
        set_option_value_callback set_option_value,
        on_option_hover_callback on_hover
    );

    Element* get_focus_element() override { return radio; }
    void update_value() override;
    void update_disabled() override;

    void update_enum_details();
    void update_enum_disabled();
};

class NovaConfigOptionNumber : public NovaConfigOptionElement {
protected:
    Slider *slider = nullptr;

    std::string_view get_type_name() override { return "NovaConfigOptionNumber"; }
public:
    NovaConfigOptionNumber(
        Element *parent,
        std::string option_id,
        size_t option_index,
        ConfigOptionPropertyCallbacks *callbacks,
        set_option_value_callback set_option_value,
        on_option_hover_callback on_hover
    );

    Element* get_focus_element() override { return slider; }
    void update_value() override;
    void update_disabled() override;
};

class NovaConfigOptionString : public NovaConfigOptionElement {
protected:
    TextInput *text_input = nullptr;

    std::string_view get_type_name() override { return "NovaConfigOptionString"; }
public:
    NovaConfigOptionString(
        Element *parent,
        std::string option_id,
        size_t option_index,
        ConfigOptionPropertyCallbacks *callbacks,
        set_option_value_callback set_option_value,
        on_option_hover_callback on_hover
    );

    Element* get_focus_element() override { return text_input; }
    void update_value() override;
    void update_disabled() override;
};

class NovaConfigOptionBool : public NovaConfigOptionElement {
protected:
    Toggle *toggle = nullptr;

    std::string_view get_type_name() override { return "NovaConfigOptionBool"; }
public:
    NovaConfigOptionBool(
        Element *parent,
        std::string option_id,
        size_t option_index,
        ConfigOptionPropertyCallbacks *callbacks,
        set_option_value_callback set_option_value,
        on_option_hover_callback on_hover
    );

    Element* get_focus_element() override { return toggle; }
    void update_value() override;
    void update_disabled() override;
};

} // namespace recompui
