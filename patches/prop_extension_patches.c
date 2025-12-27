#include "patches.h"
#include "prop.h"
#include "actor.h"
#include "functions.h"
#include "object_extension_funcs.h"
#include "bk_api.h"
#include "core2/coords.h"
#include "core2/file.h"
#include "note_saving.h"

// Max props per cube, limited by cube->prop2Cnt which is only 6 bits.
#define CUBE_MAX_PROPS 63
// Hardcoded cube limit, TODO implement a better solution that doesn't involve dedicating this much memory and allows for larger cube counts.
#define MAX_CUBES 4000

typedef struct {
    u32 prop_handles[CUBE_MAX_PROPS];
} CubeHandle;

CubeHandle all_cube_handles[MAX_CUBES];
CubeHandle cube_3C_handle;
CubeHandle cube_40_handle;

extern s32 *D_8036A9E0;
extern s32 D_80383450[0x40];

void vtxList_getBounds_s32(BKVertexList *, s32[3], s32[3]);
enum map_e map_get(void);
void code7AF80_initCubeFromFile(File *file_ptr, Cube *cube);
bool func_80305D14(void);
void func_80305CD8(s32 idx, s32 count);
s32 func_803058C0(f32 arg0);
void code_A5BC0_initCubePropActorProp(Cube*);
void func_80332B2C(ActorMarker * arg0);
void bitfield_free(s32 *arg0);
void func_8032DE78(SpriteProp *sprite_prop, enum asset_e *sprite_id_ptr);

extern struct {
    Cube *cubes;
    f32 margin;
    s32 min[3];
    s32 max[3];
    s32 stride[2];
    s32 cubeCnt; 
    s32 unk2C;
    s32 width[3];
    Cube *unk3C; // fallback cube?
    Cube *unk40; // some other fallback cube?
    s32 unk44; // index of some sort
} sCubeList;

typedef struct {
    s16 map_id; //enum map_e
    s16 opa_model_id; //enum asset_e level_model_id
    s16 xlu_model_id; //enum asset_e level2_model_id
    s16 unk6[3]; // min bounds (for cubes?)
    s16 unkC[3]; // max bounds (for cubes?)
    // u8 pad12[0x2];
    f32 scale;
}MapModelDescription;

extern struct {
    void *unk0;
    void *unk4;
    BKCollisionList *collision_opa;
    BKCollisionList *collision_xlu;
    BKModel *model_opa;
    BKModel *model_xlu;
    BKModelBin *model_bin_opa;
    BKModelBin *model_bin_xlu;
    s32 unk20;
    struct5Bs *unk24;
    MapModelDescription *description;
    u8 env_red;
    u8 env_green;
    u8 env_blue;
    f32 scale;
}mapModel;

void recomp_abort(const char* msg);

// @recomp Patched to verify the cube count is less than the fixed amount.
RECOMP_PATCH void mapModel_getCubeBounds(s32 min[3], s32 max[3]) {
    vtxList_getBounds_s32(model_getVtxList(mapModel.model_bin_opa), min, max);
    coords_scale(min, max, 1000);
    min[0] = min[0] + mapModel.description->unk6[0];
    min[1] = min[1] + mapModel.description->unk6[1];
    min[2] = min[2] + mapModel.description->unk6[2];
    max[0] = max[0] + mapModel.description->unkC[0];
    max[1] = max[1] + mapModel.description->unkC[1];
    max[2] = max[2] + mapModel.description->unkC[2];

    // @recomp Calculate and validate the cube count.
    u32 width0 = max[0] - min[0] + 1;
    u32 width1 = max[1] - min[1] + 1;
    u32 width2 = max[2] - min[2] + 1;
    u32 stride0 = width0;
    u32 stride1 = stride0 * width1;
    u32 cubeCnt = stride1 * width2;

    if (cubeCnt > MAX_CUBES) {
        recomp_abort("Cube count too high\n");
    }
}

CubeHandle* get_cube_handle(Cube *cube) {
    if (cube == sCubeList.unk3C) {
        return &cube_3C_handle;
    }
    else if (cube == sCubeList.unk40) {
        return &cube_40_handle;
    }
    else {
        s32 cube_index = cube - sCubeList.cubes;
        if (cube_index < 0 || cube_index >= sCubeList.cubeCnt) {
            recomp_printf("Invalid cube index %d\n", cube_index);
            recomp_abort("Got invalid cube\n");
        }
        return &all_cube_handles[cube_index];
    }
}

