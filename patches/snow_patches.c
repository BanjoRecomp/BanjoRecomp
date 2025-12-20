#include "patches.h"
#include "transform_ids.h"
#include "functions.h"

typedef struct {
    u8 pad0[0xC];
    f32 unkC[3];
}Struct_core2_72060_0;

extern s32 D_80369284;
extern s32 D_8036928C;
extern f32 D_80381040[3];
extern f32 D_8038104C;
extern f32 D_80381050[3];
extern f32 D_80381060[3];
extern f32 D_80381070[3];
extern f32 D_80381080[3];
extern f32 D_8038108C;
extern struct4Cs *D_80369280;
extern Struct_core2_72060_0 *D_80381094;
extern Gfx *D_80381090;
extern BKModelBin *D_80369288;

extern bool func_802BEF64(void);
extern void func_802F8FFC(void);
extern void func_8033BD20(BKModelBin **arg0);
extern s32 globalTimer_getTime(void);

#define SNOW_PARTICLE_COUNT 256
#define SNOW_SKIP_INTERPOLATION_MASK 0x8000

u16 snowIDArray[SNOW_PARTICLE_COUNT];
u16 snowIDQueue[SNOW_PARTICLE_COUNT];
u16 snowIDQueueCount = 0;
struct4Ds snowData[SNOW_PARTICLE_COUNT];

// @recomp Patched to initialize the ID queue for snow particles.
RECOMP_PATCH void func_802F9054(void) {
    func_802F8FFC();
    D_80369280 = (struct4Cs *)malloc(sizeof(struct4Cs));
    D_80369280->unk0[0] = D_80369280->unk0[1] = D_80369280->unk0[2] = 0.0f;
    D_80369280->unkC[0] = D_80369280->unkC[1] = D_80369280->unkC[2] = 0.0f;
    D_8036928C = 0;
    D_80369280->unk1C = snowData;
    D_80369280->unk18 = 0;
    D_80369288 = assetcache_get(0x8a1); //2D_light

    // @recomp Initialize the snow ID queue.
    while (snowIDQueueCount < SNOW_PARTICLE_COUNT) {
        snowIDQueue[snowIDQueueCount] = snowIDQueueCount;
        snowIDQueueCount++;
    }
}

// @recomp Patched to not free the vector.
RECOMP_PATCH void func_802F8FFC(void) {
    if (D_80369280) {
        // @recomp Don't free the vector.
        // free(D_80369280->unk1C);

        func_8033BD20(&D_80369288);
        free(D_80369280);
        D_80369280 = NULL;
        D_80369284 = 0;
    }
}

// @recomp Patched to copy the ID when the snow particle is removed.
RECOMP_PATCH void func_802F9134(s32 gfx) {
    // @recomp Put the deleted ID back on the queue.
    snowIDQueue[snowIDQueueCount++] = snowIDArray[gfx];

    D_80369284 = D_80369284 - 1;
    if (gfx < D_80369284) {
        // @recomp Modified to just be a regular memcpy.
        memcpy(D_80369280->unk1C + gfx, D_80369280->unk1C + D_80369284, sizeof(struct4Ds));

        // @recomp Replicates the copy on the ID array.
        snowIDArray[gfx] = snowIDArray[D_80369284];
    }
}

