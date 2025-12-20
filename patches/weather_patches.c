#include "patches.h"
#include "transform_ids.h"
#include "functions.h"
#include "core1/core1.h"

extern s32 func_8033A170(void);

u32 weather_particle_spawn_count = 0;

u32 get_weather_particle_id(struct5s *p) {
    u8 *padding = p->pad35;
    u32 id = (padding[0] << 16) | (padding[1] << 8) | (padding[2] << 0);
    return id;
}

void set_weather_particle_id(struct5s *p, u32 id) {
    u8 *padding = p->pad35;
    padding[0] = (id >> 16) & 0xFF;
    padding[1] = (id >> 8) & 0xFF;
    padding[2] = (id >> 0) & 0xFF;
}

// @recomp Patched to assign an interpolation ID to the weather particle when it's created.
RECOMP_PATCH void func_802F87B0(struct6s *this) {
    f32 plyrPos[3]; //sp7C
    f32 camNorm[3]; //sp70
    f32 camRot[3]; //sp64
    struct5s *ptr;
    f32 f20;
    int i;
    f32 sp4C[3];

    if (vector_size(this->unk1C) >= this->unk20)
        return;

    player_getPosition(plyrPos);
    viewport_getLookVector(camNorm);
    viewport_getRotation_vec3f(camRot);
    ptr = vector_pushBackNew(&this->unk1C);
    f20 = randf2(50.0f, 1200.0f);
    sp4C[0] = 0.0f;
    sp4C[1] = randf2(200.0f, 500.0f);
    sp4C[2] = -f20;

    if (LENGTH_VEC3F((&this->unkC)) < 5.0f) {
        ml_vec3f_yaw_rotate_copy(sp4C, sp4C, randf2(0.0f, 360.0f));
    }
    else {
        ml_vec3f_yaw_rotate_copy(sp4C, sp4C, camRot[1] + randf2(-70.0f, 70.0f));
    }//L802F88F0
    sp4C[0] += plyrPos[0];
    sp4C[1] += plyrPos[1];
    sp4C[2] += plyrPos[2];
    if (f20 < 600.0) {
        for (i = 0; i < 10 && viewport_isPointPlane_3f(sp4C[0], sp4C[1] - 10.0f, sp4C[2]); i++) {
            sp4C[1] += 100.0f;
        }
    }
    ptr->unk4[0] = sp4C[0];
    ptr->unk4[1] = sp4C[1];
    ptr->unk4[2] = sp4C[2];

    ptr->unk10[0] = 0.0f;
    ptr->unk10[1] = randf2(-100.0f, -100.0f);
    ptr->unk10[2] = 0.0f;

    ptr->unk1C[0] = ptr->unk1C[1] = ptr->unk1C[2] = 0.0f;

    ptr->unk28[0] = randf2(-300.0f, 300.0f);
    ptr->unk28[1] = randf2(-300.0f, 300.0f);
    ptr->unk28[2] = randf2(-300.0f, 300.0f);

    this->unk34++;
    if (!(this->unk34 < 4))
        this->unk34 = 0;

    ptr->unk0 = this->unk24[this->unk34];

    // @recomp Set the weather particle's ID based on the particle spawn count and increment it.
    set_weather_particle_id(ptr, weather_particle_spawn_count++);
    if (weather_particle_spawn_count >= WEATHER_PARTICLE_ID_MAX) {
        weather_particle_spawn_count = 0;
    }
}

// @recomp Patched to tag the weather effect for interpolation.
RECOMP_PATCH void func_802F8A90(struct6s *this, Gfx **gdl, Mtx **mptr, Vtx **vptr) {
    struct5s *startPtr = vector_getBegin(this->unk1C);
    struct5s *iPtr;
    struct5s *endPtr = vector_getEnd(this->unk1C);
    for (iPtr = startPtr; iPtr < endPtr; iPtr++) {
        modelRender_setDepthMode(MODEL_RENDER_DEPTH_COMPARE);

        // @recomp Set the model transform ID before drawing the weather particle.
        cur_drawn_model_transform_id = WEATHER_PARTICLE_TRANSFORM_ID_START + WEATHER_PARTICLE_ID_COUNT * get_weather_particle_id(iPtr);

        modelRender_draw(gdl, mptr, iPtr->unk4, iPtr->unk1C, 1.0f, NULL, iPtr->unk0);

        // @recomp Clear the model transform ID after drawing the weather particle.
        cur_drawn_model_transform_id = 0;

        iPtr->unk34 = func_8033A170();
    }
}