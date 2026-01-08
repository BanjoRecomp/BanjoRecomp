#include "patches.h"
#include "core1/ml.h"
#include "core1/mlmtx.h"
#include "core1/core1.h"
#include "core1/vimgr.h"
#include "functions.h"

extern f32 sViewportPosition[3];
extern f32 sViewportFrustumPlanes[4][4];
void animMtxList_setBoned(AnimMtxList **this_ptr, BKAnimationList *anim_list, BoneTransformList *arg2);

extern s32 cur_model_would_have_been_culled_in_demo;
bool recomp_in_demo_playback_game_mode();
void recomp_setup_marker_skinning(ActorMarker *marker);

int frustum_checks_enabled = TRUE;
void set_frustum_checks_enabled(int enabled) {
    frustum_checks_enabled = enabled;
}

// Unpatched copy of viewport_isBoundingBoxInFrustum, used to check if a cube would have been culled originally.
bool viewport_isBoundingBoxInFrustumCopy(f32 min[3], f32 max[3]) {
    if (((sViewportFrustumPlanes[0][0] * min[0] + sViewportFrustumPlanes[0][1] * min[1] + sViewportFrustumPlanes[0][2] * min[2] + sViewportFrustumPlanes[0][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[0][0] * min[0] + sViewportFrustumPlanes[0][1] * min[1] + sViewportFrustumPlanes[0][2] * max[2] + sViewportFrustumPlanes[0][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[0][0] * min[0] + sViewportFrustumPlanes[0][1] * max[1] + sViewportFrustumPlanes[0][2] * min[2] + sViewportFrustumPlanes[0][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[0][0] * min[0] + sViewportFrustumPlanes[0][1] * max[1] + sViewportFrustumPlanes[0][2] * max[2] + sViewportFrustumPlanes[0][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[0][0] * max[0] + sViewportFrustumPlanes[0][1] * min[1] + sViewportFrustumPlanes[0][2] * min[2] + sViewportFrustumPlanes[0][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[0][0] * max[0] + sViewportFrustumPlanes[0][1] * min[1] + sViewportFrustumPlanes[0][2] * max[2] + sViewportFrustumPlanes[0][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[0][0] * max[0] + sViewportFrustumPlanes[0][1] * max[1] + sViewportFrustumPlanes[0][2] * min[2] + sViewportFrustumPlanes[0][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[0][0] * max[0] + sViewportFrustumPlanes[0][1] * max[1] + sViewportFrustumPlanes[0][2] * max[2] + sViewportFrustumPlanes[0][3]) >= 0.0f))
        return FALSE;

    if (((sViewportFrustumPlanes[1][0] * min[0] + sViewportFrustumPlanes[1][1] * min[1] + sViewportFrustumPlanes[1][2] * min[2] + sViewportFrustumPlanes[1][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[1][0] * min[0] + sViewportFrustumPlanes[1][1] * min[1] + sViewportFrustumPlanes[1][2] * max[2] + sViewportFrustumPlanes[1][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[1][0] * min[0] + sViewportFrustumPlanes[1][1] * max[1] + sViewportFrustumPlanes[1][2] * min[2] + sViewportFrustumPlanes[1][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[1][0] * min[0] + sViewportFrustumPlanes[1][1] * max[1] + sViewportFrustumPlanes[1][2] * max[2] + sViewportFrustumPlanes[1][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[1][0] * max[0] + sViewportFrustumPlanes[1][1] * min[1] + sViewportFrustumPlanes[1][2] * min[2] + sViewportFrustumPlanes[1][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[1][0] * max[0] + sViewportFrustumPlanes[1][1] * min[1] + sViewportFrustumPlanes[1][2] * max[2] + sViewportFrustumPlanes[1][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[1][0] * max[0] + sViewportFrustumPlanes[1][1] * max[1] + sViewportFrustumPlanes[1][2] * min[2] + sViewportFrustumPlanes[1][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[1][0] * max[0] + sViewportFrustumPlanes[1][1] * max[1] + sViewportFrustumPlanes[1][2] * max[2] + sViewportFrustumPlanes[1][3]) >= 0.0f))
        return FALSE;

    if (((sViewportFrustumPlanes[2][0] * min[0] + sViewportFrustumPlanes[2][1] * min[1] + sViewportFrustumPlanes[2][2] * min[2] + sViewportFrustumPlanes[2][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[2][0] * min[0] + sViewportFrustumPlanes[2][1] * min[1] + sViewportFrustumPlanes[2][2] * max[2] + sViewportFrustumPlanes[2][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[2][0] * min[0] + sViewportFrustumPlanes[2][1] * max[1] + sViewportFrustumPlanes[2][2] * min[2] + sViewportFrustumPlanes[2][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[2][0] * min[0] + sViewportFrustumPlanes[2][1] * max[1] + sViewportFrustumPlanes[2][2] * max[2] + sViewportFrustumPlanes[2][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[2][0] * max[0] + sViewportFrustumPlanes[2][1] * min[1] + sViewportFrustumPlanes[2][2] * min[2] + sViewportFrustumPlanes[2][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[2][0] * max[0] + sViewportFrustumPlanes[2][1] * min[1] + sViewportFrustumPlanes[2][2] * max[2] + sViewportFrustumPlanes[2][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[2][0] * max[0] + sViewportFrustumPlanes[2][1] * max[1] + sViewportFrustumPlanes[2][2] * min[2] + sViewportFrustumPlanes[2][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[2][0] * max[0] + sViewportFrustumPlanes[2][1] * max[1] + sViewportFrustumPlanes[2][2] * max[2] + sViewportFrustumPlanes[2][3]) >= 0.0f))
        return FALSE;

    if (((sViewportFrustumPlanes[3][0] * min[0] + sViewportFrustumPlanes[3][1] * min[1] + sViewportFrustumPlanes[3][2] * min[2] + sViewportFrustumPlanes[3][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[3][0] * min[0] + sViewportFrustumPlanes[3][1] * min[1] + sViewportFrustumPlanes[3][2] * max[2] + sViewportFrustumPlanes[3][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[3][0] * min[0] + sViewportFrustumPlanes[3][1] * max[1] + sViewportFrustumPlanes[3][2] * min[2] + sViewportFrustumPlanes[3][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[3][0] * min[0] + sViewportFrustumPlanes[3][1] * max[1] + sViewportFrustumPlanes[3][2] * max[2] + sViewportFrustumPlanes[3][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[3][0] * max[0] + sViewportFrustumPlanes[3][1] * min[1] + sViewportFrustumPlanes[3][2] * min[2] + sViewportFrustumPlanes[3][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[3][0] * max[0] + sViewportFrustumPlanes[3][1] * min[1] + sViewportFrustumPlanes[3][2] * max[2] + sViewportFrustumPlanes[3][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[3][0] * max[0] + sViewportFrustumPlanes[3][1] * max[1] + sViewportFrustumPlanes[3][2] * min[2] + sViewportFrustumPlanes[3][3]) >= 0.0f) &&
        ((sViewportFrustumPlanes[3][0] * max[0] + sViewportFrustumPlanes[3][1] * max[1] + sViewportFrustumPlanes[3][2] * max[2] + sViewportFrustumPlanes[3][3]) >= 0.0f))
        return FALSE;

    return TRUE;
}

typedef struct {
    s32 cmd_0;
    s32 size_4;
    s16 unk8[3];
    s16 unkE[3];
    s16 unk14;
}GeoCmdD;

void func_80339124(Gfx **, Mtx **, BKGeoList *);

// @recomp Patched to disable culling in model rendering.
RECOMP_PATCH void func_80338DCC(Gfx ** gfx, Mtx ** mtx, void *arg2){
    f32 sp2C[3];
    f32 sp20[3];
    GeoCmdD * cmd = (GeoCmdD *)arg2;
    if(cmd->unk14){
        // @recomp Always run the child command.
        // sp2C[0] = (f32)cmd->unk8[0] * modelRenderScale;
        // sp2C[1] = (f32)cmd->unk8[1] * modelRenderScale;
        // sp2C[2] = (f32)cmd->unk8[2] * modelRenderScale;

        // sp20[0] = (f32)cmd->unkE[0] * modelRenderScale;
        // sp20[1] = (f32)cmd->unkE[1] * modelRenderScale;
        // sp20[2] = (f32)cmd->unkE[2] * modelRenderScale;
        if(TRUE){//viewport_isBoundingBoxInFrustum(sp2C, sp20)){
            func_80339124(gfx, mtx, (BKGeoList*)((s32)cmd + cmd->unk14));
        }
    }
}

// @recomp Patched to disable frustum culling.
RECOMP_PATCH bool viewport_isBoundingBoxInFrustum(f32 min[3], f32 max[3]) {
    // @recomp Disable frustum culling for cubes outside of demos. Demos need to maintain the original
    // culling for cubes to prevent them from behaving differently than the original game.
    if (recomp_in_demo_playback_game_mode()) {
        return viewport_isBoundingBoxInFrustumCopy(min, max);
    }

    return TRUE;
}

// @recomp Patched to allow globally disabling frustum checks.
RECOMP_PATCH bool viewport_func_8024DB50(f32 pos[3], f32 distance) {
    // @recomp Return true if frustum checks are disabled (always pass the frustum check).
    if (!frustum_checks_enabled) {
        return TRUE;
    }
    f32 delta[3];
    s32 i;

    delta[0] = pos[0] - sViewportPosition[0];
    delta[1] = pos[1] - sViewportPosition[1];
    delta[2] = pos[2] - sViewportPosition[2];

    for(i = 0; i < 4; i++) {
        if(distance <= ml_vec3f_dot_product(delta, sViewportFrustumPlanes[i])) {
            return FALSE;
        }
    }

    return TRUE;
}


// @recomp Patched to use the frustum culling result to set the marker's field.
// This is only enabled during demos, and is needed so that demos play back correctly.
RECOMP_PATCH void actor_postdrawMethod(ActorMarker *marker){
    if (!cur_model_would_have_been_culled_in_demo) {
        marker->unk14_21 = TRUE;
    }
}

extern struct5Bs *D_8036E568;
bool func_80330534(Actor *actor);
bool func_8033056C(Actor *actor);
void anim_802897D4(AnimMtxList *this, BKAnimationList *arg0, Animation *dst);
void modelRender_setBoneTransformList(s32);
void modelRender_setAnimatedTexturesCacheId(s32 arg0);
s32 actor_getAnimatedTexturesCacheId(Actor *actor);

// @recomp Patched to not call the actor's callback if the actor's model would have been culled.
// This is only enabled during demos, and is needed so that demos play back correctly.
RECOMP_PATCH void actor_predrawMethod(Actor *this){
    s32 pad4C;
    BKModelBin *sp48;
    bool sp44;
    BKVertexList *sp40;
    f32 sp34[3];
    
    sp48 = marker_loadModelBin(this->marker);
    func_80330534(this);
    if(this->anctrl != NULL){
        anctrl_drawSetup(this->anctrl, this->position, 1);
    }

    if(this->marker->unk20 != NULL){
        sp44 = FALSE;
        if(this->unk148 != NULL){
            animMtxList_setBoned(&this->marker->unk20, model_getAnimationList(sp48), skeletalAnim_getBoneTransformList(this->unk148));
            sp44 = TRUE;
        }//L8032542C
        else if(this->anctrl != NULL && model_getAnimationList(sp48)){
            anim_802897D4(&this->marker->unk20, model_getAnimationList(sp48), anctrl_getAnimPtr(this->anctrl));
            sp44 = TRUE;
        }//L80325474

        if(sp44){
            func_8033A444((AnimMtxList*)this->marker->unk20);
        }
    }//L8032548C

    if(this->alpha_124_19 < 0xFF){
        modelRender_setAlpha(this->alpha_124_19);
    }

    modelRender_setDepthMode(this->depth_mode);
    if(this->marker->unk44 != 0){
        if((s32)this->marker->unk44 == 1){
            func_8033A450(D_8036E568);
        }
        else{
            func_8033A450(this->marker->unk44);
        }
    }

    // @recomp Set up skinning data for this actor.
    recomp_setup_marker_skinning(this->marker);

    if(this->unkF4_30){
        sp40 = func_80330C74(this);
        if(this->unk138_29){
            sp34[0] = this->pitch;
            sp34[1] = this->yaw;
            sp34[2] = this->roll;
            codeAC520_func_80333D48(sp40, this->position, sp34, this->scale, 0, model_getVtxList(sp48));
        }//L80325560
        modelRender_setVertexList(sp40);
        this->unkF4_29 = NOT(this->unkF4_29);
    }//L80325594

    // @recomp Only call the actor's callback if the model wouldn't have been frustum culled.
    // This only has an effect during demos, as the in frustum flag always set to true outside of demos.
    if (!cur_model_would_have_been_culled_in_demo) {
        if(this->unk130){
            this->unk130(this);
        }
    }

    if(this->unk148 && !this->marker->unk20){
        modelRender_setBoneTransformList((s32)skeletalAnim_getBoneTransformList(this->unk148));
    }

    func_8033056C(this);
    modelRender_setAnimatedTexturesCacheId(actor_getAnimatedTexturesCacheId(this));
}
