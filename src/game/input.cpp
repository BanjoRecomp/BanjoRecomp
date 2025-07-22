#include <atomic>
#include <mutex>

#include "ultramodern/ultramodern.hpp"
#include "recomp.h"
#include "recomp_input.h"
#include "banjo_config.h"
#include "recomp_ui.h"
#include "SDL.h"
#include "promptfont.h"
#include "GamepadMotion.hpp"
#include "../ui/ui_assign_players_modal.h"

constexpr float axis_threshold = 0.5f;

struct ControllerState {
    SDL_GameController* controller;
    std::array<float, 3> latest_accelerometer;
    GamepadMotion motion;
    uint32_t prev_gyro_timestamp;
    ControllerState() : controller{}, latest_accelerometer{}, motion{}, prev_gyro_timestamp{} {
        motion.Reset();
        motion.SetCalibrationMode(GamepadMotionHelpers::CalibrationMode::Stillness | GamepadMotionHelpers::CalibrationMode::SensorFusion);
    };
};

static struct {
    const Uint8* keys = nullptr;
    SDL_Keymod keymod = SDL_Keymod::KMOD_NONE;
    int numkeys = 0;
    std::atomic_int32_t mouse_wheel_pos = 0;
    std::mutex controllers_mutex;
    std::vector<SDL_GameController*> detected_controllers{};
    std::vector<recomp::ControllerOption> detected_controller_options{};
    std::array<recompinput::AssignedPlayer, recompinput::temp_max_players> assigned_controllers{}; // Only used when Multiplayer is enabled.
    std::unordered_map<SDL_JoystickID, ControllerState> controller_states;
    bool single_controller = false;

    std::array<float, 2> rotation_delta{};
    std::array<float, 2> mouse_delta{};
    std::mutex pending_input_mutex;
    std::array<float, 2> pending_rotation_delta{};
    std::array<float, 2> pending_mouse_delta{};

    std::array<float, 4> cur_rumble{};
    std::array<bool, 4> rumble_active{};
} InputState;

static struct {
    std::list<std::filesystem::path> files_dropped;
} DropState;

std::atomic<recomp::InputDevice> scanning_device = recomp::InputDevice::COUNT;
std::atomic<recomp::InputField> scanned_input;

static recompinput::BindingState binding_state;
static recompinput::PlayerAssignmentState player_assignment_state{};

void recompinput::start_scanning_for_binding(int player_index, recomp::GameInput game_input, int binding_index) {
    binding_state.is_scanning = true;
    binding_state.found_binding = false;
    binding_state.player_index = player_index;
    binding_state.game_input = game_input;
    binding_state.binding_index = binding_index;
}

void recompinput::stop_scanning_for_binding() {
    binding_state.is_scanning = false;
    binding_state.found_binding = false;
    binding_state.player_index = -1;
    binding_state.game_input = recomp::GameInput::COUNT;
    binding_state.binding_index = -1;
}

recompinput::BindingState& recompinput::get_binding_state() {
    return binding_state;
}

int recompinput::get_num_players() {
    return static_cast<int>(InputState.assigned_controllers.size());
}

recompinput::AssignedPlayer& recompinput::get_assigned_player(int player_index, bool temp_player) {
    if (temp_player) {
        return player_assignment_state.temp_assigned_players[player_index];
    } else {
        return InputState.assigned_controllers[player_index];
    }
}

bool recompinput::get_player_is_assigned(int player_index) {
    if (player_index < 0 || player_index >= recompinput::get_num_players()) {
        return false;
    }

    return player_assignment_state.temp_assigned_players[player_index].is_assigned;
}

void recompinput::start_player_assignment() {
    player_assignment_state.is_assigning = true;
    player_assignment_state.player_index = 0;

    for (auto& player : player_assignment_state.temp_assigned_players) {
        player = AssignedPlayer{};
    }
}

static bool queue_close_player_assignment_modal = false;

void recompinput::stop_player_assignment() {
    player_assignment_state.is_assigning = false;
    player_assignment_state.player_index = -1;
}

void recompinput::stop_player_assignment_and_close_modal() {
    recompinput::stop_player_assignment();
    queue_close_player_assignment_modal = true;
}

void recompinput::commit_player_assignment() {
    recompinput::stop_player_assignment_and_close_modal();

    for (int i = 0; i < recompinput::get_num_players(); i++) {
        InputState.assigned_controllers[i] = player_assignment_state.temp_assigned_players[i];
    }
}

bool recompinput::is_player_assignment_active() {
    return player_assignment_state.is_assigning;
}

bool recompinput::does_player_have_controller(int player_index) {
    if (player_index < 0 || player_index >= recompinput::get_num_players()) {
        return false;
    }
    return player_assignment_state.temp_assigned_players[player_index].controller != nullptr;
}

std::chrono::steady_clock::duration recompinput::get_player_time_since_last_button_press(int player_index) {
    if (player_index < 0 || player_index >= recompinput::get_num_players()) {
        return std::chrono::steady_clock::duration::zero();
    }
    return ultramodern::time_since_start() - player_assignment_state.temp_assigned_players[player_index].last_button_press_timestamp;
}

