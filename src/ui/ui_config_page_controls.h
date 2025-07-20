#pragma once

#include "elements/ui_config_page.h"
#include "recomp_input.h"
#include "elements/ui_icon_button.h"
#include "elements/ui_binding_button.h"
#include "elements/ui_pill_button.h"
#include "elements/ui_toggle.h"

// TODO: remove after moving to recompinput
namespace recompinput {
    using GameInput = recomp::GameInput;
    using InputField = recomp::InputField;
    using InputDevice = recomp::InputDevice;

    constexpr int num_binding_slots = 4;
} // recompinput

namespace recompui {

struct GameInputContext {
    std::string name;
    std::string description;
    recompinput::GameInput input_id;
    bool clearable;
};

using BindingList = std::vector<recompinput::InputField>;
// Receives which GameInput to be bound to, and the index of the binding that was clicked
using on_bind_click_callback = std::function<void(recompinput::GameInput, int)>;
// Player index, GameInput to be bound to, and the index of the binding that is being assigned
using on_player_bind_callback = std::function<void(int, recompinput::GameInput, int)>;

// One single row of a game input mapping
class GameInputRow : public Element {
protected:
    recompinput::GameInput input_id;
    BindingList bindings;

    int active_binding_index = -1;
    bool is_binding = false;

    std::vector<BindingButton*> binding_buttons = {};

    Style active_style;
    std::function<void()> on_hover_callback;

    virtual void process_event(const Event &e) override;
    std::string_view get_type_name() override { return "GameInputRow"; }
public:
    GameInputRow(
        Element *parent,
        GameInputContext *input_ctx,
        BindingList bindings,
        std::function<void()> on_hover_callback,
        on_bind_click_callback on_bind_click
    );
    virtual ~GameInputRow();
    void update_bindings(BindingList &new_bindings);
};

using PlayerBindings = std::map<recompinput::GameInput, BindingList>;

// Sets if keyboard should be enabled, first arg player index, second is a bool to enable/disable
using set_player_keyboard_enabled_callback = std::function<void(int, bool)>;

class ConfigPageControls : public ConfigPage {
protected:
    int selected_player = 0;
    int num_players;
    bool multiplayer_enabled;

    std::vector<GameInputContext> game_input_contexts;
    std::vector<PlayerBindings> game_input_bindings;
    std::vector<bool> player_keyboard_enabled;

    std::vector<PillButton*> player_elements;
    std::vector<GameInputRow*> game_input_rows;
    Toggle *keyboard_toggle;
    Element *description_container = nullptr;
    on_player_bind_callback on_player_bind;

    std::string_view get_type_name() override { return "ConfigPageControls"; }
private:
    void on_option_hover(uint8_t index);
    void on_bind_click(recompinput::GameInput game_input, int input_index);
public:
    ConfigPageControls(
        Element *parent,
        int num_players,
        std::vector<GameInputContext> game_input_contexts,
        std::vector<PlayerBindings> game_input_bindings,
        std::vector<bool> player_keyboard_enabled,
        on_player_bind_callback on_player_bind,
        set_player_keyboard_enabled_callback set_player_keyboard_enabled
    );
    virtual ~ConfigPageControls();

    void render_control_mappings();
    void update_control_mappings();
    void set_selected_player(int player);
};

} // namespace recompui
