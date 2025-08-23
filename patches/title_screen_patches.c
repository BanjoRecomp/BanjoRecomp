#include "patches.h"

#include "enums.h"

struct Struct_core2_9B180_1;
typedef struct Struct_core2_9B180_1 Struct_core2_9B180_1;
struct struct_core2_9B180_s;

typedef struct struct_core2_9B180_s{
    s16 unk0;
    // u8 pad2[0x2];
    Struct_core2_9B180_1 *unk4;
    void (*unk8)(struct struct_core2_9B180_s *);
    void (*unkC)(struct struct_core2_9B180_s *);
    void (*unk10)(struct struct_core2_9B180_s *);
}Struct_core2_9B180_0;

extern u8 D_80383330;
extern Struct_core2_9B180_0 D_8036DE00[];

void func_80322318(Struct_core2_9B180_0*);
int map_get(void);

// @recomp Patched to always allow skipping the intro sequence.
RECOMP_PATCH void func_80322490(void) {
    Struct_core2_9B180_0 *i_ptr;

    if (D_80383330 != 0) {
        for(i_ptr = D_8036DE00; i_ptr != &D_8036DE00[6]; i_ptr++){
            // @recomp Always allow skipping the intro sequence.
            if((i_ptr->unk4 != 0 || (i_ptr->unkC == func_80322318 && map_get() == MAP_1F_CS_START_RAREWARE)) && i_ptr->unkC != NULL){
                i_ptr->unkC(i_ptr);
            }
        }
    }
}