#include "patches.h"
#include "prop.h"
#include "actor.h"
#include "functions.h"
#include "mem_funcs.h"
#include "bk_api.h"

extern ActorArray *suBaddieActorArray;
extern Actor *suLastBaddie;

void func_803255FC(Actor *this);
void func_8032B5C0(ActorMarker *arg0, ActorMarker *arg1, struct5Cs *arg2);
s32 func_80326C18(void);
void func_8032FFD4(ActorMarker *this, s32 arg1);
void marker_setModelId(ActorMarker *this, enum asset_e modelIndex);
s32 func_80306DDC(s32 *position);
s32 func_80307258(f32 arg0[3], s32 arg1, s32 arg2);
struct5Bs *func_8034A2C8(void);
void func_8033F738(ActorMarker *arg0);
void func_8034BFF8(ActorMarker *marker);
Struct83s *func_803406B0(void);

// @recomp Patched to create extension data for the actor.
RECOMP_PATCH Actor *actor_new(s32 position[3], s32 yaw, ActorInfo* actorInfo, u32 flags){
    ActorAnimationInfo * sp54;
    s32 i;
    f32 sp44[3];
    
    if(suBaddieActorArray == NULL){
        // @recomp Reset actor data when suBaddieActorArray is initially allocated.
        recomp_clear_all_actor_data();
        suBaddieActorArray = (ActorArray *)malloc(sizeof(ActorArray) + 20*sizeof(Actor));
        suBaddieActorArray->cnt = 0;
        suBaddieActorArray->max_cnt = 20;
    }
    
    if(suBaddieActorArray->cnt + 1 > suBaddieActorArray->max_cnt){
        suBaddieActorArray->max_cnt = suBaddieActorArray->cnt + 5;
        suBaddieActorArray = (ActorArray *)realloc(suBaddieActorArray, sizeof(ActorArray) + suBaddieActorArray->max_cnt*sizeof(Actor));
    }

    ++suBaddieActorArray->cnt;
    suLastBaddie = &suBaddieActorArray->data[suBaddieActorArray->cnt - 1];

    suLastBaddie->actor_info = actorInfo;
    suLastBaddie->unk10_25 = 0;
    suLastBaddie->unk10_18 = 0;
    suLastBaddie->state = actorInfo->startAnimation;
    suLastBaddie->position_x = (f32)position[0];
    suLastBaddie->position_y = (f32)position[1];
    suLastBaddie->position_z = (f32)position[2];
    suLastBaddie->unkF4_8 = 0;
    suLastBaddie->yaw = (f32) yaw;
    suLastBaddie->yaw_ideal = (f32) yaw;
    suLastBaddie->pitch = 0.0f;
    suLastBaddie->roll = 0.0f;
    suLastBaddie->unk6C = 0.0f;
    suLastBaddie->actor_specific_1_f = 0.0f;
    suLastBaddie->unk10_12 = 0;
    suLastBaddie->unk38_0 = 0;
    suLastBaddie->unk38_31 = 0;
    suLastBaddie->unk58_0 = 1;
    suLastBaddie->unk40 = 0;
    suLastBaddie->unk44_31 = 0;
    suLastBaddie->despawn_flag = 0;
    suLastBaddie->unk44_14 = -1;
    suLastBaddie->unk48 = 0.0f;
    suLastBaddie->unk4C = 100.0f;
    suLastBaddie->unk10_1 = 1;
    suLastBaddie->unkF4_30 = 0;
    suLastBaddie->unkF4_29 = 0;
    suLastBaddie->scale = 1.0f;
    suLastBaddie->unk124_7 = 0;
    suLastBaddie->unk124_6 = 1;
    suLastBaddie->modelCacheIndex = actorInfo->actorId;
    suLastBaddie->unk44_2 = func_80326C18();
    suLastBaddie->marker = marker_init(position, actorInfo->draw_func, (asset_getFlag(actorInfo->modelId) == 1) ? 0 : 1, actorInfo->markerId, (flags & 0x400) ? 1 : 0);
    suLastBaddie->marker->unk3E_0 = 1;
    suLastBaddie->unk138_28 = 1;
    suLastBaddie->unk10_3 = -1;
    suLastBaddie->unk10_4 = 0;
    suLastBaddie->unk10_8 = 0;
    suLastBaddie->unk10_7 = 0;
    suLastBaddie->unk10_6 = 0;
    suLastBaddie->unk54 = 0.0f;
    suLastBaddie->anctrl_asset_id = 0;
    suLastBaddie->unk5C = 0.0f;
    suLastBaddie->unkF4_31 = 0;
    suLastBaddie->unk138_30 = 0;
    suLastBaddie->unk138_3 = 0;
    suLastBaddie->unk38_21 = 0;
    suLastBaddie->unk38_13 = 0;
    suLastBaddie->unk78_22 = 0;
    suLastBaddie->unk78_31 = 0;
    suLastBaddie->unk74 = 0.0f;
    suLastBaddie->unk70 = 0.0f;
    suLastBaddie->unkF4_24 = 0;
    suLastBaddie->unk140 = 0.0f;
    suLastBaddie->unk144 = 0.0f;
    suLastBaddie->unk44_1 = 0;
    suLastBaddie->unk44_0 = 0;
    suLastBaddie->initialized = FALSE;
    suLastBaddie->volatile_initialized = FALSE;
    suLastBaddie->lifetime_value = 0.0f;
    suLastBaddie->is_bundle = FALSE;
    suLastBaddie->unk104 = NULL;
    suLastBaddie->unk100 = NULL;
    suLastBaddie->unk158[0] = NULL;
    suLastBaddie->unk158[1] = NULL;
    suLastBaddie->unk78_13 = 0;
    suLastBaddie->unk124_31 = 0;
    suLastBaddie->unkF4_20 = 0;
    suLastBaddie->sound_timer = 0.0f;
    func_8032FFD4(suLastBaddie->marker, suBaddieActorArray->cnt - 1);
    marker_setModelId(suLastBaddie->marker, actorInfo->modelId);
    marker_setActorUpdateFunc(suLastBaddie->marker, actorInfo->update_func);
    marker_setActorUpdate2Func(suLastBaddie->marker, actorInfo->update2_func);
    ml_vec3f_clear(suLastBaddie->unk1C);
    ml_vec3f_clear(suLastBaddie->velocity);
    ml_vec3f_clear(suLastBaddie->spawn_position);
    suLastBaddie->stored_anctrl_index = 0;
    suLastBaddie->unk58_2 = 1;
    suLastBaddie->stored_anctrl_playbackType_ = 0;
    suLastBaddie->stored_anctrl_forwards = 0;
    suLastBaddie->stored_anctrl_smoothTransistion = 0;
    suLastBaddie->stored_anctrl_duration = 0.0f;
    suLastBaddie->stored_anctrl_timer = 0.0f;
    suLastBaddie->unk138_19 = 0;
    suLastBaddie->stored_anctrl_subrangeMin = 0.0f;
    suLastBaddie->stored_anctrl_subrangeMax = 1.0f;
    suLastBaddie->unkF4_22 = 0;
    suLastBaddie->unk58_1 = 0;
    suLastBaddie->unk138_29 = 0;
    suLastBaddie->unk18 = actorInfo->animations;
    suLastBaddie->anctrl = NULL;
    suLastBaddie->stored_anctrl_timer = 0.0f;
    suLastBaddie->unk130 = 0;
    suLastBaddie->unk124_5 = 0;
    suLastBaddie->unk124_3 = 0;
    suLastBaddie->unk138_9 = 0;
    suLastBaddie->unk138_8 = 0;
    suLastBaddie->unk138_25 = 0;
    suLastBaddie->unk16C_3 = 0;
    suLastBaddie->unk16C_2 = 0;
    suLastBaddie->unk16C_1 = 0;
    suLastBaddie->unk16C_0 = 0;
    suLastBaddie->unk17C_31 = 0;
    suLastBaddie->unk14C[0] = NULL;
    suLastBaddie->unk14C[1] = NULL;
    suLastBaddie->unk138_27 = 0;
    suLastBaddie->has_met_before = FALSE;
    suLastBaddie->unk138_23 = 0;
    suLastBaddie->unk138_22 = 0;
    suLastBaddie->unk138_21 = 0;
    suLastBaddie->unk138_20 = 0;
    suLastBaddie->unk174 = 0.0f;
    suLastBaddie->unk178 = 0.0f;
    if( actorInfo->animations){
        sp54 = &suLastBaddie->unk18[suLastBaddie->state];
        if(sp54->index != 0){
            suLastBaddie->anctrl = anctrl_new(0);
            anctrl_reset(suLastBaddie->anctrl);
            anctrl_setIndex(suLastBaddie->anctrl, sp54->index);
            anctrl_setDuration(suLastBaddie->anctrl, sp54->duration);
            anctrl_start(suLastBaddie->anctrl, "subaddie.c", 0x4A5);
        }
    }//L80327BA8
    suLastBaddie->unk124_11 = 0;
    suLastBaddie->alpha_124_19 = 0xff;
    suLastBaddie->depth_mode = MODEL_RENDER_DEPTH_FULL;
    suLastBaddie->unk124_0 = suLastBaddie->unk138_31 = 1;
    for(i = 0; i < 0x10; i++){
        ((s32 *)suLastBaddie->unk7C)[i] = 0;
    }
    for(i = 0; i < 0x0C; i++){
        ((s32 *)suLastBaddie->unkBC)[i] = 0;
    }
    if(flags & ACTOR_FLAG_UNKNOWN_0){
        suLastBaddie->unk10_25 = func_80306DDC(position) + 1;
        if(suLastBaddie->unk10_25 == 0){
            suLastBaddie->unk10_25 = 0;
        }else{
            sp44[0] = (f32)position[0];
            sp44[1] = (f32)position[1];
            sp44[2] = (f32)position[2];
            suLastBaddie->unk10_18 = func_80307258(sp44, suLastBaddie->unk10_25 - 1, 0) + 1;
        }
    }//L80327D30

    if(flags & ACTOR_FLAG_UNKNOWN_2){
        suLastBaddie->unk10_1 = 0;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_3){
        suLastBaddie->unkF4_30 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_1){
        suLastBaddie->marker->unk44 = (struct5Bs*)1;
    }
    else if(flags & ACTOR_FLAG_UNKNOWN_6){
        suLastBaddie->marker->unk44 = func_8034A2C8();
    }

    if(flags & ACTOR_FLAG_UNKNOWN_12){
        func_8033F738(suLastBaddie->marker);
        func_8034BFF8(suLastBaddie->marker);
    }

    suLastBaddie->unk148 = 0;
    if(flags & ACTOR_FLAG_UNKNOWN_11){
        suLastBaddie->unk148 = skeletalAnim_new();
    }

    if(flags & ACTOR_FLAG_UNKNOWN_14){
        suLastBaddie->marker->unk50 = (s32)func_803406B0();
    }

    if(flags & ACTOR_FLAG_UNKNOWN_4){
        suLastBaddie->unk124_31 = -1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_7){
        suLastBaddie->unkF4_22 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_19){
        suLastBaddie->unk58_1 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_8){
        suLastBaddie->unk130 = func_803255FC;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_9){
        suLastBaddie->marker->unk40_21 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_15){
        suLastBaddie->marker->unk40_20 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_17){
        suLastBaddie->marker->unk40_22 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_22){
        suLastBaddie->marker->unk40_19 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_16){
        suLastBaddie->unk138_9 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_18){
        suLastBaddie->unk138_8 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_21){
        suLastBaddie->unk138_25 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_23){
        suLastBaddie->unk16C_3 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_24){
        suLastBaddie->unk16C_2 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_25){
        suLastBaddie->unk16C_1 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_26){
        suLastBaddie->unk17C_31 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_13){
        suLastBaddie->unk138_29 = 1;
    }

    if(flags & ACTOR_FLAG_UNKNOWN_20){
        suLastBaddie->unk58_2 = 0;
    }

    suLastBaddie->unk154 = 0x005e0000;
    suLastBaddie->marker->unk54 = (void (*)(struct actorMarker_s *, struct actorMarker_s *, u16 *))func_8032B5C0;

    
    for(i = 0; i < 3; ++i){
        suLastBaddie->unk164[i] = 0x63;
    }

    suLastBaddie->unk170 = -10.0f;
    suLastBaddie->unk138_7 = 0;
    suLastBaddie->unk3C = flags;

    // @recomp Allocate extension data for this actor and place it in padding.
    suLastBaddie->pad17C_30 = recomp_create_actor_data(actorInfo->actorId) & 0x7FFFFFFFU;
    
    return suLastBaddie;
}

