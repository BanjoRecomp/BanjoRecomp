#include <array>

#include "xxHash/xxh3.h"

#include "librecomp/helpers.hpp"
#include "recomp_input.h"
#include "ultramodern/ultramodern.hpp"

// Arrays that hold the mappings for every input for keyboard and controller respectively.
using input_mapping = std::array<recomp::InputField, recomp::bindings_per_input>;
using input_mapping_array = std::array<input_mapping, static_cast<size_t>(recomp::GameInput::COUNT)>;

struct InputProfile {
    std::string key;
    std::string name;
    recomp::InputDevice device;
    input_mapping_array mappings;
    bool custom = false;
};

static std::vector<InputProfile> input_profiles{};
static std::array<std::pair<int, int>, 4> players_input_profile_indices{};
static std::unordered_map<std::string, int> input_profile_key_index_map{};
static std::vector<int> input_profile_custom_indices[2]{};

struct Controller {
    recomp::ControllerGUID guid;
    int profile_index;
};

static std::vector<Controller> controllers;
static std::unordered_map<uint64_t, int> controller_hash_index_map{};

static int keyboard_sp_profile_index = -1;
static int controller_sp_profile_index = -1;

static const std::string keyboard_sp_profile_key = "keyboard_sp";
static const std::string controller_sp_profile_key = "controller_sp";
static const std::string keyboard_sp_profile_name = "Keyboard (SP)";
static const std::string controller_sp_profile_name = "Controller (SP)";

static const std::string keyboard_mp_profile_key = "keyboard_mp_player_"; // + player index
static const std::string keyboard_mp_profile_name = "Keyboard "; // + "(player number)"

// Make the button value array, which maps a button index to its bit field.
#define DEFINE_INPUT(name, value, readable) uint16_t(value##u),
static const std::array n64_button_values = {
    DEFINE_N64_BUTTON_INPUTS()
};
#undef DEFINE_INPUT

// Make the input name array.
#define DEFINE_INPUT(name, value, readable) readable,
static const std::vector<std::string> input_names = {
    DEFINE_ALL_INPUTS()
};
#undef DEFINE_INPUT

// Make the input enum name array.
#define DEFINE_INPUT(name, value, readable) #name,
static const std::vector<std::string> input_enum_names = {
    DEFINE_ALL_INPUTS()
};
#undef DEFINE_INPUT

size_t recomp::get_num_inputs() {
    return (size_t)GameInput::COUNT;
}

const std::string& recomp::get_input_name(GameInput input) {
    return input_names.at(static_cast<size_t>(input));
}

const std::string& recomp::get_input_enum_name(GameInput input) {
    return input_enum_names.at(static_cast<size_t>(input));
}

recomp::GameInput recomp::get_input_from_enum_name(const std::string_view enum_name) {
    auto find_it = std::find(input_enum_names.begin(), input_enum_names.end(), enum_name);
    if (find_it == input_enum_names.end()) {
        return recomp::GameInput::COUNT;
    }

    return static_cast<recomp::GameInput>(find_it - input_enum_names.begin());
}

// Due to an RmlUi limitation this can't be const. Ideally it would return a const reference or even just a straight up copy.
recomp::InputField& recomp::get_input_binding(int profile_index, GameInput input, size_t binding_index) {
    input_mapping& cur_input_mapping = input_profiles[profile_index].mappings.at(static_cast<size_t>(input));
    if (binding_index < cur_input_mapping.size()) {
        return cur_input_mapping[binding_index];
    }
    else {
        static recomp::InputField dummy_field = {};
        return dummy_field;
    }
}

void recomp::set_input_binding(int profile_index, recomp::GameInput input, size_t binding_index, recomp::InputField value) {
    input_mapping& cur_input_mapping = input_profiles[profile_index].mappings.at(static_cast<size_t>(input));
    if (binding_index < cur_input_mapping.size()) {
        cur_input_mapping[binding_index] = value;
    }
}

int recomp::add_input_profile(const std::string &key, const std::string &name, InputDevice device, bool custom) {
    auto it = input_profile_key_index_map.find(key);
    if (it != input_profile_key_index_map.end()) {
        return it->second;
    }

    int index = (int)(input_profiles.size());
    InputProfile profile;
    profile.key = key;
    profile.name = name;
    profile.device = device;
    profile.custom = custom;
    input_profiles.emplace_back(profile);
    input_profile_key_index_map.emplace(key, index);

    if (custom) {
        switch (device) {
        case InputDevice::Controller:
            input_profile_custom_indices[0].emplace_back(index);
            break;
        case InputDevice::Keyboard:
            input_profile_custom_indices[1].emplace_back(index);
            break;
        default:
            assert(false && "Unknown input device.");
            break;
        }
    }

    return index;
}

int recomp::get_input_profile_by_key(const std::string &key) {
    auto it = input_profile_key_index_map.find(key);
    if (it != input_profile_key_index_map.end()) {
        return it->second;
    }
    else {
        return -1;
    }
}

const std::string &recomp::get_input_profile_key(int profile_index) {
    return input_profiles[profile_index].key;
}

