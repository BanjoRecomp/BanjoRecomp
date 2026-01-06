#include "patches.h"
#include "transform_ids.h"
#include "functions.h"
#include "input.h"

#define DYNAMIC_CAMERA_STATE_R_LOOK 0x13

extern u8 D_8037C060;
extern u8 D_8037C061;
extern u8 D_8037C062;
extern u8 D_8037DB40;
extern f32 D_8037DBA0;
extern f32 D_8037DBA4;
extern f32 D_8037DBA8;
extern f32 D_8037DBAC;
extern f32 D_8037D994;
extern f32 D_8037D998;
extern f32 D_8037D99C;
extern f32 D_8037D9E0[3];
extern f32 D_8037D9C8[3];
extern f32 D_8037DC10;
extern s32 D_8037C064;
extern s32 D_8037C068;
extern s32 D_8037C06C;
extern s32 D_8037C070;
extern s32 D_8037C074;
extern s32 D_8037C078;
extern s32 D_8037C07C;
extern s32 D_8037C080;
extern s32 D_8037C084;
extern f32 D_8037DB10;
extern f32 D_8037DB14;
extern f32 D_8037DB18;
extern f32 D_8037DB1C;
extern f32 D_8037DB20;
extern OSContStatus pfsManagerContStatus;
extern OSContPad pfsManagerContPadData[4];

extern void func_8024F35C(s32 arg0);
extern void func_80290B60(s32 arg0);
extern int func_80290D48(void);
extern int func_80290E8C(void);
extern void func_8029103C(void);
extern void func_80291488(s32 arg0);
extern s32 func_80298850(void);
extern s32 func_802BC84C(s32 arg0);
extern void func_802BD4C0(f32 arg0[3]);
extern f32 func_802BD51C(void);
extern void func_802BD720(f32 arg0[3]);
extern void func_802BDB30(f32 arg0, f32 *arg1, f32 *arg2, f32 arg3, f32 arg4, f32 arg5);
extern int func_802BE60C(void);
extern void func_802BE6FC(f32 arg0[3], f32 arg1[3]);
extern void func_802C0150(s32 arg0);
extern void func_802C019C(f32 arg0[3]);
extern void func_802C01BC(f32 arg0[3]);
extern void func_802C01DC(f32 arg0[3]);
extern void func_802C0234(f32 arg0[3]);
extern void func_802C026C(f32 arg0[3]);
extern void func_802C02B4(f32 arg0[3]);
extern void func_802C02D4(f32 arg0[3]);
extern void func_802C04B0(void);
extern bool func_802C0640(void);
extern void func_802C095C(void);
extern void func_802C2264(f32 duration);
extern void ncDynamicCamera_setPosition(f32 arg0[3]);
extern void ncDynamicCamera_getPosition(f32 arg0[3]);
extern void ncDynamicCamera_setRotation(f32 arg0[3]);
extern void ncDynamicCamera_getRotation(f32 arg0[3]);
extern void ncDynamicCamera_setState(s32);
extern int ncDynamicCamera_getState(void);
extern enum bsgroup_e player_movementGroup(void);
extern int bainput_should_rotate_camera_left(void);
extern int bainput_should_rotate_camera_right(void);
extern int bainput_should_zoom_out_camera(void);
extern int can_view_first_person(void);
extern s32 bs_getState(void);
extern s32 getGameMode(void);
extern f32 player_getYaw(void);
extern f32 player_getPitch(void);
extern enum bswatergroup_e player_getWaterState(void);
extern enum map_e map_get(void);

f32 dynamic_camera_targets[2][3];
u32 dynamic_camera_target_frames[2];
u32 dynamic_camera_target_frame;
f32 analog_zoom = 2.0f;
f32 analog_inherited_distance = 100.0f;
f32 r_look_initial_position_offset[3];
f32 r_look_initial_target_offset[3];
f32 r_look_initial_offset_weight;
bool analog_swimming_look_started;
bool r_look_initialized_from_r_button;
bool r_look_get_target_from_previous_frame_fix;

