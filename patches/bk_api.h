#ifndef __BK_API_H__
#define __BK_API_H__

#include "ultra64.h"
#include "enums.h"
#include "prop.h"

typedef u32 MarkerExtensionId;

MarkerExtensionId bkrecomp_extend_marker(enum marker_e type, u32 size);
MarkerExtensionId bkrecomp_extend_marker_all(u32 size);

void* bkrecomp_get_extended_marker_data(ActorMarker* marker, MarkerExtensionId extension);
u32 bkrecomp_get_marker_spawn_index(ActorMarker* marker);

#endif
