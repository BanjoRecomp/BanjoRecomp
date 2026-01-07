#include "patches.h"
#include "transform_ids.h"
#include "bk_api.h"
#include "core1/core1.h"
#include "core1/vimgr.h"
#include "core1/mlmtx.h"
#include "functions.h"

extern bool recomp_in_demo_playback_game_mode();

bool skip_all_interpolation = FALSE;
bool has_additional_model_scale = FALSE;
f32 additional_model_scale_x;
f32 additional_model_scale_y;
f32 additional_model_scale_z;

void set_all_interpolation_skipped(bool skipped) {
    skip_all_interpolation = skipped;
}

bool all_interpolation_skipped() {
    return skip_all_interpolation;
}

void set_additional_model_scale(f32 x, f32 y, f32 z) {
    has_additional_model_scale = TRUE;
    additional_model_scale_x = x;
    additional_model_scale_y = y;
    additional_model_scale_z = z;
}

s32 cur_drawn_model_is_map = FALSE;
s32 cur_drawn_model_transform_id = 0;
s32 cur_drawn_model_skip_interpolation = FALSE;
s32 cur_model_transform_id_offset = 0;
s32 cur_model_uses_bones = FALSE;
s32 cur_model_would_have_been_culled_in_demo = FALSE;
s32 cur_model_uses_ex_vertex = FALSE;

Mtx identity_fixed_mtx = {{
    {
        0x00010000, 0x00000000,
        0x00000001, 0x00000000, }, {
        0x00000000, 0x00010000,
        0x00000000, 0x00000001,
    },
    {
        0x00000000, 0x00000000,
        0x00000000, 0x00000000, }, {
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
    }
}};

typedef void (*GeoListFunc)(Gfx **, Mtx **, void *);

typedef struct {
    s32 cmd_0;
    s32 size_4;
    s16 unk8;
    s16 unkA;
    f32 unkC[3];
}GeoCmd0;

typedef struct {
    s32 cmd_0;
    s32 size_4;
    f32 unk8[3];
    f32 unk14[3];
    s16 unk20;
    s16 unk22;
    s32 unk24;
}GeoCmd1;

typedef struct {
    s32 cmd_0;
    s32 size_4;
    u8  unk8;
    s8  unk9;
}GeoCmd2;

typedef struct {
    s32 cmd_0;
    s32 size_4;
    s16 unk8;
}GeoCmd3;

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
extern f32 D_80383C64;
extern f32 D_80383C68[3];
extern f32 D_80383C78[3];
extern f32 D_80383C88[3];
extern f32 D_80383C98[3];

extern s32 D_80370990;
extern GeoListFunc D_80370994[];

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

void func_80339124(Gfx **, Mtx **, BKGeoList *);
MtxF *animMtxList_get(AnimMtxList *this, s32 arg1);
bool AnimTextureListCache_tryGetTextureOffset(s32 list_index, s32 texture_index, s32 *current_frame);
void animMtxList_setBoneless(AnimMtxList **this_ptr, BKAnimationList *anim_list);
void animMtxList_setBoned(AnimMtxList **this_ptr, BKAnimationList *anim_list, BoneTransformList *arg2);
void func_80349AD0(void);
void func_802ED52C(BKModelUnk20List *arg0, f32 arg1[3], f32 arg2);
void func_802E6BD0(BKModelUnk28List *arg0, BKVertexList *arg1, AnimMtxList *mtx_list);
void assetCache_free(void *arg0);

