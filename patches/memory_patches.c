#include "patches.h"

#define AUDIO_HEAP_SIZE VER_SELECT(0x21000, 0x23A00, 0x21000, 0x21000)
extern s32 osViClock; //0x80277128
extern ALHeap D_8027CFF0;
extern u8 *D_8027D000;

u8 recomp_audio_heap[AUDIO_HEAP_SIZE];

void audioManager_create(void);
void sfxInstruments_init(void);
void musicInstruments_init(void);
void audioManager_startThread(void);

// @recomp Patched to move the audio heap out of the main heap so that the main heap has less pressure.
RECOMP_PATCH void audioManager_init(void){
    // @recomp Allocate the audio heap out of a static buffer.    
    // D_8027D000 = (u8 *) malloc(AUDIO_HEAP_SIZE);
    D_8027D000 = recomp_audio_heap;
    bzero(D_8027D000, AUDIO_HEAP_SIZE);
    alHeapInit(&D_8027CFF0, D_8027D000, AUDIO_HEAP_SIZE);
#if VERSION == VERSION_USA_1_0
    if(osTvType != OS_TV_NTSC)
        osViClock = 0x2e6025c;
#elif VERSION == VERSION_PAL
    osViClock = 0x2f5b2d2;
#endif
    audioManager_create();
    sfxInstruments_init();
    musicInstruments_init();
    audioManager_startThread();
}

extern u16 *D_80276CB8; //! ml_acosPrecValTblPtr

u16 recomp_sine_table[10001];

// @recomp Patched to move the sine table out of the heap to reduce heap pressure.
RECOMP_PATCH void ml_init(void)
{
    u16 i;

    // Allocate table
    // @recomp Change the table to be in a statically declared array.
    // D_80276CB8 = (u16 *)malloc(10001 * sizeof(u16));
    D_80276CB8 = recomp_sine_table;

    // Generate all entries in the table
    for (i = 0; i < 10001; i++)
    {
        // Save value
        D_80276CB8[i] = sinf(i * 90.0 / 10000 * M_PI / 180) * 65535.f;
    }
}

RECOMP_PATCH void ml_free(void)
{
    // @recomp Can't free a statically declared array.
    // free(D_80276CB8);
    D_80276CB8 = NULL;
}