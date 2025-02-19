#ifndef __BK_API_H__
#define __BK_API_H__

#include "ultra64.h"
#include "enums.h"
#include "prop.h"

typedef u32 ActorExtensionId;

ActorExtensionId bkrecomp_extend_actor(enum actor_e type, u32 size);
ActorExtensionId bkrecomp_extend_actor_all(u32 size);

void* bkrecomp_get_extended_actor_data(Actor* actor, ActorExtensionId extension);
u32 bkrecomp_get_actor_spawn_index(Actor* actor);

#endif
