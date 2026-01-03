#include "banjo_config.h"
#include "recompui/recompui.h"
#include "recompui/config.h"
#include "recompinput/recompinput.h"
#include "banjo_sound.h"
#include "banjo_render.h"
#include "banjo_support.h"
#include "ultramodern/config.hpp"
#include "librecomp/files.hpp"
#include "librecomp/config.hpp"
#include "util/file.h"
#include <filesystem>
#include <fstream>
#include <iomanip>

#if defined(_WIN32)
#include <Shlobj.h>
#elif defined(__linux__)
#include <unistd.h>
#include <pwd.h>
#elif defined(__APPLE__)
#include "apple/rt64_apple.h"
#endif

static void add_general_options(recomp::config::Config &config) {
    using EnumOptionVector = const std::vector<recomp::config::ConfigOptionEnumOption>;

    static EnumOptionVector note_saving_mode_options = {
        {banjo::NoteSavingMode::Off, "Off", "Off"},
        {banjo::NoteSavingMode::On, "On", "On"},
    };
    config.add_enum_option(
        banjo::configkeys::general::note_saving_mode,
        "Note Saving",
        "Saves collected notes so that you don't need to collect them again when revisiting a level. <recomp-color primary>On</recomp-color> is the default, while <recomp-color primary>off</recomp-color> matches the original game.",
        note_saving_mode_options,
        banjo::NoteSavingMode::On
    );

    static EnumOptionVector camera_invert_mode_options = {
        {banjo::CameraInvertMode::InvertNone, "InvertNone", "None"},
        {banjo::CameraInvertMode::InvertX, "InvertX", "Invert X"},
        {banjo::CameraInvertMode::InvertY, "InvertY", "Invert Y"},
        {banjo::CameraInvertMode::InvertBoth, "InvertBoth", "Invert Both"}
    };
    config.add_enum_option(
        banjo::configkeys::general::camera_invert_mode,
        "Aiming Camera Inverting",
        // TODO: Update for banjo
        "Inverts the camera controls. <recomp-color primary>Invert Y</recomp-color> is the default and matches the original game.",
        camera_invert_mode_options,
        banjo::CameraInvertMode::InvertY
    );

    static EnumOptionVector analog_cam_mode_options = {
        {banjo::AnalogCamMode::Off, "Off", "Off"},
        {banjo::AnalogCamMode::On, "On", "On"},
    };
    config.add_enum_option(
        banjo::configkeys::general::analog_cam_mode,
        "Analog Camera",
        // TODO: Update for banjo
        "Enables the analog camera.",
        analog_cam_mode_options,
        banjo::AnalogCamMode::Off
    );
    config.add_enum_option(
        banjo::configkeys::general::analog_camera_invert_mode,
        "Analog Camera Inverting",
        // TODO: Update for banjo
        "Inverts the camera controls for the analog camera if it's enabled. <recomp-color primary>None</recomp-color> is the default.",
        camera_invert_mode_options,
        banjo::CameraInvertMode::InvertNone
    );
    config.add_option_disable_dependency(
        banjo::configkeys::general::analog_camera_invert_mode,
        banjo::configkeys::general::analog_cam_mode,
        banjo::AnalogCamMode::Off
    );
}

template <typename T = uint32_t>
T get_general_config_enum_value(const std::string& option_id) {
    return static_cast<T>(std::get<uint32_t>(recompui::config::get_general_config().get_option_value(option_id)));
}

banjo::NoteSavingMode banjo::get_note_saving_mode() {
    return get_general_config_enum_value<banjo::NoteSavingMode>(banjo::configkeys::general::note_saving_mode);
}

banjo::CameraInvertMode banjo::get_camera_invert_mode() {
    return get_general_config_enum_value<banjo::CameraInvertMode>(banjo::configkeys::general::camera_invert_mode);
}

banjo::CameraInvertMode banjo::get_analog_camera_invert_mode() {
    return get_general_config_enum_value<banjo::CameraInvertMode>(banjo::configkeys::general::analog_camera_invert_mode);
}

banjo::AnalogCamMode banjo::get_analog_cam_mode() {
    return get_general_config_enum_value<banjo::AnalogCamMode>(banjo::configkeys::general::analog_cam_mode);
}

static void add_sound_options(recomp::config::Config &config) {
    config.add_percent_number_option(
        banjo::configkeys::sound::bgm_volume,
        "Background Music Volume",
        "Controls the overall volume of background music.",
        100.0f
    );
}
template <typename T = uint32_t>
T get_sound_config_number_value(const std::string& option_id) {
    return static_cast<T>(std::get<double>(recompui::config::get_sound_config().get_option_value(option_id)));
}

int banjo::get_bgm_volume() {
    return get_sound_config_number_value<int>(banjo::configkeys::sound::bgm_volume);
}

void banjo::init_config() {
    std::filesystem::path recomp_dir = recompui::file::get_app_folder_path();

    if (!recomp_dir.empty()) {
        std::filesystem::create_directories(recomp_dir);
    }

    recompui::config::GeneralTabOptions general_options{};
    general_options.has_rumble_strength = true;
    general_options.has_gyro_sensitivity = false;
    general_options.has_mouse_sensitivity = false;

    auto &general_config = recompui::config::create_general_tab(general_options);
    add_general_options(general_config);

    auto &graphics_config = recompui::config::create_graphics_tab();

    recompui::config::create_controls_tab();

    auto &sound_config = recompui::config::create_sound_tab();
    add_sound_options(sound_config);

    recompui::config::create_mods_tab();

    recompui::config::finalize();

}
