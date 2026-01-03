#include "patches.h"
#include "misc_funcs.h"
#include "functions.h"
#include "bk_api.h"
#include "object_extension_funcs.h"
#include "note_saving.h"

extern ActorMarker *D_8036E7C8;
extern u8 D_80383428[0x1C];

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
    void *unk0;
    void *unk4;
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

BKGfxList *model_getDisplayList(BKModelBin *arg0);


#define INTRO_OPA_DL_LENGTH 97
#define INTRO_OPA_DL_GRASS_PATCH_INDEX 63
#define INTRO_OPA_DL_WALL_PATCH_INDEX 84
#define INTRO_OPA_DL_HASH 0xA912614C535FA0BBULL

Vtx intro_grass_extension_verts[3] = {
	{{ {-1262, 20, 1040}, 0, {230, 538}, {51, 190, 133, 255} }},
	{{ {-10, 20, 5107}, 0, {3124, -10067}, {0, 13, 169, 255} }},
	{{ {-10, 20, 1372}, 0, {3376, -195}, {51, 190, 133, 255} }},
};

Gfx intro_grass_extension_dl[] = {
    // Copy of the replaced command.
    gsSP1Triangle(31, 28, 29, 0),
    // New commands.
	gsSPVertex(intro_grass_extension_verts + 0, 3, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSPEndDisplayList(),
};

Vtx intro_wall_extension_verts[4] = {
	{{ {-1262, 20, 1040}, 0, {-15, 49}, {51, 190, 133, 255} }},
	{{ {-1262, 1004, 1040}, 0, {-44, 1966}, {255, 255, 255, 255} }},
	{{ {189, 1004, 4315}, 0, {-7400, 1817}, {255, 255, 255, 255} }},
	{{ {189, 20, 4315}, 0, {-7371, -100}, {51, 190, 133, 255} }},
};

Gfx intro_wall_extension_dl[] = {
    // Copy of the replaced command.
    gsSP1Triangle(22, 24, 21, 0),
    // New commands.
	gsSPVertex(intro_wall_extension_verts + 0, 4, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
	gsSPEndDisplayList(),
};

void hotpatch_intro_opa_map_model(BKModelBin* model_bin) {
    BKGfxList *gfx_list = model_getDisplayList(model_bin);
    Gfx* dl = &gfx_list->list[0];

    // Hash the displaylist of the model to make sure it's unmodified. This will prevent the hotpatch from
    // affecting mods.
    u64 hash = recomp_xxh3(dl, INTRO_OPA_DL_LENGTH * sizeof(Gfx));
    if (hash != INTRO_OPA_DL_HASH) {
        return;
    }

    // Patch a call to the new displaylist after the grass material.
    gSPDisplayList(&dl[INTRO_OPA_DL_GRASS_PATCH_INDEX], intro_grass_extension_dl);

    // Patch a call to the new displaylist after the wall material.
    gSPDisplayList(&dl[INTRO_OPA_DL_WALL_PATCH_INDEX], intro_wall_extension_dl);
}

// @recomp Patched to act as a point to run code when a new map is loaded.
// This includes:
//   * Resetting all extended marker data and skip interpolation for the next frame.
//   * Hotpatching the map model for the title cutscene to fix ultrawide effects. 
//   * Resetting the spawned static note count.
RECOMP_PATCH void func_803329AC(void){
    s32 i;
    
    D_8036E7C8 = (ActorMarker *)malloc(0xE0*sizeof(ActorMarker));

    for( i = 0; i < 0x1C; i++){
        D_80383428[i] = 0;
    }
       
    for(i =0; i<0xE0; i++){
        D_8036E7C8[i].unk5C = 0;
    }

    // @recomp Run any new code on map load.

    // @recomp If the current map's model is ASSET_149D_MODEL_CS_START_NINTENDO_OPA,
    // hotpatch it to fill in some regions for widescreen.
    if (mapModel.description->opa_model_id == ASSET_149D_MODEL_CS_START_NINTENDO_OPA) {
        hotpatch_intro_opa_map_model(mapModel.model_bin_opa);
    }

    // @recomp Reset all actor data and skip interpolation for the next frame.
    // Interpolation is skipped as the next frame will potentially reuse IDs from the previous frame,
    // as the marker ID tracking gets reset here.
    recomp_clear_all_object_data(EXTENSION_TYPE_MARKER);
    set_all_interpolation_skipped(TRUE);

    // @recomp Run note saving map load code.
    note_saving_on_map_load();
}
