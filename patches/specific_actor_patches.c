#include "patches.h"
#include "functions.h"
#include "transform_ids.h"
#include "bk_api.h"

#define IDS_PER_BEE 8

typedef struct {
    f32 unk0[3];
    f32 unkC[3];
    f32 unk18[3];
    f32 unk24[3];
}Struct_core2_47BD0_0;

typedef struct {
    s32 unk0;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    Struct_core2_47BD0_0 *unk8;
    f32 unkC[3];
    f32 unk18;
    f32 unk1C;
    BKModelBin *unk20;
    s32 unk24;
}ActorLocal_core2_47BD0;

extern struct0* D_8037C200;

extern f32 func_8028E82C(void);
extern void func_8028E84C(f32 arg0[3]);
extern bool func_8028F070(void);
extern bool func_8028F150(void);
extern bool func_8028F2DC(void);
extern void func_802CA790(Actor* this);
extern s32 func_8033A170(void);
extern enum map_e map_get(void);
extern enum bswatergroup_e player_getWaterState(void);

extern u32 get_cutscene_counter(void);

// @recomp Patched to give the bees in the bee swarm individual IDs. The bees can show interpolation glitches otherwise, as they all share one matrix
// group and can get culled individually based on distance. This is easily reproduceable by walking into a beehive with bees from a far away distance.
RECOMP_PATCH Actor *chBeeSwarm_draw(ActorMarker *marker, Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    Actor *this;
    ActorLocal_core2_47BD0 *local;
    BKModelBin *phi_fp;
    s32 phi_s2;
    f32 sp8C[3];
    f32 sp80[3];
    Struct_core2_47BD0_0 *phi_s0;

    this = marker_getActor(marker);
    local = (ActorLocal_core2_47BD0 *)&this->local;
    phi_fp = marker_loadModelBin(marker);
    for (phi_s2 = 0, phi_s0 = local->unk8; phi_s2 < local->unk0; phi_s2++) {
        sp80[0] = 0.0f;
        sp80[1] = phi_s0->unk24[1] - 90.0f;
        sp80[2] = 0.0f;

        sp8C[0] = this->position[0] + phi_s0->unk0[0];
        sp8C[1] = this->position[1] + phi_s0->unk0[1];
        sp8C[2] = this->position[2] + phi_s0->unk0[2];

        // @recomp Set the model transform ID.
        s32 cur_drawn_marker_spawn_index = bkrecomp_get_marker_spawn_index(marker);
        u32 transform_id = MARKER_TRANSFORM_ID_START + cur_drawn_marker_spawn_index * MARKER_TRANSFORM_ID_COUNT + phi_s2 * IDS_PER_BEE;
        u32 prev_transform_id = cur_drawn_model_transform_id;
        cur_drawn_model_transform_id = transform_id;

        modelRender_setDepthMode(MODEL_RENDER_DEPTH_COMPARE);
        modelRender_setAlpha(0xFF);
        modelRender_draw(gfx, mtx, sp8C, sp80, 0.25f, NULL, phi_fp);

        local->unk5 |= func_8033A170();
        if (phi_s2 < 10) {
            sp8C[1] = local->unk18 + 6.0f;
            modelRender_setAlpha(0xC0);
            modelRender_setDepthMode(MODEL_RENDER_DEPTH_COMPARE);
            modelRender_draw(gfx, mtx, sp8C, sp80, 0.1f, NULL, local->unk20);
            local->unk5 |= func_8033A170();
        }
        phi_s0++;

        // @recomp Reset the model transform ID.
        cur_drawn_model_transform_id = prev_transform_id;
    }
    return this;
}

extern f32 D_8036E580[3];
extern void actor_postdrawMethod(ActorMarker *);
extern void actor_predrawMethod(Actor *);
extern BKModelBin *func_803257B4(ActorMarker *marker);

// @recomp Patch to skip interpolation on the Christmas tree while the lights
// turn on to prevent visual glitches.
RECOMP_PATCH Actor *actor_draw(ActorMarker *marker, Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    f32 sp3C[3];
    Actor *this;

    this = marker_getActorAndRotation(marker, sp3C);

    // @recomp Check for the xmas tree model
    if (marker->modelId == ASSET_488_MODEL_XMAS_TREE) {
        // @recomp Check that the lights are currently turning on / flickering
        if (this->state == 2 || this->state == 3) {
            // @recomp Skip interpolation
            cur_drawn_model_skip_interpolation = TRUE;
        }
    }
    // @recomp Check for the hut model.
    else if (marker->modelId == ASSET_7D7_MODEL_MM_HUT) {
        // @recomp Check if the hut is destroyed (HUT_STATE_2_DESTROYED).
        if (this->state == 2) {
            cur_drawn_model_skip_interpolation = TRUE;
        }
    }


    modelRender_preDraw((GenFunction_1)actor_predrawMethod, (s32)this);
    modelRender_postDraw((GenFunction_1)actor_postdrawMethod, (s32)marker);
    modelRender_draw(gfx, mtx, this->position, sp3C, this->scale, (this->unk104 != NULL) ? D_8036E580 : NULL, func_803257B4(marker));
    // @recomp Re-enable interpolation
    cur_drawn_model_skip_interpolation = FALSE;
    return this;
}

