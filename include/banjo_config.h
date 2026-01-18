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
            inline const std::string third_person_camera_invert_mode = "third_person_camera_invert_mode";
            inline const std::string flying_and_swimming_invert_mode = "flying_and_swimming_invert_mode";
            inline const std::string first_person_invert_mode = "first_person_invert_mode";
            inline const std::string analog_camera_sensitivity = "analog_camera_sensitivity";
        }

        namespace sound {
            inline const std::string bgm_volume = "bgm_volume";
        }

        namespace graphics {
            inline const std::string cutscene_aspect_ratio_mode = "cutscene_aspect_ratio_mode";
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

    CameraInvertMode get_third_person_camera_mode();

    CameraInvertMode get_flying_and_swimming_invert_mode();

    CameraInvertMode get_first_person_invert_mode();

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

    uint32_t get_analog_cam_sensitivity();

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

    enum class CutsceneAspectRatioMode {
        Original,
        Clamp16x9,
        Full,
        OptionCount
    };

    CutsceneAspectRatioMode get_cutscene_aspect_ratio_mode();

    void open_quit_game_prompt();
};

#endif