// Alloc prop handles:
//   __codeA5BC0_initProp2Ptr
//   code7AF80_initCubeFromFile (covered by __code7AF80_initCubeFromFile)

// Delete prop handles:
//   func_8032D9C0
//   cube_free

// Swap prop handles:
//   __cube_sort

// Reset all prop handles candidates:
//   cubeList_free

// @recomp Patched to create a handle for the new prop.
RECOMP_PATCH Prop *__codeA5BC0_initProp2Ptr(Cube *cube) {
    Prop *sp1C;

    if (cube->prop2Ptr != NULL) {
        cube->prop2Cnt++;
        cube->prop2Ptr = realloc(cube->prop2Ptr, cube->prop2Cnt * sizeof(Prop));
    } else {
        cube->prop2Cnt = 1;
        cube->prop2Ptr = malloc(sizeof(Prop));
    }
    sp1C = &cube->prop2Ptr[cube->prop2Cnt-1];
    sp1C->is_actor = FALSE;
    code_A5BC0_initCubePropActorProp(cube);

    // @recomp Get the cube's prop handle list.
    CubeHandle* cube_handle = get_cube_handle(cube);
    u32 prop_index = cube->prop2Cnt - 1;
    // TODO prop subtypes.
    cube_handle->prop_handles[prop_index] = recomp_create_object_data(EXTENSION_TYPE_PROP, 0);

    return sp1C;
}

// @recomp Patched to initialize prop handles after a cube has been loaded.
RECOMP_PATCH void __code7AF80_initCubeFromFile(Cube *cube, File* file_ptr) {
    s32 pad[3];

    // @recomp Get the cube's prop handle list.
    CubeHandle* cube_handle = get_cube_handle(cube);

    // @recomp Free any prop handles that the cube may already have, as this initialization resets the cube's prop list.
    for (u32 i = 0; i < cube->prop2Cnt; i++) {
        recomp_destroy_object_data(EXTENSION_TYPE_PROP, cube_handle->prop_handles[i]);
        cube_handle->prop_handles[i] = 0;
    }

    while(!file_isNextByteExpected(file_ptr, 1)) {
        if (file_getNWords_ifExpected(file_ptr, 0, pad, 3)) {
            file_getNWords(file_ptr, pad, 3);
        } else if (!file_getNWords_ifExpected(file_ptr, 2, pad, 3) && file_isNextByteExpected(file_ptr, 3) 
        ) {
            code7AF80_initCubeFromFile(file_ptr, cube);
        }
    }

    // @recomp Initialize prop handles after loading the cube and handle note saving.
    for (u32 i = 0; i < cube->prop2Cnt; i++) {
        // TODO prop subtypes.
        cube_handle->prop_handles[i] = recomp_create_object_data(EXTENSION_TYPE_PROP, 0);

        // Check if this prop is a sprite prop.
        Prop *prop = &cube->prop2Ptr[i];
        if (!prop->is_3d && !prop->is_actor) {
            // Check if this sprite prop is a musical note.

            // Get the asset ID for this sprite prop.
            enum asset_e sprite_prop_asset_id;
            func_8032DE78(&prop->spriteProp, &sprite_prop_asset_id);

            if (sprite_prop_asset_id == ASSET_6D6_MODEL_MUSIC_NOTE) {
                note_saving_handle_static_note(cube, prop);
            }
        }
    }
}

