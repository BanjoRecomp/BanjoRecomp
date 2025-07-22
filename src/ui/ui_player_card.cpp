#include "ui_player_card.h"

namespace recompui {

static constexpr float assign_player_card_size = 128.0f;
static constexpr float assign_player_card_icon_size = 64.0f;

static constexpr float static_player_card_size = 256.0f;
static constexpr float static_player_card_icon_size = 128.0f;

PlayerCard::PlayerCard(
    Element *parent,
    int player_index,
    bool is_assignment_card
) : Element(parent, 0, "div", false),
    player_index(player_index),
    is_assignment_card(is_assignment_card)
{
    const float size = is_assignment_card ? assign_player_card_size : static_player_card_size;
    const float icon_size = is_assignment_card ? assign_player_card_icon_size : static_player_card_icon_size;

    set_display(Display::Flex);
    set_flex_direction(FlexDirection::Column);
    set_align_items(AlignItems::Center);
    set_justify_content(JustifyContent::Center);
    set_width(size);
    set_height(size);
    set_border_color(theme::color::BorderSoft);
    set_border_width(theme::border::width, Unit::Dp);
    set_border_radius(theme::border::radius_sm, Unit::Dp);
    set_background_color(theme::color::Transparent);

    recompui::ContextId context = get_current_context();
    icon = context.create_element<Svg>(this, "assets/icons/RecordBorder.svg");
    icon->set_width(icon_size, Unit::Dp);
    icon->set_height(icon_size, Unit::Dp);
    icon->set_color(theme::color::TextDim);
    icon->set_display(Display::None);

    if (!is_assignment_card) {
        update_player_card_icon();
    }
}

PlayerCard::~PlayerCard() {
}

void PlayerCard::update_player_card_icon() {
    recompinput::AssignedPlayer& assigned_player = recompinput::get_assigned_player(player_index, is_assignment_card);
    if (assigned_player.is_assigned) {
        if (assigned_player.controller != nullptr) {
            icon->set_display(Display::Block);
            icon->set_src("assets/icons/Cont.svg");
        } else {
            icon->set_display(Display::Block);
            icon->set_src("assets/icons/Keyboard.svg");
        }
    } else {
        if (is_assignment_card) {
            icon->set_display(Display::Block);
            icon->set_src("assets/icons/RecordBorder.svg");
        } else {
            icon->set_display(Display::None);
        }
    }
}

void PlayerCard::update_assignment_player_card() {
    static const float scale_anim_duration = 0.25f;

    update_player_card_icon();

    if (!recompinput::get_player_is_assigned(player_index)) {
        icon->set_scale_2D(1.0f, 1.0f);
        set_background_color(theme::color::Transparent);
        icon->set_color(theme::color::TextDim);
        return;
    }

    set_background_color(theme::color::PrimaryA20);

    bool has_controller =recompinput::does_player_have_controller(player_index);

    std::chrono::steady_clock::duration time_since_last_button_press = recompinput::get_player_time_since_last_button_press(player_index);
    auto millis = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(time_since_last_button_press).count());
    float seconds = millis / 1000.0f;

    if (seconds > 0 && seconds < scale_anim_duration) {
        float t = 1.0f - (seconds / scale_anim_duration);
        float scale = 1.0f + t * 0.15f;
        icon->set_scale_2D(scale, scale);
        icon->set_color(theme::color::Text, 200 + static_cast<int>(t * 55.0f));
    } else {
        icon->set_scale_2D(1.0f, 1.0f);
        icon->set_color(theme::color::Text, 200);
    }
}

} // namespace recompui
