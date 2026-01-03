#include "patches.h"
#include "transform_ids.h"
#include "functions.h"
#include "graphics.h"

#define ANIMATED_PILLARBOX_SCALE 0

extern enum map_e map_get(void);

#if ANIMATED_PILLARBOX_SCALE
f32 pillarbox_scale = 0.0f;
#else
f32 pillarbox_scale = 1.0f;
#endif

bool pillarbox_active() {
    switch (map_get()) {
    case MAP_1E_CS_START_NINTENDO:
    case MAP_1F_CS_START_RAREWARE:
    case MAP_20_CS_END_NOT_100:
    case MAP_7B_CS_INTRO_GL_DINGPOT_1:
    case MAP_7C_CS_INTRO_BANJOS_HOUSE_1:
    case MAP_7D_CS_SPIRAL_MOUNTAIN_1:
    case MAP_7E_CS_SPIRAL_MOUNTAIN_2:
    case MAP_81_CS_INTRO_GL_DINGPOT_2:
    case MAP_82_CS_ENTERING_GL_MACHINE_ROOM:
    case MAP_83_CS_GAME_OVER_MACHINE_ROOM:
    case MAP_84_CS_UNUSED_MACHINE_ROOM:
    case MAP_85_CS_SPIRAL_MOUNTAIN_3:
    case MAP_86_CS_SPIRAL_MOUNTAIN_4:
    case MAP_87_CS_SPIRAL_MOUNTAIN_5:
    case MAP_88_CS_SPIRAL_MOUNTAIN_6:
    case MAP_89_CS_INTRO_BANJOS_HOUSE_2:
    case MAP_8A_CS_INTRO_BANJOS_HOUSE_3:
    case MAP_94_CS_INTRO_SPIRAL_7:
    case MAP_95_CS_END_ALL_100:
    case MAP_96_CS_END_BEACH_1:
    case MAP_97_CS_END_BEACH_2:
    case MAP_98_CS_END_SPIRAL_MOUNTAIN_1:
    case MAP_99_CS_END_SPIRAL_MOUNTAIN_2:
        return TRUE;
    default:
        return FALSE;
    }
}

void pillarbox_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx) {
#if ANIMATED_PILLARBOX_SCALE
    const f32 pillarbox_scale_speed = 1.0f * time_getDelta();
    if (pillarbox_active()) {
        pillarbox_scale = MIN(pillarbox_scale + pillarbox_scale_speed, 1.0f);
    }
    else {
        pillarbox_scale = MAX(pillarbox_scale - pillarbox_scale_speed, 0.0f);
    }
#else
    if (!pillarbox_active()) {
        return;
    }
#endif

    float original_aspect = (float)DEFAULT_FRAMEBUFFER_WIDTH / DEFAULT_FRAMEBUFFER_HEIGHT;
    float cur_aspect = recomp_get_target_aspect_ratio(original_aspect);
    float target_aspect = MIN(recomp_get_cutscene_aspect_ratio(), cur_aspect);
    float target_width = DEFAULT_FRAMEBUFFER_HEIGHT * target_aspect;
    float wide_width = DEFAULT_FRAMEBUFFER_HEIGHT * cur_aspect;
    float pillar_width = wide_width - target_width * pillarbox_scale;
    if (pillar_width >= 0.0f) {
        u32 prev_ortho_id = cur_ortho_projection_transform_id;
        cur_ortho_projection_transform_id = PROJECTION_PILLARBOX_TRANSFORM_ID;
        viewport_setRenderViewportAndOrthoMatrix(gfx, mtx);
        
        Vtx *verts = *vtx;
        for (s32 ix = -1; ix < 2; ix += 2) {
            for (s32 iy = 0; iy < 2; iy++) {
                (*vtx)->v.ob[0] = ix * pillar_width * 2;
                (*vtx)->v.ob[1] = iy * DEFAULT_FRAMEBUFFER_HEIGHT * 4 - DEFAULT_FRAMEBUFFER_HEIGHT * 2;
                (*vtx)->v.ob[2] = -0x14;
                (*vtx)++;
            }
        }
        
        gEXPushOtherMode((*gfx)++);
        gEXPushCombineMode((*gfx)++);
        gEXPushGeometryMode((*gfx)++);
        gSPClearGeometryMode((*gfx)++, G_CULL_BOTH);
        gDPSetRenderMode((*gfx)++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
        gDPSetCycleType((*gfx)++, G_CYC_1CYCLE);
        gDPSetCombineLERP((*gfx)++, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

        u32 id = PILLARBOX_RECTANGLE_TRANSFORM_ID_START;
        for (s32 ix = -1; ix < 2; ix += 2) {
            guTranslate(*mtx, ix * wide_width * 2, 0.0f, 0.0f);
            gSPMatrix((*gfx)++, OS_K0_TO_PHYSICAL(*mtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            (*mtx)++;

            gEXMatrixGroupSimpleVerts((*gfx)++, id++, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
            gSPVertex((*gfx)++, verts, 4, 0);
            gSP1Quadrangle((*gfx)++, 0, 1, 3, 2, 0);
            gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
        }

        gEXPopOtherMode((*gfx)++);
        gEXPopCombineMode((*gfx)++);
        gEXPopGeometryMode((*gfx)++);

        cur_ortho_projection_transform_id = prev_ortho_id;
    }
}