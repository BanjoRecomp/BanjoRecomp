#include "patches.h"
#include "transform_ids.h"
#include "bk_api.h"
#include "core1/core1.h"
#include "core1/vimgr.h"
#include "core1/mlmtx.h"
#include "functions.h"

extern struct{
    s32 unk0;
    s32 game_mode; //game_mode
    f32 unk8; 
    s32 unkC; //freeze_scene_flag (used for pause menu)
    f32 unk10;
    u8 transition;
    u8 map;
    u8 exit;
    u8 unk17; //reset_on_map_load
    u8 unk18;
    u8 unk19;
    u8 unk1A;
    u8 unk1B;
    u8 unk1C;
} D_8037E8E0;

void func_802E39D0(Gfx **gdl, Mtx **mptr, Vtx **vptr, s32 framebuffer_idx, s32 arg4);

#define PRINT_FUNC() \
    recomp_printf("%s\n", __func__)

#define PRINT_FUNC_INDENT() \
    recomp_printf("  %s\n", __func__)

extern f32 sViewportFOVy;
extern f32 sViewportAspect;
extern f32 sViewportNear;
extern f32 sViewportFar;
extern f32 sViewportPosition[3];
extern f32 sViewportRotation[3];
    
void func_80256E24(f32 dst[3], f32 theta, f32 phi, f32 x, f32 y, f32 z);
void ml_vec3f_copy(f32 dst[3], f32 src[3]);

// RECOMP_PATCH void viewport_moveAlongZAxis(f32 offset) {
//     PRINT_FUNC_INDENT();
//     f32 delta_position[3];

//     func_80256E24(delta_position, sViewportRotation[0], sViewportRotation[1], 0.0f, 0.0f, offset);

//     sViewportPosition[0] += delta_position[0];
//     sViewportPosition[1] += delta_position[1];
//     sViewportPosition[2] += delta_position[2];
// }

// RECOMP_PATCH void viewport_setPosition_vec3f(f32 src[3]) {
//     PRINT_FUNC_INDENT();
//     ml_vec3f_copy(sViewportPosition, src);
// }

// RECOMP_PATCH void viewport_setPosition_vec3w(s32 src[3]) {
//     PRINT_FUNC_INDENT();
//     sViewportPosition[0] = (f32)src[0];
//     sViewportPosition[1] = (f32)src[1];
//     sViewportPosition[2] = (f32)src[2];
// }

// RECOMP_PATCH void viewport_setPosition_f3(f32 x, f32 y, f32 z) {
//     PRINT_FUNC_INDENT();
//     sViewportPosition[0] = x;
//     sViewportPosition[1] = y;
//     sViewportPosition[2] = z;
// }

