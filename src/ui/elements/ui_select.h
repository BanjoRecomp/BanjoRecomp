#pragma once

#include "ui_element.h"
#include "ui_svg.h"


namespace recompui {
    enum class SelectOptionStyle {
        Primary,
        Secondary,
        Tertiary,
        Success,
        Warning,
        Danger,
        Default = Tertiary,
    };

    struct SelectOption {
        std::string text;
        std::string value;
        SelectOptionStyle style = SelectOptionStyle::Default;

        SelectOption(const std::string &text, const std::string &value, SelectOptionStyle style = SelectOptionStyle::Default)
            : text(text), value(value), style(style) {}
    };

    class Option : public Element {
    protected:
        SelectOption option;
        Style hover_style;
        Style focus_style;
        Style disabled_style;

        // Element overrides.
        virtual void process_event(const Event &e) override;
        std::string_view get_type_name() override { return "Option"; }
    public:
        Option(Element *parent, const SelectOption &option);
    };

    class Select : public Element {
    protected:
        int selected_option_index = -1;
        std::string label;
        std::string default_text;
        Element *wrapper = nullptr;
        Svg *arrow = nullptr;
        std::vector<SelectOption> options;
        std::vector<Option*> option_elements;
        bool is_open = false;

        Style hover_style;
        Style focus_style;
        Style disabled_style;
        Style hover_disabled_style;
        std::list<std::function<void(SelectOption& option, int option_index)>> change_callbacks;

        // Element overrides.
        virtual void process_event(const Event &e) override;
        std::string_view get_type_name() override { return "Select"; }
    public:
        Select(Element *parent, std::vector<SelectOption> options = {}, const std::string &label = "", const std::string &default_text = "");
        void add_change_callback(std::function<void(SelectOption& option, int option_index)> callback);
        Style* get_hover_style() { return &hover_style; }
        Style* get_focus_style() { return &focus_style; }
        Style* get_disabled_style() { return &disabled_style; }
        Style* get_hover_disabled_style() { return &hover_disabled_style; }
    private:
        void add_option_elements();
    };

} // namespace recompui