extern bool recomp_in_demo_playback_game_mode();

// @recomp Advances the reference frame for storing camera targets.
void recomp_advance_dynamic_camera_targets() {
    dynamic_camera_target_frame++;
}

// @recomp Check whether the analog camera was enabled by the user. Analog camera can have effects
// over vanilla behavior, so we ignore the user setting while on demo playback modes.
bool recomp_analog_camera_enabled() {
    if (recomp_in_demo_playback_game_mode()) {
        return FALSE;
    }
    else {
        return recomp_get_analog_cam_enabled();
    }
}

// @recomp Check whether analog camera movement is currently allowed. Analog camera movement is disabled while crouching and its
// related states to allow the user to use the right stick as C Button inputs without rotating the camera.
bool recomp_analog_camera_allowed() {
    switch (bs_getState()) {
    case BS_CROUCH:
    case BS_9_EGG_HEAD:
    case BS_A_EGG_ASS:
    case BS_14_BTROT_ENTER:
        return FALSE;
    default:
        return TRUE;
    }
}

// @recomp Functions to get the current values of the analog camera inputs.
void recomp_analog_camera_get(f32 *x, f32 *y) {
    float input_x, input_y;
    s32 inverted_x, inverted_y;
    recomp_get_right_analog_inputs(&input_x, &input_y);
    recomp_get_analog_inverted_axes(&inverted_x, &inverted_y);
    *x = input_x * (inverted_x ? 1.0f : -1.0f);
    *y = input_y * (inverted_y ? -1.0f : 1.0f);
}

f32 recomp_analog_camera_get_x() {
    float x, y;
    recomp_analog_camera_get(&x, &y);
    return x;
}

f32 recomp_analog_camera_get_y() {
    float x, y;
    recomp_analog_camera_get(&x, &y);
    return y;
}

s32 recomp_get_third_person_inverted_x() {
    s32 inverted_x, inverted_y;
    if (recomp_in_demo_playback_game_mode()) {
        inverted_x = TRUE;
    } else {
        recomp_get_analog_inverted_axes(&inverted_x, &inverted_y);
    }
    
    return inverted_x;
}

s32 recomp_get_third_person_inverted_y() {
    s32 inverted_x, inverted_y;
    if (recomp_in_demo_playback_game_mode()) {
        inverted_y = FALSE;
    } else {
        recomp_get_analog_inverted_axes(&inverted_x, &inverted_y);
    }
    
    return inverted_y;
}

s32 recomp_get_flying_and_swimming_inverted_x() {
    s32 inverted_x, inverted_y;
    if (recomp_in_demo_playback_game_mode()) {
        inverted_x = FALSE;
    } else {
        recomp_get_flying_and_swimming_inverted_axes(&inverted_x, &inverted_y);
    }
    return inverted_x;
}

s32 recomp_get_flying_and_swimming_inverted_y() {
    s32 inverted_x, inverted_y;
    if (recomp_in_demo_playback_game_mode()) {
        inverted_y = TRUE;
    } else {
        recomp_get_flying_and_swimming_inverted_axes(&inverted_x, &inverted_y);
    }
    return inverted_y;
}

s32 recomp_get_first_person_inverted_x() {
    s32 inverted_x, inverted_y;
    if (recomp_in_demo_playback_game_mode()) {
        inverted_x = TRUE;
    } else {
        recomp_get_first_person_inverted_axes(&inverted_x, &inverted_y);
    }
    return inverted_x;
}

s32 recomp_get_first_person_inverted_y() {
    s32 inverted_x, inverted_y;
    if (recomp_in_demo_playback_game_mode()) {
        inverted_y = FALSE;
    } else {
        recomp_get_first_person_inverted_axes(&inverted_x, &inverted_y);
    }
    return inverted_y;
}


