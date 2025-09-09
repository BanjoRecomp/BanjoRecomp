#pragma once

#include "librecomp/config.hpp"
#include "ui_config_tab_manifest.h"

namespace recompui {
    class ConfigTabGraphics : public ConfigTab {
    public:
        ConfigTabGraphics(recomp::config::Config *config) : ConfigTab(config) {}

        void on_init() override;
    };

    extern recomp::config::Config config_graphics;
    extern ConfigTabGraphics config_tab_graphics;

    void update_msaa_supported(bool supported);

} // namespace recompui
