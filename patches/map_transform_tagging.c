#include "patches.h"
#include "transform_ids.h"
#include "functions.h"
#include "core2/anctrl.h"
#include "core2/modelRender.h"

typedef struct struct_1D_s {
    BKModel *model;
    s16 unk4;
    u8 xform_id;
    // u8 pad7[1];
    Struct70s unk8;//union of subtypes
}struct1Ds;

extern struct {
    u8 unk0;
    vector(struct1Ds) *unk4;
} D_80386140;

typedef struct struct_1E_s {
    void (*unk0)(void *arg0, s32 arg1, s32 arg2, s32 arg3);
    void (*unk4)(void *arg0, s32 arg1, s32 arg2);
    void (*unk8)(void *arg0);
}struct1Es;

extern struct1Es D_80372030[];

s32 func_80320708(void);
s32 levelSpecificFlags_validateCRC2(void);
s32 dummy_func_80320248(void);
enum asset_e mapModel_getOpaModelId(void);
enum map_e map_get(void);
s32 mapSpecificFlags_get(s32 i);
s32 levelSpecificFlags_get(s32 i);
u32 jiggyscore_isCollected(enum jiggy_e);
enum bsgroup_e player_movementGroup(void);
bool jigsawPicture_isJigsawPictureComplete(s32 world);
bool fileProgressFlag_get(enum file_progress_e index);
void modelRender_setAnimatedTexturesCacheId(s32 arg0);
bool mapModel_has_xlu_bin(void);
void func_802F7BC0(Gfx **, Mtx **, Vtx **);
void func_8034E660(s32 arg0, BKVtxRef *src, Vtx *dst, s32 arg3);
void BKModel_transformMesh(BKModel *model, s32 mesh_id, void (*fn)(s32, BKVtxRef *, Vtx *, s32), s32 arg3);

void recomp_setup_map_skinning(int map_model_id, float *pos_floats);
void recomp_clear_map_skinning();

typedef struct {
    s16 map_id; //enum map_e
    s16 opa_model_id; //enum asset_e level_model_id
    s16 xlu_model_id; //enum asset_e level2_model_id
    s16 unk6[3]; // min bounds (for cubes?)
    s16 unkC[3]; // max bounds (for cubes?)
    // u8 pad12[0x2];
    f32 scale;
}MapModelDescription;

extern struct {
    s32 unk0; // Changed from void* in the decomp
    s32 unk4; // Changed from void* in the decomp
    BKCollisionList *collision_opa;
    BKCollisionList *collision_xlu;
    BKModel *model_opa;
    BKModel *model_xlu;
    BKModelBin *model_bin_opa;
    BKModelBin *model_bin_xlu;
    s32 unk20;
    struct5Bs *unk24;
    MapModelDescription *description;
    u8 env_red;
    u8 env_green;
    u8 env_blue;
    f32 scale;
}mapModel;