void process_player_assignment(SDL_Event* event) {
    if (queue_close_player_assignment_modal) {
        recompui::assign_players_modal->close();
        queue_close_player_assignment_modal = false;
    }

    if (!player_assignment_state.is_assigning) {
        return;
    }

    recomp::refresh_controller_options();

    switch (event->type) {
    case SDL_EventType::SDL_KEYDOWN: {
        SDL_KeyboardEvent* keyevent = &event->key;

        switch (keyevent->keysym.scancode) {
        case SDL_Scancode::SDL_SCANCODE_ESCAPE:
            // TODO: Restore previous assignment?
            recompinput::stop_player_assignment();
            return;
        case SDL_Scancode::SDL_SCANCODE_SPACE:
            player_assignment_state.temp_assigned_players[player_assignment_state.player_index].is_assigned = true;
            player_assignment_state.temp_assigned_players[player_assignment_state.player_index].keyboard_enabled = true;
            player_assignment_state.player_index++;
            printf("Assigned keyboard to player %d\n", player_assignment_state.player_index - 1);
            break;
        default:
            for (int i = 0; i < player_assignment_state.player_index; i++) {
                if (player_assignment_state.temp_assigned_players[i].keyboard_enabled && player_assignment_state.temp_assigned_players[i].controller == nullptr) {
                    player_assignment_state.temp_assigned_players[i].last_button_press_timestamp = ultramodern::time_since_start();
                }
            }
            break;
        }
        break;
    }
    case SDL_EventType::SDL_CONTROLLERBUTTONDOWN: {
        SDL_ControllerButtonEvent* button_event = &event->cbutton;
        SDL_JoystickID joystick_id = button_event->which;
        auto controller_state = InputState.controller_states[joystick_id];

        bool can_be_mapped = true;
        for (int i = 0; i < player_assignment_state.player_index; i++) {
            if (player_assignment_state.temp_assigned_players[i].controller == controller_state.controller) {
                can_be_mapped = false;
                player_assignment_state.temp_assigned_players[i].last_button_press_timestamp = ultramodern::time_since_start();
                break;
            }
        }

        if (can_be_mapped) {
            recompinput::AssignedPlayer& assigned_player = player_assignment_state.temp_assigned_players[player_assignment_state.player_index];
            assigned_player.is_assigned = true;
            assigned_player.controller = controller_state.controller;
            assigned_player.last_button_press_timestamp = ultramodern::time_since_start();
            player_assignment_state.player_index++;
            printf("Assigned controller %d to player %d\n", joystick_id, player_assignment_state.player_index - 1);
        }

        break;
    }
    }

    if (player_assignment_state.player_index >= recompinput::get_num_players()) {
        recompinput::stop_player_assignment();
    }
}

void set_scanned_input(recomp::InputField value) {
    scanning_device.store(recomp::InputDevice::COUNT);
    scanned_input.store(value);
}

recomp::InputField recomp::get_scanned_input() {
    recomp::InputField ret = scanned_input.load();
    scanned_input.store({});
    return ret;
}

void recomp::start_scanning_input(recomp::InputDevice device) {
    scanned_input.store({});
    scanning_device.store(device);
}

void recomp::stop_scanning_input() {
    scanning_device.store(recomp::InputDevice::COUNT);
}

void queue_if_enabled(SDL_Event* event) {
    if (!recomp::all_input_disabled()) {
        recompui::queue_event(*event);
    }
}

static std::atomic_bool cursor_enabled = true;

void recompui::set_cursor_visible(bool visible) {
    cursor_enabled.store(visible);
}

bool should_override_keystate(SDL_Scancode key, SDL_Keymod mod) {
    // Override Enter when Alt is held.
    if (key == SDL_Scancode::SDL_SCANCODE_RETURN) {
        if (mod & SDL_Keymod::KMOD_ALT) {
            return true;
        }
    }

    return false;
}

