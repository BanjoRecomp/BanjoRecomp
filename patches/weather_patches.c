#include "patches.h"
#include "transform_ids.h"
#include "functions.h"
#include "core1/core1.h"

extern s32 func_8033A170(void);

extern struct6s *D_80381030;
extern struct3s *D_80381034;

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

    // @recomp Force particles to spawn 360 degrees around the player at all times.
    ml_vec3f_yaw_rotate_copy(sp4C, sp4C, randf2(0.0f, 360.0f));

#if 0
    if (LENGTH_VEC3F((&this->unkC)) < 5.0f) {
        ml_vec3f_yaw_rotate_copy(sp4C, sp4C, randf2(0.0f, 360.0f));
    }
    else {
        ml_vec3f_yaw_rotate_copy(sp4C, sp4C, camRot[1] + randf2(-70.0f, 70.0f));
    }//L802F88F0
#endif

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

    // @recomp Increase the spawn rate of the particles by calling the function recursively again.
    // Infinite recursion is prevented by keeping track of the current recursion depth. To control
    // the frequency of the spawn rate, an additional counter is used to spawn an extra particle
    // every N amount of frames.
    const u32 spawn_rate_frequency = 2;
    static u32 recursion_depth = 0;
    if (recursion_depth == 0) {
        static u32 spawn_rate_counter = 0;
        spawn_rate_counter++;

        if (spawn_rate_counter == spawn_rate_frequency) {
            recursion_depth++;
            func_802F87B0(this);
            recursion_depth--;
            spawn_rate_counter = 0;
        }
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

// @recomp Patched to increase capacity of the weather effect vector.
RECOMP_PATCH struct6s *func_802F7C38(void) {
    if (D_80381030 == NULL) {
        // @recomp Increase capacity of the weather effect vector to accomodate for more particles on screen.
        D_80381030 = func_802F8BE0(250);
        //D_80381030 = func_802F8BE0(50); //new CCW weather
    }
    return D_80381030;
}

u32 rain_particle_spawn_count = 0;

u32 get_rain_particle_id(struct4s *p) {
    u8 *padding = p->pad19;
    u32 id = (padding[0] << 16) | (padding[1] << 8) | (padding[2] << 0);
    return id;
}

void set_rain_particle_id(struct4s *p, u32 id) {
    u8 *padding = p->pad19;
    padding[0] = (id >> 16) & 0xFF;
    padding[1] = (id >> 8) & 0xFF;
    padding[2] = (id >> 0) & 0xFF;
}

// @recomp Patched to assign an interpolation ID to the rain particle when it's created.
RECOMP_PATCH void func_802F7EB0(struct3s *this) {
    f32 plyrPos[3]; //sp74
    f32 camNorm[3]; //sp68
    f32 camRot[3]; //sp5C
    s32 i;
    f32 tmpf;
    struct4s *sp50;
    f32 sp4C[3];


    if (vector_size(this->unk20) >= this->unk24)
        return;

    player_getPosition(plyrPos);
    viewport_getLookVector(camNorm);
    viewport_getRotation_vec3f(camRot);
    sp50 = vector_pushBackNew(&this->unk20);
    tmpf = randf2(50.0f, 1100.0f);
    sp4C[0] = 0.0f;
    sp4C[1] = randf2(200.0f, 300.0f);
    sp4C[2] = -tmpf;

    // @recomp Force particles to spawn 360 degrees around the player at all times.
    ml_vec3f_yaw_rotate_copy(sp4C, sp4C, randf2(0.0f, 360.0f));

#if 0
    if (gu_sqrtf(this->unk10[0] * this->unk10[0] + this->unk10[1] * this->unk10[1] + this->unk10[2] * this->unk10[2]) < 5.0f) {
        ml_vec3f_yaw_rotate_copy(sp4C, sp4C, randf2(0.0f, 360.0f));
    }
    else {
        ml_vec3f_yaw_rotate_copy(sp4C, sp4C, camRot[1] + randf2(-70.0f, 70.0f));
    }
#endif

    sp4C[0] = plyrPos[0] + sp4C[0];
    sp4C[1] = plyrPos[1] + sp4C[1];
    sp4C[2] = plyrPos[2] + sp4C[2];
    if (tmpf < 550.0)
        for (i = 0; (i < 0xa) && viewport_isPointPlane_3f(sp4C[0], sp4C[1] - 10.0f, sp4C[2]); i++) {
            sp4C[1] += 100.0f;
        }

    sp50->unk0[0] = sp4C[0];
    sp50->unk0[1] = sp4C[1];
    sp50->unk0[2] = sp4C[2];
    sp50->unkC[0] = 0.0f;
    sp50->unkC[1] = randf2(-1600.0f, -1500.0f);
    sp50->unkC[2] = 0.0f;

    // @recomp Set the rain particle's ID based on the particle spawn count and increment it.
    set_rain_particle_id(sp50, rain_particle_spawn_count++);
    if (rain_particle_spawn_count >= RAIN_PARTICLE_ID_MAX) {
        rain_particle_spawn_count = 0;
    }

    // @recomp Increase the spawn rate of the particles by calling the function recursively again.
    // Infinite recursion is prevented by keeping track of the current recursion depth. To control
    // the frequency of the spawn rate, an additional counter is used to spawn an extra particle
    // every N amount of frames.
    const u32 spawn_rate_frequency = 2;
    static u32 recursion_depth = 0;
    if (recursion_depth == 0) {
        static u32 spawn_rate_counter = 0;
        spawn_rate_counter++;

        if (spawn_rate_counter == spawn_rate_frequency) {
            recursion_depth++;
            func_802F7EB0(this);
            recursion_depth--;
            spawn_rate_counter = 0;
        }
    }
}

// @recomp Patched to tag the rain particle for interpolation.
RECOMP_PATCH void func_802F8110(struct3s *this, Gfx **gdl, Mtx **mptr, u32 arg3) {
    struct4s *startPtr; //sp4c
    struct4s *endPtr;
    struct4s *iPtr;

    startPtr = vector_getBegin(this->unk20);
    endPtr = vector_getEnd(this->unk20);
    for (iPtr = startPtr; iPtr < endPtr; iPtr++) {
        modelRender_setDepthMode(MODEL_RENDER_DEPTH_COMPARE);

        // @recomp Set the model transform ID before drawing the rain particle.
        cur_drawn_model_transform_id = RAIN_PARTICLE_TRANSFORM_ID_START + RAIN_PARTICLE_ID_COUNT * get_rain_particle_id(iPtr);

        modelRender_draw(gdl, mptr, iPtr, 0, 1.0f, 0, this->unk2C);

        // @recomp Clear the model transform ID after drawing the rain particle.
        cur_drawn_model_transform_id = 0;

        iPtr->unk18 = func_8033A170();
    }
}

// @recomp Patched to increase the capacity of the rain particle vector.
RECOMP_PATCH struct3s *func_802F7C7C(void) {
    if (D_80381034 == NULL) {
        // @recomp Increase capacity of the rain particle vector to accomodate for more droplets on screen.
        D_80381034 = func_802F8264(300);
        //D_80381034 = func_802F8264(30); //rain
    }
    return D_80381034;
}
