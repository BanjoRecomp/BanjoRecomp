#pragma once

#include "elements/ui_element.h"
#include "elements/ui_modal.h"
#include "config/ui_config_tab_manifest.h"

namespace recompui {

extern Modal *config_modal;

void add_config_modal_tab(TabContext *tab, bool add_to_front = false);
void init_config_modal();

} // namespace recompui
