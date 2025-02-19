#include <cassert>
#include <cstring>
#include <fstream>

#include "librecomp/game.hpp"
#include "banjo_game.h"
#include "recomp_data.h"

#ifdef _MSC_VER
inline uint32_t byteswap(uint32_t val) {
    return _byteswap_ulong(val);
}
#else
constexpr uint32_t byteswap(uint32_t val) {
    return __builtin_bswap32(val);
}
#endif

// Produces a decompressed BK rom. This is only needed because the game has compressed code.
// For other recomps using this repo as an example, you can omit the decompression routine and
// set the corresponding fields in the GameEntry if the game doesn't have compressed code,
// even if it does have compressed data.
std::vector<uint8_t> banjo::decompress_bk(std::span<const uint8_t> compressed_rom) {
    return {};
    // Sanity check the rom size and header. These should already be correct from the runtime's check,
    // but it should prevent this file from accidentally being copied to another recomp.
    // if (compressed_rom.size() != 0x2000000) {
    //     assert(false);
    //     return {};
    // }

    // if (compressed_rom[0x3B] != 'N' || compressed_rom[0x3C] != 'Z' || compressed_rom[0x3D] != 'S' || compressed_rom[0x3E] != 'E') {
    //     assert(false);
    //     return {};
    // }

    // struct DmaDataEntry {
    //     uint32_t vrom_start;
    //     uint32_t vrom_end;
    //     uint32_t rom_start;
    //     uint32_t rom_end;

    //     void bswap() {
    //         vrom_start = byteswap(vrom_start);
    //         vrom_end = byteswap(vrom_end);
    //         rom_start = byteswap(rom_start);
    //         rom_end = byteswap(rom_end);
    //     }
    // };

    // DmaDataEntry cur_entry{};
    // size_t cur_entry_index = 0;

    // constexpr size_t dma_data_rom_addr = 0x1A500;

    // std::vector<uint8_t> ret{};
    // ret.resize(0x2F00000);

    // size_t content_end = 0;

    // do {
    //     // Read the entry from the compressed rom.
    //     size_t cur_entry_rom_address = dma_data_rom_addr + (cur_entry_index++) * sizeof(DmaDataEntry);
    //     memcpy(&cur_entry, compressed_rom.data() + cur_entry_rom_address, sizeof(DmaDataEntry));
    //     // Swap the entry to native endianness after reading from the big endian data.
    //     cur_entry.bswap();

    //     // Rom end being 0 means the data is already uncompressed, so copy it as-is to vrom start.
    //     size_t entry_decompressed_size = cur_entry.vrom_end - cur_entry.vrom_start;
    //     if (cur_entry.rom_end == 0) {
    //         memcpy(ret.data() + cur_entry.vrom_start, compressed_rom.data() + cur_entry.rom_start, entry_decompressed_size);

    //         // Edit the entry to account for it being in a new location now.
    //         cur_entry.rom_start = cur_entry.vrom_start;
    //     }
    //     // Otherwise, decompress the input data into the output data.
    //     else {
    //         if (cur_entry.rom_end != cur_entry.rom_start) {
    //             // Validate the presence of the yaz0 header.
    //             if (compressed_rom[cur_entry.rom_start + 0] != 'Y' ||
    //                 compressed_rom[cur_entry.rom_start + 1] != 'a' ||
    //                 compressed_rom[cur_entry.rom_start + 2] != 'z' ||
    //                 compressed_rom[cur_entry.rom_start + 3] != '0')
    //             {
    //                 assert(false);
    //                 return {};
    //             }
    //             // Skip the yaz0 header.
    //             size_t compressed_data_rom_start = cur_entry.rom_start + 0x10;
    //             size_t entry_compressed_size = cur_entry.rom_end - compressed_data_rom_start;

    //             std::span input_span = std::span{ compressed_rom }.subspan(compressed_data_rom_start, entry_compressed_size);
    //             std::span output_span = std::span{ ret }.subspan(cur_entry.vrom_start, entry_decompressed_size);
    //             yaz0_decompress(input_span, output_span);

    //             // Edit the entry to account for it being decompressed now.
    //             cur_entry.rom_start = cur_entry.vrom_start;
    //             cur_entry.rom_end = 0;
    //         }
    //     }

    //     if (entry_decompressed_size != 0) {
    //         if (cur_entry.vrom_end > content_end) {
    //             content_end = cur_entry.vrom_end;
    //         }
    //     }

    //     // Swap the entry back to big endian for writing.
    //     cur_entry.bswap();
    //     // Write the modified entry to the decompressed rom.
    //     memcpy(ret.data() + cur_entry_rom_address, &cur_entry, sizeof(DmaDataEntry));
    // } while (cur_entry.vrom_end != 0);

    // // Align the start of padding to the closest 0x1000 (matches decomp rom decompression behavior).
    // content_end = (content_end + 0x1000 - 1) & -0x1000;

    // // Write 0xFF as the padding.
    // std::fill(ret.begin() + content_end, ret.end(), 0xFF);
}

void banjo::bk_on_init(uint8_t* rdram, recomp_context* ctx) {
    MEM_W(0, (int32_t)0x80000310) = 6103;
    recomp::do_rom_read(rdram, (int32_t)0x80000000, 0x100004C0, 0x2A4);
    recomp::init_extended_actor_data();
}
