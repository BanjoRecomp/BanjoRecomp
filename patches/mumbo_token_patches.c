#include "patches.h"
#include "prop.h"

enum map_e map_get(void);
s32 func_80307164(s32 position[3]);
s32 func_80306DBC(s32 arg0);

bool mumbo_token_fixes_enabled = TRUE;

RECOMP_EXPORT void bkrecomp_set_mumbo_token_fixes_enabled(bool val) {
    mumbo_token_fixes_enabled = val;
}

// @recomp Patched to replace the IDs of duplicate tokens.
RECOMP_PATCH enum mumbotoken_e func_802E0A90(Actor *this){
    s32 id;
    s32 pos[3];

    // @recomp Record the map ID.
    enum map_e map_id = map_get();
    pos[0] = (s32)this->position[0];
    pos[1] = (s32)this->position[1];
    pos[2] = (s32)this->position[2];
    id = func_80307164(pos);
    if(id < 0){
        return 0;
    }
    else{
        // @recomp Replace the IDs of duplicate tokens.
        // return func_80306DBC(id) - 199;
        s32 ret = func_80306DBC(id) - 199;

        // @recomp Only replace token IDs if the fixes are enabled.
        if (mumbo_token_fixes_enabled) {
            // @recomp Replace the ID for the following token:
            // Mumbo token ID: 0x3D @ 424, 170, 304 (Map 0x8D)
            if (ret == 0x3D && pos[0] == 424 && pos[1] == 170 && pos[2] == 304 && map_id == MAP_8D_MMM_INSIDE_LOGGO) {
                // @recomp Swap to unused ID 0x74
                ret = 0x74;
            }

            // @recomp Replace the ID for the following token:
            // Mumbo token ID: 0x5E @ -2649, 0, -395 (Map 0x43)
            if (ret == 0x5E && pos[0] == -2649 && pos[1] == 0 && pos[2] == -395 && map_id == MAP_43_CCW_SPRING) {
                // @recomp Swap to unused ID 0x5D
                ret = 0x5D;
            }
        }

        // @recomp Return the modified ID.
        return ret;
    }
}