// @recomp Patched to increase the amount of snow particles, the spawn area and assign transform IDs to them.
RECOMP_PATCH void func_802F919C(void) {
    f32 temp_f20;
    s32 sp68;
    s32 var_v1;
    s32 sp60;
    struct4Ds *sp5C;
    f32 sp58;
    f32 sp4C[3];
    f32 var_f20;
    s32 sp44;
    struct4Ds *sp40;


    if (D_80369280 != NULL) {
        if (func_802BEF64() != 0) {
            D_80369284 = 0;
            return;
        }
        temp_f20 = time_getDelta();
        sp60 = (globalTimer_getTime() & 1) * 2;
        player_getPosition(D_80381040);

        D_80369280->unkC[0] = D_80381040[0] - D_80369280->unk0[0];
        D_80369280->unkC[1] = D_80381040[1] - D_80369280->unk0[1];
        D_80369280->unkC[2] = D_80381040[2] - D_80369280->unk0[2];
        D_80369280->unk0[0] = D_80381040[0];
        D_80369280->unk0[1] = D_80381040[1];
        D_80369280->unk0[2] = D_80381040[2];
        D_8038104C = D_80381040[1] - 300.0f;

        for (sp68 = 0; sp68 < D_80369284; sp68++) {
            sp5C = D_80369280->unk1C + sp68;
            for (var_v1 = 0; var_v1 < 3; var_v1++) {
                sp5C->unk0[var_v1] += sp5C->unkC[var_v1] * temp_f20;
            }

            sp5C->unkC[sp60] += ((randf() * 30.0) - 15.0);
        }

        D_8036928C++;
        if (D_8036928C < D_80369284) {
            sp5C = &D_80369280[0].unk1C[D_8036928C];
            if (ml_vec3f_distance((*sp5C).unk0, D_80381040) > 1300.0f) {
                func_802F9134(D_8036928C);
            }
        }
        else {
            D_8036928C = 0;
        }
        if (D_80369280->unk18 == 1) {
            // @recomp Patched to use the count from the macro instead and spawn multiple snow particles at once.
            u32 particlesToSpawn = 2;
            while (D_80369284 < SNOW_PARTICLE_COUNT && particlesToSpawn > 0) {
                sp40 = D_80369280->unk1C + D_80369284;

                // @recomp Assign a new ID to this particle. Also mark it to skip interpolation.
                snowIDArray[D_80369284] = snowIDQueue[--snowIDQueueCount] | SNOW_SKIP_INTERPOLATION_MASK;
                particlesToSpawn--;

                D_80369284++;
                sp58 = randf2(100.0f, 1300.0f);
                sp4C[0] = 0.0f;
                sp4C[1] = randf() * 200.0f + 200.0f;
                sp4C[2] = -sp58;
                if ((D_80369280->unkC[0] * D_80369280->unkC[0] + D_80369280->unkC[1] * D_80369280->unkC[1] + D_80369280->unkC[2] * D_80369280->unkC[2]) < 25.0f) {
                    var_f20 = 100.0f;
                }
                else {
                    var_f20 = 70.0f;
                }

                // @recomp Use the full 360 degrees instead.
                var_f20 = 180.0f;

                ml_vec3f_yaw_rotate_copy(sp4C, sp4C, viewport_getYaw() + randf2(-var_f20, var_f20));
                sp4C[0] += D_80381040[0];
                sp4C[1] += D_80381040[1];
                sp4C[2] += D_80381040[2];
                if (sp58 < 650.0) {
                    for (sp44 = 0; sp44 < 5 && viewport_isPointPlane_3f(sp4C[0], sp4C[1] - 10.0f, sp4C[2]); sp44++) {
                        sp4C[1] += 200.0f;
                    }
                }
                sp40->unk0[0] = sp4C[0];
                sp40->unk0[1] = sp4C[1];
                sp40->unk0[2] = sp4C[2];
                sp40->unkC[0] = 0.0f;
                sp40->unkC[1] = randf2(-150.0f, -50.0f);
                sp40->unkC[2] = 0.0f;
            }
        }
    }
}

// @recomp Patched to remove the frustum check and tag the snow particle with its assigned transform ID.
RECOMP_PATCH bool func_802F989C(Gfx **gfx, Mtx **mtx, f32 arg2[3]) {
    D_80381070[0] = arg2[0] - D_80381050[0]; \
        D_80381070[1] = arg2[1] - D_80381050[1]; \
        D_80381070[2] = arg2[2] - D_80381050[2];
    if (((-17000.0f < D_80381070[0]) && (D_80381070[0] < 17000.0f))
        && (arg2[1] > -200.0f)
        && ((-17000.0f < D_80381070[2]) && (D_80381070[2] < 17000.0f))
        // @recomp Remove frustum check.
        //&& viewport_func_8024DB50(arg2, D_8038108C, aspect_ratio_scale)
        ) {
        func_80251B5C(D_80381070[0], D_80381070[1], D_80381070[2]);
        mlMtxApply(*mtx);
        mlMtx_apply_vec3f_restricted(&D_80381080, D_80381094->unkC);
        func_80251B5C(D_80381080[0], D_80381080[1], D_80381080[2]);
        mlMtx_rotate_yaw_deg(D_80381060[1]);
        mlMtx_rotate_pitch_deg(D_80381060[0]);
        func_80252A38(-(D_80381094->unkC[0]), -(D_80381094->unkC[1]), -(D_80381094->unkC[2]));
        mlMtxApply(*mtx);

        // @recomp Set a matrix group before drawing the snow particle. If the interpolation skip bit is enabled as the snow particle was just
        // created, do not interpolate and clear the bit instead.
        s32 snowIndex = (struct struct_4D_s *)(arg2)-D_80369280->unk1C;
        u32 snowId = SNOW_TRANSFORM_ID_START + snowIDArray[snowIndex];
        if (snowIDArray[snowIndex] & SNOW_SKIP_INTERPOLATION_MASK) {
            gEXMatrixGroupSkipAll((*gfx)++, snowId, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
            snowIDArray[snowIndex] ^= SNOW_SKIP_INTERPOLATION_MASK;
        }
        else {
            gEXMatrixGroupSimpleNormal((*gfx)++, snowId, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
        }

        gSPMatrix((*gfx)++, (*mtx)++, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList((*gfx)++, osVirtualToPhysical(D_80381090));
        gSPPopMatrix((*gfx)++, G_MTX_MODELVIEW);

        // @recomp Clear the matrix group.
        gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);

        return TRUE;
    }
    return FALSE;
}