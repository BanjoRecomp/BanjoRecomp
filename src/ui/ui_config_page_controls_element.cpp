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

ElementConfigPageControls::ElementConfigPageControls(const Rml::String& tag) : Rml::Element(tag) {
    SetProperty(Rml::PropertyId::Display, Rml::Style::Display::Block);
    SetProperty("width", "100%");
    SetProperty("height", "100%");
    
    recompui::Element this_compat(this);
    recompui::ContextId context = get_current_context();

    controls_page = context.create_element<ConfigPageControls>(
        &this_compat,
        recompinput::get_num_players(),
        temp_game_input_contexts
    );
}

ElementConfigPageControls::~ElementConfigPageControls() {
}

} // namespace recompui