bool sdl_event_filter(void* userdata, SDL_Event* event) {
    switch (event->type) {
    case SDL_EventType::SDL_KEYDOWN:
    {
        SDL_KeyboardEvent* keyevent = &event->key;

        // Skip repeated events when not in the menu
        if (!recompui::is_context_capturing_input() &&
            event->key.repeat) {
            break;
        }

        if ((keyevent->keysym.scancode == SDL_Scancode::SDL_SCANCODE_RETURN && (keyevent->keysym.mod & SDL_Keymod::KMOD_ALT)) ||
            keyevent->keysym.scancode == SDL_Scancode::SDL_SCANCODE_F11
            ) {
            recompui::toggle_fullscreen();
        }
        if (scanning_device != recomp::InputDevice::COUNT) {
            if (keyevent->keysym.scancode == SDL_Scancode::SDL_SCANCODE_ESCAPE) {
                recomp::cancel_scanning_input();
            }
            else if (scanning_device == recomp::InputDevice::Keyboard) {
                set_scanned_input({ recomp::InputType::Keyboard, keyevent->keysym.scancode });
            }
        }
        else {
            if (!should_override_keystate(keyevent->keysym.scancode, static_cast<SDL_Keymod>(keyevent->keysym.mod))) {
                queue_if_enabled(event);
            }
        }
    }
    break;
    case SDL_EventType::SDL_CONTROLLERDEVICEADDED:
    {
        SDL_ControllerDeviceEvent* controller_event = &event->cdevice;
        SDL_GameController* controller = SDL_GameControllerOpen(controller_event->which);
        printf("Controller added: %d\n", controller_event->which);
        if (controller != nullptr) {
            printf("  Instance ID: %d\n", SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller)));
            printf("  Path: %s\n", SDL_JoystickPath(SDL_GameControllerGetJoystick(controller)));
            ControllerState& state = InputState.controller_states[SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))];
            state.controller = controller;

            if (SDL_GameControllerHasSensor(controller, SDL_SensorType::SDL_SENSOR_GYRO) && SDL_GameControllerHasSensor(controller, SDL_SensorType::SDL_SENSOR_ACCEL)) {
                SDL_GameControllerSetSensorEnabled(controller, SDL_SensorType::SDL_SENSOR_GYRO, SDL_TRUE);
                SDL_GameControllerSetSensorEnabled(controller, SDL_SensorType::SDL_SENSOR_ACCEL, SDL_TRUE);
            }
        }
    }
    break;
    case SDL_EventType::SDL_CONTROLLERDEVICEREMOVED:
    {
        SDL_ControllerDeviceEvent* controller_event = &event->cdevice;
        printf("Controller removed: %d\n", controller_event->which);
        InputState.controller_states.erase(controller_event->which);
    }
    break;
    case SDL_EventType::SDL_QUIT: {
        if (!ultramodern::is_game_started()) {
            ultramodern::quit();
            return true;
        }

        banjo::open_quit_game_prompt();
        recompui::activate_mouse();
        break;
    }
    case SDL_EventType::SDL_MOUSEWHEEL:
    {
        SDL_MouseWheelEvent* wheel_event = &event->wheel;
        InputState.mouse_wheel_pos.fetch_add(wheel_event->y * (wheel_event->direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1));
    }
    queue_if_enabled(event);
    break;
    case SDL_EventType::SDL_CONTROLLERBUTTONDOWN:
        if (scanning_device != recomp::InputDevice::COUNT) {
            // TODO: Needs the active controller tab index.
            auto menuToggleBinding0 = recomp::get_input_binding(0, recomp::GameInput::TOGGLE_MENU, 0, recomp::InputDevice::Controller);
            auto menuToggleBinding1 = recomp::get_input_binding(0, recomp::GameInput::TOGGLE_MENU, 1, recomp::InputDevice::Controller);
            // note - magic number: 0 is InputType::None
            if ((menuToggleBinding0.input_type != recomp::InputType::None && event->cbutton.button == menuToggleBinding0.input_id) ||
                (menuToggleBinding1.input_type != recomp::InputType::None && event->cbutton.button == menuToggleBinding1.input_id)) {
                recomp::cancel_scanning_input();
            }
            else if (scanning_device == recomp::InputDevice::Controller) {
                SDL_ControllerButtonEvent* button_event = &event->cbutton;
                auto scanned_input_index = recomp::get_scanned_input_index();
                if ((scanned_input_index == static_cast<int>(recomp::GameInput::TOGGLE_MENU) ||
                    scanned_input_index == static_cast<int>(recomp::GameInput::ACCEPT_MENU) ||
                    scanned_input_index == static_cast<int>(recomp::GameInput::APPLY_MENU)) && (
                    button_event->button == SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP ||
                    button_event->button == SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN ||
                    button_event->button == SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT ||
                    button_event->button == SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
                    break;
                }

                set_scanned_input({ recomp::InputType::ControllerDigital, button_event->button });
            }
        }
        else {
            queue_if_enabled(event);
        }
        break;
    case SDL_EventType::SDL_CONTROLLERAXISMOTION:
        if (scanning_device == recomp::InputDevice::Controller) {
            auto scanned_input_index = recomp::get_scanned_input_index();
            if (scanned_input_index == static_cast<int>(recomp::GameInput::TOGGLE_MENU) ||
                scanned_input_index == static_cast<int>(recomp::GameInput::ACCEPT_MENU) ||
                scanned_input_index == static_cast<int>(recomp::GameInput::APPLY_MENU)) {
                break;
            }

            SDL_ControllerAxisEvent* axis_event = &event->caxis;
            float axis_value = axis_event->value * (1/32768.0f);
            if (axis_value > axis_threshold) {
                SDL_Event set_stick_return_event;
                set_stick_return_event.type = SDL_USEREVENT;
                set_stick_return_event.user.code = axis_event->axis;
                set_stick_return_event.user.data1 = nullptr;
                set_stick_return_event.user.data2 = nullptr;
                recompui::queue_event(set_stick_return_event);

                set_scanned_input({ recomp::InputType::ControllerAnalog, axis_event->axis + 1 });
            }
            else if (axis_value < -axis_threshold) {
                SDL_Event set_stick_return_event;
                set_stick_return_event.type = SDL_USEREVENT;
                set_stick_return_event.user.code = axis_event->axis;
                set_stick_return_event.user.data1 = nullptr;
                set_stick_return_event.user.data2 = nullptr;
                recompui::queue_event(set_stick_return_event);

                set_scanned_input({ recomp::InputType::ControllerAnalog, -axis_event->axis - 1 });
            }
        }
        else {
            queue_if_enabled(event);
        }
        break;
    case SDL_EventType::SDL_CONTROLLERSENSORUPDATE:
        if (event->csensor.sensor == SDL_SensorType::SDL_SENSOR_ACCEL) {
            // Convert acceleration to g's.
            float x = event->csensor.data[0] / SDL_STANDARD_GRAVITY;
            float y = event->csensor.data[1] / SDL_STANDARD_GRAVITY;
            float z = event->csensor.data[2] / SDL_STANDARD_GRAVITY;
            ControllerState& state = InputState.controller_states[event->csensor.which];
            state.latest_accelerometer[0] = x;
            state.latest_accelerometer[1] = y;
            state.latest_accelerometer[2] = z;
        }
        else if (event->csensor.sensor == SDL_SensorType::SDL_SENSOR_GYRO) {
            // constexpr float gyro_threshold = 0.05f;
            // Convert rotational velocity to degrees per second.
            constexpr float rad_to_deg = 180.0f / M_PI;
            float x = event->csensor.data[0] * rad_to_deg;
            float y = event->csensor.data[1] * rad_to_deg;
            float z = event->csensor.data[2] * rad_to_deg;
            ControllerState& state = InputState.controller_states[event->csensor.which];
            uint64_t cur_timestamp = event->csensor.timestamp;
            uint32_t delta_ms = cur_timestamp - state.prev_gyro_timestamp;
            state.motion.ProcessMotion(x, y, z, state.latest_accelerometer[0], state.latest_accelerometer[1], state.latest_accelerometer[2], delta_ms * 0.001f);
            state.prev_gyro_timestamp = cur_timestamp;

            float rot_x = 0.0f;
            float rot_y = 0.0f;
            state.motion.GetPlayerSpaceGyro(rot_x, rot_y);

            {
                std::lock_guard lock{ InputState.pending_input_mutex };
                InputState.pending_rotation_delta[0] += rot_x;
                InputState.pending_rotation_delta[1] += rot_y;
            }
        }
        break;
    case SDL_EventType::SDL_MOUSEMOTION:
        if (!recomp::game_input_disabled()) {
            SDL_MouseMotionEvent* motion_event = &event->motion;
            std::lock_guard lock{ InputState.pending_input_mutex };
            InputState.pending_mouse_delta[0] += motion_event->xrel;
            InputState.pending_mouse_delta[1] += motion_event->yrel;
        }
        queue_if_enabled(event);
        break;
    case SDL_EventType::SDL_DROPBEGIN:
        DropState.files_dropped.clear();
        break;
    case SDL_EventType::SDL_DROPFILE:
        DropState.files_dropped.emplace_back(std::filesystem::path(std::u8string_view((const char8_t*)(event->drop.file))));
        SDL_free(event->drop.file);
        break;
    case SDL_EventType::SDL_DROPCOMPLETE:
        recompui::drop_files(DropState.files_dropped);
        break;
    case SDL_EventType::SDL_CONTROLLERBUTTONUP:
        // Always queue button up events to avoid missing them during binding.
        recompui::queue_event(*event);
        break;
    default:
        queue_if_enabled(event);
        break;
    }
    process_player_assignment(event);
    return false;
}

