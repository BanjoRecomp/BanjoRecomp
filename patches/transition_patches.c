#include "patches.h"
#include "graphics.h"
#include "transform_ids.h"
#include "core1/core1.h"
#include "gc/gctransition.h"
#include "gc/gcbound.h"

float gu_sqrtf(float val);
BKGfxList *model_getDisplayList(BKModelBin *arg0);
s32 getGameMode(void);
void scissorBox_setSmall(void);
s16 *func_8030C704(void);

typedef enum {
    TRANSITION_ID_1_BLACK_IN = 1,
    TRANSITION_ID_2_BLACK_OUT,
    TRANSITION_ID_3_CIRCLE_IN,
    TRANSITION_ID_4_CIRCLE_OUT,
    TRANSITION_ID_5_JIGGY_IN,
    TRANSITION_ID_6_JIGGY_OUT,
    TRANSITION_ID_7,
    TRANSITION_ID_8_WHITE_IN,
    TRANSITION_ID_9_WHITE_OUT,
    TRANSITION_ID_A_WITCH_HEAD_OUT,
    TRANSITION_ID_B_CIRCLE_IN_FAST,
    TRANSITION_ID_C_CIRCLE_OUT_FAST,
    TRANSITION_ID_D,
    TRANSITION_ID_E_CIRCLE_IN_SLOW,
    TRANSITION_ID_F_CIRCLE_OUT_SLOW,
    TRANSITION_ID_10_FALLING_PIECES_IN,
    TRANSITION_ID_11_FALLING_PIECES_OUT,
    TRANSITION_ID_12_CIRCLE_IN_EXTRA_SLOW,
    TRANSITION_ID_13_CIRCLE_OUT_EXTRA_SLOW,
    TRANSITION_ID_14_CIRCLE_IN_SUPER_SLOW,
    TRANSITION_ID_15_CIRCLE_OUT_SUPER_SLOW
} TransitionId;

typedef enum {
    TRANSITION_STATE_0_NONE,
    TRANSITION_STATE_1_LOADING,
    TRANSITION_STATE_2_BLACK_IN,
    TRANSITION_STATE_3_BLACK_OUT,
    TRANSITION_STATE_4_FADE_IN,
    TRANSITION_STATE_5_FADE_OUT,
    TRANSITION_STATE_6_LOADING_WHITE,
    TRANSITION_STATE_7_WHITE_IN,
    TRANSITION_STATE_8_WHITE_OUT
} TransitionState;

typedef struct {
    u8 uid;
    u8 state;
    u8 next_state;
    // u8 pad3[1];
    f32 duration;
    s32 model_index;
    s32 anim_index;
    f32 scale;
}TransitionInfo;

extern struct{
    s32 unk0; //times drawn?
    TransitionInfo *transistion_info;
    u8 state;
    //u8 pad9[3]; //not needed
    void * model_ptr; //asset_ptr
    f32 rotation;
    f32 timer;
    AnimCtrl *anctrl;
    s32 substate; //times update called?
} s_current_transition;

extern f32 D_8036C440;
extern f32 D_8036C444;

// Dedicated framebuffer to hold the frame that was rendered as the jiggy transition texture.
u16 jiggy_transition_framebuffer[DEFAULT_FRAMEBUFFER_WIDTH * DEFAULT_FRAMEBUFFER_HEIGHT];

Gfx load_jiggy_transition_texture[] = {
    // Load the entire framebuffer as a texture, as tmem limits can be bypassed in RT64 for framebuffer textures.
    gsDPLoadTextureTile(jiggy_transition_framebuffer, G_IM_FMT_RGBA, G_IM_SIZ_16b,
        DEFAULT_FRAMEBUFFER_WIDTH, DEFAULT_FRAMEBUFFER_HEIGHT,
        0, 0,
        DEFAULT_FRAMEBUFFER_WIDTH - 1, DEFAULT_FRAMEBUFFER_HEIGHT - 1,
        0,
        G_TX_CLAMP | G_TX_NOMIRROR, G_TX_CLAMP | G_TX_NOMIRROR,
        G_TX_NOMASK, G_TX_NOMASK,
        G_TX_NOLOD, G_TX_NOLOD
    ),
    gsSPEndDisplayList(),
};

