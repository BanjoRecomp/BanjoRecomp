#pragma once
#include "ui_tab_set.h"
#include "../ui_utils.h"

namespace recompui {

Tab::Tab(Element *parent, int tab_index, std::string_view text, on_change_tab_callback on_select_tab) :
    Element(parent, Events(EventType::Click, EventType::Hover, EventType::Enable, EventType::Focus), "button", false)
{
    this->tab_index = tab_index;
    this->on_select_tab = std::move(on_select_tab);
    set_display(Display::Block);
    set_position(Position::Relative);
    set_margin(0);
    set_padding_top(tab_vertical_padding);
    set_padding_bottom(tab_vertical_padding);
    set_padding_left(tab_horizontal_padding);
    set_padding_right(tab_horizontal_padding);
    set_focusable(true);
    set_tab_index_auto();
    set_nav_none(NavDirection::Left);
    set_nav_none(NavDirection::Right);
    set_nav_auto(NavDirection::Up);
    set_nav_auto(NavDirection::Down);

    set_color(theme::color::TextInactive);
    set_background_color(theme::color::Transparent);

    hover_style.set_color(theme::color::WhiteA80);
    checked_style.set_color(theme::color::White);
    pulsing_style.set_color(theme::color::WhiteA80);

    ContextId context = get_current_context();
    label = context.create_element<Label>(this, std::string{text}, theme::Typography::Header3);

    indicator = context.create_element<Element>(this, 0, "div", false);
    indicator->set_display(Display::Block);
    indicator->set_position(Position::Absolute);
    indicator->set_height(2.0f);
    indicator->set_bottom(2.0f);
    indicator->set_left(0.0f);
    indicator->set_right(0.0f);
    indicator->set_background_color(theme::color::Transparent);

    add_style(&hover_style, { hover_state });
    add_style(&checked_style, { checked_state });
    add_style(&pulsing_style, { focus_state });
}

void Tab::process_event(const Event &e) {
    switch (e.type) {
    case EventType::Click:
        on_select_tab(tab_index);
        break;
    case EventType::Hover:
        set_style_enabled(hover_state, std::get<EventHover>(e.variant).active);
        break;
    case EventType::Enable: {
        bool active = std::get<EventEnable>(e.variant).active;
        set_style_enabled(disabled_state, !active);
        indicator->set_background_color(active ? theme::color::BorderSolid : theme::color::Transparent);
        break;
    }
    case EventType::Focus:
        {
            bool active = std::get<EventFocus>(e.variant).active;
            set_style_enabled(focus_state, active);
            if (active) {
                queue_update();
            }
        }
        break;
    case EventType::Update:
        if (is_style_enabled(focus_state)) {
            recompui::Color pulse_color = recompui::get_pulse_color(750);
            pulsing_style.set_color(pulse_color);
            if (is_selected) {
                indicator->set_background_color(pulse_color);
            } else {
                indicator->set_background_color(theme::color::Transparent);
            }
            apply_styles();
            queue_update();
        }
        break;
    default:
        break;
    }
}

void Tab::set_selected(bool enable) {
    is_selected = enable;
    set_as_primary_focus(enable);
    set_style_enabled(checked_state, enable);
    indicator->set_background_color(enable ? theme::color::BorderSolid : theme::color::Transparent);
}

TabSet::TabSet(Element *parent) :
    Element(parent, 0, "div", false)
{
    set_display(Display::Flex);
    set_flex_direction(FlexDirection::Row);
    set_justify_content(JustifyContent::FlexStart);
    set_align_items(AlignItems::Stretch);
    set_gap(0.0f);
    set_as_navigation_container(NavigationType::Horizontal);
}

void TabSet::set_change_tab_callback(on_change_tab_callback callback) {
    change_tab_callback = std::move(callback);
}

int TabSet::add_tab(std::string_view text) {
    ContextId context = get_current_context();
    Tab *tab = context.create_element<Tab>(this, tabs.size(), text, [this](int tab_index) {
        set_active_tab(tab_index);
    });
    tabs.push_back(tab);
    if (active_tab < 0) {
        active_tab = 0;
        tab->set_selected(true);
    } else {
        tab->set_selected(active_tab == tabs.size() - 1);
    }

    if (tabs.size() > 1) {
        tabs[tabs.size() - 2]->set_nav(NavDirection::Right, tab);
        tab->set_nav(NavDirection::Left, tabs[tabs.size() - 2]);
        tab->set_nav_none(NavDirection::Right);
    }

    return tabs.size() - 1;
}

void TabSet::set_active_tab(int tab_index) {
    if (tab_index < 0 || tab_index >= static_cast<int>(tabs.size())) {
        return;
    }
    if (active_tab != tab_index) {
        active_tab = tab_index;
        for (int i = 0; i < static_cast<int>(tabs.size()); i++) {
            tabs[i]->set_selected(i == active_tab);
        }
        if (change_tab_callback) {
            change_tab_callback(active_tab);
        }
    }
}

void TabSet::focus_on_active_tab() {
    if (active_tab >= 0 && active_tab < static_cast<int>(tabs.size())) {
        tabs[active_tab]->focus();
    }
};

void TabSet::set_nav_down(Element *element) {
    for (auto &tab : tabs) {
        tab->set_nav(NavDirection::Down, element);
    }
}

void TabSet::set_nav_up(Element *element) {
    for (auto &tab : tabs) {
        tab->set_nav(NavDirection::Up, element);
    }
}

} // namespace recompui