void func_8032BB88(Actor *this, s32 arg1, s32 arg2);
void func_8033E7CC(ActorMarker *arg0);
void func_8034A2A8(struct5Bs *this);
void func_8034BF54(ActorMarker *marker);
void func_8033F784(ActorMarker *arg0);
void func_80340690(Struct83s *self);
void func_8032ACA8(Actor *arg0);

// @recomp Patched to destroy the extended actor data.
RECOMP_PATCH void func_80325FE8(Actor *this) {
    ActorMarker *marker;
    u8 temp_v0;

    // @recomp Destroy the extended actor data.
    recomp_destroy_actor_data(this->pad17C_30);

    marker = this->marker;
    marker->id = 0;
    if (this->anctrl != NULL) {
        anctrl_free(this->anctrl);
    }
    temp_v0 = this->unk44_31;
    if (temp_v0 != 0) {
        sfxsource_freeSfxsourceByIndex(temp_v0);
    }
    this->anctrl = NULL;
    this->unk44_31 = 0;

    if (this->unk138_7 != 0) {
        func_8032BB88(this, -1, 8000);
        this->unk138_7 = 0;
    }
    if (marker->actorFreeFunc != NULL) {
       marker->actorFreeFunc(this);
       marker->actorFreeFunc = NULL;
    }
    if ((s32)marker->unk44 < 0) {
        func_8033E7CC(marker);
        func_8034A2A8(marker->unk44);
       marker->unk44 = 0;
    }
    if (marker->unk4C != 0) {
        func_8034BF54(this->marker);
        marker->unk4C = 0;
    }
    if (marker->unk48 != 0) {
        func_8033F784(marker);
        marker->unk48 = 0;
    }
    if (this->unk148 != NULL) {
        skeletalAnim_free(this->unk148);
        this->unk148 = NULL;
    }
    if (marker->unk50 != 0) {
        func_80340690((Struct83s *)marker->unk50);
        marker->unk50 = 0;
    }
    func_8032ACA8(this);
}

RECOMP_EXPORT ActorExtensionId bkrecomp_extend_actor(enum actor_e type, u32 size) {
    return recomp_register_actor_extension(type, size);
}

RECOMP_EXPORT ActorExtensionId bkrecomp_extend_actor_all(u32 size) {
    return recomp_register_actor_extension_generic(size);
}

RECOMP_EXPORT void* bkrecomp_get_extended_actor_data(Actor* actor, ActorExtensionId extension) {
    return recomp_get_actor_data(actor->pad17C_30, extension, actor->actor_info->actorId);
}

RECOMP_EXPORT u32 bkrecomp_get_actor_spawn_index(Actor* actor) {
    return recomp_get_actor_spawn_index(actor->pad17C_30);
}