void recomp::handle_events() {
    SDL_Event cur_event;
    static bool started = false;
    static bool exited = false;
    while (SDL_PollEvent(&cur_event) && !exited) {
        exited = sdl_event_filter(nullptr, &cur_event);

        // Lock the cursor if all three conditions are true: mouse aiming is enabled, game input is not disabled, and the game has been started. 
        bool cursor_locked = (recomp::get_mouse_sensitivity() != 0) && !recomp::game_input_disabled() && ultramodern::is_game_started();

        // Hide the cursor based on its enable state, but override visibility to false if the cursor is locked.
        bool cursor_visible = cursor_enabled;
        if (cursor_locked) {
            cursor_visible = false;
        }

        SDL_ShowCursor(cursor_visible ? SDL_ENABLE : SDL_DISABLE);
        SDL_SetRelativeMouseMode(cursor_locked ? SDL_TRUE : SDL_FALSE);
    }

    if (!started && ultramodern::is_game_started()) {
        started = true;
        recompui::process_game_started();
    }
}

constexpr SDL_GameControllerButton SDL_CONTROLLER_BUTTON_SOUTH = SDL_CONTROLLER_BUTTON_A;
constexpr SDL_GameControllerButton SDL_CONTROLLER_BUTTON_EAST = SDL_CONTROLLER_BUTTON_B;
constexpr SDL_GameControllerButton SDL_CONTROLLER_BUTTON_WEST = SDL_CONTROLLER_BUTTON_X;
constexpr SDL_GameControllerButton SDL_CONTROLLER_BUTTON_NORTH = SDL_CONTROLLER_BUTTON_Y;

const recomp::DefaultN64Mappings recomp::default_n64_keyboard_mappings = {
    .a = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_SPACE}
    },
    .b = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_LSHIFT}
    },
    .l = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_E}
    },
    .r = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_R}
    },
    .z = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_Q}
    },
    .start = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_RETURN}
    },
    .c_left = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_LEFT}
    },
    .c_right = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_RIGHT}
    },
    .c_up = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_UP}
    },
    .c_down = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_DOWN}
    },
    .dpad_left = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_J}
    },
    .dpad_right = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_L}
    },
    .dpad_up = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_I}
    },
    .dpad_down = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_K}
    },
    .analog_left = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_A}
    },
    .analog_right = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_D}
    },
    .analog_up = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_W}
    },
    .analog_down = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_S}
    },
    .toggle_menu = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_ESCAPE}
    },
    .accept_menu = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_RETURN}
    },
    .apply_menu = {
        {.input_type = InputType::Keyboard, .input_id = SDL_SCANCODE_F}
    }
};

const recomp::DefaultN64Mappings recomp::default_n64_controller_mappings = {
    .a = {
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_SOUTH},
    },
    .b = {
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_WEST},
    },
    .l = {
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
    },
    .r = {
        {.input_type = InputType::ControllerAnalog, .input_id = SDL_CONTROLLER_AXIS_TRIGGERRIGHT + 1},
    },
    .z = {
        {.input_type = InputType::ControllerAnalog, .input_id = SDL_CONTROLLER_AXIS_TRIGGERLEFT + 1},
    },
    .start = {
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_START},
    },
    .c_left = {
        {.input_type = InputType::ControllerAnalog, .input_id = -(SDL_CONTROLLER_AXIS_RIGHTX + 1)},
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_NORTH},
    },
    .c_right = {
        {.input_type = InputType::ControllerAnalog, .input_id = SDL_CONTROLLER_AXIS_RIGHTX + 1},
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_EAST},
    },
    .c_up = {
        {.input_type = InputType::ControllerAnalog, .input_id = -(SDL_CONTROLLER_AXIS_RIGHTY + 1)},
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_RIGHTSTICK},
    },
    .c_down = {
        {.input_type = InputType::ControllerAnalog, .input_id = SDL_CONTROLLER_AXIS_RIGHTY + 1},
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
    },
    .dpad_left = {
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_DPAD_LEFT},
    },
    .dpad_right = {
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_DPAD_RIGHT},
    },
    .dpad_up = {
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_DPAD_UP},
    },
    .dpad_down = {
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_DPAD_DOWN},
    },
    .analog_left = {
        {.input_type = InputType::ControllerAnalog, .input_id = -(SDL_CONTROLLER_AXIS_LEFTX + 1)},
    },
    .analog_right = {
        {.input_type = InputType::ControllerAnalog, .input_id = SDL_CONTROLLER_AXIS_LEFTX + 1},
    },
    .analog_up = {
        {.input_type = InputType::ControllerAnalog, .input_id = -(SDL_CONTROLLER_AXIS_LEFTY + 1)},
    },
    .analog_down = {
        {.input_type = InputType::ControllerAnalog, .input_id = SDL_CONTROLLER_AXIS_LEFTY + 1},
    },
    .toggle_menu = {
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_BACK},
    },
    .accept_menu = {
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_SOUTH},
    },
    .apply_menu = {
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_WEST},
        {.input_type = InputType::ControllerDigital, .input_id = SDL_CONTROLLER_BUTTON_START}
    }
};

