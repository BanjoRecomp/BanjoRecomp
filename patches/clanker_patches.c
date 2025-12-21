#include "patches.h"
#include "transform_ids.h"

extern void CC_func_80388760(Gfx **gfx, Mtx **mtx, Vtx **vtx);

// @recomp: Patched to assign an ID to Clanker.
RECOMP_PATCH void __overlay_cc_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    // @recomp Set the current transform ID to Clanker.
    u32 prev_transform_id = cur_drawn_model_transform_id;
    cur_drawn_model_transform_id = CLANKER_TRANSFORM_ID_START;

    CC_func_80388760(gfx, mtx, vtx);

    // @recomp Reset the current transform ID.
    cur_drawn_model_transform_id = prev_transform_id;
}