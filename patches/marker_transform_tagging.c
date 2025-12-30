#include "patches.h"
#include "bk_api.h"
#include "transform_ids.h"
#include "functions.h"

extern u8 D_8037BFB8;
extern s32 D_8036E7B0;

void eggShatter_draw(Gfx **gPtr, Mtx **mPtr, Vtx **vPtr);
void baModel_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx);
void func_8033A28C(bool arg0);
void func_8033A244(f32);
void func_8033A280(f32);

// @recomp Patched to set the current transform ID to banjo's when drawing the player.
RECOMP_PATCH void player_draw(Gfx **gfx, Mtx **mtx, Vtx **vtx) {
    if (D_8037BFB8) {
        eggShatter_draw(gfx, mtx, vtx);

        // @recomp Set the current transform ID to banjo's.
        u32 prev_transform_id = cur_drawn_model_transform_id;
        cur_drawn_model_transform_id = BANJO_TRANSFORM_ID_START;

        baModel_draw(gfx, mtx, vtx);

        // @recomp Reset the current transform ID.
        cur_drawn_model_transform_id = prev_transform_id;
    }
}

// @recomp Patched to set the actor being drawn before the actor's draw func is called.
RECOMP_PATCH void __marker_draw(ActorMarker *this, Gfx **gfx, Mtx **mtx, Vtx **vtx){
    Actor *actor;
    u32 draw_dist;
    f32 draw_dist_f;
    f32 percentage;

    // @recomp Calculate the current drawn marker's transform ID.
    s32 cur_drawn_marker_spawn_index = bkrecomp_get_marker_spawn_index(this);
    u32 prev_transform_id = cur_drawn_model_transform_id;
    u32 transform_id = MARKER_TRANSFORM_ID_START + cur_drawn_marker_spawn_index * MARKER_TRANSFORM_ID_COUNT;
    if(!this->unk3E_0){
        // @recomp Set the current drawn marker's transform ID.
        cur_drawn_model_transform_id = transform_id;
        this->drawFunc(this, gfx, mtx, vtx);
        // @recomp Clear the current transform ID after drawing.
        cur_drawn_model_transform_id = prev_transform_id;
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
        // @recomp Set the current drawn marker's transform ID.
        cur_drawn_model_transform_id = transform_id;
        this->drawFunc(this, gfx, mtx, vtx);
        // @recomp Set the current drawn marker to null after drawing.
        cur_drawn_model_transform_id = prev_transform_id;
    }//L8032D300
    func_8033A244(30000.0f);
    func_8033A280(1.0f);
}
