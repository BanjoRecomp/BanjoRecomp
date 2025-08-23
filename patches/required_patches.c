#include "patches.h"

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
