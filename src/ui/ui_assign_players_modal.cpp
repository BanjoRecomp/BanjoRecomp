#include "ui_assign_players_modal.h"
#include "elements/ui_label.h"
#include "recomp_ui.h"

namespace recompui {

recompui::ContextId assign_players_modal_context;

AssignPlayersModal::AssignPlayersModal(Element *parent) : Element(parent, 0, "div", false) {
    recompui::ContextId context = get_current_context();
    
    set_display(Display::Flex);
    set_flex_direction(FlexDirection::Column);
    set_background_color(theme::color::Transparent);
    set_display(Display::None);

    Element* modal_overlay = context.create_element<Element>(this);
    modal_overlay->set_background_color(theme::color::BGOverlay);
    modal_overlay->set_position(Position::Absolute);
    modal_overlay->set_top(0);
    modal_overlay->set_right(0);
    modal_overlay->set_bottom(0);
    modal_overlay->set_left(0);

    Element* modal_whole_page_wrapper = context.create_element<Element>(this);
    modal_whole_page_wrapper->set_display(Display::Flex);
    modal_whole_page_wrapper->set_position(Position::Absolute);
    modal_whole_page_wrapper->set_top(0);
    modal_whole_page_wrapper->set_right(0);
    modal_whole_page_wrapper->set_bottom(0);
    modal_whole_page_wrapper->set_left(0);
    modal_whole_page_wrapper->set_align_items(AlignItems::Center);
    modal_whole_page_wrapper->set_justify_content(JustifyContent::Center);

    Element* modal = context.create_element<Element>(modal_whole_page_wrapper);
    modal->set_display(Display::Flex);
    modal->set_position(Position::Relative);
    modal->set_flex(1.0f, 1.0f);
    modal->set_flex_basis(100, Unit::Percent);
    modal->set_flex_direction(FlexDirection::Column);
    modal->set_width(100, Unit::Percent);
    modal->set_max_width(700, Unit::Dp);
    modal->set_height_auto();
    modal->set_margin_auto();
    modal->set_border_width(theme::border::width, Unit::Dp);
    modal->set_border_radius(theme::border::radius_lg, Unit::Dp);
    modal->set_border_color(theme::color::WhiteA20);
    modal->set_background_color(theme::color::ModalOverlay);

    context.create_element<Label>(modal, "Assign Players", LabelStyle::Large);

    player_elements_wrapper = context.create_element<Element>(modal, 0, "div", false);
    player_elements_wrapper->set_display(Display::Flex);
    player_elements_wrapper->set_flex_direction(FlexDirection::Row);
    player_elements_wrapper->set_justify_content(JustifyContent::SpaceBetween);
    player_elements_wrapper->set_align_items(AlignItems::Center);
    player_elements_wrapper->set_width(100, Unit::Percent);
    player_elements_wrapper->set_padding(24, Unit::Dp);
}

AssignPlayersModal::~AssignPlayersModal() {
}

static void set_player_element_assigned(Element* player_element, bool assigned, bool as_controller) {
    if (assigned) {
        if (as_controller) {
            player_element->set_background_color(theme::color::PrimaryA50);
        } else {
            player_element->set_background_color(theme::color::SecondaryA50);
        }
    } else {
        player_element->set_background_color(theme::color::Transparent);
    }
}

void AssignPlayersModal::process_event(const Event &e) {
    if (!is_open) {
        return;
    }
    if (e.type == EventType::Update) {
        if (!recompinput::is_player_assignment_active()) {
            return;
        }

        if (player_elements.empty() || player_elements.size() != recompinput::get_num_players()) {
            create_player_elements();
        }
        for (int i = 0; i < recompinput::get_num_players(); i++) {
            set_player_element_assigned(player_elements[i], recompinput::get_player_is_assigned(i), recompinput::does_player_have_controller(i));
        }
        queue_update();
    }
}

void AssignPlayersModal::create_player_elements() {
    player_elements_wrapper->clear_children();
    player_elements.clear();
    recompui::ContextId context = get_current_context();

    for (int i = 0; i < recompinput::get_num_players(); i++) {
        Element* player_element = context.create_element<Element>(player_elements_wrapper, 0, "div", false);
        player_element->set_display(Display::Flex);
        player_element->set_flex_direction(FlexDirection::Column);
        player_element->set_align_items(AlignItems::Center);
        player_element->set_justify_content(JustifyContent::Center);
        player_element->set_width(100);
        player_element->set_height(100);
        player_element->set_border_color(theme::color::BorderSoft);
        player_element->set_border_width(theme::border::width, Unit::Dp);
        player_element->set_border_radius(theme::border::radius_sm, Unit::Dp);
        player_element->set_background_color(theme::color::Transparent);

        player_elements.push_back(player_element);
    }
}

void AssignPlayersModal::open() {
    if (!recompui::is_context_shown(assign_players_modal_context)) {
        recompui::show_context(assign_players_modal_context, "");
    }

    is_open = true;
    set_display(Display::Block);
    create_player_elements();
    queue_update();
}
void AssignPlayersModal::close() {
    if (recompui::is_context_shown(assign_players_modal_context)) {
        set_display(Display::None);
        recompui::hide_context(assign_players_modal_context);
    }

    is_open = false;
}

recompui::AssignPlayersModal *assign_players_modal = nullptr;

void init_assign_players_modal() {
    assign_players_modal_context = recompui::create_context();
    assign_players_modal_context.open();
    assign_players_modal = assign_players_modal_context.create_element<AssignPlayersModal>(assign_players_modal_context.get_root_element());
    assign_players_modal_context.close();
}

} // namespace recompui
