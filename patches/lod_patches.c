#include "patches.h"
#include "core1/mlmtx.h"
#include "functions.h"

typedef struct {
    s32 cmd_0;
    s32 size_4;
    f32 max_8;
    f32 min_C;
    f32 unk10[3];
    s32 subgeo_offset_1C;
}GeoCmd8;

extern f32 D_80383C98[3];
extern void func_80339124(Gfx **gfx, Mtx **mtx, BKGeoList *geo_list);
extern float gu_sqrtf(float val);

// @recomp Patched to force all LODs to act as if they were a fixed distance from the camera.
RECOMP_PATCH void func_80338B50(Gfx **gfx, Mtx **mtx, void *arg2) {
    GeoCmd8 *cmd = (GeoCmd8 *)arg2;
    f32 dist;

    if (cmd->subgeo_offset_1C) {
        mlMtx_apply_vec3f(D_80383C98, cmd->unk10);
        // @recomp Force distance for LOD to be 50.
        dist = 50.0f; //gu_sqrtf(D_80383C98[0] * D_80383C98[0] + D_80383C98[1] * D_80383C98[1] + D_80383C98[2] * D_80383C98[2]);
        if (cmd->min_C < dist && dist <= cmd->max_8) {
            func_80339124(gfx, mtx, (BKGeoList *)((s32)cmd + cmd->subgeo_offset_1C));
        }
    }
}

// @recomp Patched to force the player Banjo & Kazooie model to always be high poly
RECOMP_PATCH s32 func_802985F0(void){
    switch(bsStoredState_getTransformation())
    {
        case TRANSFORM_2_TERMITE: 
            return ASSET_34F_MODEL_BANJO_TERMITE;
        case TRANSFORM_3_PUMPKIN: 
            return ASSET_36F_MODEL_BANJO_PUMPKIN;
        case TRANSFORM_5_CROC: 
            return ASSET_374_MODEL_BANJO_CROC;
        case TRANSFORM_4_WALRUS: 
            return ASSET_359_MODEL_BANJO_WALRUS;
        case TRANSFORM_6_BEE: 
            return ASSET_362_MODEL_BANJO_BEE;
        case TRANSFORM_7_WISHWASHY: 
            return ASSET_356_MODEL_BANJO_WISHYWASHY;
        // @recomp Force high poly BK player model on all maps
        case TRANSFORM_1_BANJO: 
        default:
            return ASSET_34E_MODEL_BANJOKAZOOIE_HIGH_POLY;
    }
}
