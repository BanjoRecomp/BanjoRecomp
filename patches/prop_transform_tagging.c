#include "patches.h"
#include "bk_api.h"
#include "transform_ids.h"
#include "functions.h"
#include "core2/vla.h"

typedef union{
    struct{
        u32 pad31: 27;
        u32 unk4: 1;
        u32 pad3: 1;
        u32 unk2: 1;
        u32 unk1: 1;
        u32 unk0: 1;
    };
    u32 word;
} tmp_bitfield;

extern vector(ActorMarker *) *D_80383550;
extern vector(ActorMarker *) *D_80383554;

void propModelList_drawSprite(Gfx **, Mtx **, Vtx **, f32[3], f32, s32, Cube*,s32 ,s32, s32, s32, s32);  
void propModelList_drawModel(Gfx **, Mtx **, Vtx **, f32[3], f32[3], f32, s32, Cube*);
void __marker_draw(ActorMarker *this, Gfx **gfx, Mtx **mtx, Vtx **vtx);
void __cube_sort(Cube *cube, bool global);
void func_8032CD60(Prop *);

// @recomp Patched to add transform tagging when drawing sprite and model props.
RECOMP_PATCH void func_8032D510(Cube *cube, Gfx **gfx, Mtx **mtx, Vtx **vtx){
    Prop *iProp;
    int i;
    f32 position[3];
    f32 rotation[3];
    tmp_bitfield tmp_v0;
    int iOffset;
    ActorMarker **markerPtr;

    if(cube->prop2Cnt == 0 ) return;

    __cube_sort(cube, 0);
    iOffset = 0;
    for(i = 0; i < cube->prop2Cnt; i++){//L8032D5A0
        iOffset = i * 0xC;
        iProp = (Prop *)((s32)cube->prop2Ptr + iOffset);
        tmp_v0.word = *(u32 *)((s32)iProp + 0x8);
        if(!tmp_v0.unk4){
        
        }else{
            if(!tmp_v0.unk1){
                func_8032CD60(iProp);
            }
            tmp_v0.word = *(u32 *)((s32)iProp + 0x8);
            if(tmp_v0.unk0){//actorProp;
                if(iProp->actorProp.marker->unk40_22){
                    markerPtr = (ActorMarker **)vector_pushBackNew(&D_80383550);
                    *markerPtr = iProp->actorProp.marker;
                }
                else if(iProp->actorProp.marker->unk40_19){
                    markerPtr = (ActorMarker **)vector_pushBackNew(&D_80383554);
                    *markerPtr = iProp->actorProp.marker;
                }
                else{
                    __marker_draw(iProp->actorProp.marker, gfx, mtx, vtx);
                }//L8032D62C
            }
            else{//L8032D640
                // @recomp Calculate the base transform ID for the prop.
                u32 spawn_index = bkrecomp_get_prop_spawn_index(cube, iProp);
                u32 base_transform_id = spawn_index * PROP_TRANSFORM_ID_COUNT + PROP_TRANSFORM_ID_START;

                position[0] = (f32)iProp->modelProp.position[0];
                position[1] = (f32)iProp->modelProp.position[1];
                position[2] = (f32)iProp->modelProp.position[2];
                if(iProp->is_3d){
                    rotation[0] = 0.0f;
                    rotation[1] = (f32)((s32)iProp->modelProp.yaw*2);
                    rotation[2] = (f32)((s32)iProp->modelProp.roll*2);

                    // @recomp Set the model render transform ID before drawing the model.
                    cur_drawn_model_transform_id = base_transform_id;

                    propModelList_drawModel(gfx, mtx, vtx, 
                        position, rotation, (f32)iProp->modelProp.scale/100.0,
                        iProp->modelProp.model_index, cube
                    );
                    

                    // @recomp Clear the model render transform ID after drawing the model.
                    cur_drawn_model_transform_id = 0;
                }
                else{//L8032D72C
                    // @recomp Set the matrix group before drawing the sprite.
                    // Skip interpolation on vertices to account for vertex lists changing between frames of the sprite.
                    // Also skip interpolation on scale to account for the scale inverting when sprites are mirrored.
                    if (perspective_interpolation_skipped()) {
                        gEXMatrixGroupSkipAll((*gfx)++, base_transform_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
                    }
                    else {
                        gEXMatrixGroupDecomposed((*gfx)++, base_transform_id, G_EX_PUSH, G_MTX_MODELVIEW,
                            G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE,
                            G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE,
                            G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
                    }
                    
                    // @recomp Also set the model render transform ID before drawing the sprite. This won't have any effect
                    // in the unmodified game, but will allow transform tagging for mods that draw models in place of sprites.
                    cur_drawn_model_transform_id = base_transform_id;

                    propModelList_drawSprite(gfx, mtx, vtx, 
                        position, (f32)iProp->spriteProp.scale/100.0, iProp->spriteProp.sprite_index, cube, 
                        iProp->spriteProp.r, iProp->spriteProp.b, iProp->spriteProp.g,
                        iProp->spriteProp.mirrored, iProp->spriteProp.frame
                    );

                    // @recomp Pop the sprite's matrix group.
                    gEXPopMatrixGroup((*gfx)++, G_MTX_MODELVIEW);
                    
                    // @recomp Clear the model render transform ID after drawing the sprite.
                    cur_drawn_model_transform_id = 0;
                }
            }//L8032D7C4
        }
        iOffset+=0xC;
    }//L8032D7D4
}