const s32 fb_tiles_per_row = 10;
const s32 fb_tiles_per_column = 8;
const s32 fb_tile_size = 32;
const s32 fb_grid_width = fb_tile_size * fb_tiles_per_row;
const s32 fb_grid_height = fb_tile_size * fb_tiles_per_column;

// Input vertex coordinates for the corners of the jiggy transition effect.
const float left_x_input = -701.0f;
const float top_y_input = 701.0f;
const float right_x_input = 701.0f;
const float bottom_y_input = -701.0f;

// NDC output vertex coordinates for the corners of the jiggy transition effect, measured via renderdoc.
const float left_x_output   = -1.0941545714285714285714285714286f;
const float top_y_output    = 1.4791303428571428571428571428571f;
const float right_x_output  = 1.101004f;
const float bottom_y_output = -1.4883895428571428571428571428571;

const float output_per_input_x = (right_x_output - left_x_output) / (right_x_input - left_x_input);
const float output_per_input_y = (bottom_y_output - top_y_output) / (bottom_y_input - top_y_input);

void set_vertex_texcoord(Vtx* v) {
    // Calculate vertex texture coordinates based on the input coordinate. The untransformed positions of the jiggy piece vertices
    // reflect where they end up in the combined picture already, so they can be used to calculate the correct texcoord without
    // taking into account the animation.

    float output_x = output_per_input_x * v->v.ob[0];
    float output_y = output_per_input_y * v->v.ob[1];
    
    // Convert from -1,1 to 0,1. Also flip the Y-axis.
    float output_x_adjusted = output_x * 0.5f + 0.5f; 
    float output_y_adjusted = output_y * -0.5f + 0.5f;

    // Calculate texcoords based on the input texture size, accounting for a texcoord scale of 0.5.
    float texcoord_s = output_x_adjusted * DEFAULT_FRAMEBUFFER_WIDTH * 2.0f;
    float texcoord_t = output_y_adjusted * DEFAULT_FRAMEBUFFER_HEIGHT * 2.0f;

    // Convert the texcoords to fixed point with 5 bits of fractional precision.
    v->v.tc[0] = (s32)(texcoord_s * 32.0f);
    v->v.tc[1] = (s32)(texcoord_t * 32.0f);
}

