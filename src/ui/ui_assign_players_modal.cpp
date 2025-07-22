#include "ui_assign_players_modal.h"
#include "elements/ui_label.h"
#include "elements/ui_container.h"
#include "recomp_ui.h"

namespace recompui {

recompui::ContextId assign_players_modal_context;

static const float assignPlayersHFPaddingVert = 20.0f;
static const float assignPlayersHFPaddingHorz = 20.0f;

static void set_button_side_styles(Element *el) {
    el->set_align_items(AlignItems::Center);
    el->set_width_auto();
    el->set_height_auto();
    el->set_flex_basis_auto();
    el->set_gap(8.0f);
}

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

    fake_focus_button = context.create_element<Element>(modal, 0, "button", false);
    fake_focus_button->set_position(Position::Absolute);
    fake_focus_button->set_width(0, Unit::Dp);
    fake_focus_button->set_height(0, Unit::Dp);
    fake_focus_button->set_opacity(0);

    context.create_element<Label>(modal, "Assign Players", LabelStyle::Large);

    player_elements_wrapper = context.create_element<Element>(modal, 0, "div", false);
    player_elements_wrapper->set_display(Display::Flex);
    player_elements_wrapper->set_flex_direction(FlexDirection::Row);
    player_elements_wrapper->set_justify_content(JustifyContent::SpaceBetween);
    player_elements_wrapper->set_align_items(AlignItems::Center);
    player_elements_wrapper->set_width(100, Unit::Percent);
    player_elements_wrapper->set_padding(24, Unit::Dp);

    Element* footer = context.create_element<Element>(modal, 0, "div", false);
    footer->set_display(Display::Flex);
    footer->set_position(Position::Relative);
    footer->set_flex_direction(FlexDirection::Row);
    footer->set_align_items(AlignItems::Center);
    footer->set_justify_content(JustifyContent::SpaceBetween);
    footer->set_width(100.0f, Unit::Percent);
    footer->set_height_auto();

    footer->set_padding_top(assignPlayersHFPaddingVert);
    footer->set_padding_bottom(assignPlayersHFPaddingVert);
    footer->set_padding_left(assignPlayersHFPaddingHorz);
    footer->set_padding_right(assignPlayersHFPaddingHorz);

    auto left = context.create_element<Container>(footer, FlexDirection::Row, JustifyContent::FlexStart, 0);
    set_button_side_styles(left);
    close_button = context.create_element<Button>(left, "Cancel", ButtonStyle::Tertiary);
    close_button->add_pressed_callback(recompinput::stop_player_assignment_and_close_modal);

    auto right = context.create_element<Container>(footer, FlexDirection::Row, JustifyContent::FlexEnd, 0);
    retry_button = context.create_element<Button>(right, "Retry", ButtonStyle::Warning);
    retry_button->set_enabled(false);
    retry_button->add_pressed_callback(recompinput::start_player_assignment);

    confirm_button = context.create_element<Button>(right, "Confirm", ButtonStyle::Primary);
    confirm_button->set_enabled(false);
    confirm_button->add_pressed_callback(recompinput::commit_player_assignment);
    set_button_side_styles(right);
}

AssignPlayersModal::~AssignPlayersModal() {
}

void AssignPlayersModal::process_event(const Event &e) {
    if (!is_open) {
        return;
    }
    if (e.type == EventType::Update) {
        if (player_elements.empty() || player_elements.size() != recompinput::get_num_players()) {
            create_player_elements();
        }

        for (int i = 0; i < recompinput::get_num_players(); i++) {
            player_elements[i]->update_assignment_player_card();
        }

        if (!recompinput::is_player_assignment_active()) {
            if (recompinput::get_player_is_assigned(0)) {
                confirm_button->set_enabled(true);
                retry_button->set_enabled(true);
                if (was_assigning) {
                    confirm_button->focus();
                }
            }

            was_assigning = false;
        } else {
            fake_focus_button->focus();
            was_assigning = true;
        }

        if (recompinput::get_player_is_assigned(0)) {
            confirm_button->set_enabled(true);
            retry_button->set_enabled(true);
        } else {
            confirm_button->set_enabled(false);
            retry_button->set_enabled(false);
        }

        queue_update();
    }
}

void AssignPlayersModal::create_player_elements() {
    player_elements_wrapper->clear_children();
    player_elements.clear();
    recompui::ContextId context = get_current_context();

    for (int i = 0; i < recompinput::get_num_players(); i++) {
        PlayerCard* player_element = context.create_element<PlayerCard>(player_elements_wrapper, i, true);
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