// @recomp Check whether the analog camera stick is currently held.
bool recomp_analog_camera_held() {
    if (recomp_analog_camera_enabled() && recomp_analog_camera_allowed()) {
        float input_x, input_y;
        recomp_analog_camera_get(&input_x, &input_y);
        return (mlAbsF(input_x) > 1e-6f) || (mlAbsF(input_y) > 1e-6f);
    }
    else {
        return FALSE;
    }
}

// @recomp If movement is allowed, update the current camera mode's yaw with the input.
void recomp_analog_camera_update() {
    if (recomp_analog_camera_enabled() && recomp_analog_camera_allowed()) {
        f32 analog_yaw = recomp_analog_camera_get_x() * 120.0f * time_getDelta();
        if (mlAbsF(analog_yaw) > 1e-6f) {
            if (ncDynamicCamera_getState() != DYNAMIC_CAMERA_STATE_R_LOOK) {
                ncDynamicCamera_setState(DYNAMIC_CAMERA_STATE_R_LOOK);
                func_80291488(0x4);
            }

            D_8037DBA4 = mlNormalizeAngle(D_8037DBA4 + analog_yaw);
            D_8037DBA8 = mlNormalizeAngle(D_8037DBA8 + analog_yaw);
        }
    }
}

// @recomp Updates the current yaw based on the analog camera's horizontal movement.
// Also allows axis inversion for vanilla (non-analog) camera.
RECOMP_PATCH int func_8029105C(s32 arg0) {
    if (func_80298850())
        return FALSE;

    // @recomp: Allow camera axis inversion on the vanilla camera
    float axisInversionModifier = -1;
    bool inverted_x = recomp_get_third_person_inverted_x();
    if (inverted_x) {
        axisInversionModifier = 1;
    }

    // @recomp Update the analog camera input.
    recomp_analog_camera_update();

    // @recomp Account for axis inversion
    if (bainput_should_rotate_camera_left() && ncDynamicCamA_func_802C1DB0(-45.0f * axisInversionModifier)) {
        func_80291488(arg0);
        func_8029103C();
        return TRUE;
    }

    // @recomp Account for axis inversion
    if (bainput_should_rotate_camera_right() && ncDynamicCamA_func_802C1DB0(45.0f * axisInversionModifier)) {
        func_80291488(arg0);
        func_8029103C();
        return TRUE;
    }

    return FALSE;
}

// @recomp Computes the zoom value coordinate using a 2nd order polynomial and the three
// reference coordinate values.
f32 zoom_value(f32 a, f32 b, f32 c) {
    // 2nd order polynomial.
    // https://math.stackexchange.com/a/680695
    const f32 x1 = 1.0f;
    const f32 x2 = 2.0f;
    const f32 x3 = 3.0f;
    f32 pa = (x1 * (c - b) + x2 * (a - c) + x3 * (b - a)) / ((x1 - x2) * (x1 - x3) * (x2 - x3));
    f32 pb = (b - a) / (x2 - x1) - pa * (x1 + x2);
    f32 pc = a - pa * x1 * x1 - pb * x1;
    f32 x = analog_zoom;
    return pa * x * x + pb * x + pc;
}

// @recomp Check whether the camera is in a state that has inherited its target from a previous mode.
bool recomp_analog_camera_r_look_inherit_mode() {
    bool swimming_state = player_getWaterState() == BSWATERGROUP_2_UNDERWATER;
    bool prev_camera_is_pole = D_8037C060 == 0x10;
    return swimming_state || prev_camera_is_pole;
}

// @recomp Check whether the camera is in flight mode by checking what the previous stored camera mode is.
bool recomp_analog_camera_r_look_flight_mode() {
    bool prev_camera_is_flying = D_8037C060 == 0x4;
    return prev_camera_is_flying;
}

