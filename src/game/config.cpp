#include "banjo_config.h"
#include "recompui/recompui.h"
#include "recompui/config.h"
#include "recompinput/recompinput.h"
#include "banjo_sound.h"
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
    static EnumOptionVector analog_cam_mode_options = {
        {banjo::AnalogCamMode::Off, "Off", "Off"},
        {banjo::AnalogCamMode::On, "On", "On"},
    };
    config.add_enum_option(
        banjo::configkeys::general::analog_cam_mode,
        "Analog Camera",
        "Enables the analog camera.",
        analog_cam_mode_options,
        banjo::AnalogCamMode::Off
    );
    static EnumOptionVector camera_invert_mode_options = {
        {banjo::CameraInvertMode::InvertNone, "InvertNone", "None"},
        {banjo::CameraInvertMode::InvertX, "InvertX", "Invert X"},
        {banjo::CameraInvertMode::InvertY, "InvertY", "Invert Y"},
        {banjo::CameraInvertMode::InvertBoth, "InvertBoth", "Invert Both"}
    };
    config.add_enum_option(
        banjo::configkeys::general::third_person_camera_invert_mode,
        "Invert Camera",
        "Inverts the camera controls for the third person camera if it's enabled. <recomp-color primary>Invert X</recomp-color> is the default and matches the original game.<br /><br />If analog camera is off, only the <recomp-color primary>Invert X</recomp-color> setting will take effect.",
        camera_invert_mode_options,
        banjo::CameraInvertMode::InvertX
    );
    static EnumOptionVector first_person_invert_mode_options = {
        {banjo::CameraInvertMode::InvertNone, "InvertNone", "None"},
        {banjo::CameraInvertMode::InvertX, "InvertX", "Invert X"},
        {banjo::CameraInvertMode::InvertY, "InvertY", "Invert Y"},
        {banjo::CameraInvertMode::InvertBoth, "InvertBoth", "Invert Both"}
    };
    config.add_enum_option(
        banjo::configkeys::general::first_person_invert_mode,
        "Invert First Person View",
        "Inverts the camera controls in first person view. <recomp-color primary>Invert Y</recomp-color> is the default and matches the original game.",
        first_person_invert_mode_options,
        banjo::CameraInvertMode::InvertY
    );
    static EnumOptionVector flying_and_swimming_invert_options = {
        {banjo::CameraInvertMode::InvertNone, "InvertNone", "None"},
        {banjo::CameraInvertMode::InvertX, "InvertX", "Invert X"},
        {banjo::CameraInvertMode::InvertY, "InvertY", "Invert Y"},
        {banjo::CameraInvertMode::InvertBoth, "InvertBoth", "Invert Both"}
    };
    config.add_enum_option(
        banjo::configkeys::general::flying_and_swimming_invert_mode,
        "Invert Flying & Swimming",
        "Inverts the controls for swimming and flying. <recomp-color primary>Invert Y</recomp-color> is the default and matches the original game.",
        flying_and_swimming_invert_options,
        banjo::CameraInvertMode::InvertY
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

banjo::CameraInvertMode banjo::get_third_person_camera_mode() {
    return get_general_config_enum_value<banjo::CameraInvertMode>(banjo::configkeys::general::third_person_camera_invert_mode);
}

banjo::CameraInvertMode banjo::get_flying_and_swimming_invert_mode() {
    return get_general_config_enum_value<banjo::CameraInvertMode>(banjo::configkeys::general::flying_and_swimming_invert_mode);
}

banjo::CameraInvertMode banjo::get_first_person_invert_mode() {
    return get_general_config_enum_value<banjo::CameraInvertMode>(banjo::configkeys::general::first_person_invert_mode);
}

banjo::AnalogCamMode banjo::get_analog_cam_mode() {
    return get_general_config_enum_value<banjo::AnalogCamMode>(banjo::configkeys::general::analog_cam_mode);
}

template <typename T = uint32_t>
T get_graphics_config_enum_value(const std::string& option_id) {
    return static_cast<T>(std::get<uint32_t>(recompui::config::get_graphics_config().get_option_value(option_id)));
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

static void add_graphics_options(recomp::config::Config &config) {
    using EnumOptionVector = const std::vector<recomp::config::ConfigOptionEnumOption>;
    static EnumOptionVector cutscene_aspect_ratio_mode_options = {
        {banjo::CutsceneAspectRatioMode::Original, "Original", "Original"},
        {banjo::CutsceneAspectRatioMode::Clamp16x9, "Clamp16x9", "16:9"},
        {banjo::CutsceneAspectRatioMode::Full, "Expand", "Expand"},
    };
    config.add_enum_option(
        banjo::configkeys::graphics::cutscene_aspect_ratio_mode,
        "Cutscene Aspect Ratio",
        "Sets the aspect ratio limit for cutscenes. Cutscenes have been adjusted to work in <recomp-color primary>16:9</recomp-color>, which is the default option. Wider aspect ratios may show details that weren't meant to be on-screen.",
        cutscene_aspect_ratio_mode_options,
        banjo::CutsceneAspectRatioMode::Clamp16x9
    );    
}

static void set_control_descriptions() {
    recompinput::set_game_input_description(recompinput::GameInput::Y_AXIS_POS, "Used to move and for steering while flying and swimming. Axis inversion for flying and swimming can be configured in the General tab.");
    recompinput::set_game_input_description(recompinput::GameInput::Y_AXIS_NEG, "Used to move and for steering while flying and swimming. Axis inversion for flying and swimming can be configured in the General tab.");
    recompinput::set_game_input_description(recompinput::GameInput::X_AXIS_NEG, "Used to move and for steering while flying and swimming. Axis inversion for flying and swimming can be configured in the General tab.");
    recompinput::set_game_input_description(recompinput::GameInput::X_AXIS_POS, "Used to move and for steering while flying and swimming. Axis inversion for flying and swimming can be configured in the General tab.");
    recompinput::set_game_input_description(recompinput::GameInput::A, "Used to jump and select options in menus. Also used for flying upwards.");
    recompinput::set_game_input_description(recompinput::GameInput::B, "Used for attacks, which change depending on whether you are stationary, moving, in the air, or crouching.");
    recompinput::set_game_input_description(recompinput::GameInput::Z, "Used to crouch, which enables A, B and the C-Buttons to perform different actions.");
    recompinput::set_game_input_description(recompinput::GameInput::L, "Unused. Mods may use it for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::R, "Used to center the camera behind Banjo on the ground, and to perform tighter turns while flying or swimming.");
    recompinput::set_game_input_description(recompinput::GameInput::START, "Used for pausing and for skipping certain cutscenes.");
    recompinput::set_game_input_description(recompinput::GameInput::C_UP, "Used to enter first-person mode, and to shoot eggs while holding Z.");
    recompinput::set_game_input_description(recompinput::GameInput::C_DOWN, "Used to toggle between the different camera zoom levels, and to shoot eggs backwards while holding Z.");
    recompinput::set_game_input_description(recompinput::GameInput::C_LEFT, "Used to rotate the camera sideways. Axis inversion can be configured in the General tab. Also used to enter Talon Trot while holding Z.");
    recompinput::set_game_input_description(recompinput::GameInput::C_RIGHT, "Used to rotate the camera sideways. Axis inversion can be configured in the General tab). Also used to enter Wonderwing while holding Z.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_UP, "Unused. Mods may use it for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_DOWN, "Unused. Mods may use it for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_LEFT, "Unused. Mods may use it for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_RIGHT, "Unused. Mods may use it for additional features.");
}

banjo::CutsceneAspectRatioMode banjo::get_cutscene_aspect_ratio_mode() {
    return get_graphics_config_enum_value<banjo::CutsceneAspectRatioMode>(banjo::configkeys::graphics::cutscene_aspect_ratio_mode);
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
    add_graphics_options(graphics_config);

    set_control_descriptions();
    recompui::config::create_controls_tab();

    auto &sound_config = recompui::config::create_sound_tab();
    add_sound_options(sound_config);

    recompui::config::create_mods_tab();

    recompui::config::finalize();

}
