#include "patches.h"
#include "functions.h"

extern vector(struct21s) *D_80383CE0[2];

#define PRINT_DEFRAG_CHANGES 0

// @recomp Patched to just use a normal DMA.
RECOMP_PATCH s32 boot_osPiRawStartDma(s32 direction, u32 devAddr, void *dramAddr, u32 size) {
    OSMesgQueue mq;
    OSMesg buffer;
    OSIoMesg io;
    osCreateMesgQueue(&mq, &buffer, 1);

	// Complete the DMA synchronously (the game immediately waits until it's done anyways)
    osPiStartDma(&io, OS_MESG_PRI_HIGH, direction, devAddr, dramAddr, size, &mq);
    osRecvMesg(&mq, NULL, OS_MESG_BLOCK);

    return 0;
}

// @recomp Patched to not read hardware registers.
RECOMP_PATCH s32 boot_osPiGetStatus() {
	// PI not busy
	return 0;
}

// @recomp Patched to fix a bug where defrag was used instead of vector_defrag, which would break these vectors
// if the defrag call moved the vector's memory. 
RECOMP_PATCH void func_8033B268(void) {
#if PRINT_DEFRAG_CHANGES
    vector(struct21s) *old_0 = D_80383CE0[0];
    vector(struct21s) *old_1 = D_80383CE0[1];
#endif

    D_80383CE0[0] = (vector(struct21s) *)vector_defrag(D_80383CE0[0]);
    D_80383CE0[1] = (vector(struct21s) *)vector_defrag(D_80383CE0[1]);

#if PRINT_DEFRAG_CHANGES
    if (D_80383CE0[0] != old_0) {
        recomp_printf("vector_defrag() moved D_80383CE0[0] from 0x%08X to 0x%08X.\n", old_0, D_80383CE0[0]);
    }
    
    if (D_80383CE0[1] != old_1) {
        recomp_printf("vector_defrag() moved D_80383CE0[1] from 0x%08X to 0x%08X.\n", old_1, D_80383CE0[1]);
    }
#endif
}