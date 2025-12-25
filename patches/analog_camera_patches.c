#include "patches.h"
#include "transform_ids.h"
#include "functions.h"
#include "input.h"

#define DYNAMIC_CAMERA_STATE_R_LOOK 0x13

extern u8 D_8037C061;
extern u8 D_8037DB40;
extern f32 D_8037DBA4;
extern f32 D_8037DBA8;
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
extern int func_802BE60C(void);
extern void func_802BE6FC(f32 arg0[3], f32 arg1[3]);
extern void func_802C0150(s32 arg0);
extern void func_802C02D4(f32 arg0[3]);
extern void func_802C04B0(void);
extern bool func_802C0640(void);
extern void func_802C069C(void);
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

f32 analog_zoom = 2.0f;

// @recomp Check whether the game is currently on a mode that uses demo playback instead of the player's inputs.
bool in_demo_playback_game_mode() {
    switch (getGameMode()) {
    case GAME_MODE_6_FILE_PLAYBACK:
    case GAME_MODE_7_ATTRACT_DEMO:
    case GAME_MODE_8_BOTTLES_BONUS:
    case GAME_MODE_A_SNS_PICTURE:
    case GAME_MODE_9_BANJO_AND_KAZOOIE:
        return TRUE;
    default:
        return FALSE;
    }
}

