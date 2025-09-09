#include "ui_config_tab_graphics.h"
#include "banjo_render.h"

namespace recompui {
recomp::config::Config config_graphics("Graphics", "graphics_2", true);
ConfigTabGraphics config_tab_graphics(&config_graphics);

using EnumOptionVector = const std::vector<recomp::config::ConfigOptionEnumOption>;

namespace gfx_keys {
    const std::string developer_mode = "developer_mode";
    const std::string res_option = "res_option";
    const std::string wm_option = "wm_option";
    const std::string hr_option = "hr_option";
    const std::string api_option = "api_option";
    const std::string ar_option = "ar_option";
    const std::string msaa_option = "msaa_option";
    const std::string rr_option = "rr_option";
    const std::string hpfb_option = "hpfb_option";
    const std::string rr_manual_value = "rr_manual_value";
    const std::string ds_option = "ds_option";
}

static bool is_steam_deck = false;

ultramodern::renderer::WindowMode wm_default() {
    return is_steam_deck ? ultramodern::renderer::WindowMode::Fullscreen : ultramodern::renderer::WindowMode::Windowed;
}

#ifdef __gnu_linux__
void detect_steam_deck() {
    // Check if the board vendor is Valve.
    std::ifstream board_vendor_file("/sys/devices/virtual/dmi/id/board_vendor");
    std::string line;
    if (std::getline(board_vendor_file, line).good() && line == "Valve") {
        is_steam_deck = true;
        return;
    }

    // Check if the SteamDeck variable is set to 1.
    const char* steam_deck_env = getenv("SteamDeck");
    if (steam_deck_env != nullptr && std::string{steam_deck_env} == "1") {
        is_steam_deck = true;
        return;
    }

    is_steam_deck = false;
    return;
}
#else
void detect_steam_deck() { is_steam_deck = false; }
#endif

EnumOptionVector resolution_options = {
    {ultramodern::renderer::Resolution::Original, "Original", "Original"},
    {ultramodern::renderer::Resolution::Original2x, "Original2x", "Original 2x"},
    {ultramodern::renderer::Resolution::Auto, "Auto", "Auto"},
};

enum class DownsamplingOption {
    Off = 0,
    X2 = 2,
    X4 = 4,
};
EnumOptionVector downsampling_options = {
    {DownsamplingOption::Off, "Off"},
    {DownsamplingOption::X2, "2x"},
    {DownsamplingOption::X4, "4x"},
};

EnumOptionVector window_mode_options = {
    {ultramodern::renderer::WindowMode::Windowed, "Windowed"},
    {ultramodern::renderer::WindowMode::Fullscreen, "Fullscreen"}
};

#if defined(_WIN32)
#define ALLOW_D3D12
#endif
#if defined(_WIN32) || defined(__linux__)
#define ALLOW_VULKAN
#endif
#if defined(__APPLE__)
#define ALLOW_METAL
#endif

EnumOptionVector graphics_api_options = {
    {ultramodern::renderer::GraphicsApi::Auto, "Auto"},
#ifdef ALLOW_D3D12
    {ultramodern::renderer::GraphicsApi::D3D12, "D3D12"},
#endif
#ifdef ALLOW_VULKAN
    {ultramodern::renderer::GraphicsApi::Vulkan, "Vulkan"},
#endif
#ifdef ALLOW_METAL
    {ultramodern::renderer::GraphicsApi::Metal, "Metal"},
#endif
};

EnumOptionVector aspect_ratio_options = {
    {ultramodern::renderer::AspectRatio::Original, "Original"},
    {ultramodern::renderer::AspectRatio::Expand, "Expand"},
    // {ultramodern::renderer::AspectRatio::Manual, "Manual"},
};

EnumOptionVector antialiasing_options = {
    {ultramodern::renderer::Antialiasing::None, "None"},
    {ultramodern::renderer::Antialiasing::MSAA2X, "MSAA2X", "2X"},
    {ultramodern::renderer::Antialiasing::MSAA4X, "MSAA4X", "4X"},
    // {ultramodern::renderer::Antialiasing::MSAA8X, "MSAA8X"},
};

EnumOptionVector refresh_rate_options = {
    {ultramodern::renderer::RefreshRate::Original, "Original"},
    {ultramodern::renderer::RefreshRate::Display, "Display"},
    {ultramodern::renderer::RefreshRate::Manual, "Manual"},
};

EnumOptionVector hpfb_options = {
    {ultramodern::renderer::HighPrecisionFramebuffer::Auto, "Auto"},
    {ultramodern::renderer::HighPrecisionFramebuffer::On, "On"},
    {ultramodern::renderer::HighPrecisionFramebuffer::Off, "Off"},
};

EnumOptionVector hud_ratio_mode_options = {
    {ultramodern::renderer::HUDRatioMode::Original, "Original"},
    {ultramodern::renderer::HUDRatioMode::Clamp16x9, "Clamp16x9", "16:9"},
    {ultramodern::renderer::HUDRatioMode::Full, "Expand"},
};

std::string get_downsampling_details(ultramodern::renderer::Resolution res_option, DownsamplingOption ds_option) {
    switch (res_option) {
        default:
        case ultramodern::renderer::Resolution::Auto:
            return "Downsampling is not available at auto resolution";
        case ultramodern::renderer::Resolution::Original:
            if (ds_option == DownsamplingOption::X2) {
                return "Rendered in 480p and scaled to 240p";
            } else if (ds_option == DownsamplingOption::X4) {
                return "Rendered in 960p and scaled to 240p";
            }
            return "";
        case ultramodern::renderer::Resolution::Original2x:
            if (ds_option == DownsamplingOption::X2) {
                return "Rendered in 960p and scaled to 480p";
            } else if (ds_option == DownsamplingOption::X4) {
                return "Rendered in 4K and scaled to 480p";
            }
            return "";
    }
}

using OptionChangeContext = recomp::config::OptionChangeContext;

template <typename T = uint32_t>
T get_graphics_enum_value(const std::string& key, OptionChangeContext change_context = OptionChangeContext::Permanent) {
    if (change_context == OptionChangeContext::Temporary) {
        return static_cast<T>(std::get<uint32_t>(config_graphics.get_temp_option_value(key)));
    } else {
        return static_cast<T>(std::get<uint32_t>(config_graphics.get_option_value(key)));
    }
}
template <typename T = uint32_t>
T get_graphics_number_value(const std::string& key, OptionChangeContext change_context = OptionChangeContext::Permanent) {
    if (change_context == OptionChangeContext::Temporary) {
        return static_cast<T>(std::get<double>(config_graphics.get_temp_option_value(key)));
    } else {
        return static_cast<T>(std::get<double>(config_graphics.get_option_value(key)));
    }
}

bool get_graphics_bool_value(const std::string& key, OptionChangeContext change_context = OptionChangeContext::Permanent) {
    if (change_context == OptionChangeContext::Temporary) {
        return std::get<bool>(config_graphics.get_temp_option_value(key));
    } else {
        return std::get<bool>(config_graphics.get_option_value(key));
    }
}

void determine_downsampling_display(ultramodern::renderer::Resolution res_option, OptionChangeContext change_context) {
    DownsamplingOption ds_opt = get_graphics_enum_value<DownsamplingOption>(gfx_keys::ds_option, change_context);
    config_graphics.update_option_enum_details(gfx_keys::ds_option, get_downsampling_details(res_option, ds_opt));
}

std::string get_framerate_text() {
    return
        "Sets the game's output framerate. This option does not affect gameplay."
        "<br />"
        "<br />"
        "Note: If you have issues with <recomp-color primary>Display</recomp-color> mode while using an external frame limiter, use <recomp-color primary>Manual</recomp-color> mode instead and configure it to that same frame limit."
        "<br />"
        "<br />"
        "<recomp-color primary>Detected display refresh rate: " + std::to_string(ultramodern::get_display_refresh_rate()) + "hz</recomp-color>";
}

void save_graphics_config() {
    ultramodern::renderer::GraphicsConfig new_config;
    new_config.developer_mode = get_graphics_bool_value(gfx_keys::developer_mode);
    new_config.res_option = get_graphics_enum_value<ultramodern::renderer::Resolution>(gfx_keys::res_option);
    new_config.wm_option = get_graphics_enum_value<ultramodern::renderer::WindowMode>(gfx_keys::wm_option);
    new_config.hr_option = get_graphics_enum_value<ultramodern::renderer::HUDRatioMode>(gfx_keys::hr_option);
    new_config.api_option = get_graphics_enum_value<ultramodern::renderer::GraphicsApi>(gfx_keys::api_option);
    new_config.ar_option = get_graphics_enum_value<ultramodern::renderer::AspectRatio>(gfx_keys::ar_option);
    new_config.msaa_option = get_graphics_enum_value<ultramodern::renderer::Antialiasing>(gfx_keys::msaa_option);
    new_config.rr_option = get_graphics_enum_value<ultramodern::renderer::RefreshRate>(gfx_keys::rr_option);
    new_config.hpfb_option = get_graphics_enum_value<ultramodern::renderer::HighPrecisionFramebuffer>(gfx_keys::hpfb_option);
    new_config.rr_manual_value = get_graphics_number_value<int>(gfx_keys::rr_manual_value);

    new_config.ds_option = get_graphics_enum_value<int>(gfx_keys::ds_option);

    ultramodern::renderer::set_graphics_config(new_config);
}

void ConfigTabGraphics::on_init() {
    config_graphics.set_save_callback(save_graphics_config);

    config_graphics.add_bool_option(
        gfx_keys::developer_mode,
        "Dev Mode",
        "Enables developer features.",
        false,
        true
    );

    config_graphics.add_enum_option(
        gfx_keys::res_option,
        "Resolution",
        "Sets the output resolution of the game. <recomp-color primary>Original</recomp-color> matches the game's original 240p resolution. <recomp-color primary>Original 2x</recomp-color> will render at 480p. <recomp-color primary>Auto</recomp-color> will scale based on the game window's resolution.",
        resolution_options,
        ultramodern::renderer::Resolution::Auto
    );

    config_graphics.add_enum_option(
        gfx_keys::ds_option,
        "Downsampling Quality",
        "Renders at a higher resolution and scales it down to the output resolution for increased quality. Only available in <recomp-color primary>Original</recomp-color> and <recomp-color primary>Original 2x</recomp-color> resolution."
        "<br />"
        "<br />"
        "Note: <recomp-color primary>4x</recomp-color> downsampling quality at <recomp-color primary>Original 2x</recomp-color> resolution may cause performance issues on low end devices, as it will cause the game to render <recomp-color warning>at almost 4k internal resolution</recomp-color>.",
        downsampling_options,
        DownsamplingOption::Off
    );
    config_graphics.add_option_change_callback(
        gfx_keys::res_option,
        [](recomp::config::ConfigValueVariant cur_value, recomp::config::ConfigValueVariant prev_value, OptionChangeContext change_context) {
            auto new_opt = static_cast<ultramodern::renderer::Resolution>(std::get<uint32_t>(cur_value));
            determine_downsampling_display(new_opt, change_context);
        }
    );
    config_graphics.add_option_change_callback(
        gfx_keys::ds_option,
        [](recomp::config::ConfigValueVariant cur_value, recomp::config::ConfigValueVariant prev_value, OptionChangeContext change_context) {
            auto resolution_opt = get_graphics_enum_value<ultramodern::renderer::Resolution>(gfx_keys::res_option, change_context);
            determine_downsampling_display(resolution_opt, change_context);
        }
    );
    config_graphics.add_option_disable_dependency(
        gfx_keys::ds_option,
        gfx_keys::res_option,
        ultramodern::renderer::Resolution::Auto
    );

    config_graphics.on_json_parse_option(gfx_keys::ds_option, [](const nlohmann::json& j) {
        return j.get<uint32_t>();
    });

    config_graphics.on_json_serialize_option(gfx_keys::ds_option, [](const recomp::config::ConfigValueVariant& value) {
        return nlohmann::json(std::get<uint32_t>(value));
    });

    config_graphics.add_enum_option(
        gfx_keys::ar_option,
        "Aspect Ratio",
        "Sets the horizontal aspect ratio. <recomp-color primary>Original</recomp-color> uses the game's original 4:3 aspect ratio. <recomp-color primary>Expand</recomp-color> will adjust to match the game window's aspect ratio.",
        aspect_ratio_options,
        ultramodern::renderer::AspectRatio::Expand
    );

    config_graphics.add_enum_option(
        gfx_keys::wm_option,
        "Window Mode",
        "Sets whether the game should display <recomp-color primary>Windowed</recomp-color> or <recomp-color primary>Fullscreen</recomp-color>. You can also use <recomp-color primary>F11</recomp-color> or <recomp-color primary>Alt + Enter</recomp-color> to toggle this option.",
        window_mode_options,
        wm_default()
    );

    config_graphics.add_enum_option(
        gfx_keys::rr_option,
        "Framerate",
        get_framerate_text(),
        refresh_rate_options,
        ultramodern::renderer::RefreshRate::Display
    );
    config_graphics.add_option_change_callback(
        gfx_keys::res_option,
        [](recomp::config::ConfigValueVariant cur_value, recomp::config::ConfigValueVariant prev_value, OptionChangeContext change_context) {
            auto new_opt = static_cast<ultramodern::renderer::Resolution>(std::get<uint32_t>(cur_value));
            determine_downsampling_display(new_opt, change_context);
        }
    );
    config_graphics.add_number_option(
        gfx_keys::rr_manual_value,
        "",
        get_framerate_text(),
        20.0, 240.0, 1.0, 0, false, 60.0
    );

    config_graphics.add_option_hidden_dependency(
        gfx_keys::rr_manual_value,
        gfx_keys::rr_option,
        ultramodern::renderer::RefreshRate::Original,
        ultramodern::renderer::RefreshRate::Display
    );

    config_graphics.add_enum_option(
        gfx_keys::msaa_option,
        "MS Anti-Aliasing",
        "Sets the multisample anti-aliasing (MSAA) quality level. This reduces jagged edges in the final image at the expense of rendering performance."
        "<br />"
        "<br />"
        "<recomp-color primary>Note: This option won't be available if your GPU does not support programmable MSAA sample positions, as it is currently required to avoid rendering glitches.</recomp-color>",
        antialiasing_options,
        ultramodern::renderer::Antialiasing::MSAA2X
    );

    config_graphics.add_enum_option(
        gfx_keys::hr_option,
        "HUD Placement",
        "Adjusts the placement of HUD elements to fit the selected aspect ratio. <recomp-color primary>Expand</recomp-color> will use the aspect ratio of the game's output window.",
        hud_ratio_mode_options,
        ultramodern::renderer::HUDRatioMode::Clamp16x9
    );

    config_graphics.add_enum_option(
        gfx_keys::api_option,
        "Graphics API",
        "Selects the graphics API to use.",
        graphics_api_options,
        ultramodern::renderer::GraphicsApi::Auto
    );
    config_graphics.add_option_hidden_dependency(
        gfx_keys::api_option,
        gfx_keys::developer_mode,
        false
    );
    config_graphics.add_enum_option(
        gfx_keys::hpfb_option,
        "High Precision Framebuffer",
        "Sets whether to use a high precision framebuffer.",
        hpfb_options,
        ultramodern::renderer::HighPrecisionFramebuffer::Off
    );
    config_graphics.add_option_hidden_dependency(
        gfx_keys::hpfb_option,
        gfx_keys::developer_mode,
        false
    );

    config_graphics.load_config();
    save_graphics_config();
}

void update_msaa_supported(bool supported) {
    if (!supported) {
        config_graphics.update_option_enum_details(
            gfx_keys::msaa_option,
            supported ? "Available" : "Not available (missing sample positions support)"
        );
        config_graphics.update_option_disabled(
            gfx_keys::msaa_option,
            true
        );
    } else {
        auto max_msaa = banjo::renderer::RT64MaxMSAA();
        if (max_msaa < RT64::UserConfiguration::Antialiasing::MSAA2X) {
            config_graphics.update_enum_option_disabled(gfx_keys::msaa_option, static_cast<uint32_t>(ultramodern::renderer::Antialiasing::MSAA2X), true);
        }
        if (max_msaa < RT64::UserConfiguration::Antialiasing::MSAA4X) {
            config_graphics.update_enum_option_disabled(gfx_keys::msaa_option, static_cast<uint32_t>(ultramodern::renderer::Antialiasing::MSAA4X), true);
        }
    }

}


} // namespace recompui
