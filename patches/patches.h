#ifndef __PATCHES_H__
#define __PATCHES_H__

#define RECOMP_EXPORT __attribute__((section(".recomp_export")))
#define RECOMP_PATCH __attribute__((section(".recomp_patch")))
#define RECOMP_FORCE_PATCH __attribute__((section(".recomp_force_patch")))
#define RECOMP_DECLARE_EVENT(func) \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") \
    __attribute__((noinline, weak, used, section(".recomp_event"))) void func {} \
    _Pragma("GCC diagnostic pop")

#define osInvalDCache osInvalDCache_recomp
#define osInvalICache osInvalICache_recomp
#define osWritebackDCache osWritebackDCache_recomp
#define osWriteBackDCacheAll osWritebackDCacheAll_recomp
#define osSendMesg osSendMesg_recomp
#define osRecvMesg osRecvMesg_recomp
#define osGetCount osGetCount_recomp
#define osCreateMesgQueue osCreateMesgQueue_recomp
void osWriteBackDCacheAll(void);
#define bzero bzero_recomp
#define bcopy bcopy_recomp
#define osDpSetStatus osDpSetStatus_recomp
#define malloc malloc_recomp
#define free free_recomp
#define realloc realloc_recomp
#define memcpy memcpy_recomp
#define strchr strchr_recomp
#define strlen strlen_recomp
#define osVirtualToPhysical osVirtualToPhysical_recomp
#define osPiStartDma osPiStartDma_recomp
#define sinf sinf_recomp
#define cosf __cosf_recomp
#define osContGetReadData osContGetReadData_recomp

#include "ultra64.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvisibility"
#include "structs.h"
#pragma GCC diagnostic pop
#include "rt64_extended_gbi.h"

#ifndef ARRLEN
#   define ARRLEN(x) ((s32)(sizeof(x) / sizeof(x[0])))
#endif

#ifndef gEXFillRectangle
#define gEXFillRectangle(cmd, lorigin, rorigin, ulx, uly, lrx, lry) \
    G_EX_COMMAND2(cmd, \
        PARAM(RT64_EXTENDED_OPCODE, 8, 24) | PARAM(G_EX_FILLRECT_V1, 24, 0), \
        PARAM(lorigin, 12, 0) | PARAM(rorigin, 12, 12), \
        \
        PARAM((ulx) * 4, 16, 16) | PARAM((uly) * 4, 16, 0), \
        PARAM((lrx) * 4, 16, 16) | PARAM((lry) * 4, 16, 0) \
    )
#endif

#define gEXMatrixGroupSkipAll(cmd, id, push, proj, edit) \
    gEXMatrixGroup(cmd, id, G_EX_INTERPOLATE_SIMPLE, push, proj, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP, G_EX_ORDER_LINEAR, edit, G_EX_ASPECT_AUTO, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP)

#define gEXMatrixGroupSkipAllAspect(cmd, id, push, proj, edit, aspect) \
    gEXMatrixGroup(cmd, id, G_EX_INTERPOLATE_SIMPLE, push, proj, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP, G_EX_ORDER_LINEAR, edit, aspect, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP)

#define gEXMatrixGroupSimpleNormal(cmd, id, push, proj, edit) \
    gEXMatrixGroup(cmd, id, G_EX_INTERPOLATE_SIMPLE, push, proj, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, edit, G_EX_ASPECT_AUTO, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO)
    
#define gEXMatrixGroupSimpleVerts(cmd, id, push, proj, edit) \
    gEXMatrixGroup(cmd, id, G_EX_INTERPOLATE_SIMPLE, push, proj, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, edit, G_EX_ASPECT_AUTO, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_AUTO)

#define gEXMatrixGroupSimpleVertsSkipRot(cmd, id, push, proj, edit) \
    gEXMatrixGroup(cmd, id, G_EX_INTERPOLATE_SIMPLE, push, proj, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, edit, G_EX_ASPECT_AUTO, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_AUTO)

#define gEXMatrixGroupDecomposedNormal(cmd, id, push, proj, edit) \
    gEXMatrixGroupDecomposed(cmd, id, push, proj, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, edit, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO)

#define gEXMatrixGroupDecomposedSkipRot(cmd, id, push, proj, edit) \
    gEXMatrixGroupDecomposed(cmd, id, push, proj, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, edit, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO)

#define gEXMatrixGroupDecomposedSkipPosRot(cmd, id, push, proj, edit) \
    gEXMatrixGroupDecomposed(cmd, id, push, proj, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, edit, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO)

#define gEXMatrixGroupDecomposedVerts(cmd, id, push, proj, edit) \
    gEXMatrixGroupDecomposed(cmd, id, push, proj, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, edit, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_AUTO)

#define gEXMatrixGroupDecomposedVertsSkipRot(cmd, id, push, proj, edit) \
    gEXMatrixGroupDecomposed(cmd, id, push, proj, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, edit, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_AUTO)

#define gEXMatrixGroupDecomposedVertsOrderAuto(cmd, id, push, proj, edit) \
    gEXMatrixGroupDecomposed(cmd, id, push, proj, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_AUTO, edit, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_AUTO)

typedef int bool;

typedef unsigned size_t;
void memcpy(void * dst, void *src, int size);
char* strchr(const char* s, int c);
int recomp_printf(const char* fmt, ...);

void set_additional_model_scale(f32 x, f32 y, f32 z);
void set_frustum_checks_enabled(int enabled);
void set_all_interpolation_skipped(bool skipped);
bool all_interpolation_skipped();
bool perspective_interpolation_skipped();

#endif