// @recomp Check whether the analog camera was enabled by the user. Analog camera can have effects
// over vanilla behavior, so we ignore the user setting while on demo playback modes.
bool recomp_analog_camera_enabled() {
    if (in_demo_playback_game_mode()) {
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
    recomp_get_camera_inputs(&input_x, &input_y);
    recomp_get_analog_inverted_axes(&inverted_x, &inverted_y);
    *x = input_x * (inverted_x ? -1.0f : 1.0f);
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

// @recomp Check wehther the analog camera stick is currently held.
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

// @recomp Updates the current yaw based on the analog camera's horizontal movement.
RECOMP_PATCH int func_8029105C(s32 arg0) {
    if (func_80298850())
        return FALSE;

    // @recomp If movement is allowed, update the current camera mode's yaw with the input.
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

    if (bainput_should_rotate_camera_left() && ncDynamicCamA_func_802C1DB0(-45.0f)) {
        func_80291488(arg0);
        func_8029103C();
        return TRUE;
    }

    if (bainput_should_rotate_camera_right() && ncDynamicCamA_func_802C1DB0(45.0f)) {
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

// @recomp Patched to return the smoothed X value of the current camera offset based on the zoom level.
RECOMP_PATCH f32 func_802BD8D4(void) {
    if (recomp_analog_camera_enabled()) {
        return zoom_value(D_8037C064, D_8037C070, D_8037C07C);
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

// @recomp Patched to reset the position and rotation velocity vectors when this camera mode initializes.
RECOMP_PATCH void ncDynamicCamB_init(void) {
    func_802BE244(5.0f, 10.0f);
    func_802BE230(3.0f, 8.0f);
    func_802C0150(2);
    func_802C04B0();
    
    // @recomp A bug exists in the game where the velocity for both the position and rotation of this camera
    // mode is not reset when it is initialized, carrying over the velocity from the last time it was used.
    // This results in visible shifting of the camera when entering this mode. Since it seems like a bug
    // and not intentional behavior, we only fix this if not playing pre-recorded inputs.
    // 
    // This fixes large discontinuities when going back from the analog camera mode and can be reproduced in
    // vanilla by using the R button after moving for a while and standing still.
    if (!in_demo_playback_game_mode()) {
        ml_vec3f_clear(D_8037D9E0);
        ml_vec3f_clear(D_8037D9C8);
    }
}

// @recomp Patched to skip over initialization of the target yaw based on the player's angle if analog cam is enabled.
RECOMP_PATCH void ncDynamicCam13_init(void) {
    func_802BE230(5.0f, 8.0f);
    func_802BE244(8.0f, 15.0f);

    // @recomp We don't change the target type in this initialization routine when the analog camera is enabled,
    // unless the the R button is currently held.
    // func_802C0150(6);
    if (!recomp_analog_camera_enabled() || bakey_held(BUTTON_R)) {
        func_802C0150(6);
    }

    func_802C2264(0.5f);
    func_802C069C();

    // @recomp We don't update the target yaw to match the player's angle if analog camera is enabled,
    // unless the the R button is currently held.
    // func_802C095C();
    if (!recomp_analog_camera_enabled() || bakey_held(BUTTON_R)) {
        func_802C095C();
    }
    else {
        D_8037DBA4 = D_8037DBA8;
    }
}

// @recomp Patched to adjust the target height of the R Look camera mode if it inherited a target mode from another camera mode.
RECOMP_PATCH f32 func_802C0780(void) {
    // @recomp Adjust the target height of this mode based on the inherited target from a previous mode.
    if (D_8037DB40 == 1 || D_8037DB40 == 3) {
        f32 camera_pos[3];
        ncDynamicCamera_getPosition(camera_pos);
        return camera_pos[1];
    }

    return func_802BD51C();
}

// @recomp Patched to add analogue controls to the swimming camera.
RECOMP_PATCH void ncDynamicCam3_update(void) {
    f32 sp7C[3];
    f32 sp70[3];
    f32 sp64[3];
    f32 sp58[3];
    f32 sp4C[3];
    f32 sp40[3];
    f32 sp3C;
    f32 sp38;
    f32 sp34;
    f32 sp30;

    // @recomp
    f32 analogue_x = 0.0f;
    f32 analogue_y = 0.0f;
    if (recomp_analog_camera_enabled() && recomp_analog_camera_allowed()) {
        recomp_analog_camera_get(&analogue_x, &analogue_y);
    }

    ncDynamicCamera_getPosition(sp64);
    sp34 = D_8037DC10;
    func_802C02D4(sp7C);
    sp30 = time_getDelta();
    if (sp30);
    ml_vec3f_diff_copy(sp40, sp64, sp7C);
    sp3C = gu_sqrtf(sp40[0] * sp40[0] + sp40[2] * sp40[2]);
    sp3C += func_80259198(sp30 * (sp34 - sp3C) * 2, sp30 * 800.0f);
    func_8025727C(sp7C[0], sp7C[1], sp7C[2], sp64[0], sp64[1], sp64[2], &sp4C[0], &sp4C[1]);

    // @recomp 
    //sp40[1] = sp30 * 0.77 * mlDiffDegF(mlNormalizeAngle(player_getYaw() + 180.0f), sp4C[1]);
    sp40[1] = sp30 * 0.77 * mlDiffDegF(mlNormalizeAngle(player_getYaw() + 180.0f + analogue_x * 120.0f), sp4C[1]);

    sp40[1] = func_80259198(sp40[1], sp30 * 300.0f);
    sp4C[1] = mlNormalizeAngle(sp4C[1] + sp40[1]);
    func_80256E24(&sp58, 0.0f, sp4C[1], 0.0f, 0.0f, sp3C);
    sp70[0] = sp7C[0] + sp58[0];
    sp70[1] = sp64[1];
    sp70[2] = sp7C[2] + sp58[2];
    sp40[1] = sp7C[1] - sp64[1];
    if (mlAbsF(sp40[1]) > 200.0f) {
        sp70[1] = sp64[1] - ((sp40[1] > 0.0f) ? sp30 * (200.0f - sp40[1]) * 2 : sp30 * (-200.0f - sp40[1]) * 2);
    }
    ncDynamicCamera_setPosition(sp70);
    if (func_802BE60C()) {
        func_802BC84C(0);
    }
    func_802BE6FC(sp4C, sp7C);
    func_802BD720(sp4C);
}


// @recomp Patched to add analogue controls to the flying camera.
RECOMP_PATCH void ncDynamicCam4_update(void) {
    f32 sp84[3];
    f32 sp78[3];
    f32 sp6C[3];
    f32 sp60[3];
    f32 sp54[3];
    f32 sp48[3];
    f32 sp44;
    f32 temp_f10;
    f32 sp3C;
    f32 sp38;
    f32 sp34;

    ncDynamicCamera_getPosition(sp6C);
    func_802BD4C0(sp84);
    sp84[1] += 40.0f;

    // @recomp
    f32 analogue_x = 0.0f;
    f32 analogue_y = 0.0f;
    if (recomp_analog_camera_enabled() && recomp_analog_camera_allowed()) {
        recomp_analog_camera_get(&analogue_x, &analogue_y);
    }

    sp34 = player_getPitch();
    if (sp34 > 180.0f) {
        sp3C = ml_map_f(sp34, 300.0f, 360.0f, 900.0f, D_8037DB18);
        sp84[1] += ml_map_f(sp34, 300.0f, 360.0f, -140.0f, 70.0f);
    }
    else {
        sp3C = D_8037DB18;
        sp84[1] += 70.0f;
    }
    sp38 = time_getDelta();
    ml_vec3f_diff_copy(sp48, sp6C, sp84);
    sp44 = gu_sqrtf(sp48[0] * sp48[0] + sp48[1] * sp48[1] + sp48[2] * sp48[2]);
    temp_f10 = (sp3C - sp44) * sp38;
    sp44 += func_80259198(temp_f10 * D_8037DB10, sp38 * D_8037DB14);
    func_8025727C(sp84[0], sp84[1], sp84[2], sp6C[0], sp6C[1], sp6C[2], &sp54[0], &sp54[1]);
    if ((sp34 > 180.0f) && (sp34 < 360.0f)) {
        sp34 = ml_min_f(100.0f, (f32)((f64)(360.0f - sp34) * 1.4));
    }
    // @recomp
    //sp48[0] = mlDiffDegF(mlNormalizeAngle(sp34), sp54[0]);
    sp48[0] = mlDiffDegF(mlNormalizeAngle(sp34 + analogue_y * 120.0f), sp54[0]);
    
    // @recomp
    //sp48[1] = mlDiffDegF(mlNormalizeAngle(player_getYaw() + 180.0f), sp54[1]);
    sp48[1] = mlDiffDegF(mlNormalizeAngle(player_getYaw() + 180.0f + analogue_x * 120.0f), sp54[1]);

    sp48[2] = 0.0f;
    sp48[0] = (f32)((f64)sp48[0] * ((f64)sp38 * 0.8));
    sp48[1] = sp48[1] * (sp38 * D_8037DB1C);
    sp48[0] = func_80259198(sp48[0], sp38 * 40.0f);
    sp48[1] = func_80259198(sp48[1], sp38 * D_8037DB20);
    sp54[0] = mlNormalizeAngle(sp54[0] + sp48[0]);
    sp54[1] = mlNormalizeAngle(sp54[1] + sp48[1]);

    func_80256E24(sp60, -sp54[0], sp54[1], 0.0f, 0.0f, sp44);
    ml_vec3f_add(sp78, sp84, sp60);
    ncDynamicCamera_setPosition(sp78);
    func_8025727C(sp84[0], sp84[1], sp84[2], sp78[0], sp78[1], sp78[2], &sp54[0], &sp54[1]);
    sp54[0] = -sp54[0];
    sp54[2] = 0.0f;
    func_802BD720(sp54);
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

        // @recomp Update the analog zoom level to match.
        analog_zoom = (f32)D_8037C061;
    }
}

// @recomp Patched to add the target yaw initialization back on the cases where the camera state was changed when pressing the R button.
// Also patched to switch the camera to the R look mode if the analog camera input is held.
RECOMP_PATCH void func_80291154(void) {
    int tmp;
    if (!func_80290D48() && !func_80290E8C()) {
        if (bakey_held(BUTTON_R)) {
            ncDynamicCamera_setState(DYNAMIC_CAMERA_STATE_R_LOOK);
            func_80291488(0x4);
            func_80290F14();
        }
        // @recomp Switch to the R Look mode if the analog camera input is held. Unlike the R BUtton input, this one will not initialize the
        // target yaw to match the player's angle, but will rather use whatever current yaw is present.
        else if (recomp_analog_camera_held()) {
            ncDynamicCamera_setState(DYNAMIC_CAMERA_STATE_R_LOOK);
            func_80291488(0x4);
            func_80290F14();
        }
        else {
            tmp = func_8029105C(7);
            func_80290F14();
            if (!tmp)
                ncDynamicCamera_setState(0xB);
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

// @recomp
RECOMP_PATCH void pfsManager_readData() {
    // @recomp
    recomp_set_right_analog_suppressed(recomp_analog_camera_enabled() && recomp_analog_camera_allowed());

    func_8024F35C(0);
    if (!pfsManagerContStatus.errno)
        osContGetReadData(pfsManagerContPadData);
}