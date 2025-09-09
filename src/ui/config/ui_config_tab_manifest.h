#pragma once
#include "librecomp/config.hpp"
#include "ui_config_common.h"
#include "ui_config_page_options_menu.h"
#include "../elements/ui_element.h"
#include "../elements/ui_modal.h"

namespace recompui {

    class ConfigTab : public TabContext {
    private:
        recomp::config::Config *config = nullptr;
        ConfigOptionPropertyCallbacks callbacks;
    public:
        ConfigTab(recomp::config::Config *config);

        Element* create_tab_contents(recompui::ContextId context, Element* parent) override;
        bool can_be_closed() override;
        void on_tab_close() override;
    };

} // namespace recompui