// @recomp Patched to delete the handle for the deleted prop and shift the remaining prop handles.
RECOMP_PATCH s32 func_8032D9C0(Cube *cube, Prop* prop){
    s32 sp24;
    s32 tmp;

    // @recomp Get the cube's prop handle list.
    CubeHandle* cube_handle = get_cube_handle(cube);

    sp24 = 0;
    if(cube->prop2Cnt != 0){
        // @recomp Delete the handle for the deleted prop.
        u32 prop_index = prop - cube->prop2Ptr;

        sp24 = prop->is_3d; 
        if(func_80305D14()){
            func_80305CD8(func_803058C0(prop->unk4[1]), -1);
        }

        // @recomp Destroy the prop's extension data.
        recomp_destroy_object_data(EXTENSION_TYPE_PROP, cube_handle->prop_handles[prop_index]);

        if((prop - cube->prop2Ptr) < (cube->prop2Cnt - 1)){
            memcpy(prop, prop + 1, (s32)(&cube->prop2Ptr[cube->prop2Cnt-1]) - (s32)(prop));
            // @recomp Shift the prop handles back by 1 to remove the gap.
            for (u32 cur_prop_index = prop_index; cur_prop_index < cube->prop2Cnt - 1; cur_prop_index++) {
                cube_handle->prop_handles[cur_prop_index] = cube_handle->prop_handles[cur_prop_index + 1];
            }
            // @recomp Clear the handle at the end of the list to account for the list decreasing in size.
            cube_handle->prop_handles[cube->prop2Cnt - 1] = 0;
        }
        cube->prop2Cnt--;
        if(cube->prop2Cnt){
            cube->prop2Ptr = realloc(cube->prop2Ptr, cube->prop2Cnt*sizeof(Prop));
            code_A5BC0_initCubePropActorProp(cube);
        }else{
            free(cube->prop2Ptr);
            cube->prop2Ptr = NULL;
        }
        return sp24;
    }
    return 0;
}

// @recomp Patched to clear all prop handles for a cube when freeing a cube.
RECOMP_PATCH void cube_free(Cube *cube){
    Prop *iProp;

    // @recomp Get the cube's prop handle list.
    CubeHandle* cube_handle = get_cube_handle(cube);
    
    // @recomp Delete and clear the cube's prop handles.
    for (u32 prop_index = 0; prop_index < cube->prop2Cnt; prop_index++) {
        recomp_destroy_object_data(EXTENSION_TYPE_PROP, cube_handle->prop_handles[prop_index]);
        cube_handle->prop_handles[prop_index] = 0;
    }

    if(cube->prop2Ptr){
        for(iProp = cube->prop2Ptr; iProp < cube->prop2Ptr +cube->prop2Cnt; iProp++){
            if(iProp->is_actor){
                func_80332B2C(iProp->actorProp.marker);
            }
        }
        free(cube->prop2Ptr);
        cube->prop2Ptr = NULL;
    }
    if(cube->prop1Ptr){
        free(cube->prop1Ptr);
        cube->prop1Ptr = NULL;
    }
    cube->prop2Cnt = 0;
    cube->prop1Cnt = 0;
    cube->unk0_4 = 0;
}

// @recomp Patched to swap prop handles when sorting a cube's props.
RECOMP_PATCH void __cube_sort(Cube *cube, bool global) {
    s32 ref_position[3];
    Prop *var_v1;
    Prop *start_prop;
    s32 temp_a2;
    Prop *var_t1;
    Prop * var_a3;
    Prop * var_t0;
    s32 i;
    Prop *new_var;

    // @recomp Get the cube's prop handle list.
    CubeHandle* cube_handle = get_cube_handle(cube);

    if (cube->prop2Cnt >= 2) {
        if (global == 0) {
            viewport_getPosition_vec3w(ref_position); //distance from viewport
        } else {
            ref_position[0] = 0;
            ref_position[1] = 0;
            ref_position[2] = 0;
        }

        //calculate prop distances
        new_var = var_v1 = cube->prop2Ptr;
        for(i = 0; i < cube->prop2Cnt; var_v1++, i++){
            D_80383450[i] =  (var_v1->actorProp.x - ref_position[0])*(var_v1->actorProp.x - ref_position[0]) 
                               +  (var_v1->actorProp.y - ref_position[1])* (var_v1->actorProp.y - ref_position[1])
                               +  (var_v1->actorProp.z - ref_position[2])* (var_v1->actorProp.z - ref_position[2]);
        }

        //sort prop list
        start_prop = cube->prop2Ptr;
        var_t0 = cube->prop2Ptr + (cube->prop2Cnt - 1);   
        do {
            new_var = start_prop;
            var_t1 = var_t0;
            start_prop = NULL;
            var_v1 = new_var;
            i = (new_var - cube->prop2Ptr);
            while(var_v1 < var_t1){
                if(D_80383450[i] < D_80383450[i + 1]){
                    var_t0 = var_v1 + 1;
                    if (start_prop != 0) {
                        var_t0 = var_v1;
                    } else {
                        start_prop = (var_v1 == cube->prop2Ptr) ? var_v1 : var_v1 - 1;
                    }
                    
                    //swap_distances
                    temp_a2 = D_80383450[i];
                    D_80383450[i] = D_80383450[i + 1];
                    D_80383450[i + 1] = temp_a2;

                    //swap_props
                    temp_a2 = ((s32*)(&var_v1[0]))[0];
                    ((s32*)(&var_v1[0]))[0] = ((s32*)(&var_v1[1]))[0];
                    ((s32*)(&var_v1[1]))[0] = temp_a2;

                    temp_a2 = ((s32*)(&var_v1[0]))[1];
                    ((s32*)(&var_v1[0]))[1] = ((s32*)(&var_v1[1]))[1];
                    ((s32*)(&var_v1[1]))[1] = temp_a2;

                    temp_a2 = ((s32*)(&var_v1[0]))[2];
                    ((s32*)(&var_v1[0]))[2] = ((s32*)(&var_v1[1]))[2];
                    ((s32*)(&var_v1[1]))[2] = temp_a2;

                    // @recomp Swap prop extension data handles.
                    u32 temp_handle = cube_handle->prop_handles[i];
                    cube_handle->prop_handles[i] = cube_handle->prop_handles[i + 1];
                    cube_handle->prop_handles[i + 1] = temp_handle;
                }

                var_v1++;
                i++;
            }
        } while (start_prop != NULL);
        code_A5BC0_initCubePropActorProp(cube);
    }
}