extern void func_802E0710(Actor *this);

// @recomp Make sure the model being skipped are the bulls during the intro cutscene, and only during the
// first few seconds of the cutscene while the camera is panning.
bool skip_drawing_intro_bulls(u32 model_id) {
    return 
        (map_get() == MAP_1E_CS_START_NINTENDO) && 
        (model_id == ASSET_353_MODEL_BIGBUTT || model_id == ASSET_354_MODEL_BULL_INTRO) &&
        get_cutscene_counter() < 180;
}

// @recomp Patched to skip drawing the bull actors on the intro cutscene's start.
RECOMP_PATCH Actor *func_802E0738(ActorMarker *marker, Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    f32 sp34[3];
    Actor *this;

    this = marker_getActorAndRotation(marker, &sp34);
    
    // @recomp Check the condition for not drawing this actor if it's one of the bulls during the intro.
    if (skip_drawing_intro_bulls(marker->modelId)) {
        return this;
    }

    modelRender_preDraw((GenFunction_1)func_802E0710, (s32)this);
    modelRender_postDraw((GenFunction_1)actor_postdrawMethod, (s32)marker);
    modelRender_draw(gfx, mtx, this->position, sp34, this->scale, NULL, marker_loadModelBin(marker));
    return this;
}

// @recomp Patched to skip interpolating the player shadow on large surface changes.
RECOMP_PATCH Actor* func_802CA7BC(ActorMarker* marker, Gfx** gfx, Mtx** mtx, Vtx** vtx) {
    Actor* this;
    f32 sp60;
    f32 sp54[3];
    f32 rotation[3];
    f32 sp44;
    f32 sp40;
    f32 sp34[3];

    sp60 = (player_getWaterState() == BSWATERGROUP_2_UNDERWATER) ? 8.0f : 4.0f;
    this = marker_getActor(marker);
    if (!func_8028F070()
        || !func_8028F150()
        || !func_8028F2DC()
        ) {
        return this;
    }

    player_getPosition(sp54);
    sp40 = func_8028E82C();
    func_8028E84C(sp34);
    this->position_x = sp54[0];
    this->position_y = sp40 + sp60;
    this->position_z = sp54[2];

    func_80258108(sp34, &this->yaw, &this->pitch);

    rotation[0] = this->pitch;
    rotation[1] = this->yaw;
    rotation[2] = this->roll;
    sp44 = ml_map_f(sp54[1] - sp40, 0.0f, 300.0f, 0.43f, 0.28f);
    
    // @recomp Check for the player shadow model.
    if (marker->modelId == ASSET_3BF_MODEL_PLAYER_SHADOW) {
        // @recomp Skip interpolation if the player's projected position on the floor has changed too quickly. Only do this check when the triangle has changed.
        static f32 previousPosY = 0.0f;
        static f32 previousNormY = 1.0f;
        static s16 previousTri[3] = { 0, 0, 0 };
        s16* currentTri = &D_8037C200->unk4.unk0[0];
        if (currentTri[0] != previousTri[0] || currentTri[1] != previousTri[1] || currentTri[2] != previousTri[2]) {
            f32 projectedHeightDistance = previousNormY * mlAbsF(this->position_y - previousPosY);
            const f32 skipHeightThreshold = 20.0f;
            cur_drawn_model_skip_interpolation = projectedHeightDistance >= skipHeightThreshold;

            previousNormY = D_8037C200->normY;
            previousTri[0] = currentTri[0];
            previousTri[1] = currentTri[1];
            previousTri[2] = currentTri[2];
        }

        previousPosY = this->position_y;
    }

    modelRender_preDraw((GenFunction_1)func_802CA790, (s32)this);
    modelRender_draw(gfx, mtx, this->position, rotation, sp44, NULL, marker_loadModelBin(marker));

    // @recomp Re-enable interpolation
    cur_drawn_model_skip_interpolation = FALSE;

    return this;
}
