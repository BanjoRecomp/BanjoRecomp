#include "ui_config_page_controls_element.h"

// TODO: remove hardcoded recompinput funcs and data
namespace recompinput {

}

namespace recompui {

ConfigPageControls *controls_page = nullptr;

static bool is_multiplayer_enabled() {
    return true;
} 


#define DEFINE_INPUT(name, value, readable) GameInputContext{ readable, "Description for " #readable " input.", recompinput::GameInput::##name, !(recompinput::GameInput::##name == recompinput::GameInput::TOGGLE_MENU || recompinput::GameInput::##name == recompinput::GameInput::ACCEPT_MENU) },
static std::vector<struct GameInputContext> temp_game_input_contexts = {
    DEFINE_ALL_INPUTS()
};
#undef DEFINE_INPUT

struct BindingInfo {
    int player_index;
    recompinput::GameInput game_input;
    int binding_index;
    bool is_scanning = false;
};

static BindingInfo temp_binding_info = { 0, recompinput::GameInput::COUNT, 0 };

static void temp_on_bind_player(int player_index, recompinput::GameInput game_input, int binding_index) {
    temp_binding_info.player_index = player_index;
    temp_binding_info.game_input = game_input;
    temp_binding_info.binding_index = binding_index;
    temp_binding_info.is_scanning = true;
}

ElementConfigPageControls::ElementConfigPageControls(const Rml::String& tag) : Rml::Element(tag) {
    SetProperty(Rml::PropertyId::Display, Rml::Style::Display::Block);
    SetProperty("width", "100%");
    SetProperty("height", "100%");
    
    recompui::Element this_compat(this);
    recompui::ContextId context = get_current_context();

    controls_page = context.create_element<ConfigPageControls>(
        &this_compat,
        recompinput::get_num_players(),
        temp_game_input_contexts,
        temp_on_bind_player
    );
}

ElementConfigPageControls::~ElementConfigPageControls() {
}

} // namespace recompui
