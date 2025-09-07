#ifndef __TRANSFORM_IDS_H__
#define __TRANSFORM_IDS_H__

#include "PR/ultratypes.h"

#define MAP_MODEL_OPA_TRANSFORM_ID_START 0x1000
#define MAP_MODEL_XLU_TRANSFORM_ID_START 0x2000

#define MARKER_TRANSFORM_ID_COUNT 256 // Number of transform IDs for each ActorMarker.
#define BANJO_TRANSFORM_ID_START 0x01000000
#define MARKER_TRANSFORM_ID_START (BANJO_TRANSFORM_ID_START + MARKER_TRANSFORM_ID_COUNT)

extern u32 cur_drawn_model_transform_id;

#endif
