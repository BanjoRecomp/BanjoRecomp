#include "ui_player_card.h"
#include "elements/ui_label.h"

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

    recompui::ContextId context = get_current_context();

    set_display(Display::Flex);
    set_flex_direction(FlexDirection::Column);
    set_align_items(AlignItems::FlexStart);
    set_justify_content(JustifyContent::FlexStart);
    set_width(size);
    set_height_auto();
    set_gap(8.0f);

    if (!is_assignment_card) {
        auto player_label = context.create_element<Label>(this, "Player " + std::to_string(player_index + 1), LabelStyle::Small);
    }

    card = context.create_element<Element>(this, 0, "div", false);
    card->set_display(Display::Flex);
    card->set_flex_direction(FlexDirection::Column);
    card->set_align_items(AlignItems::Center);
    card->set_justify_content(JustifyContent::Center);
    card->set_width(size);
    card->set_height(size);
    card->set_border_color(theme::color::BorderSoft);
    card->set_border_width(theme::border::width, Unit::Dp);
    card->set_border_radius(theme::border::radius_sm, Unit::Dp);
    card->set_background_color(theme::color::Transparent);

    icon = context.create_element<Svg>(card, "icons/RecordBorder.svg");
    icon->set_width(icon_size, Unit::Dp);
    icon->set_height(icon_size, Unit::Dp);
    icon->set_color(theme::color::TextDim);
    icon->set_display(Display::None);

    if (!is_assignment_card) {
        recompinput::AssignedPlayer& assigned_player = recompinput::get_assigned_player(player_index, is_assignment_card);
        update_player_card_icon();

        auto profile_label = context.create_element<Label>(this, "Profile", LabelStyle::Small);

        profile_label->set_margin_top(8.0f);

        std::vector<SelectOption> options;
        options.emplace_back("Controller", "cont");
        options.emplace_back("customprofile", "unassigned");

        auto select = context.create_element<Select>(
            this,
            options,
            "Player",
            "Select Player"
        );
        select->add_change_callback([this, player_index](SelectOption& option, int option_index) {
            printf("Selected option: %s at index %d for player %d\n", option.text.c_str(), option_index, player_index);
        });
        select->set_width(100.0f, Unit::Percent);
        select->set_enabled(assigned_player.is_assigned);
        
        auto edit_profile_button = context.create_element<Button>(this, "Edit Profile", ButtonStyle::Secondary, ButtonSize::Medium);
        edit_profile_button->add_pressed_callback([this, player_index]() {
            printf("Edit Profile button pressed for player %d\n", player_index);
        });
        edit_profile_button->set_width(100.0f, Unit::Percent);
        edit_profile_button->set_enabled(assigned_player.is_assigned);
    }
}

PlayerCard::~PlayerCard() {
}

void PlayerCard::update_player_card_icon() {
    recompinput::AssignedPlayer& assigned_player = recompinput::get_assigned_player(player_index, is_assignment_card);
    if (assigned_player.is_assigned) {
        if (assigned_player.controller != nullptr) {
            icon->set_display(Display::Block);
            icon->set_src("icons/Cont.svg");
        } else {
            icon->set_display(Display::Block);
            icon->set_src("icons/Keyboard.svg");
        }
    } else {
        if (is_assignment_card) {
            icon->set_display(Display::Block);
            icon->set_src("icons/RecordBorder.svg");
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
        card->set_background_color(theme::color::Transparent);
        icon->set_color(theme::color::TextDim);
        return;
    }

    card->set_background_color(theme::color::PrimaryA20);

    bool has_controller = recompinput::does_player_have_controller(player_index);

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
