#include "patches.h"
#include "prop.h"
#include "actor.h"
#include "functions.h"
#include "object_extension_funcs.h"
#include "bk_api.h"
#include "note_saving.h"

// Array of handles for ActorMarker instances.
// Normally the game only has at most 0xE0 ActorMarker instances, but this is larger to account for mods increasing
// the ActorMarker count.
u32 marker_handles[0x1000];

extern ActorMarker *D_8036E7C8;
extern u8 D_80383428[0x1C];
void func_8032F3D4(s32 arg0[3], ActorMarker *marker, s32 arg2);
ActorMarker * func_80332A60(void);

extern Actor *suLastBaddie;

// @recomp Patched to create extension data for the marker.
RECOMP_PATCH ActorMarker * marker_init(s32 *pos, MarkerDrawFunc draw_func, int arg2, int marker_id, int arg4){
    ActorMarker * marker = func_80332A60();
    marker->propPtr = NULL;
    marker->cubePtr = NULL;
    marker->drawFunc = draw_func;
    marker->id = marker_id;
    marker->unk40_23 = arg4;
    func_8032F3D4(pos, marker, arg2);
    marker->actrArrayIdx = 0;
    marker->unk14_10 = 0;
    marker->modelId = 0;
    marker->isBanjoOnTop = 0;
    marker->unk14_22 = 0;
    marker->unk14_21 = 0;
    marker->yaw = 0;
    marker->pitch =  0;
    marker->roll = 0;
    marker->unk2C_2 = 0;
    marker->unk2C_1 = 0;
    marker->collidable = TRUE;
    marker->unk3E_0 = 0; 
    marker->unk40_22 = 0;
    marker->unk40_19 = 0;
    marker->unk40_21 = 0;
    marker->collisionFunc = NULL;
    marker->collision2Func = NULL;
    marker->dieFunc = NULL;
    marker->unk54 = NULL;
    marker->unk58 = 0;
    marker->unk18 = 0;
    marker->actorUpdateFunc = NULL;
    marker->actorFreeFunc = NULL;
    marker->commonParticleIndex = 0;
    marker->actorUpdate2Func = NULL;
    marker->unk38[0] = 0;
    marker->unk38[1] = 0;
    marker->unk38[2] = 0;
    marker->unk44 = 0;
    marker->unk20 = 0;
    marker->unk50 = 0;
    marker->unk48 = 0;
    marker->unk4C = 0;
    marker->unk40_20 = 0;
    marker->unk40_31 = 0;

    // @recomp Set the marker's handle.
    u32 index = marker - D_8036E7C8;
    marker_handles[index] = recomp_create_object_data(EXTENSION_TYPE_MARKER, marker_id);
    
    // @recomp If this is a note marker, initialize it. Make sure that this marker belongs to suLastBaddie.
    if (marker->id == MARKER_5F_MUSIC_NOTE && suLastBaddie && suLastBaddie->actor_info->markerId == marker_id) {
        note_saving_handle_dynamic_note(suLastBaddie, marker);
    }

    return marker;
}

// @recomp Patched to destroy the marker's extended data.
RECOMP_PATCH void func_80332B2C(ActorMarker * arg0){
    static s32 D_8036E804[8] = {(u8)~(1 << 7), (u8)~(1 << 6), (u8)~(1 << 5), (u8)~(1 << 4), (u8)~(1 << 3), (u8)~(1 << 2), (u8)~(1 << 1), (u8)~(1 << 0)};

    s32 index = (arg0 - D_8036E7C8);
    arg0->unk5C = 0;
    D_80383428[index >> 3] =  D_80383428[index >> 3] & D_8036E804[index & 7];

    // @recomp Delete the handle for this marker.
    recomp_destroy_object_data(EXTENSION_TYPE_MARKER, marker_handles[index]);
    marker_handles[index] = 0;
}

RECOMP_EXPORT MarkerExtensionId bkrecomp_extend_marker(enum marker_e type, u32 size) {
    return recomp_register_object_extension(EXTENSION_TYPE_MARKER, type, size);
}

RECOMP_EXPORT MarkerExtensionId bkrecomp_extend_marker_all(u32 size) {
    return recomp_register_object_extension_generic(EXTENSION_TYPE_MARKER, size);
}

RECOMP_EXPORT void* bkrecomp_get_extended_marker_data(ActorMarker* marker, MarkerExtensionId extension) {
    s32 index = (marker - D_8036E7C8);
    u32 handle = marker_handles[index];
    return recomp_get_object_data(EXTENSION_TYPE_MARKER, marker->id, handle, extension);
}

RECOMP_EXPORT u32 bkrecomp_get_marker_spawn_index(ActorMarker* marker) {
    s32 index = (marker - D_8036E7C8);
    u32 handle = marker_handles[index];
    return recomp_get_object_spawn_index(EXTENSION_TYPE_MARKER, handle);
}