// @recomp Patched to set the transform ID when drawing the map's opaque model.
RECOMP_PATCH void mapModel_opa_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    s32 temp_a0;

    if (func_80320708() && levelSpecificFlags_validateCRC2() && dummy_func_80320248()) {
        if (mapModel_getOpaModelId() == ASSET_14CF_MODEL_SM_SPIRAL_MOUNTAIN_OPA) {
            func_8033A45C(1, 0);
            func_8033A45C(2, 1);
        }
        switch (map_get()) {                          /* irregular */
        case MAP_88_CS_SPIRAL_MOUNTAIN_6:
            if (mapSpecificFlags_get(0) != 0) {
                func_8033A45C(1, 1);
                func_8033A45C(2, 0);
            }
            break;

        case MAP_12_GV_GOBIS_VALLEY:
            func_8033A45C(1, levelSpecificFlags_get(LEVEL_FLAG_6_GV_UNKNOWN) ? 1 : 0);
            func_8033A45C(2, jiggyscore_isCollected(JIGGY_42_GV_WATER_PYRAMID) ? 0 : 1);
            func_8033A45C(5, jiggyscore_isCollected(JIGGY_42_GV_WATER_PYRAMID) ? 1 : 0);
            break;

        case MAP_14_GV_SANDYBUTTS_MAZE:
            func_8033A45C(5, (player_movementGroup() == BSGROUP_4_LOOK) ? 1 : 0);
            break;

        case MAP_E_MM_MUMBOS_SKULL:
            func_8033A45C(1, 1);
            func_8033A45C(5, 1);
            break;

        case MAP_47_BGS_MUMBOS_SKULL:
            func_8033A45C(1, 2);
            func_8033A45C(5, 2);
            break;

        case MAP_48_FP_MUMBOS_SKULL:
            func_8033A45C(1, 3);
            func_8033A45C(5, 3);
            break;

        case MAP_30_MMM_MUMBOS_SKULL:
            func_8033A45C(1, 4);
            func_8033A45C(5, 4);
            break;
            
        case MAP_4A_CCW_SPRING_MUMBOS_SKULL:
            func_8033A45C(1, 5);
            func_8033A45C(5, 5);
            break;
        case MAP_4B_CCW_SUMMER_MUMBOS_SKULL:
            func_8033A45C(1, 6);
            func_8033A45C(5, 6);
            break;
        case MAP_4C_CCW_AUTUMN_MUMBOS_SKULL:
            func_8033A45C(1, 7);
            func_8033A45C(5, 7);
            break;
        case MAP_4D_CCW_WINTER_MUMBOS_SKULL:
            func_8033A45C(1, 8);
            func_8033A45C(5, 8);
            break;
        case MAP_5E_CCW_SPRING_NABNUTS_HOUSE:
        case MAP_5F_CCW_SUMMER_NABNUTS_HOUSE:
        case MAP_60_CCW_AUTUMN_NABNUTS_HOUSE:
            func_8033A45C(1, 1);
            func_8033A45C(2, 0);
            break;
        case MAP_61_CCW_WINTER_NABNUTS_HOUSE:
            func_8033A45C(1, 0);
            func_8033A45C(2, 1);
            break;
        case MAP_1D_MMM_CELLAR:
            func_8033A45C(1, actorArray_findActorFromActorId(0x191) ?  0 : 1);
            break;
        case MAP_7C_CS_INTRO_BANJOS_HOUSE_1:
        case MAP_89_CS_INTRO_BANJOS_HOUSE_2:
        case MAP_8A_CS_INTRO_BANJOS_HOUSE_3:
        case MAP_8C_SM_BANJOS_HOUSE:
        case MAP_91_FILE_SELECT:
            func_8033A45C(5, 1);
            break;
        case MAP_7B_CS_INTRO_GL_DINGPOT_1:
        case MAP_81_CS_INTRO_GL_DINGPOT_2:
            func_8033A45C(4, 0);
            func_8033A45C(5, 0);
            func_8033A45C(6, 0);
            break;
        case MAP_82_CS_ENTERING_GL_MACHINE_ROOM:
        case MAP_83_CS_GAME_OVER_MACHINE_ROOM:
        case MAP_84_CS_UNUSED_MACHINE_ROOM:
            func_8033A45C(4, 1);
            func_8033A45C(5, 1);
            func_8033A45C(6, 1);
            break;
        case MAP_93_GL_DINGPOT:
            func_8033A45C(4, 1);
            func_8033A45C(5, 1);
            func_8033A45C(6, (jigsawPicture_isJigsawPictureComplete(0xA) || fileProgressFlag_get(FILEPROG_E2_DOOR_OF_GRUNTY_OPEN) || volatileFlag_get(VOLATILE_FLAG_C1_IN_FINAL_CHARACTER_PARADE)));
            break;
        }
        func_8033A450(mapModel.unk24);
        modelRender_setDepthMode(MODEL_RENDER_DEPTH_FULL);
        temp_a0 = mapModel.unk0;
        if (temp_a0 != 0) {
            modelRender_setAnimatedTexturesCacheId(temp_a0);
        }
        modelRender_setEnvColor(mapModel.env_red, mapModel.env_green, mapModel.env_blue, 0xFF);
        
        // @recomp Set the map opaque model transform id.
        cur_drawn_model_is_map = TRUE;
        cur_drawn_model_transform_id = MAP_MODEL_OPA_TRANSFORM_ID_START;

        modelRender_draw(gfx, mtx, NULL, NULL, mapModel.description->scale, NULL, mapModel.model_bin_opa);
        
        // @recomp Clear the current model transform id after drawing.
        cur_drawn_model_is_map = FALSE;
        cur_drawn_model_transform_id = 0;

        if (!mapModel_has_xlu_bin()) {
            func_802F7BC0(gfx, mtx, vtx);
        }
    }
}