// @recomp Patched to return the smoothed X value of the current camera offset based on the zoom level.
RECOMP_PATCH f32 func_802BD8D4(void) {
    if (recomp_analog_camera_enabled()) {
        bool r_look_mode = ncDynamicCamera_getState() == DYNAMIC_CAMERA_STATE_R_LOOK;
        if (r_look_mode && recomp_analog_camera_r_look_flight_mode()) {
            return D_8037DB18 + 300.0f;
        }
        else if (r_look_mode && recomp_analog_camera_r_look_inherit_mode()) {
            return analog_inherited_distance;
        }
        else {
            return zoom_value(D_8037C064, D_8037C070, D_8037C07C);
        }
    }
    else {
        return D_8037D994;
    }
}

// @recomp Patched to return the smoothed Y value of the current camera offset based on the zoom level.
RECOMP_PATCH f32 func_802BD8E0(void) {
    if (recomp_analog_camera_enabled()) {
        return zoom_value(D_8037C068, D_8037C074, D_8037C080);
    }
    else {
        return D_8037D998;
    }
}

// @recomp Patched to return the smoothed Z value of the current camera offset based on the zoom level.
RECOMP_PATCH f32 func_802BD8C8(void) {
    if (recomp_analog_camera_enabled()) {
        return zoom_value(D_8037C06C, D_8037C078, D_8037C084);
    }
    else {
        return D_8037D99C;
    }
}

// @recomp Patched to store the last retrieved camera target for the current frame.
RECOMP_PATCH void func_802C02D4(f32 arg0[3]) {
    switch (D_8037DB40) {
    case 1:
        func_802C019C(arg0);
        break;

    case 2:
        func_802C01BC(arg0);
        break;

    case 3:
        func_802C01DC(arg0);
        break;

    case 4:
        func_802C0234(arg0);
        break;

    case 5:
        func_802C026C(arg0);
        break;

    case 6:
        func_802C02B4(arg0);
        break;
    }

    // @recomp Store the camera target to be used in a future frame as well as the frame number that wrote it.
    u32 index = dynamic_camera_target_frame & 1;
    ml_vec3f_copy(dynamic_camera_targets[index], arg0);
    dynamic_camera_target_frames[index] = dynamic_camera_target_frame;
}

// @recomp Patched to replace the camera target used for computing the yaw with a version that uses the previous
// frame's camera target. The regular version of this function has a bug where the camera target is the one
// determined by the current frame, so it is never able to compute the yaw properly by using the previous frame's
// camera position and ends up causing a slight rotation of the resulting yaw.
RECOMP_PATCH void func_802C069C(void) {
    f32 sp34[3];
    f32 sp28[3];
    f32 sp1C[3];

    ncDynamicCamera_getPosition(sp34);
    
    // @recomp If the camera target was written last frame and the bug fix is required, we use that value instead
    // to compute the yaw. Otherwise, use the original function.
    //func_802C02D4(sp1C);
    u32 index = (dynamic_camera_target_frame & 1) ^ 1;
    if (r_look_get_target_from_previous_frame_fix && recomp_analog_camera_enabled() && (dynamic_camera_target_frames[index] == (dynamic_camera_target_frame - 1))) {
        ml_vec3f_copy(sp1C, dynamic_camera_targets[index]);
    }
    else {
        func_802C02D4(sp1C);
    }

    ml_vec3f_diff_copy(sp28, sp34, sp1C);
    D_8037DBA0 = gu_sqrtf(sp28[0] * sp28[0] + sp28[2] * sp28[2]);
    func_8025801C(sp28, &D_8037DBA8);
    D_8037DBAC = 0.0f;
}

