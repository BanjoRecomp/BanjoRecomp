#include "patches.h"
#include "transform_ids.h"
#include "functions.h"
#include "rand.h"
#include "core1/ml.h"
#include "core2/anctrl.h"
#include "core2/modelRender.h"
#include "core2/particle.h"
#include "core2/timedfunc.h"

u32 particle_spawn_count = 0;

extern ParticleEmitter **partEmitMgr;
extern s32 partEmitMgrLength;

typedef struct particle{
    f32 acceleration[3];
    f32 fade;
    f32 frame; //frame
    f32 framerate; //framerate
    f32 position[3];
    f32 rotation[3];
    f32 scale; //size
    f32 initialSize_34; //initial_size
    f32 finalSizeDiff; //delta_size
    f32 angluar_velocity[3];
    f32 age_48;
    f32 lifetime_4C;
    f32 velocity_50[3];
    u8 unk5C;
    //u8 pad5D[3];
} Particle;

u32 get_particle_id(Particle *p) {
    u8 *padding = &p->unk5C + 1;

    u32 id = (padding[0] << 16) | (padding[1] << 8) | (padding[2] << 0);
    return id;
}

void set_particle_id(Particle *p, u32 id) {
    u8 *padding = &p->unk5C + 1;

    padding[0] = (id >> 16) & 0xFF;
    padding[1] = (id >>  8) & 0xFF;
    padding[2] = (id >>  0) & 0xFF;
}

extern Gfx D_80368978[];
extern Gfx D_80368940[];

