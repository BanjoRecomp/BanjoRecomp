#include "patches.h"
#include "transform_ids.h"
#include "functions.h"
#include "core2/anctrl.h"
#include "core2/modelRender.h"

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
        cur_drawn_model_transform_id = MAP_MODEL_OPA_TRANSFORM_ID_START;

        modelRender_draw(gfx, mtx, NULL, NULL, mapModel.description->scale, NULL, mapModel.model_bin_opa);
        
        // @recomp Clear the current model transform id after drawing.
        cur_drawn_model_transform_id = 0;

        if (!mapModel_has_xlu_bin()) {
            func_802F7BC0(gfx, mtx, vtx);
        }
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
        cur_drawn_model_transform_id = MAP_MODEL_XLU_TRANSFORM_ID_START;

        modelRender_draw(gfx, mtx, NULL, NULL, mapModel.description->scale, NULL, mapModel.model_bin_xlu);
        
        // @recomp Clear the current model transform id after drawing.
        cur_drawn_model_transform_id = 0;

        func_802F7BC0(gfx, mtx, vtx);
    }
}