bool set_model_matrix_group(Gfx **gfx, void *geo_list, bool skip_rotation) {
    if (cur_drawn_model_transform_id != 0) {
        u32 group_id;
        // Pick a group ID based on whether this is a map or not.
        if (cur_drawn_model_is_map) {
            // Map models use a group ID determined by the offset of the geo command to guarantee they're unique and consistent between frames.
            group_id = cur_drawn_model_transform_id + (u32)geo_list - (u32)modelRenderModelBin - modelRenderModelBin->geo_list_offset_4;
        }
        else {
            // Other models use a group ID determined by the transform ID offset.
            group_id = cur_drawn_model_transform_id + cur_model_transform_id_offset;
        }

        if (skip_all_interpolation || cur_drawn_model_skip_interpolation) {
            // Skip interpolation if all interpolation is currently skipped or the transform was specified to be skipped.
            gEXMatrixGroupSkipAll((*gfx)++, group_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
        }
        else {
            // Tag the matrix with simple matrix interpolation if the model uses bones.
            // Use decomposed matrix interpolation on any other model.
            u8 interpolation_mode = cur_model_uses_bones ? G_EX_INTERPOLATE_SIMPLE : G_EX_INTERPOLATE_DECOMPOSE;
            u8 rotation_mode = skip_rotation ? G_EX_COMPONENT_SKIP : G_EX_COMPONENT_INTERPOLATE;
            u8 vertex_interpolation_mode = cur_drawn_model_is_map && !cur_model_uses_ex_vertex ? G_EX_COMPONENT_INTERPOLATE : G_EX_COMPONENT_SKIP;
            u8 texcoord_interpolation_mode = cur_drawn_model_is_map ? G_EX_COMPONENT_INTERPOLATE : G_EX_COMPONENT_SKIP;
            gEXMatrixGroup((*gfx)++, group_id, interpolation_mode, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_COMPONENT_INTERPOLATE, rotation_mode,
                G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, vertex_interpolation_mode, G_EX_COMPONENT_INTERPOLATE,
                G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO, texcoord_interpolation_mode, G_EX_COMPONENT_AUTO);
        }

        return TRUE;
    }
    else if (skip_all_interpolation) {
        gEXMatrixGroupNoInterpolate((*gfx)++, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void pop_model_matrix_group(Gfx **gfx) {
    gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
}

// @recomp Patched to multiply the identity matrix and create a new matrix group for each display list.
RECOMP_PATCH void func_80338904(Gfx **gfx, Mtx **mtx, void *arg2){
    GeoCmd3 *cmd = (GeoCmd3 *)arg2;
    Gfx *vptr;

    if(D_80370990){
        // @recomp Create a new matrix by multiplying in the identity matrix.
        bool pushed_matrix_group = FALSE;
        gSPMatrix((*gfx)++, &identity_fixed_mtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        pushed_matrix_group = set_model_matrix_group(gfx, arg2, FALSE);

        vptr = &modelRenderDisplayList->list[cmd->unk8];
        // @recomp Remove unnecessary usage of osVirtualToPhysical to allow extended addresses.
        gSPDisplayList((*gfx)++, /*osVirtualToPhysical*/(vptr));
        
        // @recomp Pop the matrix and pop the matrix group if one was created.
        gSPPopMatrix((*gfx)++, G_MTX_MODELVIEW);
        if (pushed_matrix_group) {
            pop_model_matrix_group(gfx);
        }
    }
}

// @recomp Patched to set matrix groups when processing geo bones.
RECOMP_PATCH void func_803387F8(Gfx **gfx, Mtx **mtx, void *arg2){
    GeoCmd2 *cmd = (GeoCmd2 *)arg2;
    bool pushed_matrix_group = FALSE;

    // @recomp Increment the transform ID offset when encountering a bone command and set the flag.
    cur_model_transform_id_offset++;
    cur_model_uses_bones = TRUE;

    if(D_8038371C){
        mlMtx_push_multiplied_2(&D_80383BF8, animMtxList_get(D_8038371C, cmd->unk9));
        if(D_80370990){
            mlMtxApply(*mtx);
            gSPMatrix((*gfx)++, (*mtx)++, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            // @recomp Create a matrix group for this bone.
            pushed_matrix_group = set_model_matrix_group(gfx, arg2, FALSE);
        }
    }
    if(cmd->unk8){
        func_80339124(gfx, mtx, (BKGeoList*)((u8*)cmd + cmd->unk8));
    }
    if(D_8038371C){
        mlMtxPop();
        if(D_80370990){
            gSPPopMatrix((*gfx)++, G_MTX_MODELVIEW);

            if (pushed_matrix_group) {
                // @recomp Pop the matrix group.
                pop_model_matrix_group(gfx);
            }
        }
    }
}

// @recomp Patched to set process sorted geo commands in a consistent order while still drawing them in the original order.
// This allows a consistent ID scheme even when sorting changes.
RECOMP_PATCH void func_803385BC(Gfx **gfx, Mtx **mtx, void *arg2){
    GeoCmd1 *cmd = (GeoCmd1 *)arg2;
    f32 f14;
    s32 tmp_v0;

    mlMtx_apply_vec3f(D_80383C78, cmd->unk8);
    mlMtx_apply_vec3f(D_80383C88, cmd->unk14);

    D_80383C68[0] = D_80383C88[0] - D_80383C78[0];
    D_80383C68[1] = D_80383C88[1] - D_80383C78[1];
    D_80383C68[2] = D_80383C88[2] - D_80383C78[2];

    f14 = D_80383C68[0]*D_80383C78[0] + D_80383C68[1]*D_80383C78[1] + D_80383C68[2]*D_80383C78[2];
    f14 = -f14;
    if(cmd->unk20 & 1){
        // @recomp Increment the transform ID offset before the child node.
        cur_model_transform_id_offset++;

        if(0.0f <= f14 && (tmp_v0 = cmd->unk24)){
            D_80383C64 = f14;
            func_80339124(gfx, mtx, (BKGeoList*)((s32)cmd + tmp_v0));
        }
        else{
            D_80383C64 = f14;
            if(f14 < 0.0f){
                if(cmd->unk22)
                    func_80339124(gfx, mtx, (BKGeoList*)((s32)cmd + cmd->unk22));
            }
        }
    }
    else{
        D_80383C64 = f14;
        if(0.0f <= f14){
            // @recomp Increment the transform ID offset before the first child node.
            cur_model_transform_id_offset++;

            if(cmd->unk22)
                func_80339124(gfx, mtx, (BKGeoList*)((s32)cmd + cmd->unk22));
                
            // @recomp Increment the transform ID offset before the second child node.
            cur_model_transform_id_offset++;

            if(cmd->unk24)
                func_80339124(gfx, mtx, (BKGeoList*)((s32)cmd + cmd->unk24));
        }
        else{
            // recomp_printf("stuff 3 %08X\n", cur_drawn_model_transform_id);
            // @recomp Nodes have been sorted into the reverse order. This code has been modified
            // to process the nodes in the forward order, but uses DL branch list commands to run the 
            // DL commands of the nodes in the reverse order.
            // This makes matrix group IDs consistent between frames while still running the actual DL commands
            // in the sorted order.
            // The resulting DL will look like this:
            //      before_cmds:
            //        BranchList(before_unk24)──────╖
            //      before_unk22: <─────────────────╫────╖
            //        Commands for node 22          ║    ║
            //      between_cmds:                   ║    ║
            //        BranchList(after_commands) ───╫────╫───╖
            //      before_unk24: <─────────────────╜    ║   ║
            //        Commands for node 24               ║   ║
            //      after_unk24:                         ║   ║
            //        BranchList(before_unk22)───────────╜   ║
            //      after_commands: <────────────────────────╜

            // @recomp Reserve one command worth of space for the branch list to the unk22 node.
            Gfx* before_cmds = (*gfx);
            (*gfx)++;
            Gfx* before_unk22 = (*gfx);

            // @recomp Increment the transform ID offset before the first child node.
            cur_model_transform_id_offset++;

            // @recomp Run the unk22 node's processing first.
            // Branch list commands will be used to run these commands after the unk24 node's commands.
            if(cmd->unk22)
                func_80339124(gfx, mtx, (BKGeoList*)((s32)cmd + cmd->unk22));

            // @recomp Reserve another command for the branch list that takes the command cursor past the unk24 node's DL commands
            // after running the unk22 node's DL commands.
            Gfx* between_cmds = (*gfx);
            (*gfx)++;
            Gfx* before_unk24 = (*gfx);
                
            // @recomp Increment the transform ID offset before the second child node.
            cur_model_transform_id_offset++;

            // @recomp Run the unk24 node's processing second.
            if(cmd->unk24)
                func_80339124(gfx, mtx, (BKGeoList*)((s32)cmd + cmd->unk24));
            
            // @recomp Reserve the command for the third branch list.
            Gfx* after_unk24 = (*gfx);
            (*gfx)++;
            Gfx* after_commands = (*gfx);

            // @recomp Populate the branch list commands.
            gSPBranchList(before_cmds, before_unk24);
            gSPBranchList(between_cmds, after_commands);
            gSPBranchList(after_unk24, before_unk22);
        }
    }
}

// @recomp Patched to skip rotation interpolation for billboards when the camera skips interpolation.
RECOMP_PATCH void func_803384A8(Gfx **gfx, Mtx **mtx, void *arg2){
    GeoCmd0 *cmd = (GeoCmd0 *)arg2;
    f32 sp30[3];
    
    // @recomp Increment the transform ID offset when encountering a billboard command.
    cur_model_transform_id_offset++;

    if(cmd->unk8){
        mlMtx_apply_vec3f(sp30, cmd->unkC);
        mlMtx_push_translation(sp30[0], sp30[1], sp30[2]);
        mlMtxRotYaw(modelRenderCameraRotation[1]);
        if(!cmd->unkA){
            mlMtxRotPitch(modelRenderCameraRotation[0]);
        }
        mlMtxScale(modelRenderScale);
        mlMtxTranslate(-cmd->unkC[0], -cmd->unkC[1], -cmd->unkC[2]);
        mlMtxApply(*mtx);

        // @recomp Create a matrix group for the billboarded matrix. Skip rotation interpolation if the perspective projection skipped interpolation.
        bool pushed_matrix_group = set_model_matrix_group(gfx, arg2, perspective_interpolation_skipped());

        gSPMatrix((*gfx)++, (*mtx)++, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        func_80339124(gfx, mtx, (BKGeoList*)((s32)cmd + cmd->unk8));
        mlMtxPop();
        gSPPopMatrix((*gfx)++, G_MTX_MODELVIEW);

        // @recomp Pop the matrix group if one was pushed.
        if (pushed_matrix_group) {
            pop_model_matrix_group(gfx);
        }
    }
}

RECOMP_PATCH void func_80339124(Gfx ** gfx, Mtx ** mtx, BKGeoList *geo_list){
    do{
        D_80370994[geo_list->cmd_0](gfx, mtx, geo_list);
        if(geo_list->size_4 == 0)
            return;
        geo_list = (BKGeoList*)((s32)geo_list + geo_list->size_4);
    }while(1);
}

typedef struct {
    u32 frameCount;
    u32 modelId;
    u32 floatStart;
} ModelSkinningData;

// @recomp Applies CPU skinning and saves the result to a higher precision vertex buffer.
#define SKINNING_POSITIONS_MAX 65536

f32 sSkinningPosFloats[2][SKINNING_POSITIONS_MAX] __attribute__((aligned(8)));
f32 sSkinningVelFloats[SKINNING_POSITIONS_MAX] __attribute__((aligned(8)));
u32 sSkinningFloatCount;
u32 sSkinningFloatFrame;
MarkerExtensionId sMarkerSkinningExtensionId;
ModelSkinningData *sCurModelSkinningData;
u32 sCurModelId;

ModelSkinningData sOverlaySkinningData;
ModelSkinningData sMapSkinningData;
f32 *sMapSkinningPosFloats;

void recomp_setup_overlay_skinning(int overlay_id) {
    sCurModelSkinningData = &sOverlaySkinningData;
    sCurModelId = overlay_id; // Using the overlay ID as the model ID since there's no easy way to get the model ID for overlay draws.
}

void recomp_clear_overlay_skinning() {
    sCurModelSkinningData = NULL;
    sCurModelId = 0;
}

void recomp_reset_skinning_stack() {
    sSkinningFloatCount = 0;
    sSkinningFloatFrame++;
}

void recomp_init_vertex_skinning() {
    sMarkerSkinningExtensionId = bkrecomp_extend_marker_all(sizeof(ModelSkinningData));
}

void recomp_setup_marker_skinning(ActorMarker *marker) {
    sCurModelSkinningData = bkrecomp_get_extended_marker_data(marker, sMarkerSkinningExtensionId);
    sCurModelId = marker->modelId;
}

void recomp_setup_map_skinning(int map_model_id, float *pos_floats) {
    sCurModelSkinningData = &sMapSkinningData;
    sCurModelId = map_model_id;
    sMapSkinningPosFloats = pos_floats;
}

void recomp_clear_map_skinning() {
    sCurModelSkinningData = NULL;
    sCurModelId = 0;
    sMapSkinningPosFloats = NULL;
}

bool recomp_apply_cpu_skinning(BKModelUnk28List *arg0, BKVertexList *arg1, AnimMtxList *mtx_list, float *pos_override, float **pos, float **vel) {
    *pos = NULL;
    *vel = NULL;

    ModelSkinningData prev_skinning_data = {0};
    u32 cur_model_id = sCurModelId;
    if (sCurModelSkinningData != NULL) {
        prev_skinning_data = *sCurModelSkinningData;
        sCurModelSkinningData->frameCount = sSkinningFloatFrame;
        sCurModelSkinningData->modelId = cur_model_id;
        sCurModelSkinningData->floatStart = sSkinningFloatCount;
    }
    sCurModelSkinningData = NULL;
    sCurModelId = 0;

    if (sSkinningFloatCount + (arg1->count * 3) > SKINNING_POSITIONS_MAX) {
        return FALSE;
    }

    // Copy unmodified positions.
    s32 i, j = 0;
    float *dst_pos = &sSkinningPosFloats[sSkinningFloatFrame & 0x1][sSkinningFloatCount];
    float *dst_vel = &sSkinningVelFloats[sSkinningFloatCount];
    if (pos_override != NULL) {
        memcpy(dst_pos, pos_override, sizeof(float) * arg1->count * 3);
    }
    else {
        for (i = 0; i < arg1->count; i++) {
            dst_pos[j++] = arg1->vtx_18[i].v.ob[0];
            dst_pos[j++] = arg1->vtx_18[i].v.ob[1];
            dst_pos[j++] = arg1->vtx_18[i].v.ob[2];
        }
    }
    
    // Increase the current float count. Always align to multiples of 2.
    sSkinningFloatCount += arg1->count * 3;
    if (sSkinningFloatCount & 0x1) {
        sSkinningFloatCount++;
    }

    if (arg0 != NULL) {
        // Apply animation.
        BKModelUnk28 *i_ptr = (BKModelUnk28 *)(arg0 + 1);
        s32 mtx_index = -2;
        f32 src_coord[3];
        f32 dst_coord[3];
        s32 vertex_index;
        for (i = 0; i < arg0->count; i++) {
            if (mtx_index != i_ptr->anim_index) {
                mtx_index = i_ptr->anim_index;
                mlMtxSet(animMtxList_get(mtx_list, mtx_index));
            }

            src_coord[0] = i_ptr->coord[0];
            src_coord[1] = i_ptr->coord[1];
            src_coord[2] = i_ptr->coord[2];
            mlMtx_apply_vec3f(dst_coord, src_coord);

            for (j = 0; j < i_ptr->vtx_count; j++) {
                vertex_index = i_ptr->vtx_list[j] * 3;
                dst_pos[vertex_index++] = dst_coord[0];
                dst_pos[vertex_index++] = dst_coord[1];
                dst_pos[vertex_index] = dst_coord[2];
            }

            i_ptr = (BKModelUnk28 *)((s16 *)(i_ptr + 1) + (i_ptr->vtx_count - 1));
        }
    }

    // Compute velocities if applicable.
    // To apply, the frame index stored on the extended marker data must be exactly the previous frame and the model ID must match.
    if (prev_skinning_data.frameCount == sSkinningFloatFrame - 1 && prev_skinning_data.modelId == cur_model_id) {
        const float *prev_pos = &sSkinningPosFloats[(sSkinningFloatFrame & 0x1) ^ 1][prev_skinning_data.floatStart];
        j = 0;
        for (i = 0; i < arg1->count; i++) {
            for (s32 k = 0; k < 3; k++) {
                dst_vel[j] = dst_pos[j] - prev_pos[j];
                j++;
            }
        }

        *vel = dst_vel;
    }

    // Return the vertex float segments that should be used.
    *pos = dst_pos;

    return TRUE;
}


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

    // @recomp Record the frustum check result, but ignore it during this function to disable frustum culling.
    // It will get set before this function returns. 
    cur_model_would_have_been_culled_in_demo = !((D_80383704) ? viewport_func_8024DB50(object_position, spD0*scale) : 1);
    D_80370990 = TRUE;

    // @recomp Force the frustum check to be true if the game isn't in demo playback mode.
    if (!recomp_in_demo_playback_game_mode()) {
        cur_model_would_have_been_culled_in_demo = FALSE;
    }

    if(D_80370990 == 0){
        modelRender_reset();
        // @recomp Clear the flag indicating that the model would have been culled before returning.
        cur_model_would_have_been_culled_in_demo = FALSE;
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
    gSPSegment((*gfx)++, 0x01, &modelRendervertexList->vtx_18);
    gSPSegment((*gfx)++, 0x02, &modelRenderTextureList->tex_8[modelRenderTextureList->cnt_4]);

    //segments 11 to 15 contain animated textures
    if(modelRenderAnimatedTexturesCacheId){
        int i_segment;
        s32 texture_offset;
        
        for(i_segment = 0; i_segment < 4; i_segment++){
            if(AnimTextureListCache_tryGetTextureOffset(modelRenderAnimatedTexturesCacheId, i_segment, &texture_offset)) {
                // @recomp Remove unnecessary usage of osVirtualToPhysical to allow extended addresses.
                gSPSegment((*gfx)++, 15 - i_segment, /*osVirtualToPhysical*/((u8*)&modelRenderTextureList->tex_8[modelRenderTextureList->cnt_4] + texture_offset));
            }
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

    // @recomp Use higher precision vertex buffer when the model requires CPU skinning.
    f32 *skinned_pos = NULL, *skinned_vel = NULL;
    cur_model_uses_ex_vertex = FALSE;

    if(model_bin->unk28 != NULL && D_8038371C != NULL){
        func_802E6BD0((u8*)modelRenderModelBin + modelRenderModelBin->unk28, modelRendervertexList, D_8038371C);
        
        // @recomp Do the skinning again on a high precision version of the vertex buffer. Force its usage for any subsequent display lists.
        recomp_apply_cpu_skinning((u8 *)modelRenderModelBin + modelRenderModelBin->unk28, modelRendervertexList, D_8038371C, NULL, &skinned_pos, &skinned_vel);
    }
    // @recomp Apply skinning using the floats applied by the map model.
    else if (sMapSkinningPosFloats != NULL) {
        recomp_apply_cpu_skinning(NULL, modelRendervertexList, NULL, sMapSkinningPosFloats, &skinned_pos, &skinned_vel);
    }

    if (skinned_pos != NULL) {
        gEXSetVertexSegment((*gfx)++, G_EX_VERTEX_POSITION, G_EX_ENABLED, skinned_pos, &modelRendervertexList->vtx_18);

        if (skinned_vel != NULL) {
            gEXSetVertexSegment((*gfx)++, G_EX_VERTEX_VELOCITY, G_EX_ENABLED, skinned_vel, &modelRendervertexList->vtx_18);
        }

        cur_model_uses_ex_vertex = TRUE;
    }

    mlMtxIdent();
    if(D_80383758.unk18){
        func_80252AF0(D_80383758.unk1C, object_position, rotation, scale, arg5);
    }
    else{
        func_80252AF0(modelRenderCameraPosition, object_position, rotation, scale, arg5);
    }

    // @recomp Patched to provide a spot to apply a non-uniform scale.
    if (has_additional_model_scale) {
        mlMtxScale_xyz(additional_model_scale_x, additional_model_scale_y, additional_model_scale_z);
        has_additional_model_scale = FALSE;
    }

    if(D_803837B0.unk0){
        mlMtxRotatePYR(D_803837B0.unk4[0], D_803837B0.unk4[1], D_803837B0.unk4[2]);
    }
    mlMtxGet(&D_80383BF8);

    mlMtxApply(*mtx);
    gSPMatrix((*gfx)++, (*mtx)++, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    
    // @recomp Create a matrix group if a transform id is set.
    bool pushed_matrix_group = FALSE;
    if (skip_all_interpolation || cur_drawn_model_skip_interpolation) {
        // @recomp Skip interpolation if all interpolation is currently skipped or the transform was specified to be skipped.
        gEXMatrixGroupSkipAll((*gfx)++, cur_drawn_model_transform_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
        pushed_matrix_group = TRUE;
    }
    else if (cur_drawn_model_is_map && !cur_model_uses_ex_vertex) {
        gEXMatrixGroupDecomposedVerts((*gfx)++, cur_drawn_model_transform_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
        pushed_matrix_group = TRUE;
    }
    else if (cur_drawn_model_transform_id != 0) {
        gEXMatrixGroupDecomposedNormalTcs((*gfx)++, cur_drawn_model_transform_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
        pushed_matrix_group = TRUE;
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

    // @recomp Disable frustum checks before processing bones.
    set_frustum_checks_enabled(FALSE);

    // @recomp Reset the transform ID offset and the bones flag.
    cur_model_transform_id_offset = 0;
    cur_model_uses_bones = FALSE;

    func_80339124(gfx, mtx, (BKGeoList *)((u8 *)model_bin + model_bin->geo_list_offset_4));
    
    // @recomp Re-enable frustum checks after processing bones.
    set_frustum_checks_enabled(TRUE);

    gSPPopMatrix((*gfx)++, G_MTX_MODELVIEW);

    // @recomp Pop the matrix group if a transform id is set.
    if (pushed_matrix_group) {
        gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
    }

    // @recomp Clear use of high precision vertex buffers.
    if (cur_model_uses_ex_vertex) {
        gEXSetVertexSegment((*gfx)++, G_EX_VERTEX_POSITION, G_EX_DISABLED, 0, 0);
        gEXSetVertexSegment((*gfx)++, G_EX_VERTEX_VELOCITY, G_EX_DISABLED, 0, 0);
    }

    if(modelRenderCallback.post_method != NULL){
        modelRenderCallback.post_method(modelRenderCallback.post_arg);
    }

    if(D_803837C8.model_id){
        assetCache_free(model_bin);
    }

    modelRender_reset();
    
    // @recomp Save the actual frustum culling result before returning.
    D_80370990 = !cur_model_would_have_been_culled_in_demo;
    // @recomp Clear the flag indicating that the model would have been culled before returning.
    cur_model_would_have_been_culled_in_demo = FALSE;
    return model_bin;
}