// @recomp Patched to reset the prop handle list.
RECOMP_PATCH void cubeList_free(){
    Cube *iCube;
    
    for(iCube = sCubeList.cubes; iCube < sCubeList.cubes + sCubeList.cubeCnt; iCube++){
        cube_free(iCube);
    }
    free(sCubeList.cubes);
    
    if(sCubeList.unk3C){
        cube_free(sCubeList.unk3C);
        free(sCubeList.unk3C);
    }

    if(sCubeList.unk40){
        cube_free(sCubeList.unk40);
        free(sCubeList.unk40);
    }
    bitfield_free(D_8036A9E0);
    D_8036A9E0 = NULL;

    // @recomp Reset the prop handle list.
    recomp_clear_all_object_data(EXTENSION_TYPE_PROP);
}

RECOMP_EXPORT PropExtensionId bkrecomp_extend_prop_all(u32 size) {
    return recomp_register_object_extension_generic(EXTENSION_TYPE_PROP, size);
}

RECOMP_EXPORT void *bkrecomp_get_extended_prop_data(Cube* cube, Prop* prop, PropExtensionId extension_id) {
    CubeHandle* cube_handle = get_cube_handle(cube);
    s32 prop_index = prop - cube->prop2Ptr;
    if (prop_index < 0 || prop_index >= cube->prop2Cnt) {
        recomp_printf("bkrecomp_get_extended_prop_data: Invalid Cube/Prop pair\n");
        recomp_abort("Fatal error in mod - Invalid call to bkrecomp_get_extended_prop_data");
    }

    // TODO prop subtypes.
    void* data = recomp_get_object_data(EXTENSION_TYPE_PROP, 0, cube_handle->prop_handles[prop_index], extension_id);
    if (data == NULL) {
        recomp_printf("bkrecomp_get_extended_prop_data: Invalid extension id 0x%08X\n", extension_id);
        recomp_abort("Fatal error in mod - Invalid call to bkrecomp_get_extended_prop_data");
    }

    return data;
}

RECOMP_EXPORT u32 bkrecomp_get_prop_spawn_index(Cube* cube, Prop* prop) {
    CubeHandle* cube_handle = get_cube_handle(cube);
    s32 prop_index = prop - cube->prop2Ptr;
    if (prop_index < 0 || prop_index >= cube->prop2Cnt) {
        recomp_printf("bkrecomp_get_prop_spawn_index: Invalid Cube/Prop pair\n");
        recomp_abort("Fatal error in mod - Invalid call to bkrecomp_get_prop_spawn_index");
    }

    u32 spawn_index = recomp_get_object_spawn_index(EXTENSION_TYPE_PROP, cube_handle->prop_handles[prop_index]);
    if (spawn_index == 0xFFFFFFFF) {
        recomp_printf("bkrecomp_get_prop_spawn_index: Internal error\n");
        recomp_abort("Fatal error - Internal error in bkrecomp_get_prop_spawn_index");
    }

    return spawn_index;
}
