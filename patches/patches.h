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
#define osDpSetStatus osDpSetStatus_recomp
#define malloc malloc_recomp
#define free free_recomp
#define realloc realloc_recomp
#define memcpy memcpy_recomp

#include "ultra64.h"

typedef int bool;

int recomp_printf(const char* fmt, ...);

#endif
