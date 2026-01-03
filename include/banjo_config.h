#ifndef __BANJO_CONFIG_H__
#define __BANJO_CONFIG_H__

#include <filesystem>
#include <string>
#include <string_view>

#include "json/json.hpp"

namespace banjo {
    inline const std::u8string program_id = u8"BanjoRecompiled";
    inline const std::string program_name = "Banjo: Recompiled";

    namespace configkeys {
        namespace general {
            inline const std::string note_saving_mode = "note_saving_mode";
            inline const std::string camera_invert_mode = "camera_invert_mode";
            inline const std::string analog_cam_mode = "analog_cam_mode";
            inline const std::string analog_camera_invert_mode = "analog_camera_invert_mode";
        }

        namespace sound {
            inline const std::string bgm_volume = "bgm_volume";
        }
    }

    // TODO: Move loading configs to the runtime once we have a way to allow per-project customization.
    void init_config();

    enum class CameraInvertMode {
        InvertNone,
        InvertX,
        InvertY,
        InvertBoth
    };

    CameraInvertMode get_camera_invert_mode();

    CameraInvertMode get_analog_camera_invert_mode();

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

    enum class NoteSavingMode {
        On,
        Off,
        OptionCount
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(banjo::NoteSavingMode, {
        {banjo::NoteSavingMode::On, "On"},
        {banjo::NoteSavingMode::Off, "Off"}
    });

    NoteSavingMode get_note_saving_mode();

    void open_quit_game_prompt();
};

#endif