// @recomp Reserve a static amount of memory to be used for storing the current modifications to the map model with higher precision floats.
#define MAP_MODEL_XLU_VERTEX_COUNT_MAX 0x400
f32 map_model_xlu_pos_floats[MAP_MODEL_XLU_VERTEX_COUNT_MAX * 3];
u32 map_model_xlu_pos_floats_count = 0;

void recomp_reset_map_model_skinning() {
    map_model_xlu_pos_floats_count = 0;
}

// @recomp Patched to store the moved water vertices with higher precision floats.
RECOMP_PATCH void func_8034E8E4(Struct73s *arg0, BKModel *arg1, s32 arg2) {
    f32 sp3C;
    f32 sp38;
    f32 sp30[2];
    f32 sin;
    f32 cos;
    f32 sp2C;
    f32 sp28;

    sp38 = time_getDelta();
    arg0->unk4 += sp38;
    cos = cosf(arg0->unk4 * 0.2 * BAD_PI);
    sin = sinf(arg0->unk4 * 0.08 * BAD_PI);
    sp30[0] = (sin * 100.0f + 150.0f * cos) * 0.8;


    cos = sinf(arg0->unk4 * 0.5 * BAD_PI);
    sin = cosf(arg0->unk4 * 0.22 * BAD_PI);
    sp30[1] = (sin * 100.0f + 50.0f * cos) * 0.8;

    arg0->d_tc[0] = (sp30[0] >= 0.0) ? sp30[0] + 0.5 : sp30[0] - 0.5;
    arg0->d_tc[1] = (sp30[1] >= 0.0) ? sp30[1] + 0.5 : sp30[1] - 0.5;

    cos = cosf(arg0->unk4 * 0.5 * BAD_PI);
    sin = sinf(arg0->unk4 * 0.11 * BAD_PI);
    sp2C = sin * (arg0->unk8 * 0.25) + (arg0->unk8 * 0.75) * cos;

    if (arg0->unk14 < arg0->unk1C) {
        arg0->unk18 = arg0->unk14;
        arg0->unk14 += sp38;
        if (arg0->unk1C < arg0->unk14) {
            arg0->unk14 = arg0->unk1C;
        }
    }
    sp28 = ((arg0->unk14 < arg0->unk1C) ? arg0->unkC + ((arg0->unk14 / arg0->unk1C) * (arg0->unkE - arg0->unkC)) : arg0->unkE);
    sp28 += sp2C;
    arg0->dy = (sp28 >= 0.0) ? sp28 + 0.5 : sp28 - 0.5;
    BKModel_transformMesh(arg1, arg2, func_8034E660, (s32)arg0);

    // @recomp Don't use high precision floats if the model exceeds the bounds of the floats array, in case the model was modified.
    if (arg1->vtxList_4->count <= MAP_MODEL_XLU_VERTEX_COUNT_MAX) {
        // @recomp Make sure to copy all the vertices from the model to the higher precision floats at least once per frame.
        s32 i, j = 0;
        if (map_model_xlu_pos_floats_count < arg1->vtxList_4->count) {
            Vtx *vtx = vtxList_getVertices(arg1->vtxList_4);
            for (i = 0; i < arg1->vtxList_4->count; i++) {
                map_model_xlu_pos_floats[j++] = vtx->v.ob[0];
                map_model_xlu_pos_floats[j++] = vtx->v.ob[1];
                map_model_xlu_pos_floats[j++] = vtx->v.ob[2];
                vtx++;
            }

            map_model_xlu_pos_floats_count = arg1->vtxList_4->count;
        }

        // @recomp Run the logic of BKModel_transformMesh again with only the modification of the Y component as seen in func_8034E660.
        // The result is stored in the higher precision floats instead of the model binary itself. The original value of dy is used
        // before it's rounded to make the animation smoother.
        BKMesh *iMesh = (BKMesh *)(arg1 + 1);
        BKVtxRef *iVtx;
        BKVtxRef *start_vtx_ref;
        BKVtxRef *end_vtx_ref;
        for (i = 0; i < arg1->meshList_0->meshCount_0; i++) {
            if (arg2 == iMesh->uid_0) {
                start_vtx_ref = (BKVtxRef *)(iMesh + 1);
                end_vtx_ref = start_vtx_ref + iMesh->vtxCount_2;
                for (iVtx = start_vtx_ref; iVtx < end_vtx_ref; iVtx++) {
                    j = iVtx->unk10 * 3;
                    map_model_xlu_pos_floats[j + 0] = iVtx->v.v.ob[0];
                    map_model_xlu_pos_floats[j + 1] = iVtx->v.v.ob[1] + sp28;
                    map_model_xlu_pos_floats[j + 2] = iVtx->v.v.ob[2];
                }

                break;
            }

            iMesh = (BKMesh *)(((BKVtxRef *)(iMesh + 1)) + iMesh->vtxCount_2);
        };
    }
}

