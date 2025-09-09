#include "ui_config_tab_controls.h"

namespace recompui {

#define DEFINE_INPUT(name, value, readable) GameInputContext{ readable, "Description for " #readable " input.", recompinput::GameInput::##name, !(recompinput::GameInput::##name == recompinput::GameInput::TOGGLE_MENU || recompinput::GameInput::##name == recompinput::GameInput::ACCEPT_MENU) },
static std::vector<struct GameInputContext> temp_game_input_contexts = {
    DEFINE_ALL_INPUTS()
};
#undef DEFINE_INPUT

Element* ConfigTabControls::create_tab_contents(recompui::ContextId context, Element* parent) {
    Modal *modal = get_parent_modal();
    apply_set_first_focusable_below_tabs_callback();
    Element *active_tab = modal != nullptr ? modal->get_active_tab() : nullptr;

    ConfigPage *controls_page = context.create_element<ConfigPageControls>(
        parent,
        recompinput::get_num_players(),
        temp_game_input_contexts,
        active_tab,
        set_first_focusable_below_tabs
    );

    return controls_page;
}

ConfigTabControls config_tab_controls = ConfigTabControls();

} // namespace recompui