// @recomp Patched to skip over initialization of the target yaw based on the player's angle if analog cam is enabled.
RECOMP_PATCH void ncDynamicCam13_init(void) {
    func_802BE230(5.0f, 8.0f);
    func_802BE244(8.0f, 15.0f);

    // @recomp We don't change the target type in this initialization routine when the analog camera is enabled,
    // unless the the R button is currently held. We also compute the current camera distance to use it as the zoom
    // level in the case the current mode does not support camera zoom.
    // func_802C0150(6);
    if (recomp_analog_camera_enabled() && !r_look_initialized_from_r_button) {
        f32 camera_position[3];
        f32 camera_target[3];
        f32 camera_delta[3];
        ncDynamicCamera_getPosition(camera_position);
        func_802C02D4(camera_target);
        ml_vec3f_diff_copy(camera_delta, camera_position, camera_target);
        analog_inherited_distance = gu_sqrtf(camera_delta[0] * camera_delta[0] + camera_delta[2] * camera_delta[2]);
    }
    else {
        func_802C0150(6);
    }

    // @recomp Reduce the duration of the interpolation to this mode when the analog cam is enabled. Thanks to the
    // bug fix to the target computation, this is less necessary than before.
    //func_802C2264(0.5f);
    func_802C2264(recomp_analog_camera_enabled() ? (7.0f / 30.0f) : 0.5f);

    // @recomp Enable the global variable that controls whether the previous frame target fix is used.
    r_look_get_target_from_previous_frame_fix = TRUE;

    func_802C069C();

    // @recomp Clear the global variable.
    r_look_get_target_from_previous_frame_fix = FALSE;
    
    // @recomp We don't update the target yaw to match the player's angle if analog camera is enabled,
    // unless the the R button is currently held.
    // func_802C095C();
    if (!recomp_analog_camera_enabled() || r_look_initialized_from_r_button) {
        func_802C095C();
    }
    else {
        D_8037DBA4 = D_8037DBA8;
    }

    // @recomp A bug exists in the game where the velocity for both the position and rotation of the camera
    // mode is not reset when another mode that doesn't use it is initialized, carrying over the velocity
    // from the last time it was used.
    // 
    // This results in visible shifting of the camera when returning from this mode to another one that uses
    // the velocity. Since it seems like a bug and not intentional behavior, we only fix this if not playing
    // pre-recorded inputs.
    // 
    // This fixes large discontinuities when going back from the analog camera mode and can be reproduced in
    // vanilla by using the R button after moving for a while and standing still.
    if (!recomp_in_demo_playback_game_mode()) {
        ml_vec3f_clear(D_8037D9E0);
        ml_vec3f_clear(D_8037D9C8);
    }
}

// @recomp Patched to adjust the target height of the R Look camera mode if it inherited a target mode from another camera mode.
RECOMP_PATCH f32 func_802C0780(void) {
    // @recomp Adjust the target height of this mode based on the inherited target from a previous mode.
    // On these other modes, just use the current camera position as the target height.
    if (recomp_analog_camera_r_look_flight_mode()) {
        f32 player_pos[3];
        player_getPosition(player_pos);
        return player_pos[1] + 250.0f;
    }
    else if (recomp_analog_camera_r_look_inherit_mode()) {
        f32 camera_pos[3];
        ncDynamicCamera_getPosition(camera_pos);
        return camera_pos[1];
    }

    return func_802BD51C();
}

// @recomp Updates the current zoom level based on the analog camera's vertical movement.
// The zoom level is a variable between 0.5 and 3.0, with 1.0 indicating the closest
// original zoom level possible and 3.0 the furthest one. The range is lowered to 0.5
// to allow the player to look a bit closer than normally allowed.
RECOMP_PATCH void func_80290F14(void) {
    // @recomp Replicate the existing group of conditions to check if zoom level changes are allowed.
    // If they are, use the vertical movement to update the zoom level.
    if (recomp_analog_camera_enabled()) {
        if (!func_80298850() && player_movementGroup() != BSGROUP_4_LOOK && batimer_get(7) == 0.0f && recomp_analog_camera_allowed()) {
            analog_zoom = ml_clamp_f(analog_zoom + recomp_analog_camera_get_y() * 0.15f, 0.5f, 3.0f);
        }
    }

    if (!func_80298850()
        && player_movementGroup() != BSGROUP_4_LOOK
        && batimer_get(7) == 0.0f
        && bainput_should_zoom_out_camera()
        ) {
        switch (D_8037C061) {
        case 1://L80290FA4
            basfx_80299D2C(SFX_12E_CAMERA_ZOOM_MEDIUM, 1.0f, 12000);
            func_80290B60(2);
            break;
        case 2://L80290FBC
            if (D_8037C07C) {
                basfx_80299D2C(SFX_12E_CAMERA_ZOOM_MEDIUM, 1.2f, 12000);
                func_80290B60(3);
            }
            else {
                basfx_80299D2C(SFX_12D_CAMERA_ZOOM_CLOSEST, 1.0f, 12000);
                func_80290B60(1);
            }
            break;
        case 3://L80291008
            basfx_80299D2C(SFX_12D_CAMERA_ZOOM_CLOSEST, 1.0f, 12000);
            func_80290B60(1);
            break;
        }
        batimer_set(0x7, 0.4f);
    }
}