// Returns true if the guid can be fetched.
static bool get_controller_guid_from_sdl_controller(SDL_GameController* controller, recomp::ControllerGUID *guid) {
    if (controller == nullptr) {
        return false;
    }

    SDL_Joystick *joystick = SDL_GameControllerGetJoystick(controller);
    if (joystick == nullptr) {
        return false;
    }

    Uint16 vendor, product, version, crc16;
    const char *joystick_name = SDL_JoystickName(joystick);
    const char *joystick_serial = SDL_JoystickGetSerial(joystick);
    std::string joystick_name_string = joystick_name != nullptr ? std::string(joystick_name) : "Unknown controller";
    SDL_JoystickGUID joystick_guid = SDL_JoystickGetGUID(joystick);
    int joystick_player_index = SDL_JoystickGetPlayerIndex(joystick);
    SDL_GetJoystickGUIDInfo(joystick_guid, &vendor, &product, &version, &crc16);

    *guid = { joystick_serial != nullptr ? std::string(joystick_serial) : std::string(), vendor, product, version, crc16, joystick_player_index };
    return true;
}

static bool compare_controller_guid(const recomp::ControllerGUID& a, const recomp::ControllerGUID& b) {
    return a.vendor == b.vendor && a.product == b.product && a.version == b.version && a.crc16 == b.crc16 && a.serial == b.serial;
}

void recomp::poll_inputs() {
    InputState.keys = SDL_GetKeyboardState(&InputState.numkeys);
    InputState.keymod = SDL_GetModState();
    static bool first_poll = true;

    {
        std::lock_guard lock{ InputState.controllers_mutex };
        InputState.detected_controllers.clear();

        static std::vector<size_t> free_controllers;
        free_controllers.clear();

        for (const auto& [id, state] : InputState.controller_states) {
            (void)id; // Avoid unused variable warning.
            SDL_GameController* controller = state.controller;
            if (controller != nullptr) {
                free_controllers.emplace_back(InputState.detected_controllers.size());
                InputState.detected_controllers.push_back(controller);
            }
        }

        if (first_poll) {
            first_poll = false;

            // Assign controllers based on configuration.
            for (auto& player : InputState.assigned_controllers) {
                player.controller = nullptr;
                player.is_assigned = false;
                player.keyboard_enabled = false;
            }
            
            // FIXME: Use active player count instead of iterating on all possible players.
            Uint16 vendor, product, version, crc16;
            for (size_t i = 0; i < 4; i++) {
                recomp::ControllerGUID controller_guid = recomp::get_input_controller_guid(i);
                int min_index_difference = INT_MAX;
                size_t j = 0;
                while (j < free_controllers.size()) {
                    SDL_GameController *controller = InputState.detected_controllers[free_controllers[j]];
                    recomp::ControllerGUID controller_guid_from_sdl;
                    if (!get_controller_guid_from_sdl_controller(controller, &controller_guid_from_sdl)) {
                        // If we can't get the controller guid, skip this controller.
                        j++;
                        continue;
                    }
                    if (!compare_controller_guid(controller_guid, controller_guid_from_sdl)) {
                        // If the controller guid doesn't match, skip this controller.
                        j++;
                        continue;
                    }
    
                    // The controller seems to be a match, but we use the controller with the least difference in player index to sort out potential duplicates.
                    int index_difference = abs(controller_guid.player_index - controller_guid_from_sdl.player_index);
                    if (min_index_difference > index_difference) {
                        InputState.assigned_controllers[i].controller = controller;
                        min_index_difference = index_difference;
                        free_controllers.erase(free_controllers.begin() + j);
                        continue;
                    }
    
                    j++;
                }
            }
        }

        // Do a second pass to assign controllers that are currently unused to the remaining players that failed to be assigned a controller.
        for (int i = 0; i < 4; i++) {
            if (i == 0) {
                InputState.assigned_controllers[i].keyboard_enabled = true;
            }

            if (InputState.assigned_controllers[i].controller != nullptr) {
                continue;
            }

            if (!free_controllers.empty()) {
                // Assign a free controller only.
                InputState.assigned_controllers[i].controller = InputState.detected_controllers[free_controllers.front()];
                free_controllers.erase(free_controllers.begin());
            }
            else {
                // Prefer not to assign a controller if none of them are free.
                break;
            }
        }
    }

    // Read the deltas while resetting them to zero.
    {
        std::lock_guard lock{ InputState.pending_input_mutex };

        InputState.rotation_delta = InputState.pending_rotation_delta;
        InputState.pending_rotation_delta = { 0.0f, 0.0f };

        InputState.mouse_delta = InputState.pending_mouse_delta;
        InputState.pending_mouse_delta = { 0.0f, 0.0f };
    }
}

void recomp::set_rumble(int controller_num, bool on) {
    InputState.rumble_active[controller_num] = on;
}

ultramodern::input::connected_device_info_t recomp::get_connected_device_info(int controller_num) {
    switch (controller_num) {
    case 0:
        return ultramodern::input::connected_device_info_t{
            .connected_device = ultramodern::input::Device::Controller,
            .connected_pak = ultramodern::input::Pak::RumblePak,
        };
    }

    return ultramodern::input::connected_device_info_t{
        .connected_device = ultramodern::input::Device::None,
        .connected_pak = ultramodern::input::Pak::None,
    };
}

static float smoothstep(float from, float to, float amount) {
    amount = (amount * amount) * (3.0f - 2.0f * amount);
    return std::lerp(from, to, amount);
}

