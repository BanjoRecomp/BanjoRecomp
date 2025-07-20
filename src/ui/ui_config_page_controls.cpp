#include "ui_config_page_controls.h"
#include "ui_assign_players_modal.h"
#include "elements/ui_button.h"
#include "elements/ui_label.h"
#include "elements/ui_toggle.h"
#include "elements/ui_container.h"
#include "elements/ui_binding_button.h"

namespace recompui {

const std::string_view active_state_style_name = "cont_opt_active";

GameInputRow::GameInputRow(
    Element *parent,
    GameInputContext *input_ctx,
    BindingList bindings,
    std::function<void()> on_hover_callback,
    on_bind_click_callback on_bind_click
) : Element(parent, Events(EventType::Hover), "div", false) {
    this->input_id = input_ctx->input_id;
    this->on_hover_callback = on_hover_callback;
    this->bindings = bindings;

    set_display(Display::Flex);
    set_position(Position::Relative);
    set_flex_direction(FlexDirection::Row);
    set_align_items(AlignItems::Center);
    set_justify_content(JustifyContent::SpaceBetween);
    set_width(100.0f, Unit::Percent);
    set_height_auto();

    set_padding_top(4.0f);
    set_padding_right(16.0f);
    set_padding_bottom(4.0f);
    set_padding_left(20.0f);
    set_border_radius(theme::border::radius_sm);
    set_background_color(theme::color::Transparent);

    active_style.set_background_color(theme::color::BGOverlay);
    add_style(&active_style, active_state_style_name);

    recompui::ContextId context = get_current_context();

    auto label = context.create_element<Label>(this, input_ctx->name, LabelStyle::Normal);
    label->set_flex_grow(2.0f);
    label->set_flex_shrink(1.0f);
    label->set_flex_basis(300.0f);
    label->set_height_auto();
    // TODO: whitespace nowrap impl

    auto bindings_container = context.create_element<Element>(this, 0, "div", false);
    {
        bindings_container->set_display(Display::Flex);
        bindings_container->set_position(Position::Relative);
        bindings_container->set_flex_grow(2.0f);
        bindings_container->set_flex_shrink(1.0f);
        bindings_container->set_flex_basis(400.0f);
        bindings_container->set_flex_direction(FlexDirection::Row);
        bindings_container->set_align_items(AlignItems::Center);
        bindings_container->set_justify_content(JustifyContent::SpaceBetween);
        bindings_container->set_width(100.0f, Unit::Percent);
        bindings_container->set_height(56.0f);
        bindings_container->set_padding_right(12.0f);
        bindings_container->set_padding_left(4.0f);
        bindings_container->set_gap(4.0f);

        for (size_t i = 0; i < bindings.size(); i++) {
            BindingButton *binding_button = context.create_element<BindingButton>(bindings_container, "");
            binding_button->set_binding(bindings[i].to_string());
            binding_button->add_pressed_callback([this, i, on_bind_click]() {
               on_bind_click(this->input_id, i);
            });
            binding_buttons.push_back(binding_button);
        }
    }

    if (input_ctx->clearable) {
        auto clear_button = context.create_element<IconButton>(this, "icons/Trash.svg", ButtonStyle::Danger, IconButtonSize::Large);
        clear_button->add_pressed_callback([this]() {
            // TODO: Add clear callback
        });
    } else {
        auto reset_button = context.create_element<IconButton>(this, "icons/Reset.svg", ButtonStyle::Warning, IconButtonSize::Large);
        reset_button->add_pressed_callback([this]() {
            // TODO: Add reset callback
        });
    }
}

GameInputRow::~GameInputRow() {
}

void GameInputRow::update_bindings(BindingList &new_bindings) {
    for (size_t i = 0; i < new_bindings.size(); i++) {
        // skip update if no changes
        if (
            new_bindings[i].input_id == bindings[i].input_id &&
            new_bindings[i].input_type == bindings[i].input_type) {
            continue;
        }

        binding_buttons[i]->set_binding(new_bindings[i].to_string());
        bindings[i] = new_bindings[i];
    }
}

void GameInputRow::process_event(const Event &e) {
    switch (e.type) {
    case EventType::Hover:
        {
            bool hover_active = std::get<EventHover>(e.variant).active;
            set_style_enabled(active_state_style_name, hover_active);
            if (hover_active && on_hover_callback) {
                on_hover_callback();
            }
        }
        break;
    default:
        break;
    }
}

ConfigPageControls::ConfigPageControls(
    Element *parent,
    int num_players,
    std::vector<GameInputContext> game_input_contexts,
    std::vector<PlayerBindings> game_input_bindings,
    std::vector<bool> player_keyboard_enabled,
    on_player_bind_callback on_player_bind,
    set_player_keyboard_enabled_callback set_player_keyboard_enabled
) : ConfigPage(parent) {
    this->on_player_bind = on_player_bind;
    this->game_input_contexts = game_input_contexts;
    this->num_players = num_players;
    this->game_input_bindings = game_input_bindings;
    this->player_keyboard_enabled = player_keyboard_enabled;
    this->multiplayer_enabled = num_players > 1;

    recompui::ContextId context = get_current_context();

    add_header();
    {
        auto header_left = header->get_left();
        for (uint8_t i = 0; i < num_players; i++) {
            std::string player_text = "P" + std::to_string(i + 1);
            auto player_button = context.create_element<PillButton>(header_left, player_text, "icons/Cont.svg", ButtonStyle::Basic, PillButtonSize::XLarge);
            player_elements.push_back(player_button);
            player_button->add_pressed_callback([this, i]() {
                set_selected_player(i);
                update_control_mappings();
            });
        }
    }
    {
        auto header_right = header->get_right();
        Button* assign_players_button = context.create_element<Button>(header_right, "Assign players", ButtonStyle::Primary);
        assign_players_button->add_pressed_callback([]() {
            recompui::assign_players_modal->open();
            recompinput::start_player_assignment();
        });
    }

    add_footer();
    {
        auto footer_left = footer->get_left();
        keyboard_toggle = context.create_element<Toggle>(footer_left);
        keyboard_toggle->set_checked(player_keyboard_enabled[selected_player]);
        keyboard_toggle->add_checked_callback([this, set_player_keyboard_enabled](bool checked) {
            set_player_keyboard_enabled(this->selected_player, checked);
            update_control_mappings();
        });
        auto kb_label = context.create_element<Label>(footer_left, "Enable keyboard", LabelStyle::Normal);
        kb_label->set_margin_left(12.0f);
    }
    {
        auto footer_right = footer->get_right();
        context.create_element<Button>(footer_right, "Reset to defaults", ButtonStyle::Warning);
    }

    description_container = context.create_element<Element>(body->get_right(), 0, "p", true);
    description_container->set_text(
        "Sometimes, the windows combine with the seams in a way\n"
        "That twitches on a peak at the place where the spirit was slain\n"
        "Hey, one foot leads to another\n"
        "Night's for sleep, blue curtains, covers, sequins in the eyes\n"
        "That's a fine time to dine\n"
        "Divine who's circling, feeding the cards to the midwives"
    );

    set_selected_player(selected_player);
    render_control_mappings();
}

void ConfigPageControls::render_control_mappings() {
    recompui::ContextId context = get_current_context();
    auto body_left = body->get_left();
    body_left->clear_children();

    body_left->set_display(Display::Block);
    body_left->set_position(Position::Relative);
    body_left->set_height(100.0f, Unit::Percent);
    
    {
        auto body_left_scroll = context.create_element<Element>(body_left, 0, "div", false);
        body_left_scroll->set_display(Display::Block);
        body_left_scroll->set_width(100.0f, Unit::Percent);
        body_left_scroll->set_max_height(100.0f, Unit::Percent);
        body_left_scroll->set_overflow_y(Overflow::Scroll);
    
        for (int i = 0; i < game_input_contexts.size(); i++) {
            auto &ctx = game_input_contexts[i];
            context.create_element<GameInputRow>(
                body_left_scroll,
                &ctx,
                game_input_bindings[selected_player].at(ctx.input_id),
                [this, i]() {
                    this->on_option_hover(i);
                },
                [this](recompinput::GameInput game_input, int input_index) {
                    this->on_bind_click(game_input, input_index);
                }
            );
        }
    }
}

void ConfigPageControls::update_control_mappings() {
    for (size_t i = 0; i < game_input_rows.size(); i++) {
        game_input_rows[i]->update_bindings(
            game_input_bindings[selected_player].at(game_input_contexts[i].input_id)
        );
    }
}

ConfigPageControls::~ConfigPageControls() {
}

void ConfigPageControls::on_bind_click(recompinput::GameInput game_input, int input_index) {
    on_player_bind(this->selected_player, game_input, input_index);
}

void ConfigPageControls::set_selected_player(int player) {
    static const std::array<theme::color, 8> player_colors = {
        theme::color::Player1,
        theme::color::Player2,
        theme::color::Player3,
        theme::color::Player4,
        theme::color::Player5,
        theme::color::Player6,
        theme::color::Player7,
        theme::color::Player8
    };

    selected_player = player;
    for (uint8_t i = 0; i < num_players; i++) {
        auto player_button = player_elements[i];
        theme::color player_color = player_colors[i % player_colors.size()];
        if (i == selected_player) {
            player_button->apply_theme_style(player_color, false, true);
        } else {
            player_button->apply_theme_style(player_color, true, true);
        }
    }

    keyboard_toggle->set_checked(player_keyboard_enabled[selected_player]);
}

void ConfigPageControls::on_option_hover(uint8_t index) {
    if (description_container) {
        description_container->set_text(game_input_contexts[index].description);
    }
}


} // namespace recompui