// @recomp Patched to update the analog zoom level every time the original one is modified.
RECOMP_PATCH void func_80290B60(s32 arg0) {
    D_8037C061 = arg0;

    // @recomp Update the analog zoom level.
    analog_zoom = (f32)arg0;
}

// @recomp Patched to allow the flying camera to switch to analog look.
RECOMP_PATCH void func_80291108(void) {
    if (!func_80290D48() && ncDynamicCamera_getState() == 0x10) {
        func_80290F14();
        func_8029105C(8);
    }

    // @recomp Switch to the analog camera mode.
    if (!func_80290D48() && recomp_analog_camera_held() && ncDynamicCamera_getState() == 0x4) {
        ncDynamicCamera_setState(DYNAMIC_CAMERA_STATE_R_LOOK);
        func_80291488(0x4);
    }
}

// @recomp Patched to add the target yaw initialization back on the cases where the camera state was changed when pressing the R button.
// Also patched to switch the camera to the R look mode if the analog camera input is held.
RECOMP_PATCH void func_80291154(void) {
    int tmp;
    if (!func_80290D48() && !func_80290E8C()) {
        if (bakey_held(BUTTON_R)) {
            // @recomp Set a global flag to indicate the camera initialization was done by pressing the R button.
            r_look_initialized_from_r_button = TRUE;

            ncDynamicCamera_setState(0x13);
            func_80291488(0x4);
            func_80290F14();

            // @recomp Clear the R button initialization flag.
            r_look_initialized_from_r_button = FALSE;
        }
        // @recomp Switch to the R Look mode if the analog camera input is held. Unlike the R Button input, this one will not initialize the
        // target yaw to match the player's angle, but will rather use whatever current yaw is present.
        else if (recomp_analog_camera_held()) {
            ncDynamicCamera_setState(DYNAMIC_CAMERA_STATE_R_LOOK);
            func_80291488(0x4);
            func_80290F14();
        }
        else {
            tmp = func_8029105C(7);
            func_80290F14();
            if (!tmp) {
                ncDynamicCamera_setState(0xB);
            }
        }
    }
}

// @recomp Patched to extend the condition of the R button in this function to consider the analog camera movement.
RECOMP_PATCH void func_802911E0(void) {
    if (!func_80290D48() && !func_80290E8C() && !func_8029105C(7)) {
        func_80290F14();
        if (bakey_held(BUTTON_R)) {
            func_802C095C();
        }
        // @recomp Don't execute the other branch if analog camera movement is present.
        else if (recomp_analog_camera_held()) {
            // Do nothing.
        }
        else {
            if (func_802C0640())
                func_80291488(2);
        }
    }
}