RECOMP_PATCH void viewport_setRenderPerspectiveMatrix(Gfx **gfx, Mtx **mtx, f32 near, f32 far) {
    u16 perspNorm;

    near = MAX(sViewportNear, near);
    far = MIN(sViewportFar, far);

    if(*(u32*)OS_PHYSICAL_TO_K0(0x1D8) + 0x53D4FFF0) { 
        near = 750.0f; 
        far = 1250.0f;
    }
    
    guPerspective(*mtx, &perspNorm, sViewportFOVy, sViewportAspect, near, far, 0.5f);
    gSPPerspNormalize((*gfx)++, perspNorm);
    gSPMatrix((*gfx)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    guRotate(*mtx, -sViewportRotation[2], 0.0f, 0.0f, -1.0f);
    gSPMatrix((*gfx)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);

    guRotate(*mtx, -sViewportRotation[0], 1.0f, 0.0f, 0.0f);
    gSPMatrix((*gfx)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);

    guRotate(*mtx, -sViewportRotation[1], 0.0f, 1.0f, 0.0f);
    gSPMatrix((*gfx)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);

    guTranslate(*mtx, 0.0f, 0.0f, 0.0f);
    gSPMatrix((*gfx)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    // @recomp Create an inverse matrix for the viewport translation and provide it as the inverse view matrix to counteract the camera translation.
    MtxF* invView = (MtxF*)*mtx;
    (*mtx)++;
    guTranslateF(invView->m, -sViewportPosition[0], -sViewportPosition[1], -sViewportPosition[2]);
    gEXSetInvViewMatrixFloat((*gfx)++, invView->m);   
}

typedef void (*GeoListFunc)(Gfx **, Mtx **, void *);

typedef struct {
    s32 cmd_0;
    s32 size_4;
    u8  unk8;
    s8  unk9;
}GeoCmd2;

extern u8 D_8037BFB8;
extern s32 D_8036E7B0;
extern AnimMtxList *D_8038371C;
extern MtxF D_80383BF8;
extern s32 D_80370990;
extern GeoListFunc D_80370994[];

void eggShatter_draw(Gfx **gPtr, Mtx **mPtr, Vtx **vPtr);
void baModel_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx);
Actor *marker_getActor(ActorMarker *);
void func_8033A28C(bool arg0);
void func_8033A244(f32);
void func_8033A280(f32);
void func_80339124(Gfx **, Mtx **, BKGeoList *);
MtxF *animMtxList_get(AnimMtxList *this, s32 arg1);
bool AnimTextureListCache_tryGetTextureOffset(s32 list_index, s32 texture_index, s32 *current_frame);
void animMtxList_setBoneless(AnimMtxList **this_ptr, BKAnimationList *anim_list);
void animMtxList_setBoned(AnimMtxList **this_ptr, BKAnimationList *anim_list, BoneTransformList *arg2);
void func_80349AD0(void);
void func_802ED52C(BKModelUnk20List *arg0, f32 arg1[3], f32 arg2);
void func_802E6BD0(BKModelUnk28List *arg0, BKVertexList *arg1, AnimMtxList *mtx_list);
void assetCache_free(void *arg0);

Actor* cur_drawn_actor = NULL;
u32 cur_drawn_actor_spawn_index = 0;
u32 cur_drawn_actor_transform_id = 0;

// @recomp Patched to set the current transform ID to banjo's when drawing the player.
RECOMP_PATCH void player_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    if (D_8037BFB8) {
        eggShatter_draw(gfx, mtx, vtx);

        // @recomp Set the current transform ID to banjo's.
        cur_drawn_actor_transform_id = BANJO_TRANSFORM_ID_START;

        baModel_draw(gfx, mtx, vtx);

        // @recomp Clear the current transform ID.
        cur_drawn_actor_transform_id = 0;
    }
}

// @recomp Patched to set the actor being drawn before the actor's draw func is called.
RECOMP_PATCH void __marker_draw(ActorMarker *this, Gfx **gfx, Mtx **mtx, Vtx **vtx){
    Actor *actor;
    u32 draw_dist;
    f32 draw_dist_f;
    f32 percentage;
    if(!this->unk3E_0){
        // @recomp Set the current drawn actor to null, as this marker has no actor.
        cur_drawn_actor = NULL;
        cur_drawn_actor_spawn_index = 0;
        cur_drawn_actor_transform_id = 0;

        this->drawFunc(this, gfx, mtx, vtx);
        return;
    }
    actor =  marker_getActor(this);
    func_8033A28C(actor->unk58_2);
    if( actor->unk58_2 && !this->unk40_23 && !this->unk40_21 && !D_8036E7B0){
        func_8033A244(3700.0f);
    }
    
    if(actor->unk124_7 && !actor->despawn_flag && actor->unk58_0){
        draw_dist = actor->actor_info->draw_distance;
        if(draw_dist != 0){
            percentage = (f32)draw_dist*(1/(f64)0x400);
        }
        else if(this->unk40_21){
            percentage = 2.0f;
        }
        else{
            percentage = 1.0f;
        }
        func_8033A280(percentage);
        
        // @recomp Set the current drawn actor.
        cur_drawn_actor = actor;
        cur_drawn_actor_spawn_index = bkrecomp_get_actor_spawn_index(actor);
        cur_drawn_actor_transform_id = ACTOR_TRANSFORM_ID_START + cur_drawn_actor_spawn_index * ACTOR_TRANSFORM_ID_COUNT;
        // printf("Drawing actor %02X\n", actor->actor_info->actorId);

        this->drawFunc(this, gfx, mtx, vtx);
        
        // @recomp Clear the current drawn actor after drawing.
        cur_drawn_actor = NULL;
        cur_drawn_actor_spawn_index = 0;
        cur_drawn_actor_transform_id = 0;
    }//L8032D300
    func_8033A244(30000.0f);
    func_8033A280(1.0f);
}

