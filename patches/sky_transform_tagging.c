#include "patches.h"
#include "transform_ids.h"
#include "core1/core1.h"

typedef struct {
    s16 model_id;
    // u8 pad2[0x2];
    f32 scale;
    f32 rotation_speed;
}SkyInfo;

typedef struct {
    s16 map;
    // u8 pad2[2];
    SkyInfo sky_list[3];
}MapSkyInfo;

extern struct  
{
    MapSkyInfo *sky_info;
    BKModel *model[3];
    BKModelBin *model_bins[3];
    f32 timer;
} gcSky;

typedef struct {
    f32 unk0;
    s32 unk4[4];
    f32 unk14;
}Struct_core2_C97F0_2;

typedef struct {
    u8 unk0;
    // u8 pad1[3];
    Struct_core2_C97F0_2 unk4[15];
}Struct_core2_C97F0_1;

typedef struct {
    s16 unk0;
    f32 unk4[3];
    u8 unk10;
}Struct_core2_C97F0_0;

extern struct {
    Struct_core2_C97F0_1 *unk0;
    Struct_core2_C97F0_0 *unk4;
    void *unk8;
} D_80386170;

extern struct {
    u8 unk0;
    u8 unk1;
}D_8038617C;

extern s32 D_803725A8[4];

// @recomp Patched to tag the skybox projection and skybox models.
RECOMP_PATCH void sky_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx){
    int i;
    f32 position[3];
    f32 rotation[3];
    BKModelBin *iAsset;

    viewport_setNearAndFar(5.0f, 15000.0f);
    if(gcSky.model_bins[0]){
        drawRectangle2D(gfx, 0, 0, (s32)(f32) gFramebufferWidth, (s32)(f32)gFramebufferHeight,0, 0, 0); //fill screen with black
        // @recomp Set the skybox projection matrix group.
        s32 prev_perspective_projection_transform = cur_perspective_projection_transform_id;
        cur_perspective_projection_transform_id = PROJECTION_SKYBOX_TRANSFORM_ID;

        viewport_setRenderViewportAndPerspectiveMatrix(gfx, mtx);
        viewport_getPosition_vec3f(position);
        for(i = 0; i < 3; i++){
            iAsset = gcSky.model_bins[i];
            if(iAsset){
                rotation[0] = 0.0f;
                rotation[1] = gcSky.sky_info->sky_list[i].rotation_speed * gcSky.timer;
                rotation[2] = 0.0f;

                // @recomp Set the model transform ID before drawing the skybox.
                cur_drawn_model_transform_id = SKYBOX_TRANSFORM_ID_START + SKYBOX_MODEL_TRANSFORM_ID_COUNT * i;

                modelRender_draw(gfx, mtx, position, rotation, gcSky.sky_info->sky_list[i].scale, NULL, iAsset);

                // @recomp Clear the model transform ID after drawing the skybox.
                cur_drawn_model_transform_id = 0;
            }
        }

        // @recomp Restore the previous perpsective projection transform ID.
        cur_perspective_projection_transform_id = prev_perspective_projection_transform;
    }
    else{//L8030B200
        drawRectangle2D(gfx, 0, 0, (s32)(f32) gFramebufferWidth, (s32)(f32)gFramebufferHeight, 0, 0, 0);
    }//L8030B254
}

// @recomp Patched to tag the lens flare from the sun.
RECOMP_PATCH void func_80350818(Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    f32 spDC[3];
    f32 spD0[3];
    f32 spC4[3];
    f32 spB8[3];
    Struct_core2_C97F0_0 *temp_s1;
    Struct_core2_C97F0_1 *temp_s2;
    f32 var_f22;
    s32 i;
    f32 sp9C[3];
    f32 sp90[3];
    s32 sp80[4];

    temp_s2 = D_80386170.unk0;
    if (temp_s2 != NULL) {

    }

    temp_s1 = D_80386170.unk4;
    if ((temp_s1 != NULL) && D_8038617C.unk0) {
        viewport_getPosition_vec3f(spDC);
        viewport_getRotation_vec3f(spD0);
        sp9C[0] = temp_s1->unk4[0];
        sp9C[1] = temp_s1->unk4[1];
        sp9C[2] = temp_s1->unk4[2];
        ml_vec3f_yaw_rotate_copy(sp9C, sp9C, -spD0[1]);
        ml_vec3f_pitch_rotate_copy(sp9C, sp9C, -spD0[0]);
        if (!(((1.2 * (f32)gFramebufferWidth) / 2) < sp9C[0]) && !(sp9C[0] < ((-1.2 * (f32)gFramebufferWidth) / 2))) {
            if (!(((1.2 * (f32)gFramebufferHeight) / 2) < sp9C[1]) && !(sp9C[1] < ((-1.2 * (f32)gFramebufferHeight) / 2))) {
                sp90[0] = -sp9C[0];
                sp90[1] = -sp9C[1];
                sp90[2] = sp9C[2];
                ml_vec3f_pitch_rotate_copy(sp90, sp90, spD0[0]);
                ml_vec3f_yaw_rotate_copy(sp90, sp90, spD0[1]);
                var_f22 = 1.0f - (((sp9C[0] * sp9C[0]) + (sp9C[1] * sp9C[1])) / ((f32)gFramebufferHeight * (f32)gFramebufferHeight));
                if (var_f22 < 0.0f) {
                    var_f22 = 0.0f;
                }
                if (var_f22 > 1.0f) {
                    var_f22 = 1.0f;
                }
                spC4[0] = sp90[0] - temp_s1->unk4[0];
                spC4[1] = sp90[1] - temp_s1->unk4[1];
                spC4[2] = sp90[2] - temp_s1->unk4[2];
                for (i = 0; temp_s2->unk4[i].unk0 != 0.0f; i++) {
                    spB8[0] = (spDC[0] + temp_s1->unk4[0]) + (temp_s2->unk4[i].unk0 * spC4[0]);
                    spB8[1] = (spDC[1] + temp_s1->unk4[1]) + (temp_s2->unk4[i].unk0 * spC4[1]);
                    spB8[2] = (spDC[2] + temp_s1->unk4[2]) + (temp_s2->unk4[i].unk0 * spC4[2]);
                    sp80[0] = temp_s2->unk4[i].unk4[0];
                    sp80[1] = temp_s2->unk4[i].unk4[1];
                    sp80[2] = temp_s2->unk4[i].unk4[2];
                    sp80[3] = temp_s2->unk4[i].unk4[3];
                    sp80[3] *= var_f22;
                    modelRender_setPrimAndEnvColors(sp80, D_803725A8);

                    // @recomp Set the model transform ID before drawing the lens flare.
                    cur_drawn_model_transform_id = LENS_FLARE_TRANSFORM_ID_START + LENS_FLARE_TRANSFORM_ID_COUNT * i;

                    modelRender_draw(gfx, mtx, spB8, spD0, temp_s2->unk4[i].unk14 * 0.25, NULL, D_80386170.unk8);

                    // @recomp Clear the model transform ID after drawing the lens flare.
                    cur_drawn_model_transform_id = 0;
                }
            }
        }
    }
}