// @recomp Patched to allow swimming to use the analog camera mode.
RECOMP_PATCH int func_80290E8C(void) {
    if (player_getWaterState() != BSWATERGROUP_2_UNDERWATER) {
        // @recomp Clear the analog look flag when not underwater.
        analog_swimming_look_started = FALSE;

        return FALSE;
    }

    // @recomp Check whether the analog camera is active or if it was activated.
    // Switching to a diving state will reset back to the regular camera.
    //ncDynamicCamera_setState(3);
    if (recomp_analog_camera_held() || analog_swimming_look_started) {
        ncDynamicCamera_setState(DYNAMIC_CAMERA_STATE_R_LOOK);
        recomp_analog_camera_update();
        analog_swimming_look_started = recomp_analog_camera_held() || (bs_getState() != BS_2C_DIVE_B && bs_getState() != BS_39_DIVE_A); 
    }
    else {
        ncDynamicCamera_setState(3);
    }

    func_80291488(0xB);

    if (map_get() == MAP_B_CC_CLANKERS_CAVERN
        && player_getYPosition() < 1201.0f
        ) {
        func_802C1B20(1100.0f);
    }
    return TRUE;
}

// @recomp Patched to supress right stick analog inputs on situations where the analog camera is allowed.
RECOMP_PATCH void pfsManager_readData() {
    // @recomp Supress right stick analog input if analog camera movement is allowed. This will prevent
    // any bindings associated to the right analog stick from triggering.
    recomp_set_right_analog_suppressed(recomp_analog_camera_enabled() && recomp_analog_camera_allowed());

    func_8024F35C(0);
    if (!pfsManagerContStatus.errno)
        osContGetReadData(pfsManagerContPadData);
}

extern f32 bastick_getX(void);
extern f32 bastick_getY(void);

// @recomp Patched to allow configuring Y axis inversion when flying
RECOMP_PATCH void func_802A3648(void){
    // @recomp Get the axis inversion setting
    s32 inverted_y = recomp_get_flying_and_swimming_inverted_y();

    // @recomp Apply axis inversion setting
    f32 tmp_f0 = inverted_y ? bastick_getY() : -bastick_getY();

    if(tmp_f0 < 0.0f)
        pitch_setIdeal(ml_map_f(tmp_f0, -1.0f, 0.0f, 300.0f, 360.0f));
    else
        pitch_setIdeal(ml_map_f(tmp_f0, 0.0f, 1.0f, 0.0f, 80.0f));
}

// @recomp Patched to allow configuring X axis inversion when flying
RECOMP_PATCH void func_802A354C(void){
    f32 yaw_range;
    f32 roll_range;
    f32 sp2C; 

    // @recomp Get the axis inversion setting
    s32 inverted_x = recomp_get_flying_and_swimming_inverted_x();

    // @recomp Apply axis inversion setting
    sp2C = inverted_x ? -bastick_getX() : bastick_getX();
    if(bakey_held(BUTTON_R)){
        yaw_setVelocityBounded(500.0f, 30.0f);
        yaw_range = 6.0f;
        roll_range = 85.0f;
    }
    else{
        yaw_setVelocityBounded(500.0f, 2.0f);
        yaw_range = 3.0f;
        roll_range = 75.0f;
    }
    roll_setIdeal(ml_map_f(sp2C, -1.0f, 1.0f, -roll_range, roll_range));
    yaw_setIdeal(mlNormalizeAngle(yaw_getIdeal() + ml_map_f(sp2C, -1.0f, 1.0f, yaw_range, -yaw_range)));
}

// @recomp Patched to allow configuring Y axis inversion when swimming
f32 func_802A716C();
RECOMP_PATCH void func_802A7304() {
    f32 temp_f0;

    // @recomp Get the axis inversion setting
    s32 inverted_y = recomp_get_flying_and_swimming_inverted_y();

    pitch_setAngVel(ml_interpolate_f(func_802A716C(), 70.0f, 30.0f), 0.9f);

    // @recomp Apply axis inversion setting
    temp_f0 = inverted_y ? bastick_getY() : -bastick_getY();
    if (temp_f0 < 0.0f) {
        pitch_setIdeal(ml_map_f(temp_f0, -1.0f, 0.0f, 275.0f, 360.0f));
        return;
    }
    pitch_setIdeal(ml_map_f(temp_f0, 0.0f, 1.0f, 0.0f, 85.0f));
}

