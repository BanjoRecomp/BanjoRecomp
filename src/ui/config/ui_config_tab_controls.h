#pragma once

#include "../elements/ui_element.h"
#include "../elements/ui_modal.h"
#include "ui_config_page_controls.h"

namespace recompui {
    class ConfigTabControls : public TabContext {
    public:
        ConfigTabControls() : TabContext("Controls") {}
        Element* create_tab_contents(recompui::ContextId context, Element* parent) override;
    };

    extern ConfigTabControls config_tab_controls;

} // namespace recompui