const std::string &recomp::get_input_profile_name(int profile_index) {
    return input_profiles[profile_index].name;
}

recomp::InputDevice recomp::get_input_profile_device(int profile_index) {
    return input_profiles[profile_index].device;
}

bool recomp::is_input_profile_custom(int profile_index) {
    return input_profiles[profile_index].custom;
}

int recomp::get_input_profile_count() {
    return (int)(input_profiles.size());
}

const std::vector<int> recomp::get_indices_for_custom_profiles(InputDevice device) {
    static std::vector<int> empty;

    switch (device) {
    case InputDevice::Controller:
        return input_profile_custom_indices[0];
    case InputDevice::Keyboard:
        return input_profile_custom_indices[1];
    default:
        assert(false && "Unknown input device.");
        return empty;
    }
}

void recomp::set_input_profile_for_player(int player_index, int profile_index, InputDevice device) {
    switch (device) {
    case InputDevice::Controller:
        players_input_profile_indices[player_index].first = profile_index;
        break;
    case InputDevice::Keyboard:
        players_input_profile_indices[player_index].second = profile_index;
        break;
    default:
        assert(false && "Unknown input device.");
        break;
    }
}

int recomp::get_input_profile_for_player(int player_index, InputDevice device) {
    switch (device) {
    case InputDevice::Controller:
        return players_input_profile_indices[player_index].first;
    case InputDevice::Keyboard:
        return players_input_profile_indices[player_index].second;
    default:
        assert(false && "Unknown input device.");
        return -1;
    }
}

int recomp::add_controller(ControllerGUID guid, int profile_index) {
    auto it = controller_hash_index_map.find(guid.hash);
    if (it != controller_hash_index_map.end()) {
        controllers[it->second].profile_index = profile_index;
        return it->second;
    }

    int index = (int)(controllers.size());
    Controller controller;
    controller.guid = guid;
    controller.profile_index = profile_index;
    controllers.emplace_back(controller);
    controller_hash_index_map.emplace(guid.hash, index);
    return index;
}

const recomp::ControllerGUID &recomp::get_controller_guid(int controller_index) {
    return controllers[controller_index].guid;
}

int recomp::get_controller_profile_index(int controller_index) {
    return controllers[controller_index].profile_index;
}

int recomp::get_controller_by_guid(ControllerGUID guid) {
    auto it = controller_hash_index_map.find(guid.hash);
    if (it != controller_hash_index_map.end()) {
        return it->second;
    }
    else {
        return -1;
    }
}

int recomp::get_controller_count() {
    return (int)(controllers.size());
}

recomp::ControllerGUID recomp::get_guid_from_sdl_controller(SDL_GameController *game_controller) {
    if (game_controller == nullptr) {
        return {};
    }

    SDL_Joystick *joystick = SDL_GameControllerGetJoystick(game_controller);
    if (joystick == nullptr) {
        return {};
    }

    Uint16 vendor, product, version, crc16;
    const char *joystick_serial = SDL_JoystickGetSerial(joystick);
    SDL_JoystickGUID joystick_guid = SDL_JoystickGetGUID(joystick);
    SDL_GetJoystickGUIDInfo(joystick_guid, &vendor, &product, &version, &crc16);

    recomp::ControllerGUID guid;
    guid.serial = joystick_serial != nullptr ? joystick_serial : "";
    guid.vendor = vendor;
    guid.product = product;
    guid.version = version;
    guid.crc16 = crc16;

    // Compute the hash from the GUID.
    XXH3_state_t state;
    XXH3_64bits_reset(&state);
    XXH3_64bits_update(&state, &guid.vendor, sizeof(guid.vendor));
    XXH3_64bits_update(&state, &guid.product, sizeof(guid.product));
    XXH3_64bits_update(&state, &guid.version, sizeof(guid.version));
    XXH3_64bits_update(&state, &guid.crc16, sizeof(guid.crc16));
    guid.hash = XXH3_64bits_digest(&state);

    return guid;
}

int recomp::get_controller_profile_index_from_sdl_controller(SDL_GameController *game_controller) {
    if (game_controller == nullptr) {
        return -1;
    }

    ControllerGUID guid = recomp::get_guid_from_sdl_controller(game_controller);
    if (guid.hash == 0) {
        return -1;
    }

    int controller_index = recomp::get_controller_by_guid(guid);
    if (controller_index < 0) {
        return -1;
    }

    return recomp::get_controller_profile_index(controller_index);
}

std::string get_mp_keyboard_profile_key(int player_index) {
    return keyboard_mp_profile_key + std::to_string(player_index);
}

std::string get_mp_keyboard_profile_name(int player_index) {
    return keyboard_mp_profile_name + "(Player " + std::to_string(player_index + 1) + ")";
}

void clear_mapping(int profile_index, recomp::GameInput input) {
    for (size_t binding_index = 0; binding_index < recomp::bindings_per_input; binding_index++) {
        recomp::set_input_binding(profile_index, input, binding_index, recomp::InputField{});
    }
};