void patch_falling_jiggies_model(BKModelBin* model) {
    // Validate that this is the unmodified falling jiggy transition model.
    if (model->texture_list_offset_8 != 0x38) {
        return;
    }
    if (model->gfx_list_offset_C != 0x28770) {
        return;
    }
    if (model->vtx_list_offset_10 != 0x2EC40) {
        return;
    }
    // Start and end segmented addresses of the framebuffer tile range.
    const u32 fb_tiles_start = 0x02000000;
    const u32 fb_tiles_end = 0x02028000;
    const u32 fb_tile_bytes = 32 * 32 * sizeof(u16);
    // // Texcoord offsets, multiplied by 2 to account for 0.5 texcoord scaling and converted to fixed point.
    // const u32 fb_s_offset = (((fb_grid_width - DEFAULT_FRAMEBUFFER_WIDTH /*- 1*/) * 2 / 2)) << 5;
    // const u32 fb_t_offset = (((fb_grid_height - DEFAULT_FRAMEBUFFER_HEIGHT /*- 1*/) * 2 / 2)) << 5;

    BKGfxList* gfx_list = model_getDisplayList(model);

    // Iterate until the end of the displaylist data, which lines up with the start of the vertex data.
    Gfx* cur_gfx = &gfx_list->list[0];
    Gfx* end_gfx = (Gfx*)((char*)model + model->vtx_list_offset_10);
    Vtx* model_verts = model_getVtxList(model)->vtx_18;
    s32 prev_tile_col = -1;
    s32 prev_tile_row = -1;
    while (cur_gfx < end_gfx) {
        // Check if this is a set texture image command.
        if (cur_gfx->words.w0 >> 24 == G_SETTIMG) {
            // Check if the texture pointer is a framebuffer tile.
            if (cur_gfx->words.w1 >= fb_tiles_start && cur_gfx->words.w1 < fb_tiles_end) {
                // Determine the row and column of the tile.
                u32 tile_offset = cur_gfx->words.w1 - fb_tiles_start;
                u32 tile_index = tile_offset / fb_tile_bytes;
                prev_tile_row = tile_index / fb_tiles_per_row;
                prev_tile_col = tile_index % fb_tiles_per_row;

                // Overwrite this command with a displaylist call to the new texture load.
                gSPDisplayList(cur_gfx++, load_jiggy_transition_texture);

                // Overwrite the next 6 commands with noops.
                gSPNoOp(cur_gfx++);
                gSPNoOp(cur_gfx++);
                gSPNoOp(cur_gfx++);
                gSPNoOp(cur_gfx++);
                gSPNoOp(cur_gfx++);
                gSPNoOp(cur_gfx++);

                // Rewind the graphics pointer by 1 to account for the increment that happens at the end of the loop.
                cur_gfx--;
            }
        }
        // Patch vertices.
        else if (cur_gfx->words.w0 >> 24 == G_VTX) {
            // Get the loaded vertex count for this command.
            u32 cur_vtx_count = (cur_gfx->words.w0 >> 10) & 0b111111;
            
            // Get the vertex address, converting from a segmented address if necessary.
            Vtx* cur_vtx;
            if (cur_gfx->words.w1 >> 24 == 0x01) {
                cur_vtx = (Vtx*)(SEGMENT_OFFSET(cur_gfx->words.w1) + (char*)model_verts);
            }
            else {
                // Not a segmented address (should never happen, but may help with compatibility for future mods).
                cur_vtx = (Vtx*)cur_gfx->words.w1;
            }

            for (u32 i = 0; i < cur_vtx_count; i++) {
                // Recalculate texture coordinates based on the vertex coordinate. The untransformed vertices of the jiggy pieces
                // have their final position already, so they can be used to calculate the correct texcoord.

                // All texcoords in the model can be written to without an issue, as any vertices that aren't used to display
                // the framebuffer have texcoord generation enabled, so these edits will be ignored.
                set_vertex_texcoord(&cur_vtx[i]);
            }
        }
        cur_gfx++;
    }
};

bool should_draw_to_transition_framebuffer() {
    if (s_current_transition.transistion_info == NULL) {
        return FALSE;
    }
    
    if (s_current_transition.transistion_info->uid == TRANSITION_ID_10_FALLING_PIECES_IN) {
        return s_current_transition.substate <= 2;
    }
    else {
        return s_current_transition.substate == 2;
    }
}

// @recomp Patched to use the dedicated transition framebuffer if this is drawing a transition frame.
RECOMP_PATCH void scissorBox_SetForGameMode(Gfx **gdl, s32 framebuffer_idx) {
    if(getGameMode() == GAME_MODE_8_BOTTLES_BONUS || getGameMode() == GAME_MODE_A_SNS_PICTURE)
    {
        scissorBox_setSmall();
        func_80253640(gdl, func_8030C704());
    }
    else{
        scissorBox_setDefault();
        func_80253640(gdl, gFramebuffers[framebuffer_idx]);
        // @recomp After clearing the main framebuffer, switch to and clear the dedicated transition framebuffer.
        if (should_draw_to_transition_framebuffer()) {
            func_80253640(gdl, jiggy_transition_framebuffer);
        }
    }
}

// s_current_transition.transistion_info != NULL
// s_current_transition.transistion_info->model_index == ASSET_467_MODEL_TRANSITION_FALLING_JIGGIES
// s_current_transition.substate == 1 (or maybe < 2)