// @recomp Patched to set the transform ID when drawing the map's translucent model.
RECOMP_PATCH void mapModel_xlu_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    s32 temp_a0;

    if (mapModel.model_bin_xlu != NULL) {
        if (map_get() == MAP_1D_MMM_CELLAR) {
            func_8033A45C(1, (actorArray_findActorFromActorId(0x191) != NULL) ? 0 : 1);
        }
        modelRender_setDepthMode(MODEL_RENDER_DEPTH_COMPARE);
        temp_a0 = mapModel.unk4;
        if (temp_a0 != 0) {
            modelRender_setAnimatedTexturesCacheId(temp_a0);
        }
        modelRender_setEnvColor(mapModel.env_red, mapModel.env_green, mapModel.env_blue, 0xFF);
        
        // @recomp Set the map opaque model transform id.
        cur_drawn_model_is_map = TRUE;
        cur_drawn_model_transform_id = MAP_MODEL_XLU_TRANSFORM_ID_START;

        // @recomp Because func_8034E8E4 runs after this function, run through the vector and see if there's a model that uses
        // the vertex modification function that was patched. The address must be hardcoded as it's not possible to retrieve
        // the right function address inside a recompiled patch by referencing func_8034E8E4 instead.
        BKVertexList *vtxList = (BKVertexList *)((s32)mapModel.model_bin_xlu + mapModel.model_bin_xlu->vtx_list_offset_10);
        if (vtxList->count <= MAP_MODEL_XLU_VERTEX_COUNT_MAX) {
            struct1Ds *iPtr;
            struct1Ds *endPtr = vector_getEnd(D_80386140.unk4);
            for (iPtr = vector_getBegin(D_80386140.unk4); iPtr < endPtr; iPtr++) {
                if (D_80372030[iPtr->xform_id].unk4 == 0x8034E8E4) {
                    recomp_setup_map_skinning(mapModel.description->xlu_model_id, map_model_xlu_pos_floats);
                    break;
                }
            }
        }

        modelRender_draw(gfx, mtx, NULL, NULL, mapModel.description->scale, NULL, mapModel.model_bin_xlu);
        
        // @recomp Clear the current model transform id after drawing.
        cur_drawn_model_is_map = FALSE;
        cur_drawn_model_transform_id = 0;
        recomp_clear_map_skinning();

        func_802F7BC0(gfx, mtx, vtx);
    }
}