// Used primarily for multiplayer keyboard input profiles
void clear_all_mappings(int profile_index) {
    clear_mapping(profile_index, recomp::GameInput::A);
    clear_mapping(profile_index, recomp::GameInput::B);
    clear_mapping(profile_index, recomp::GameInput::Z);
    clear_mapping(profile_index, recomp::GameInput::START);
    clear_mapping(profile_index, recomp::GameInput::DPAD_UP);
    clear_mapping(profile_index, recomp::GameInput::DPAD_DOWN);
    clear_mapping(profile_index, recomp::GameInput::DPAD_LEFT);
    clear_mapping(profile_index, recomp::GameInput::DPAD_RIGHT);
    clear_mapping(profile_index, recomp::GameInput::L);
    clear_mapping(profile_index, recomp::GameInput::R);
    clear_mapping(profile_index, recomp::GameInput::C_UP);
    clear_mapping(profile_index, recomp::GameInput::C_DOWN);
    clear_mapping(profile_index, recomp::GameInput::C_LEFT);
    clear_mapping(profile_index, recomp::GameInput::C_RIGHT);

    clear_mapping(profile_index, recomp::GameInput::X_AXIS_NEG);
    clear_mapping(profile_index, recomp::GameInput::X_AXIS_POS);
    clear_mapping(profile_index, recomp::GameInput::Y_AXIS_NEG);
    clear_mapping(profile_index, recomp::GameInput::Y_AXIS_POS);

    clear_mapping(profile_index, recomp::GameInput::TOGGLE_MENU);
    clear_mapping(profile_index, recomp::GameInput::ACCEPT_MENU);
    clear_mapping(profile_index, recomp::GameInput::APPLY_MENU);
};

void recomp::initialize_input_bindings() {
    keyboard_sp_profile_index = recomp::add_input_profile(keyboard_sp_profile_key, keyboard_sp_profile_name, recomp::InputDevice::Keyboard, false);
    controller_sp_profile_index = recomp::add_input_profile(controller_sp_profile_key, controller_sp_profile_name, recomp::InputDevice::Controller, false);

    // Set Player 1 to the SP profiles by default.
    recomp::set_input_profile_for_player(0, keyboard_sp_profile_index, recomp::InputDevice::Keyboard);
    recomp::set_input_profile_for_player(0, controller_sp_profile_index, recomp::InputDevice::Controller);

    for (int i = 0; i < recompinput::get_num_players(); i++) {
        int profile_index = recomp::add_input_profile(
            get_mp_keyboard_profile_key(i),
            get_mp_keyboard_profile_name(i),
            recomp::InputDevice::Keyboard,
            false
        );
        clear_all_mappings(profile_index);
    }
}

int recomp::get_sp_controller_profile_index() {
    return controller_sp_profile_index;
}

int recomp::get_sp_keyboard_profile_index() {
    return keyboard_sp_profile_index;
}

int recomp::get_mp_keyboard_profile_index(int player_index) {
    return recomp::get_input_profile_by_key(get_mp_keyboard_profile_key(player_index));
}

std::string recomp::get_string_from_controller_guid(ControllerGUID guid) {
    return "SERIAL_" + guid.serial + "_VID_" + std::to_string(guid.vendor) + "_PID_" + std::to_string(guid.product) +"_VERSION_" + std::to_string(guid.version) +"_CRC16_" + std::to_string(guid.crc16);
}

bool recomp::get_n64_input(int player_index, uint16_t* buttons_out, float* x_out, float* y_out) {
    uint16_t cur_buttons = 0;
    float cur_x = 0.0f;
    float cur_y = 0.0f;
    if (!recomp::game_input_disabled()) {
        auto check_buttons = [&](int profile_index) {
            if (profile_index < 0) {
                return;
            }

            const input_mapping_array &mappings = input_profiles[profile_index].mappings;
            for (size_t i = 0; i < n64_button_values.size(); i++) {
                cur_buttons |= recomp::get_input_digital(player_index, mappings[(size_t)(GameInput::N64_BUTTON_START) + i]) ? n64_button_values[i] : 0;
            }
        };

        auto check_joystick = [&](int profile_index) {
            if (profile_index < 0) {
                return;
            }

            const input_mapping_array &mappings = input_profiles[profile_index].mappings;
            cur_x += recomp::get_input_analog(player_index, mappings[(size_t)GameInput::X_AXIS_POS]) - recomp::get_input_analog(player_index, mappings[(size_t)GameInput::X_AXIS_NEG]);
            cur_y += recomp::get_input_analog(player_index, mappings[(size_t)GameInput::Y_AXIS_POS]) - recomp::get_input_analog(player_index, mappings[(size_t)GameInput::Y_AXIS_NEG]);
        };

        check_buttons(players_input_profile_indices[player_index].first);
        check_buttons(players_input_profile_indices[player_index].second);

        check_joystick(players_input_profile_indices[player_index].first);
        recomp::apply_joystick_deadzone(cur_x, cur_y, &cur_x, &cur_y);
        check_joystick(players_input_profile_indices[player_index].second);
    }

    *buttons_out = cur_buttons;
    *x_out = std::clamp(cur_x, -1.0f, 1.0f);
    *y_out = std::clamp(cur_y, -1.0f, 1.0f);

    return true;
}
