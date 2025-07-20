#ifndef __BANJO_CONFIG_H__
#define __BANJO_CONFIG_H__

#include <filesystem>
#include <string_view>
#include "ultramodern/config.hpp"
#include "recomp_input.h"

namespace banjo {
    constexpr std::u8string_view program_id = u8"BanjoRecompiled";
    constexpr std::string_view program_name = "Banjo: Recompiled";

    // TODO: Move loading configs to the runtime once we have a way to allow per-project customization.
    void load_config();
    void save_config();
    
    void reset_input_bindings();
    void reset_cont_input_bindings(int controller_num);
    void reset_kb_input_bindings(int controller_num);
    void reset_single_input_binding(int controller_num, recomp::InputDevice device, recomp::GameInput input);

    std::filesystem::path get_app_folder_path();
    
    bool get_debug_mode_enabled();
    void set_debug_mode_enabled(bool enabled);
    
    enum class CameraInvertMode {
        InvertNone,
        InvertX,
        InvertY,
        InvertBoth,
        OptionCount
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(banjo::CameraInvertMode, {
        {banjo::CameraInvertMode::InvertNone, "InvertNone"},
        {banjo::CameraInvertMode::InvertX, "InvertX"},
        {banjo::CameraInvertMode::InvertY, "InvertY"},
        {banjo::CameraInvertMode::InvertBoth, "InvertBoth"}
    });

    CameraInvertMode get_camera_invert_mode();
    void set_camera_invert_mode(CameraInvertMode mode);

    CameraInvertMode get_analog_camera_invert_mode();
    void set_analog_camera_invert_mode(CameraInvertMode mode);

    enum class AnalogCamMode {
        On,
        Off,
		OptionCount
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(banjo::AnalogCamMode, {
        {banjo::AnalogCamMode::On, "On"},
        {banjo::AnalogCamMode::Off, "Off"}
    });

    AnalogCamMode get_analog_cam_mode();
    void set_analog_cam_mode(AnalogCamMode mode);

    void open_quit_game_prompt();
};

#endif
