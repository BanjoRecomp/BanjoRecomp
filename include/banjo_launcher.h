#ifndef __BANJO_LAUNCHER_H__
#define __BANJO_LAUNCHER_H__

#include "recompui/recompui.h"

namespace banjo {
    void launcher_animation_setup(recompui::LauncherMenu *menu, recompui::Element *title);
    void launcher_animation_update(recompui::LauncherMenu *menu);

    constexpr float launcher_options_right_position_start = 96.0f;
    constexpr float launcher_options_right_position_end = 96.0f + 24.0f;
    constexpr float launcher_options_top_offset = 96.0f;
    constexpr float launcher_options_title_offset = 120.0f;
}

#endif
