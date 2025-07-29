#include "ui_select.h"

#include <cassert>

namespace recompui {
    Option::Option(Element *parent, const SelectOption &option) :
        Element(parent, Events(EventType::Click, EventType::Hover, EventType::Enable, EventType::Focus), "option", true),
        option(option)
    {
        set_text(option.text);
        set_input_text(option.value);
        set_color(theme::color::Primary);

        set_padding(12.0f);
        set_height_auto();
        set_width_auto();
        set_overflow(Overflow::Hidden);

        set_min_width(128.0f);

        set_font_size(18.0f);
        set_letter_spacing(2.52f);
        set_line_height(18.0f);
        set_font_weight(400);

        set_background_color(theme::color::Transparent);
        set_cursor(Cursor::Pointer);

        hover_style.set_color(theme::color::TextActive);
        hover_style.set_background_color(theme::color::White, 77);

        focus_style.set_color(theme::color::TextActive);
        focus_style.set_background_color(theme::color::White, 77);

        disabled_style.set_color(theme::color::TextDim, 128);
        disabled_style.set_background_color(theme::color::Transparent);
        disabled_style.set_cursor(Cursor::None);

        add_style(&hover_style, hover_state);
        add_style(&focus_style, focus_state);
        add_style(&disabled_style, disabled_state);
    }

    void Option::process_event(const Event &e) {
        switch (e.type) {
        case EventType::Click:
            break;
        case EventType::Hover:
            printf("Option hovered: %s\n", option.text.c_str());
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
            break;
        default:
            assert(false && "Unknown event type.");
            break;
        }
    }


    constexpr std::string_view select_element_selectbox = "selectbox";
    constexpr std::string_view select_element_selectarrow = "selectarrow";
    constexpr std::string_view select_element_selectvalue = "selectvalue";

    constexpr float select_element_height = 48.0f;
    constexpr float select_element_min_width = 128.0f + 64.0f; // 128px for the select box, 64px for the arrow
    constexpr float select_element_padding = 16.0f;
    constexpr float select_element_caret_size = 24.0f;