#define gEXMatrixGroupSimpleNormal(cmd, id, push, proj, edit) \
    gEXMatrixGroup(cmd, id, G_EX_INTERPOLATE_SIMPLE, push, proj, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, edit)
    
#define gEXMatrixGroupSimpleVerts(cmd, id, push, proj, edit) \
    gEXMatrixGroup(cmd, id, G_EX_INTERPOLATE_SIMPLE, push, proj, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, edit)

// @recomp Patched to set matrix groups when processing geo bones.
RECOMP_PATCH void func_803387F8(Gfx **gfx, Mtx **mtx, void *arg2){
    GeoCmd2 *cmd = (GeoCmd2 *)arg2;

    if(D_8038371C){
        mlMtx_push_multiplied_2(&D_80383BF8, animMtxList_get(D_8038371C, cmd->unk9));
        if(D_80370990){
            mlMtxApply(*mtx);
            gSPMatrix((*gfx)++, (*mtx)++, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            if (cur_drawn_actor_transform_id != 0) {
                // @recomp Tag the matrix.
                // gEXMatrixGroupSimpleNormal((*gfx)++, cur_drawn_actor_transform_id + cmd->unk9, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_ALLOW);
                gEXMatrixGroupSimpleVerts((*gfx)++, cur_drawn_actor_transform_id + cmd->unk9, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_ALLOW);
                // gEXMatrixGroupDecomposedNormal((*gfx)++, cur_drawn_actor_transform_id + cmd->unk9 + 1, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_ALLOW);
            }
        }
    }
    if(cmd->unk8){
        func_80339124(gfx, mtx, (BKGeoList*)((u8*)cmd + cmd->unk8));
    }
    if(D_8038371C){
        mlMtxPop();
        if(D_80370990){
            gSPPopMatrix((*gfx)++, G_MTX_MODELVIEW);

            if (cur_drawn_actor_transform_id != 0) {
                // @recomp Pop the matrix group.
                gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
            }
        }
    }
}

extern Gfx setup2CycleDL[];
extern Gfx setup2CycleBlackPrimDL[];
extern Gfx setup2CycleWhiteEnvDL[];
extern Gfx setup2CycleDL_copy[];
extern Gfx renderModesNoDepthOpa[][2];
extern Gfx renderModesFullDepthOpa[][2];
extern Gfx renderModesDepthCompareOpa[][2];
extern Gfx renderModesNoDepthXlu[][2];
extern Gfx renderModesFullDepthXlu[][2];
extern Gfx renderModesDepthCompareXlu[][2];
extern Gfx mipMapClampDL[];
extern Gfx mipMapWrapDL[];

extern struct{
    f32 unk0[3];
    f32 unkC[3];
    s32 unk18;
    f32 unk1C[3];
    f32 unk28[3];
} D_80383758;

extern struct{
    GenFunction_1 pre_method;
    s32 pre_arg;
    GenFunction_1 post_method;
    s32 post_arg;
} modelRenderCallback;

extern s32 modelRenderDynEnvColor[4];

extern struct {
    s32 unk0;
    f32 unk4[3];
}D_803837B0;

extern u8 modelRenderDynAlpha;

extern struct {
    s32 model_id; //model_asset_index
    f32 unk4;
    f32 unk8;
    u8 padC[0x4];
} D_803837C8;

extern enum model_render_depth_mode_e modelRenderDepthMode;

extern struct {
    LookAt lookat_buffer[32];
    LookAt *cur_lookat;
    LookAt *lookat_buffer_end;
    f32 eye_pos[3];
} D_803837E0;
extern MtxF D_80383BF8;
extern f32 modelRenderCameraPosition[3];
extern f32 modelRenderCameraRotation[3];
extern BKModelBin *modelRenderModelBin;
extern f32 modelRenderRotation[3];
extern f32 D_80383C64;
extern f32 D_80383C68[3];
extern f32 D_80383C78[3];
extern f32 D_80383C88[3];
extern f32 D_80383C98[3];

enum model_render_color_mode_e{
    COLOR_MODE_DYNAMIC_PRIM_AND_ENV,
    COLOR_MODE_DYNAMIC_ENV,
    COLOR_MODE_STATIC_OPAQUE,
    COLOR_MODE_STATIC_TRANSPARENT
};

extern struct5Bs *D_80383650;
extern s32  D_80383658[0x2A];
extern BoneTransformList *modelRenderBoneTransformList;
extern bool D_80383704;
extern f32  D_80383708;
extern f32  D_8038370C;
extern s32  D_80383710;
extern enum model_render_color_mode_e  modelRenderColorMode;
extern BKGfxList *            modelRenderDisplayList;
extern AnimMtxList *            D_8038371C;
extern BKTextureList * modelRenderTextureList;
extern s32                    modelRenderAnimatedTexturesCacheId;
extern BKVertexList *  modelRendervertexList;
extern BKModelUnk20List *     D_8038372C;
extern AnimMtxList *            modelRenderAnimMtxList;
extern f32                    modelRenderScale;

extern struct{
    s32 env[4];
    s32 prim[4];
} modelRenderDynColors;

// @recomp Patched to set an initial matrix group for the draw.
RECOMP_PATCH BKModelBin *modelRender_draw(Gfx **gfx, Mtx **mtx, f32 position[3], f32 rotation[3], f32 scale, f32*arg5, BKModelBin* model_bin){
    f32 camera_focus[3];
    f32 camera_focus_distance;
    f32 padEC;
    f32 object_position[3];
    void *rendermode_table_opa; // Table of render modes to use for opaque rendering
    void *rendermode_table_xlu; // Table of render modes to use for translucent rendering
    f32 spD4;
    f32 spD0;
    BKVertexList *verts;
    s32 alpha; 
    f32 tmp_f0;
    f32 padB8;
    
    if( (!model_bin && !D_803837C8.model_id)
        || (model_bin && D_803837C8.model_id)
    ){
        modelRender_reset();
        return 0;
    }

    D_80370990 = 0;
    viewport_getPosition_vec3f(modelRenderCameraPosition);
    viewport_getRotation_vec3f(modelRenderCameraRotation);
    if(D_80383758.unk18){
        D_80383758.unk1C[0] = modelRenderCameraPosition[0];
        D_80383758.unk1C[1] = modelRenderCameraPosition[1];
        D_80383758.unk1C[2] = modelRenderCameraPosition[2];

        D_80383758.unk28[0] = modelRenderCameraRotation[0];\
        D_80383758.unk28[1] = modelRenderCameraRotation[1];\
        D_80383758.unk28[2] = modelRenderCameraRotation[2];
    }

    if(position){
        object_position[0] = position[0];
        object_position[1] = position[1];
        object_position[2] = position[2];
    }
    else{
        object_position[0] = object_position[1] = object_position[2] = 0.0f;
    }

    camera_focus[0] = object_position[0] - modelRenderCameraPosition[0];
    camera_focus[1] = object_position[1] - modelRenderCameraPosition[1];
    camera_focus[2] = object_position[2] - modelRenderCameraPosition[2];

    if( ((camera_focus[0] < -17000.0f) || (17000.0f < camera_focus[0]))
        || ((camera_focus[1] < -17000.0f) || (17000.0f < camera_focus[1]))
        || ((camera_focus[2] < -17000.0f) || (17000.0f < camera_focus[2]))
    ){
        modelRender_reset();
        return 0;
    }

    if(D_80383758.unk18){
        modelRenderCameraPosition[0] = D_80383758.unk0[0];
        modelRenderCameraPosition[1] = D_80383758.unk0[1];
        modelRenderCameraPosition[2] = D_80383758.unk0[2];

        modelRenderCameraRotation[0] = D_80383758.unkC[0],
        modelRenderCameraRotation[1] = D_80383758.unkC[1],
        modelRenderCameraRotation[2] = D_80383758.unkC[2];
        viewport_setPosition_vec3f(modelRenderCameraPosition);
        viewport_setRotation_vec3f(modelRenderCameraRotation);
        viewport_update();
        camera_focus[0] = object_position[0] - modelRenderCameraPosition[0];
        camera_focus[1] = object_position[1] - modelRenderCameraPosition[1];
        camera_focus[2] = object_position[2] - modelRenderCameraPosition[2];
    }

    if(model_bin){
        verts = modelRendervertexList ? modelRendervertexList : (BKVertexList *)((s32)model_bin + model_bin->vtx_list_offset_10);
        spD0 = verts->global_norm;
        spD4 = verts->local_norm;
    }
    else{
        spD0 = D_803837C8.unk8;
        spD4 = D_803837C8.unk4;
    }
    camera_focus_distance = gu_sqrtf(camera_focus[0]*camera_focus[0] + camera_focus[1]*camera_focus[1] + camera_focus[2]*camera_focus[2]);
    if( 4000.0f <= camera_focus_distance && spD4*scale*D_8038370C*50.0f < D_80383708){
        D_80383708 = spD4*scale*D_8038370C*50.0f;
    }

    if(D_80383708 <= camera_focus_distance){
        modelRender_reset();
        return 0;
    }

    D_80370990 = (D_80383704) ? viewport_func_8024DB50(object_position, spD0*scale) : 1;
    if(D_80370990 == 0){
        modelRender_reset();
        return 0;
    }

    if(modelRenderCallback.pre_method != NULL){
        modelRenderCallback.pre_method(modelRenderCallback.pre_arg);
    }
    func_80349AD0();
    if(model_bin == NULL){
        model_bin = assetcache_get(D_803837C8.model_id);
    }
    modelRenderModelBin = model_bin;
    modelRenderDisplayList = modelRenderDisplayList ? modelRenderDisplayList : (BKGfxList *)((s32)modelRenderModelBin + modelRenderModelBin->gfx_list_offset_C),
    modelRenderTextureList = modelRenderTextureList ? modelRenderTextureList : (BKTextureList *)((s32)modelRenderModelBin + modelRenderModelBin->texture_list_offset_8),
    modelRendervertexList = modelRendervertexList ? modelRendervertexList : (BKVertexList *)((s32)modelRenderModelBin + modelRenderModelBin->vtx_list_offset_10),
    D_8038372C = (modelRenderModelBin->unk20 == NULL) ? NULL : (BKModelUnk20List *)((u8*)model_bin + model_bin->unk20);

    if(D_80383710){
        tmp_f0 = D_80383708 - 500.0f;
        if(tmp_f0 < camera_focus_distance){
            alpha = (s32)((1.0f - (camera_focus_distance - tmp_f0)/500.0f)*255.0f);
            if(modelRenderColorMode == COLOR_MODE_DYNAMIC_PRIM_AND_ENV){
                modelRenderDynColors.prim[3] = (modelRenderDynColors.prim[3] * alpha) / 0xff;
            }
            else if(modelRenderColorMode == COLOR_MODE_DYNAMIC_ENV){
                modelRenderDynEnvColor[3] = (modelRenderDynEnvColor[3] * alpha)/0xff;
            }
            else if(modelRenderColorMode == COLOR_MODE_STATIC_OPAQUE){
                modelRender_setAlpha(alpha);
            }
            else if(modelRenderColorMode == COLOR_MODE_STATIC_TRANSPARENT){
                modelRenderDynAlpha = (modelRenderDynAlpha *alpha)/0xff;
            }
        }
    }

    // Set up segments 1 and 2 to point to vertices and textures respectively
    gSPSegment((*gfx)++, 0x01, osVirtualToPhysical(&modelRendervertexList->vtx_18));
    gSPSegment((*gfx)++, 0x02, osVirtualToPhysical(&modelRenderTextureList->tex_8[modelRenderTextureList->cnt_4]));

    //segments 11 to 15 contain animated textures
    if(modelRenderAnimatedTexturesCacheId){
        int i_segment;
        s32 texture_offset;
        
        for(i_segment = 0; i_segment < 4; i_segment++){
            if(AnimTextureListCache_tryGetTextureOffset(modelRenderAnimatedTexturesCacheId, i_segment, &texture_offset))
                gSPSegment((*gfx)++, 15 - i_segment, osVirtualToPhysical((u8*)&modelRenderTextureList->tex_8[modelRenderTextureList->cnt_4] + texture_offset));
        }
    }

    if(modelRenderDepthMode != MODEL_RENDER_DEPTH_NONE){
        gSPSetGeometryMode((*gfx)++, G_ZBUFFER);
    }
    else{
        gSPClearGeometryMode((*gfx)++, G_ZBUFFER);
    }

    // Pick a table of render modes for opaque and translucent rendering
    if(modelRenderDepthMode == MODEL_RENDER_DEPTH_NONE){ // No depth buffering
        rendermode_table_opa = renderModesNoDepthOpa;
        rendermode_table_xlu = renderModesNoDepthXlu;
    }
    else if(modelRenderDepthMode == MODEL_RENDER_DEPTH_FULL){ // Full depth buffering
        rendermode_table_opa = renderModesFullDepthOpa;
        rendermode_table_xlu = renderModesFullDepthXlu;
    }
    else if(modelRenderDepthMode == MODEL_RENDER_DEPTH_COMPARE){ // Depth compare but no depth write
        rendermode_table_opa = renderModesDepthCompareOpa;
        rendermode_table_xlu = renderModesDepthCompareXlu;
    }

    if(modelRenderColorMode == COLOR_MODE_DYNAMIC_PRIM_AND_ENV){
        s32 alpha;
        
        alpha = modelRenderDynColors.prim[3] + (modelRenderDynColors.env[3]*(0xFF - modelRenderDynColors.prim[3]))/0xff;
        gSPDisplayList((*gfx)++, setup2CycleDL);
        gDPSetEnvColor((*gfx)++, modelRenderDynColors.env[0], modelRenderDynColors.env[1], modelRenderDynColors.env[2], alpha);
        gDPSetPrimColor((*gfx)++, 0, 0, modelRenderDynColors.prim[0], modelRenderDynColors.prim[1], modelRenderDynColors.prim[2], 0);

        // Set up segment 3 to point to the right render mode table based on the alpha value
        if(alpha == 0xFF){
            gSPSegment((*gfx)++, 0x03, osVirtualToPhysical(rendermode_table_opa));
        }
        else{
            gSPSegment((*gfx)++, 0x03, osVirtualToPhysical(rendermode_table_xlu));
        }
        //TODO
    }
    else if(modelRenderColorMode == COLOR_MODE_DYNAMIC_ENV){
        gSPDisplayList((*gfx)++, setup2CycleBlackPrimDL);
        gDPSetEnvColor((*gfx)++, modelRenderDynEnvColor[0], modelRenderDynEnvColor[1], modelRenderDynEnvColor[2], modelRenderDynEnvColor[3]);

        // Set up segment 3 to point to the right render mode table based on the alpha value
        if(modelRenderDynEnvColor[3] == 0xFF){
            gSPSegment((*gfx)++, 0x03, osVirtualToPhysical(rendermode_table_opa));
        }
        else{
            gSPSegment((*gfx)++, 0x03, osVirtualToPhysical(rendermode_table_xlu));
        }
    }
    else if(modelRenderColorMode == COLOR_MODE_STATIC_OPAQUE){
        gSPDisplayList((*gfx)++, setup2CycleWhiteEnvDL);
        gSPSegment((*gfx)++, 0x03, osVirtualToPhysical(rendermode_table_opa));
    }
    else if(modelRenderColorMode == COLOR_MODE_STATIC_TRANSPARENT){
        gSPDisplayList((*gfx)++, setup2CycleDL_copy);
        gDPSetEnvColor((*gfx)++, 0xFF, 0xFF, 0xFF, modelRenderDynAlpha);
        gSPSegment((*gfx)++, 0x03, osVirtualToPhysical(rendermode_table_xlu));
    }

    if(modelRenderModelBin->geo_typ_A & 2){ //trilinear mipmapping
        gSPDisplayList((*gfx)++, mipMapWrapDL);
    }

    if(modelRenderModelBin->geo_typ_A & 4){ //env mapping
        if(0.0f == camera_focus[2]){
            camera_focus[2] = -0.1f;
        }
        guLookAtReflect(*mtx, D_803837E0.cur_lookat,
            D_803837E0.eye_pos[0], D_803837E0.eye_pos[1], D_803837E0.eye_pos[2],
            camera_focus[0], camera_focus[1], camera_focus[2],
            0.0f, 1.0f, 0.0f);
        gSPLookAt((*gfx)++, D_803837E0.cur_lookat);
        osWritebackDCache(D_803837E0.cur_lookat, sizeof(LookAt));
        D_803837E0.cur_lookat++;
        if(D_803837E0.cur_lookat == D_803837E0.lookat_buffer_end)
            D_803837E0.cur_lookat = D_803837E0.lookat_buffer;
    }

    if(D_8038371C && !modelRenderModelBin->animation_list_offset_18){
        D_8038371C = 0;
    }
    else if(D_8038371C == 0 && modelRenderModelBin->animation_list_offset_18){
        if(modelRenderBoneTransformList == NULL){
            animMtxList_setBoneless(&modelRenderAnimMtxList, (u8*)model_bin + model_bin->animation_list_offset_18);
        }
        else{
            animMtxList_setBoned(&modelRenderAnimMtxList, (u8*)model_bin + model_bin->animation_list_offset_18, modelRenderBoneTransformList);
        }
        D_8038371C = modelRenderAnimMtxList;
    }

    if(D_8038372C){
        func_802ED52C(D_8038372C, modelRenderCameraPosition, scale);
    }

    if(model_bin->unk28 != NULL && D_8038371C != NULL){
        func_802E6BD0((u8*)modelRenderModelBin + modelRenderModelBin->unk28, modelRendervertexList, D_8038371C);
    }

    mlMtxIdent();
    if(D_80383758.unk18){
        func_80252AF0(D_80383758.unk1C, object_position, rotation, scale, arg5);
    }
    else{
        func_80252AF0(modelRenderCameraPosition, object_position, rotation, scale, arg5);
    }

    if(D_803837B0.unk0){
        mlMtxRotatePYR(D_803837B0.unk4[0], D_803837B0.unk4[1], D_803837B0.unk4[2]);
    }
    mlMtxGet(&D_80383BF8);

    mlMtxApply(*mtx);
    gSPMatrix((*gfx)++, (*mtx)++, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    
    // @recomp Create a matrix group if a transform id is set.
    if (cur_drawn_actor_transform_id != 0) {
        gEXMatrixGroupDecomposedVerts((*gfx)++, cur_drawn_actor_transform_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_ALLOW);
    }
    
    modelRenderScale = scale;
    if(rotation){
        modelRenderRotation[0] = rotation[0];
        modelRenderRotation[1] = rotation[1];
        modelRenderRotation[2] = rotation[2];
    }
    else{
        modelRenderRotation[0] = modelRenderRotation[1] = modelRenderRotation[2] = 0.0f;
    }

    func_80339124(gfx, mtx, (BKGeoList *)((u8 *)model_bin + model_bin->geo_list_offset_4));
    gSPPopMatrix((*gfx)++, G_MTX_MODELVIEW);

    // @recomp Pop the matrix group if a transform id is set.
    if (cur_drawn_actor_transform_id != 0) {
        gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
    }

    if(modelRenderCallback.post_method != NULL){
        modelRenderCallback.post_method(modelRenderCallback.post_arg);
    }

    if(D_803837C8.model_id){
        assetCache_free(model_bin);
    }

    modelRender_reset();
    return model_bin;
}