void __particleEmitter_drawOnPass(ParticleEmitter *this, Gfx **gfx, Mtx **mtx, Vtx **vtx, u32 draw_pass);
void func_803382E4(s32);
void func_80338338(s32, s32, s32);
void func_803382B4(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void func_80338370(void);
void func_80335D30(Gfx **);
void func_802EED1C(ParticleEmitter *this, f32 age, f32 arg2[3]);
void func_80344C2C(bool arg0);
void func_80344720(BKSpriteDisplayData *arg0, s32 frame, bool mirrored, f32 position[3], f32 rotation[3], f32 scale[3], Gfx **gfx, Mtx **mtx);
void func_80344424(BKSpriteDisplayData *arg0, s32 frame, bool mirrored, f32 position[3], f32 scale[3], f32 rotation, Gfx **gfx, Mtx **mtx);
void func_8033687C(Gfx **);

// @recomp Patched to tag particles as they're drawn.
RECOMP_PATCH void __particleEmitter_drawOnPass(ParticleEmitter *this, Gfx **gfx, Mtx **mtx, Vtx **vtx, u32 draw_pass){
    f32 position[3];
    f32 flat_rotation[3];
    f32 scale[3];
    Particle *iPtr;

    if(reinterpret_cast(u32, draw_pass) != (this->draw_mode & 0x4) != 0)
        return;

    if(this->model_20 != NULL){
        for(iPtr = this->pList_start_124; iPtr < this->pList_end_128; iPtr++){
            position[0] = iPtr->position[0] + this->unk4[0];
            position[1] = iPtr->position[1] + this->unk4[1];
            position[2] = iPtr->position[2] + this->unk4[2];
            if( 0.0f != this->fade_in || 1.0 != this->fade_out || this->alpha != 0xff ){
                modelRender_setAlpha((s32) (iPtr->fade*this->alpha));
            }//L802EEF5C
            modelRender_setDepthMode((this->draw_mode & PART_EMIT_NO_DEPTH)? MODEL_RENDER_DEPTH_NONE : MODEL_RENDER_DEPTH_FULL);
            // @recomp Get the particle's ID and use it to set the current model transform ID.
            cur_drawn_model_transform_id = MODEL_PARTICLE_TRANSFORM_ID_START + (get_particle_id(iPtr) % MODEL_PARTICLE_ID_MAX) * MARKER_TRANSFORM_ID_COUNT;

            modelRender_draw(gfx, mtx, position, iPtr->rotation, iPtr->scale, NULL, this->model_20);

            // @recomp Reset the current model transform ID.
            cur_drawn_model_transform_id = 0;
        }
        return;
    }
    
    if(this->unk34){//L802EEFC4
        if( this->rgb[0] != 0xff 
            || this->rgb[1] != 0xff 
            || this->rgb[2] != 0xff 
            || this->alpha != 0xff 
        ){
            func_803382E4((this->draw_mode & PART_EMIT_NO_DEPTH)? 9: 0xf);
            func_80338338(this->rgb[0], this->rgb[1], this->rgb[2]);
            func_803382B4(
                (this->rgb[0] < 8)? 0 : this->rgb[0] - 8,
                (this->rgb[1] < 8)? 0 : this->rgb[1] - 8,
                (this->rgb[2] < 8)? 0 : this->rgb[2] - 8,
                (this->draw_mode & PART_EMIT_NO_OPA)? 0xff : this->alpha
            );
            func_80338370();
            func_80335D30(gfx);
        }
        else if(this->draw_mode & PART_EMIT_NO_DEPTH){//L802EF0C0
            gSPDisplayList((*gfx)++, D_80368978);
        }
        else{//L802EF0EC
            gSPDisplayList((*gfx)++, D_80368940);
        }//L802EF10C
        flat_rotation[0] = 90.0f;
        flat_rotation[1] = 0.0f;
        flat_rotation[2] = 0.0f;
        for(iPtr = this->pList_start_124; iPtr < this->pList_end_128; iPtr++){
            gDPSetPrimColor((*gfx)++, 0, 0, this->rgb[0], this->rgb[1], this->rgb[2], iPtr->fade*this->alpha);
            position[0] = iPtr->position[0] + this->unk4[0];
            position[1] = iPtr->position[1] + this->unk4[1];
            position[2] = iPtr->position[2] + this->unk4[2];

            scale[0] = iPtr->scale;
            scale[1] = iPtr->scale;
            scale[2] = iPtr->scale;
            if(0.0f != this->unk108){
                func_802EED1C(this, iPtr->age_48, scale);
            }
            func_80344C2C(this->unk0_16);
            // @recomp Get the particle's ID and use it to set a matrix group for this particle.
            u32 transform_id = NORMAL_PARTICLE_TRANSFORM_ID_START + get_particle_id(iPtr);
            gEXMatrixGroupDecomposedNormal((*gfx)++, transform_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);

            if(this->draw_mode & PART_EMIT_ROTATABLE){
                func_80344720(this->unk34, (s32)iPtr->frame, 0, position, flat_rotation, scale, gfx, mtx);
            }//L802EF2F8
            else{
                func_80344424(this->unk34, (s32)iPtr->frame, 0, position, scale, iPtr->rotation[2], gfx, mtx);
            }//L802EF324

            // @recomp Pop the particle's matrix group.
            gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
        }//L802EF338
        if( this->rgb[0] != 0xff || this->rgb[1] != 0xff || this->rgb[2] != 0xff || this->alpha != 0xff 
        ){
            func_8033687C(gfx);
        }
    }
}

// @recomp Patched to set an incrementing ID for the particle.
RECOMP_PATCH void __particleEmitter_initParticle(ParticleEmitter *this, Particle *particle){
    particle->acceleration[0] = randf2(this->particleAccerationRange_4C_min_x, this->particleAccerationRange_4C_max_x);
    particle->acceleration[1] = randf2(this->particleAccerationRange_4C_min_y, this->particleAccerationRange_4C_max_y);
    particle->acceleration[2] = randf2(this->particleAccerationRange_4C_min_z, this->particleAccerationRange_4C_max_z);
    particle->unk5C = this->unk64;
    
    particle->fade = (0.0f == this->fade_in) ? 1.0f : 0; 
    particle->frame = randf2((f32)this->particleStartingFrameRange_84_min, (f32)this->particleStartingFrameRange_84_max);
    particle->framerate = randf2(this->particleFramerateRange_8C_min, this->particleFramerateRange_8C_max);

    particle->position[0] = this->postion_28[0];
    particle->position[1] = this->postion_28[1];
    particle->position[2] = this->postion_28[2];

    particle->position[0] = particle->position[0] + randf2(this->particleSpawnPositionRange_94_min_x, this->particleSpawnPositionRange_94_max_x);
    particle->position[1] = particle->position[1] + randf2(this->particleSpawnPositionRange_94_min_y, this->particleSpawnPositionRange_94_max_y);
    particle->position[2] = particle->position[2] + randf2(this->particleSpawnPositionRange_94_min_z, this->particleSpawnPositionRange_94_max_z);

    particle->initialSize_34 = particle->scale = randf2(this->particleStartingScaleRange_AC_min, this->particleStartingScaleRange_AC_max);
    if(0.0f == this->particleFinalScaleRange_B4_min && 0.0f == this->particleFinalScaleRange_B4_max)
        particle->finalSizeDiff = 0.0f;
    else
        particle->finalSizeDiff = randf2(this->particleFinalScaleRange_B4_min, this->particleFinalScaleRange_B4_max)- particle->initialSize_34;

    particle->rotation[2] = 0.0f;
    particle->rotation[1] = 0.0f;
    particle->rotation[0] = 0.0f;

    particle->angluar_velocity[0] = randf2(this->unkBC[0], this->unkC8[0]);
    particle->angluar_velocity[1] = randf2(this->unkBC[1], this->unkC8[1]);
    particle->angluar_velocity[2] = randf2(this->unkBC[2], this->unkC8[2]);
    
    particle->age_48 = 0.0f;
    particle->lifetime_4C = randf2(this->particleLifeTimeRange[0], this->particleLifeTimeRange[1]) + 0.001;
    if(!this->sphericalParticleVelocity_48){
        particle->velocity_50[0] = randf2(this->particleVelocityRange_E4.cartisian_min_x, this->particleVelocityRange_E4.cartisian_max_x);
        particle->velocity_50[1] = randf2(this->particleVelocityRange_E4.cartisian_min_y, this->particleVelocityRange_E4.cartisian_max_y);
        particle->velocity_50[2] = randf2(this->particleVelocityRange_E4.cartisian_min_z, this->particleVelocityRange_E4.cartisian_max_z);
    }
    else{
        func_80256E24(particle->velocity_50, 
            mlNormalizeAngle(randf2(this->particleVelocityRange_E4.spherical.pitch_min, this->particleVelocityRange_E4.spherical.pitch_max)),
            mlNormalizeAngle(randf2(this->particleVelocityRange_E4.spherical.yaw_min, this->particleVelocityRange_E4.spherical.yaw_max)),
            0.0f,
            0.0f,
            randf2(this->particleVelocityRange_E4.spherical.radius_min, this->particleVelocityRange_E4.spherical.radius_max)
        );
    }
    
    // @recomp Set the particle's ID based on the particle spawn count.
    set_particle_id(particle, particle_spawn_count);
    // @recomp Increment the particle spawn count.
    particle_spawn_count++;
    if (particle_spawn_count >= NORMAL_PARTICLE_ID_MAX) {
        particle_spawn_count = 0;
    }
}

extern void mlMtxRotatePYR(f32, f32, f32);

typedef struct {
    BKSprite *sprite_0;
    f32 position[3];
    f32 rotation[3];
    f32 unk1C;
    s16 unk20[2];
    u8 color[3];
    u8  unk27;
    u32 frame_28_31:8;
    u32 unk28_23:2;
    u32 unk28_21:8;
    u32 unk28_13:1;
    u32 unk28_12:1;
    u32 pad28_11:12;
} Struct_B8860_0s;

void projectile_setRoll(u8 indx, f32 angle);
f32 projectile_getRoll(u8 indx);
void projectile_freeByIndex(u8 arg0);
void func_803382FC(s32);
void func_80338308(s32 arg0, s32 arg1);
void func_8033837C(s32 arg0);
void spriteRender_draw(Gfx **gfx, Vtx **vtx, BKSprite *sp, u32 frame);

extern Struct_B8860_0s D_80385000[0x32];

// @recomp Patched to tag projectiles.
RECOMP_PATCH void func_8033F7F0(u8 indx, Gfx **gfx, Mtx **mtx, Vtx **vtx){
    Struct_B8860_0s *sp54;
    f32 sp48[3];
    f32 sp3C[3];
    f32 sp30[3];

    sp54 = &D_80385000[indx];
    if(sp54->unk28_23 != 1){
        ml_vec3f_copy(sp48,  sp54->position);
        sp48[1] += (sp54->unk1C*sp54->unk20[1])/100.0;
        viewport_getPosition_vec3f(sp3C);
        ml_vec3f_diff_copy(sp30, sp48, sp3C);
        if(sp54->unk28_12){
            mlMtxSet(viewport_getMatrix());
        }
        else{
            mlMtxIdent();
        }
        mlMtxRotatePYR(sp54->rotation[0], sp54->rotation[1], sp54->rotation[2]);
        func_80252330(sp30[0], sp30[1], sp30[2]);
        mlMtxApply(*mtx);
        gSPMatrix((*gfx)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        func_803382E4(sp54->unk28_21);
        func_80338338(sp54->color[0], sp54->color[1],sp54->color[2]);
        func_803382FC(sp54->unk27);
        func_80338308(sp54->unk20[0], sp54->unk20[1]);
        func_8033837C(1);
        func_80338370();

        // @recomp Set a matrix group before drawing the sprite.
        gEXMatrixGroupSimpleNormal((*gfx)++, PROJECTILE_TRANSFORM_ID_START + indx, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);

        spriteRender_draw(gfx, vtx, sp54->sprite_0, sp54->frame_28_31);
        gSPPopMatrix((*gfx)++, G_MTX_MODELVIEW);

        // @recomp Clear the matrix group.
        gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
    }
}
