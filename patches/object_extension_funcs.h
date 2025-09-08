#ifndef __MEM_FUNCS_H__
#define __MEM_FUNCS_H__

#include "patch_helpers.h"

DECLARE_FUNC(u32, recomp_register_object_extension, u32 type_index, u32 subtype_index, u32 size);
DECLARE_FUNC(u32, recomp_register_object_extension_generic, u32 type_index, u32 size);
DECLARE_FUNC(void, recomp_clear_all_object_data, u32 type_index);
DECLARE_FUNC(u32, recomp_create_object_data, u32 type_index, u32 subtype_index);
DECLARE_FUNC(void, recomp_destroy_object_data, u32 type_index, u32 object_handle);
DECLARE_FUNC(void*, recomp_get_object_data, u32 type_index, u32 subtype_index, u32 object_handle, u32 extension_handle);
DECLARE_FUNC(u32, recomp_get_object_spawn_index, u32 type_index, u32 object_handle);

#endif