// @recomp Patched to allow configuring X axis inversion when swimming
RECOMP_PATCH void func_802A71D8(void) {
    f32 yaw_range;
    f32 sp38;
    f32 roll_range;
    f32 sp30;
    
    // @recomp Get the axis inversion setting
    s32 inverted_x = recomp_get_flying_and_swimming_inverted_x();

    // @recomp Apply axis inversion setting
    sp30 = inverted_x ? -bastick_getX() : bastick_getX();
    sp38 = func_802A716C();
    if (bakey_held(BUTTON_R)) {
        roll_range = 45.0f;
        yaw_range = 4.3f;
        yaw_setVelocityBounded(250.0f, 20.0f);
    } else {
        roll_range = 35.0f;
        yaw_range = ml_interpolate_f(sp38, 3.1f, 2.4f);
        yaw_setVelocityBounded(90.0f, ml_interpolate_f(sp38, 3.8f, 2.2f));
    }
    roll_setIdeal(ml_map_f(sp30, -1.0f, 1.0f, -roll_range, roll_range));
    yaw_setIdeal(mlNormalizeAngle(yaw_getIdeal() + ml_map_f(sp30, -1.0f, 1.0f, yaw_range, -yaw_range)));
}

extern s32 ncFirstPersonCamera_getState(void);
extern void ncFirstPersonCamera_getZoomedInRotation(f32 *);
extern void ncFirstPersonCamera_setZoomedOutRotation(f32 src[3]);
extern bool func_8028B254(s32 arg0);
extern void __bsDroneLook_getEyePos(f32 arg0[3]);
extern enum bs_e func_8029BDBC(void);
extern void ncFirstPersonCamera_setZoomedOutPosition(f32 src[3]);

// @recomp: Patched to allow inverting the first person camera
RECOMP_PATCH void bsDroneLook_update(void) {
    s32 next_state;
    f32 eye_rotation[3];
    f32 eye_position[3];
    f32 dt;
    s32 exit_first_person;

    // @recomp: Get the axis inversion setting
    s32 inverted_x = recomp_get_first_person_inverted_x();
    s32 inverted_y = recomp_get_first_person_inverted_y();
    float x = inverted_x ? -bastick_getX() : bastick_getX();
    float y = inverted_y ? bastick_getY() : -bastick_getY();

    next_state = 0;
    dt = time_getDelta();
    if (ncFirstPersonCamera_getState() == FIRSTPERSON_STATE_2_IDLE) {
        //camera is in "idle" state
        ncFirstPersonCamera_getZoomedInRotation(eye_rotation);
        // @recomp: Apply the axis inversion setting
        eye_rotation[0] -= y * 90.0f * dt;
        eye_rotation[1] -= x * 90.0f * dt;
        eye_rotation[2] = 0.0f;
        eye_rotation[0] = (eye_rotation[0] > 180.0f) ? ml_max_f(305.0f, eye_rotation[0]) : ml_min_f(70.0f, eye_rotation[0]);
        ncFirstPersonCamera_setZoomedOutRotation(eye_rotation);
        yaw_setIdeal(eye_rotation[1] + 180.0f);

        exit_first_person = FALSE;
        // 1st person cancelled via input
        if (bakey_pressed(BUTTON_B) || bakey_pressed(BUTTON_A) || bakey_pressed(BUTTON_C_UP)) {
            exit_first_person = TRUE;
        }
        // 1st person cancelled via entering water
        if (player_inWater()) {
            if (player_getTransformation() == TRANSFORM_1_BANJO && player_getWaterState() == BSWATERGROUP_0_NONE) {
                exit_first_person += TRUE;
            }
        } else if (func_8028B254(25) == 0) {
            exit_first_person += TRUE;
        }
        if (exit_first_person) {
            next_state = func_8029BDBC();
        }
    }
    __bsDroneLook_getEyePos(eye_position);
    ncFirstPersonCamera_setZoomedOutPosition(eye_position);
    bs_setState(next_state);
}
