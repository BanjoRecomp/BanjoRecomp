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

extern s32 func_8033A170(void);

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

    modelRender_preDraw((GenFunction_1)actor_predrawMethod, (s32)this);
    modelRender_postDraw((GenFunction_1)actor_postdrawMethod, (s32)marker);
    modelRender_draw(gfx, mtx, this->position, sp3C, this->scale, (this->unk104 != NULL) ? D_8036E580 : NULL, func_803257B4(marker));
    // @recomp Re-enable interpolation
    cur_drawn_model_skip_interpolation = FALSE;
    return this;
}
