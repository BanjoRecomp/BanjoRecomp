#include "patches.h"

typedef struct Cube_s Cube;

RECOMP_PATCH bool viewport_cube_isInFrustum(Cube *cube) {
    // f32 sp24[3];
    // f32 sp18[3];

    // sp24[0] = (f32) ((cube->x * 1000) - 150);
    // sp24[1] = (f32) ((cube->y * 1000) - 150);
    // sp24[2] = (f32) ((cube->z * 1000) - 150);
    // sp18[0] = sp24[0] + 1300.0f;
    // sp18[1] = sp24[1] + 1300.0f;
    // sp18[2] = sp24[2] + 1300.0f;
    // return func_8024D374(sp24, sp18);
    return TRUE;
}

RECOMP_PATCH bool viewport_cube_isInFrustum2(Cube *cube) {
    // f32 sp34[3];
    // f32 sp28[3];
    // f32 sp1C[3];

    // if (cube->x == -0x10) {
    //     return TRUE;
    // }
    // sp1C[0] = (f32) ((cube->x * 1000) + 500) - viewportPosition[0];
    // sp1C[1] = (f32) ((cube->y * 1000) + 500) - viewportPosition[1];
    // sp1C[2] = (f32) ((cube->z * 1000) + 500) - viewportPosition[2];
    // if (LENGTH_SQ_VEC3F(sp1C) > 1.6e7f) {
    //     return FALSE;
    // }
    // sp34[0] = (f32) ((cube->x * 1000) - 150);
    // sp34[1] = (f32) ((cube->y * 1000) - 150);
    // sp34[2] = (f32) ((cube->z * 1000) - 150);
    // sp28[0] = sp34[0] + 1300.0f;
    // sp28[1] = sp34[1] + 1300.0f;
    // sp28[2] = sp34[2] + 1300.0f;
    // return func_8024D374(sp34, sp28);
    return TRUE;
}

RECOMP_PATCH bool viewport_func_8024DB50(f32 arg0[3], f32 arg1) {
    // f32 sp3C[3];
    // s32 i;

    // sp3C[0] = arg0[0] - viewportPosition[0];
    // sp3C[1] = arg0[1] - viewportPosition[1];
    // sp3C[2] = arg0[2] - viewportPosition[2];
    // for(i = 0; i < 4; i++){
    //     if(arg1 <= ml_dotProduct_vec3f(sp3C, D_80280ED0[i])){
    //         return FALSE;
    //     }
    // }
    return TRUE;
}

RECOMP_PATCH bool viewport_isBoundingBoxInFrustum(f32 arg0[3], f32 arg1[3]) {
    return TRUE;
}