// Update rumble to attempt to mimic the way n64 rumble ramps up and falls off
void recomp::update_rumble() {
    for (size_t i = 0; i < InputState.cur_rumble.size(); i++) {
        // Note: values are not accurate! just approximations based on feel
        if (InputState.rumble_active[i]) {
            InputState.cur_rumble[i] += 0.17f;
            if (InputState.cur_rumble[i] > 1) InputState.cur_rumble[i] = 1;
        }
        else {
            InputState.cur_rumble[i] *= 0.92f;
            InputState.cur_rumble[i] -= 0.01f;
            if (InputState.cur_rumble[i] < 0) InputState.cur_rumble[i] = 0;
        }
        float smooth_rumble = smoothstep(0, 1, InputState.cur_rumble[i]);

        uint16_t rumble_strength = smooth_rumble * (recomp::get_rumble_strength() * 0xFFFF / 100);
        uint32_t duration = 1000000; // Dummy duration value that lasts long enough to matter as the game will reset rumble on its own.
        {
            std::lock_guard lock{ InputState.controllers_mutex };
            if (InputState.single_controller) {
                for (const auto &controller : InputState.detected_controllers) {
                    SDL_GameControllerRumble(controller, 0, rumble_strength, duration);
                }
            }
            else {
                if (InputState.assigned_controllers[i].controller != nullptr) {
                    SDL_GameControllerRumble(InputState.assigned_controllers[i].controller, 0, rumble_strength, duration);
                }
            }
        }
    }
}

bool controller_button_state(int controller_num, int32_t input_id) {
    if (input_id >= 0 && input_id < SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_MAX) {
        SDL_GameControllerButton button = (SDL_GameControllerButton)input_id;
        bool ret = false;
        {
            std::lock_guard lock{ InputState.controllers_mutex };
            if (InputState.single_controller) {
                for (const auto &controller : InputState.detected_controllers) {
                    ret |= SDL_GameControllerGetButton(controller, button);
                }
            }
            else {
                if (InputState.assigned_controllers[controller_num].controller != nullptr) {
                    ret |= SDL_GameControllerGetButton(InputState.assigned_controllers[controller_num].controller, button);
                }
            }
        }

        return ret;
    }
    return false;
}

static std::atomic_bool right_analog_suppressed = false;

float controller_axis_state(int controller_num, int32_t input_id, bool allow_suppression) {
    if (abs(input_id) - 1 < SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_MAX) {
        SDL_GameControllerAxis axis = (SDL_GameControllerAxis)(abs(input_id) - 1);
        bool negative_range = input_id < 0;
        float ret = 0.0f;

        {
            auto gather_axis_state = [&](SDL_GameController* controller) {
                float cur_val = SDL_GameControllerGetAxis(controller, axis) * (1 / 32768.0f);
                if (negative_range) {
                    cur_val = -cur_val;
                }

                // Check if this input is a right analog axis and suppress it accordingly.
                if (allow_suppression && right_analog_suppressed.load() &&
                    (axis == SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX || axis == SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY)) {
                    cur_val = 0;
                }
                ret += std::clamp(cur_val, 0.0f, 1.0f);
            };

            std::lock_guard lock{ InputState.controllers_mutex };
            if (InputState.single_controller) {
                for (SDL_GameController *controller : InputState.detected_controllers) {
                    gather_axis_state(controller);
                }
            }
            else {
                if (InputState.assigned_controllers[controller_num].controller != nullptr) {
                    gather_axis_state(InputState.assigned_controllers[controller_num].controller);
                }
            }
        }

        return std::clamp(ret, 0.0f, 1.0f);
    }
    return false;
}

float recomp::get_input_analog(int controller_num, const recomp::InputField& field) {
    switch (field.input_type) {
    case InputType::Keyboard:
        if (InputState.keys && field.input_id >= 0 && field.input_id < InputState.numkeys) {
            if (should_override_keystate(static_cast<SDL_Scancode>(field.input_id), InputState.keymod)) {
                return 0.0f;
            }
            return InputState.keys[field.input_id] ? 1.0f : 0.0f;
        }
        return 0.0f;
    case InputType::ControllerDigital:
        return controller_button_state(controller_num, field.input_id) ? 1.0f : 0.0f;
    case InputType::ControllerAnalog:
        return controller_axis_state(controller_num, field.input_id, true);
    case InputType::Mouse:
        // TODO mouse support
        return 0.0f;
    case InputType::None:
        return false;
    }
}

float recomp::get_input_analog(int controller_num, const std::span<const recomp::InputField> fields) {
    float ret = 0.0f;
    for (const auto& field : fields) {
        ret += get_input_analog(controller_num, field);
    }
    return std::clamp(ret, 0.0f, 1.0f);
}

bool recomp::get_input_digital(int controller_num, const recomp::InputField& field) {
    switch (field.input_type) {
    case InputType::Keyboard:
        if (InputState.keys && field.input_id >= 0 && field.input_id < InputState.numkeys) {
            if (should_override_keystate(static_cast<SDL_Scancode>(field.input_id), InputState.keymod)) {
                return false;
            }
            return InputState.keys[field.input_id] != 0;
        }
        return false;
    case InputType::ControllerDigital:
        return controller_button_state(controller_num, field.input_id);
    case InputType::ControllerAnalog:
        // TODO adjustable threshold
        return controller_axis_state(controller_num, field.input_id, true) >= axis_threshold;
    case InputType::Mouse:
        // TODO mouse support
        return false;
    case InputType::None:
        return false;
    }
}

bool recomp::get_input_digital(int controller_num, const std::span<const recomp::InputField> fields) {
    bool ret = 0;
    for (const auto& field : fields) {
        ret |= get_input_digital(controller_num, field);
    }
    return ret;
}

void recomp::get_gyro_deltas(float* x, float* y) {
    std::array<float, 2> cur_rotation_delta = InputState.rotation_delta;
    float sensitivity = (float)recomp::get_gyro_sensitivity() / 100.0f;
    *x = cur_rotation_delta[0] * sensitivity;
    *y = cur_rotation_delta[1] * sensitivity;
}

void recomp::get_mouse_deltas(float* x, float* y) {
    std::array<float, 2> cur_mouse_delta = InputState.mouse_delta;
    float sensitivity = (float)recomp::get_mouse_sensitivity() / 100.0f;
    *x = cur_mouse_delta[0] * sensitivity;
    *y = cur_mouse_delta[1] * sensitivity;
}

