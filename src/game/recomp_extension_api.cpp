#include <vector>
#include <mutex>
#include <deque>

#include "slot_map.h"

#include "librecomp/helpers.hpp"
#include "librecomp/addresses.hpp"
#include "ultramodern/error_handling.hpp"
#include "recompui/recompui.h"
#include "recomp_data.h"
#include "../patches/object_extension_funcs.h"

struct ExtensionInfo {
    // Either the object's subtype ID, or 0xFFFFFFFF if this is for generic data.
    uint32_t subtype_index;
    // The offset from either the start of the object's extension data (for generic extensions) or from the start of the objects's specific extension data.
    uint32_t data_offset;
};

struct ExtensionData {
    uint32_t object_spawn_index;
    PTR(void) data_addr;
};

using extension_data_map_t = dod::slot_map32<ExtensionData>;

std::mutex extension_mutex;
struct ExtensionContext {
    // The total size of subtype-specific extension data for each subtype. 
    std::vector<uint32_t> data_sizes{};
    // The total size of all generic extension data.
    uint32_t generic_data_size = 0;
    // The registered extensions.
    std::vector<ExtensionInfo> extensions{};
    // The extension data for every object.
    extension_data_map_t extension_data{};
    // The number of objects spawned since the last reset.
    uint32_t spawn_count = 0;
    // Debug counters.
    size_t alloc_count = 0;
    size_t free_count = 0;
};

// The extension contexts for each object type. This must be a deque instead of a vector due to a lack of a copy constructor.
std::deque<ExtensionContext> type_contexts{};
// Whether or not extensions can be registered at this time.
bool can_register = false;

void recomputil::init_extended_object_data(size_t num_types) {
    type_contexts.clear();

    can_register = true;
    // Create a dummy extension so the first extension handle is nonzero, should help catch bugs.
    for (size_t i = 0; i < num_types; i++) {
        type_contexts.emplace_back(ExtensionContext{});
        type_contexts[i].extensions.push_back({});
    }
}

void recomputil::clear_all_object_data(size_t type_index) {
    ExtensionContext& context = type_contexts[type_index];
    std::lock_guard lock{ extension_mutex };
    context.extension_data.reset();
    context.spawn_count = 0;

    assert(context.alloc_count == context.free_count);
    context.alloc_count = 0;
    context.free_count = 0;
}

constexpr uint32_t round_up_16(uint32_t value) {
    return (value + 15) & (~15);
}

extern "C" void recomp_register_object_extension(uint8_t* rdram, recomp_context* ctx) {
    u32 type_index = _arg<0, u32>(rdram, ctx);
    u32 subtype_index = _arg<1, u32>(rdram, ctx);
    u32 size = _arg<2, u32>(rdram, ctx);

    ExtensionContext& context = type_contexts[type_index];

    if (!can_register) {
        recompui::message_box("Fatal error in mod - attempted to register object extension data after objects have been spawned.");
        assert(false);
        ultramodern::error_handling::quick_exit(__FILE__, __LINE__, __FUNCTION__);
    }

    if (context.data_sizes.size() <= subtype_index) {
        context.data_sizes.resize(subtype_index + 1);
    }

    // Increase the object subtype's extension data size by the provided size (rounded up to a multiple of 16).
    uint32_t data_offset = context.data_sizes[subtype_index];
    context.data_sizes[subtype_index] += round_up_16(size);

    // Register the extension.
    uint32_t ret = static_cast<uint32_t>(context.extensions.size());
    context.extensions.emplace_back(ExtensionInfo{.subtype_index = subtype_index, .data_offset = data_offset});

    // printf("Registered object extension data for type %u subtype %u (size 0x%08X, offset 0x%08X)\n", type_index, subtype_index, size, data_offset);

    _return<u32>(ctx, ret);
}

extern "C" void recomp_register_object_extension_generic(uint8_t* rdram, recomp_context* ctx) {
    u32 type_index = _arg<0, u32>(rdram, ctx);
    u32 size = _arg<1, u32>(rdram, ctx);

    ExtensionContext& context = type_contexts[type_index];

    // Increase the generic extension data size by the provided size (rounded up to a multiple of 16).
    uint32_t data_offset = context.generic_data_size;
    context.generic_data_size += round_up_16(size);

    // Register the extension.
    uint32_t ret = static_cast<uint32_t>(context.extensions.size());
    context.extensions.emplace_back(ExtensionInfo{.subtype_index = 0xFFFFFFFFU, .data_offset = data_offset});

    // printf("Registered generic object extension data for type %u (size 0x%08X, offset 0x%08X)\n", type_index, size, data_offset);
    _return<u32>(ctx, ret);
}

extern "C" void recomp_clear_all_object_data(uint8_t* rdram, recomp_context* ctx) {
    u32 type_index = _arg<0, u32>(rdram, ctx);
    recomputil::clear_all_object_data(type_index);
}