    Select::Select(
        Element *parent, std::vector<SelectOption> options, const std::string &label, const std::string &default_text
    ) :
        Element(
            parent,
            Events(EventType::Text, EventType::Click, EventType::Hover, EventType::Enable, EventType::Focus),
            "select",
            false),
        label(label),
        default_text(default_text)
    {
        this->options = options;

        ContextId context = get_current_context();
        wrapper = context.create_element<Element>(parent, 0, "div", false);
        set_parent(wrapper);
        wrapper->set_height_auto();
        wrapper->set_width(100.0f, Unit::Percent);
        wrapper->set_position(Position::Relative);

        set_display(Display::Flex);
        set_flex_direction(FlexDirection::Row);
        set_align_items(AlignItems::Center);
        set_justify_content(JustifyContent::FlexStart);
        set_gap(16.0f);
        set_text_align(TextAlign::Left);
        set_position(Position::Relative);
        set_padding_right(select_element_padding);
        set_padding_left(select_element_padding);
        set_height(select_element_height);
        set_width_auto();
        set_overflow(Overflow::Hidden);

        set_min_width(select_element_min_width);

        set_border_width(theme::border::width);
        set_border_radius(theme::border::radius_md);
        set_border_color(theme::color::Border, 204);

        set_font_size(20.0f);
        set_letter_spacing(0.0f);
        set_line_height(20.0f);
        set_font_weight(400);
        set_font_style(FontStyle::Normal);

        set_focusable(true);
        set_color(theme::color::Text);
        set_background_color(theme::color::Transparent);
        set_cursor(Cursor::Pointer);

        set_nav_auto(NavDirection::Up);
        set_nav_auto(NavDirection::Right);
        set_nav_auto(NavDirection::Down);
        set_nav_auto(NavDirection::Left);

        hover_style.set_color(theme::color::TextActive);
        hover_style.set_background_color(theme::color::White, 77);
        hover_style.set_border_color(theme::color::Border, 255);

        focus_style.set_color(theme::color::TextActive);
        focus_style.set_background_color(theme::color::White, 77);
        focus_style.set_border_color(theme::color::Border, 255);
        
        disabled_style.set_color(theme::color::TextDim, 128);
        disabled_style.set_background_color(theme::color::Transparent);
        disabled_style.set_border_color(theme::color::Border, 77);
        disabled_style.set_cursor(Cursor::None);
        
        hover_disabled_style.set_color(theme::color::TextDim, 128);
        hover_disabled_style.set_background_color(theme::color::Transparent);
        hover_disabled_style.set_border_color(theme::color::Border, 77);
        hover_disabled_style.set_cursor(Cursor::None);

        Element selectbox_element = get_element_with_tag_name(select_element_selectbox);
        selectbox_element.set_margin_top(4.0f);
        selectbox_element.set_width_auto();
        selectbox_element.set_padding_top(8.0f);
        selectbox_element.set_padding_bottom(8.0f);
        selectbox_element.set_background_color(theme::color::Background3);
        selectbox_element.set_border_width(theme::border::width);
        selectbox_element.set_border_radius(theme::border::radius_sm);
        selectbox_element.set_border_color(theme::color::BorderSoft);

        Element selectvalue_element = get_element_with_tag_name(select_element_selectvalue);
        selectvalue_element.set_display(Display::Block);
        selectvalue_element.set_position(Position::Absolute);
        selectvalue_element.set_top(50.0f, recompui::Unit::Percent);
        selectvalue_element.set_left(select_element_padding);
        selectvalue_element.set_width_auto();
        selectvalue_element.set_height_auto();
        selectvalue_element.set_translate_2D(0.0f, -50.0f, recompui::Unit::Percent);

        Element selectarrow_element = get_element_with_tag_name(select_element_selectarrow);
        selectarrow_element.set_display(Display::None);
        
        add_option_elements();

        arrow = context.create_element<Svg>(
            wrapper,
            "icons/Caret.svg"
        );

        arrow->set_display(Display::Block);

        arrow->set_position(Position::Absolute);
        arrow->set_top(50.0f, recompui::Unit::Percent);
        arrow->set_right(select_element_padding);
        arrow->set_translate_2D(0, -50.0f, recompui::Unit::Percent);

        arrow->set_width(select_element_caret_size);
        arrow->set_height(select_element_caret_size);
        arrow->set_color(theme::color::TextDim);

        add_style(&hover_style, hover_state);
        add_style(&focus_style, focus_state);
        add_style(&disabled_style, disabled_state);
        add_style(&hover_disabled_style, { hover_state, disabled_state });
    }

    void Select::process_event(const Event &e) {
        switch (e.type) {
        case EventType::Click:
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
        case EventType::Focus: {
            bool focus_active = std::get<EventFocus>(e.variant).active;
            set_style_enabled(focus_state, focus_active);
            break;
        }
        case EventType::Text:
            {
                const std::string& opt_value = std::get<EventText>(e.variant).text;

                int index = -1;
                for (int i = 0; i < options.size(); i++) {
                    if (options[i].value == opt_value) {
                        index = i;
                        break;
                    }
                }

                if (index == -1) {
                    break;
                }

                selected_option_index = index;
                for (const auto &callback : change_callbacks) {
                    callback(options[index], selected_option_index);
                }
            }
            break;
        case EventType::Update: {
            Element selectvalue_element = get_element_with_tag_name(select_element_selectvalue);
            bool is_now_open = selectvalue_element.is_pseudo_class_set("checked");
            if (is_now_open != is_open) {
                is_open = is_now_open;
                if (is_open) {
                    arrow->set_rotation(180.0f);
                } else {
                    arrow->set_rotation(0.0f);
                }
            }
            queue_update();
            break;
        }
        default:
            assert(false && "Unknown event type.");
            break;
        }
    }

    void Select::add_change_callback(std::function<void(SelectOption& option, int option_index)> callback) {
        change_callbacks.emplace_back(callback);
    }

    void Select::add_option_elements() {
        clear_children();
        option_elements.clear();

        ContextId context = get_current_context();

        for (int i = 0; i < options.size(); i++) {
            auto &option = options[i];
            Option *option_element = context.create_element<Option>(this, option);
            option_elements.push_back(option_element);
        }
    }
};