void recomp::apply_joystick_deadzone(float x_in, float y_in, float* x_out, float* y_out) {
    float joystick_deadzone = (float)recomp::get_joystick_deadzone() / 100.0f;

    if (fabsf(x_in) < joystick_deadzone) {
        x_in = 0.0f;
    }
    else {
        if (x_in > 0.0f) {
            x_in -= joystick_deadzone;
        }
        else {
            x_in += joystick_deadzone;
        }

        x_in /= (1.0f - joystick_deadzone);
    }

    if (fabsf(y_in) < joystick_deadzone) {
        y_in = 0.0f;
    }
    else {
        if (y_in > 0.0f) {
            y_in -= joystick_deadzone;
        }
        else {
            y_in += joystick_deadzone;
        }

        y_in /= (1.0f - joystick_deadzone);
    }

    *x_out = x_in;
    *y_out = y_in;
}

void recomp::get_right_analog(int controller_num, float* x, float* y) {
    float x_val =
        controller_axis_state(controller_num, (SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX + 1), false) -
        controller_axis_state(controller_num, -(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX + 1), false);
    float y_val =
        controller_axis_state(controller_num, (SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY + 1), false) -
        controller_axis_state(controller_num, -(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY + 1), false);
    recomp::apply_joystick_deadzone(x_val, y_val, x, y);
}

void recomp::set_right_analog_suppressed(bool suppressed) {
    right_analog_suppressed.store(suppressed);
}

bool recomp::game_input_disabled() {
    // Disable input if any menu that blocks input is open.
    return recompui::is_context_capturing_input();
}

bool recomp::all_input_disabled() {
    // Disable all input if an input is being polled.
    return scanning_device != recomp::InputDevice::COUNT || recompinput::is_player_assignment_active();
}

bool recomp::get_single_controller_mode() {
    return InputState.single_controller;
}

void recomp::set_single_controller_mode(bool single_controller) {
    InputState.single_controller = single_controller;
}

std::string controller_button_to_string(SDL_GameControllerButton button) {
    switch (button) {
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A:
        return PF_GAMEPAD_A;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B:
        return PF_GAMEPAD_B;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X:
        return PF_GAMEPAD_X;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y:
        return PF_GAMEPAD_Y;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_BACK:
        return PF_XBOX_VIEW;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_GUIDE:
        return PF_GAMEPAD_HOME;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_START:
        return PF_XBOX_MENU;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSTICK:
        return PF_ANALOG_L_CLICK;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSTICK:
        return PF_ANALOG_R_CLICK;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        return PF_XBOX_LEFT_SHOULDER;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        return PF_XBOX_RIGHT_SHOULDER;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP:
        return PF_DPAD_UP;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        return PF_DPAD_DOWN;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        return PF_DPAD_LEFT;
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        return PF_DPAD_RIGHT;
        // case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_MISC1:
        //     return "";
        // case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_PADDLE1:
        //     return "";
        // case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_PADDLE2:
        //     return "";
        // case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_PADDLE3:
        //     return "";
        // case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_PADDLE4:
        //     return "";
    case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_TOUCHPAD:
        return PF_SONY_TOUCHPAD;
    default:
        return "Button " + std::to_string(button);
    }
}

std::unordered_map<SDL_Scancode, std::string> scancode_codepoints{
    {SDL_SCANCODE_LEFT, PF_KEYBOARD_LEFT},
    // NOTE: UP and RIGHT are swapped with promptfont.
    {SDL_SCANCODE_UP, PF_KEYBOARD_RIGHT},
    {SDL_SCANCODE_RIGHT, PF_KEYBOARD_UP},
    {SDL_SCANCODE_DOWN, PF_KEYBOARD_DOWN},
    {SDL_SCANCODE_A, PF_KEYBOARD_A},
    {SDL_SCANCODE_B, PF_KEYBOARD_B},
    {SDL_SCANCODE_C, PF_KEYBOARD_C},
    {SDL_SCANCODE_D, PF_KEYBOARD_D},
    {SDL_SCANCODE_E, PF_KEYBOARD_E},
    {SDL_SCANCODE_F, PF_KEYBOARD_F},
    {SDL_SCANCODE_G, PF_KEYBOARD_G},
    {SDL_SCANCODE_H, PF_KEYBOARD_H},
    {SDL_SCANCODE_I, PF_KEYBOARD_I},
    {SDL_SCANCODE_J, PF_KEYBOARD_J},
    {SDL_SCANCODE_K, PF_KEYBOARD_K},
    {SDL_SCANCODE_L, PF_KEYBOARD_L},
    {SDL_SCANCODE_M, PF_KEYBOARD_M},
    {SDL_SCANCODE_N, PF_KEYBOARD_N},
    {SDL_SCANCODE_O, PF_KEYBOARD_O},
    {SDL_SCANCODE_P, PF_KEYBOARD_P},
    {SDL_SCANCODE_Q, PF_KEYBOARD_Q},
    {SDL_SCANCODE_R, PF_KEYBOARD_R},
    {SDL_SCANCODE_S, PF_KEYBOARD_S},
    {SDL_SCANCODE_T, PF_KEYBOARD_T},
    {SDL_SCANCODE_U, PF_KEYBOARD_U},
    {SDL_SCANCODE_V, PF_KEYBOARD_V},
    {SDL_SCANCODE_W, PF_KEYBOARD_W},
    {SDL_SCANCODE_X, PF_KEYBOARD_X},
    {SDL_SCANCODE_Y, PF_KEYBOARD_Y},
    {SDL_SCANCODE_Z, PF_KEYBOARD_Z},
    {SDL_SCANCODE_0, PF_KEYBOARD_0},
    {SDL_SCANCODE_1, PF_KEYBOARD_1},
    {SDL_SCANCODE_2, PF_KEYBOARD_2},
    {SDL_SCANCODE_3, PF_KEYBOARD_3},
    {SDL_SCANCODE_4, PF_KEYBOARD_4},
    {SDL_SCANCODE_5, PF_KEYBOARD_5},
    {SDL_SCANCODE_6, PF_KEYBOARD_6},
    {SDL_SCANCODE_7, PF_KEYBOARD_7},
    {SDL_SCANCODE_8, PF_KEYBOARD_8},
    {SDL_SCANCODE_9, PF_KEYBOARD_9},
    {SDL_SCANCODE_ESCAPE, PF_KEYBOARD_ESCAPE},
    {SDL_SCANCODE_F1, PF_KEYBOARD_F1},
    {SDL_SCANCODE_F2, PF_KEYBOARD_F2},
    {SDL_SCANCODE_F3, PF_KEYBOARD_F3},
    {SDL_SCANCODE_F4, PF_KEYBOARD_F4},
    {SDL_SCANCODE_F5, PF_KEYBOARD_F5},
    {SDL_SCANCODE_F6, PF_KEYBOARD_F6},
    {SDL_SCANCODE_F7, PF_KEYBOARD_F7},
    {SDL_SCANCODE_F8, PF_KEYBOARD_F8},
    {SDL_SCANCODE_F9, PF_KEYBOARD_F9},
    {SDL_SCANCODE_F10, PF_KEYBOARD_F10},
    {SDL_SCANCODE_F11, PF_KEYBOARD_F11},
    {SDL_SCANCODE_F12, PF_KEYBOARD_F12},
    {SDL_SCANCODE_PRINTSCREEN, PF_KEYBOARD_PRINT_SCREEN},
    {SDL_SCANCODE_SCROLLLOCK, PF_KEYBOARD_SCROLL_LOCK},
    {SDL_SCANCODE_PAUSE, PF_KEYBOARD_PAUSE},
    {SDL_SCANCODE_INSERT, PF_KEYBOARD_INSERT},
    {SDL_SCANCODE_HOME, PF_KEYBOARD_HOME},
    {SDL_SCANCODE_PAGEUP, PF_KEYBOARD_PAGE_UP},
    {SDL_SCANCODE_DELETE, PF_KEYBOARD_DELETE},
    {SDL_SCANCODE_END, PF_KEYBOARD_END},
    {SDL_SCANCODE_PAGEDOWN, PF_KEYBOARD_PAGE_DOWN},
    {SDL_SCANCODE_SPACE, PF_KEYBOARD_SPACE},
    {SDL_SCANCODE_BACKSPACE, PF_KEYBOARD_BACKSPACE},
    {SDL_SCANCODE_TAB, PF_KEYBOARD_TAB},
    {SDL_SCANCODE_RETURN, PF_KEYBOARD_ENTER},
    {SDL_SCANCODE_CAPSLOCK, PF_KEYBOARD_CAPS},
    {SDL_SCANCODE_NUMLOCKCLEAR, PF_KEYBOARD_NUM_LOCK},
    {SDL_SCANCODE_LSHIFT, "L" PF_KEYBOARD_SHIFT},
    {SDL_SCANCODE_RSHIFT, "R" PF_KEYBOARD_SHIFT},
};