extern "C" void recomp_create_object_data(uint8_t* rdram, recomp_context* ctx) {
    u32 type_index = _arg<0, u32>(rdram, ctx);
    u32 subtype_index = _arg<1, u32>(rdram, ctx);

    ExtensionContext& context = type_contexts[type_index];

    std::lock_guard lock{ extension_mutex };

    can_register = false;

    // Determine the number of bytes to allocate based on the subtype's extensions and the generic extensions.
    u32 alloc_size = context.generic_data_size;
    [[maybe_unused]] u32 subtype_data_size = 0; 

    if (subtype_index < context.data_sizes.size()) {
        subtype_data_size = context.data_sizes[subtype_index];
        alloc_size += subtype_data_size;
    }

    // Allocate the extension data if it's of nonzero size.
    PTR(void) data_ptr = NULLPTR;
    if (alloc_size != 0) {
        void* data = recomp::alloc(rdram, alloc_size);
        context.alloc_count++;
        data_ptr = reinterpret_cast<uint8_t*>(data) - rdram + 0xFFFFFFFF80000000U;
        // Zero the allocated memory.
        // A memset should be fine here since this data is aligned, but use a byteswapped loop just to be safe.
        for (size_t i = 0; i < alloc_size; i++) {
            MEM_B(i, data_ptr) = 0;
        }
    }

    // Add the object's data to the type's data slotmap.
    u32 spawn_index = context.spawn_count++;
    dod::slot_map_key32<ExtensionData> key = context.extension_data.emplace(ExtensionData{.object_spawn_index = spawn_index, .data_addr = data_ptr});

    // printf("Allocated object data: type %u address 0x%08X with 0x%08X bytes total (0x%08X bytes generic and 0x%08X bytes specific), handle 0x%08X, spawn index %d\n",
    //     type_index, data_ptr, alloc_size, generic_data_size, type_data_size, key.raw, spawn_index);

    _return<u32>(ctx, key.raw);
}

extern "C" void recomp_destroy_object_data(uint8_t* rdram, recomp_context* ctx) {
    u32 type_index = _arg<0, u32>(rdram, ctx);
    u32 object_handle = _arg<1, u32>(rdram, ctx);
    ExtensionContext& context = type_contexts[type_index];

    std::lock_guard lock{ extension_mutex };

    extension_data_map_t::key object_key{object_handle};

    ExtensionData* data = context.extension_data.get(object_key);
    if (data != nullptr) {
        // printf("Freeing object data: address 0x%08X handle 0x%08X\n", data->data_addr, object_handle);
        if (data->data_addr != NULLPTR) {
            recomp::free(rdram, TO_PTR(void, data->data_addr));
            context.free_count++;
        }
        context.extension_data.erase(extension_data_map_t::key{object_handle});
    }
    else {
        // Not an irrecoverable error, but catch it in debug mode with an assert to help find bugs.
        // *(volatile int*)object_handle = 0;
        assert(false);
    }
}

extern "C" void recomp_get_object_data(uint8_t* rdram, recomp_context* ctx) {
    u32 type_index = _arg<0, u32>(rdram, ctx);
    u32 subtype_index = _arg<1, u32>(rdram, ctx);
    u32 object_handle = _arg<2, u32>(rdram, ctx);
    u32 extension_handle = _arg<3, u32>(rdram, ctx);

    ExtensionContext& context = type_contexts[type_index];
    std::lock_guard lock{ extension_mutex };

    // Check if the extension handle is valid.
    if (extension_handle == 0 || extension_handle >= context.extensions.size()) {
        _return<PTR(void)>(ctx, NULLPTR);
        return;
    }

    ExtensionInfo& extension = context.extensions[extension_handle];
    bool generic_extension = extension.subtype_index == 0xFFFFFFFFU;

    // Check if the extension is generic or for the provided subtype.
    if (!generic_extension && extension.subtype_index != subtype_index) {
        _return<PTR(void)>(ctx, NULLPTR);
        return;
    }

    extension_data_map_t::key object_key{object_handle};
    ExtensionData* data = context.extension_data.get(object_key);

    // Check if object handle is valid.
    if (data == nullptr) {
        _return<PTR(void)>(ctx, NULLPTR);
        return;
    }

    // Calculate the address for this specific extension's data.
    PTR(void) base_address = data->data_addr;
    u32 offset = extension.data_offset;
    // Specific object data is after generic object data, so increase the offset by the total generic object data if this isn't generic data.
    if (!generic_extension) {
        offset += context.generic_data_size;
    }

    PTR(void) ret = base_address + offset;
    _return<PTR(void)>(ctx, ret);
}

extern "C" void recomp_get_object_spawn_index(uint8_t* rdram, recomp_context* ctx) {
    u32 type_index = _arg<0, u32>(rdram, ctx);
    u32 object_handle = _arg<1, u32>(rdram, ctx);
    ExtensionContext& context = type_contexts[type_index];

    std::lock_guard lock{ extension_mutex };


    extension_data_map_t::key object_key{object_handle};
    ExtensionData* data = context.extension_data.get(object_key);

    // Check if object handle is valid.
    if (data == nullptr) {
        _return<u32>(ctx, 0xFFFFFFFFU);
        return;
    }

    _return<u32>(ctx, data->object_spawn_index);
}