// @recomp Patched to set a transform ID for transition models and the transition projection transform ID.
RECOMP_PATCH void gctransition_draw(Gfx **gdl, Mtx **mptr, Vtx **vptr){
    f32 vp_position[3];
    f32 vp_rotation[3];
    f32 percentage;
    f32 sp58[3];
    f32 scale;

    s_current_transition.unk0++;
    if(s_current_transition.state == 0)
        return;

    viewport_backupState();
    if(s_current_transition.anctrl != NULL){
        vp_position[0] = 0.0f;
        vp_position[1] = 0.0f;
        vp_position[2] = 350.0f;
    }else{
        vp_position[0] = 0.0f;
        vp_position[1] = 0.0f;
        vp_position[2] = 300.0f;
    }

    vp_rotation[0] = 0.0f;
    vp_rotation[1] = 0.0f;
    vp_rotation[2] = 0.0f;
    viewport_setNearAndFar(D_8036C440, D_8036C444);
    viewport_setPosition_vec3f(vp_position); //viewport_getPosition_vec3f
    viewport_setRotation_vec3f(vp_rotation); //viewport_getRotation_vec3f
    viewport_update(); //camera_updateNormal

    // @recomp Set the perspective projection transform ID for the transition projection.
    cur_perspective_projection_transform_id = PROJECTION_TRANSITION_TRANSFORM_ID;

    viewport_setRenderViewportAndPerspectiveMatrix(gdl, mptr);

    sp58[0] = 0.0f;
    sp58[1] = 0.0f;
    sp58[2] = 0.0f;
    if(s_current_transition.anctrl != NULL){
        gDPSetTextureFilter((*gdl)++, G_TF_POINT);
        gDPSetColorDither((*gdl)++, G_CD_DISABLE);
        anctrl_drawSetup(s_current_transition.anctrl, sp58, 1);
        modelRender_setDepthMode(MODEL_RENDER_DEPTH_FULL);
    }
    
    // @recomp Calculate the ratio between the original aspect ratio and the current aspect ratio, except when in picture mode.
    float original_aspect = (float)DEFAULT_FRAMEBUFFER_WIDTH / DEFAULT_FRAMEBUFFER_HEIGHT;
    float cur_aspect = recomp_get_target_aspect_ratio(original_aspect);
    if (getGameMode() == GAME_MODE_A_SNS_PICTURE) {
        cur_aspect = original_aspect;
    }

    float aspect_ratio_ratio = cur_aspect / original_aspect;

    // @recomp Determine what scaling to use based on the transition type.
    float transition_scale = 1.0f;
    float transition_x_scale = 1.0f;
    if (s_current_transition.transistion_info != NULL) {
        // recomp_printf("  Transition uid: %d\n", s_current_transition.transistion_info->uid);
        // recomp_printf("  Transition model index: %04X\n", s_current_transition.transistion_info->model_index);
        switch (s_current_transition.transistion_info->uid) {
            // Scale the jiggy framebuffer transition by the aspect ratio ratio as it just needs to cover the additional horizontal space.
            // Only scale along the X axis.
            case TRANSITION_ID_10_FALLING_PIECES_IN:
            case TRANSITION_ID_11_FALLING_PIECES_OUT:
                transition_x_scale = aspect_ratio_ratio;
                break;
            // Scale other transitions by slightly more to account for overscan removal.
            default:
                transition_scale = aspect_ratio_ratio + 0.1f;
        }

        // Check if the transition model is the falling jiggies transition model, and patch it if it hasn't been patched yet.
        if (s_current_transition.transistion_info->model_index == ASSET_467_MODEL_TRANSITION_FALLING_JIGGIES && s_current_transition.model_ptr != NULL) {
            // Use pad0 to tell if the model has been patched.
            BKModelBin* transition_model = s_current_transition.model_ptr;
            if (transition_model->pad0[0] != 0xBA) {
                patch_falling_jiggies_model(transition_model);
                transition_model->pad0[0] = 0xBA;
            }
        }
    }
    else {
        // recomp_printf("  No transition_info\n");
        transition_scale = aspect_ratio_ratio + 0.1f;
    }

    //complex animation (from animation bin file)
    if(s_current_transition.state == 1 || s_current_transition.state == 6){
        // @recomp Set the current transform ID to the transition model's.
        cur_drawn_model_transform_id = TRANSITION_MODEL_TRANSFORM_ID_START;

        // recomp_printf("  A\n");

        // @recomp Scale the transition.
        set_additional_model_scale(transition_x_scale, 1.0f, 1.0f);
        modelRender_draw(gdl, mptr, sp58, vp_rotation, transition_scale, 0, s_current_transition.model_ptr);
        
        // @recomp Clear the current transform ID.
        cur_drawn_model_transform_id = 0;
        if(s_current_transition.anctrl != NULL){
            gDPSetTextureFilter((*gdl)++, G_TF_BILERP);
            gDPSetColorDither((*gdl)++, G_CD_MAGICSQ);
        }
        return;
    }


    percentage = s_current_transition.timer/(s_current_transition.transistion_info->duration);
    if(s_current_transition.state == TRANSITION_STATE_4_FADE_IN){
        //rotate and zoom in
        if(s_current_transition.transistion_info->uid == 0x10){
            scale = s_current_transition.transistion_info->scale;
            
        }
        else{
            vp_rotation[2] = s_current_transition.rotation - 90.0f*percentage;
            scale = percentage*s_current_transition.transistion_info->scale + 0.1;
        }
        // @recomp Set the current transform ID to the transition model's.
        cur_drawn_model_transform_id = TRANSITION_MODEL_TRANSFORM_ID_START;

        // recomp_printf("  B\n");

        // @recomp Scale the transition.
        set_additional_model_scale(transition_x_scale, 1.0f, 1.0f);
        modelRender_draw(gdl, mptr, sp58, vp_rotation, scale * transition_scale, 0, s_current_transition.model_ptr);
        
        // @recomp Clear the current transform ID.
        cur_drawn_model_transform_id = 0;
    }
    else if(s_current_transition.state == TRANSITION_STATE_5_FADE_OUT){//L8030B9EC
        switch (s_current_transition.transistion_info->uid)
        {
            default:
                //rotate and zoom out
                vp_rotation[2] = s_current_transition.rotation - 90.0f*percentage;
                scale = (1.0f - percentage)*s_current_transition.transistion_info->scale + 0.1;
                break;
            case 0x11:
                //static
                scale = s_current_transition.transistion_info->scale;
                break;
            case 0xA:
                //zoom out only
                vp_rotation[2] = 0.0f;
                scale = (1.0f - func_80257618(percentage))*s_current_transition.transistion_info->scale + 0.1;
                break;
        }
        if(!(s_current_transition.substate < 3) || s_current_transition.transistion_info->uid != 0x11){
            // @recomp Set the current transform ID to the transition model's.
            cur_drawn_model_transform_id = TRANSITION_MODEL_TRANSFORM_ID_START;

            // recomp_printf("  C\n");

            // @recomp Scale the transition.
            set_additional_model_scale(transition_x_scale, 1.0f, 1.0f);
            modelRender_draw(gdl, mptr, sp58, vp_rotation, scale * transition_scale, 0, s_current_transition.model_ptr);
        
            // @recomp Clear the current transform ID.
            cur_drawn_model_transform_id = 0;
        }
        else{
            modelRender_reset();
        }

    }
    else if(s_current_transition.state == TRANSITION_STATE_2_BLACK_IN){//L8030BAF4
        //fade in black (i.e. get less black)
        gcbound_reset();
        gcbound_alpha((1.0f - percentage)*255.0f + 0.5);
        gcbound_color(0,0,0);
        gcbound_draw(gdl);
    }
    else if(s_current_transition.state == TRANSITION_STATE_3_BLACK_OUT){//L8030BB6C
        //fade out black (i.e. get more black)
        gcbound_reset();
        gcbound_alpha(percentage*255.0f + 0.5);
        gcbound_color(0,0,0);
        gcbound_draw(gdl);
    }
    else if(s_current_transition.state == TRANSITION_STATE_7_WHITE_IN){//L8030BBD8
        //fade in white (i.e. get less white)
        percentage = (percentage <= 0.5)? 1.0 : 1.0 - (percentage-0.5)/0.5;
        gcbound_reset();
        gcbound_alpha(percentage*255.0f + 0.5);
        gcbound_color(0xff,0xff,0xff);
        gcbound_draw(gdl);
    }
    else if(s_current_transition.state == TRANSITION_STATE_8_WHITE_OUT){//L8030BC8C
        //fade out white (i.e. get more white)
        gcbound_reset();
        gcbound_alpha(percentage*255.0f + 0.5);
        gcbound_color(0xff,0xff,0xff);
        gcbound_draw(gdl);
    }//L8030BD00
    else{
        
    }
    if(s_current_transition.anctrl != NULL){
        gDPSetTextureFilter((*gdl)++, G_TF_BILERP);
    }
    viewport_restoreState();
    viewport_setRenderViewportAndPerspectiveMatrix(gdl, mptr);
    
}