std::string keyboard_input_to_string(SDL_Scancode key) {
    if (scancode_codepoints.find(key) != scancode_codepoints.end()) {
        return scancode_codepoints[key];
    }
    return std::to_string(key);
}

std::string controller_axis_to_string(int axis) {
    bool positive = axis > 0;
    SDL_GameControllerAxis actual_axis = SDL_GameControllerAxis(abs(axis) - 1);
    switch (actual_axis) {
    case SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX:
        return positive ? "\u21C0" : "\u21BC";
    case SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY:
        return positive ? "\u21C2" : "\u21BE";
    case SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX:
        return positive ? "\u21C1" : "\u21BD";
    case SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY:
        return positive ? "\u21C3" : "\u21BF";
    case SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERLEFT:
        return positive ? "\u2196" : "\u21DC";
    case SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
        return positive ? "\u2197" : "\u21DD";
    default:
        return "Axis " + std::to_string(actual_axis) + (positive ? '+' : '-');
    }
}

std::string recomp::InputField::to_string() const {
    switch (input_type) {
    case InputType::None:
        return "";
    case InputType::ControllerDigital:
        return controller_button_to_string((SDL_GameControllerButton)input_id);
    case InputType::ControllerAnalog:
        return controller_axis_to_string(input_id);
    case InputType::Keyboard:
        return keyboard_input_to_string((SDL_Scancode)input_id);
    default:
        return std::to_string((uint32_t)input_type) + "," + std::to_string(input_id);
    }
}

void recomp::refresh_controller_options() {
    std::lock_guard lock{ InputState.controllers_mutex };
    InputState.detected_controllers.clear();
    
    for (const auto& [id, state] : InputState.controller_states) {
        (void)id; // Avoid unused variable warning.
        SDL_GameController* controller = state.controller;
        if (controller != nullptr) {
            InputState.detected_controllers.push_back(controller);
        }
    }

    InputState.detected_controller_options.clear();
    for (SDL_GameController* controller : InputState.detected_controllers) {
        SDL_Joystick *joystick = SDL_GameControllerGetJoystick(controller);
        if (joystick == nullptr) {
            continue;
        }

        Uint16 vendor, product, version, crc16;
        const char *joystick_name = SDL_JoystickName(joystick);
        const char *joystick_serial = SDL_JoystickGetSerial(joystick);
        std::string joystick_name_string = joystick_name != nullptr ? std::string(joystick_name) : "Unknown controller";
        SDL_JoystickGUID joystick_guid = SDL_JoystickGetGUID(joystick);
        int joystick_player_index = SDL_JoystickGetPlayerIndex(joystick);
        SDL_GetJoystickGUIDInfo(joystick_guid, &vendor, &product, &version, &crc16);

        ControllerGUID guid = { joystick_serial != nullptr ? std::string(joystick_serial) : std::string(), vendor, product, version, crc16, joystick_player_index};
        InputState.detected_controller_options.emplace_back(ControllerOption{ joystick_name_string, guid });
    }
}

const std::vector<recomp::ControllerOption> &recomp::get_controller_options() {
    return InputState.detected_controller_options;
}
