#include "external.hpp"

#ifndef SYMBOLS_HPP
#define SYMBOLS_HPP

typedef struct {
    u8 data[566];
} encounter_tables_t;

namespace Field {
    #ifdef VERSION_SHIELD
    static const u64 FetchAreaHash_offset = 0xd7e310;
    #else
    // TODO: sword offsets
    static const u64 FetchAreaHash_offset = 0xd7e310;
    #endif

    static u64 FetchAreaHash() {
        return external<u64>(FetchAreaHash_offset);
    }
}

namespace OverworldEncount {
    #ifdef VERSION_SHIELD
    static const u64 GenerateSymbolEncountParam_offset = 0xd050b0;
    static const u64 FetchSymbolEncountTable_offset = 0xd05750;
    static const u64 TryGenerateSymbolEncount_offset = 0xdaf380;
    #else
    // TODO: sword offsets
    static const u64 GenerateSymbolEncountParam_offset = 0xd050b0;
    static const u64 FetchSymbolEncountTable_offset = 0xd05750;
    static const u64 TryGenerateSymbolEncount_offset = 0xdaf380;
    #endif

    typedef void EncountSpawner;
    
    static bool GenerateSymbolEncountParam(void* param_1, encounter_tables_t* encounter_tables, u64 unused, void* overworld_spec) {
        return external<bool>(GenerateSymbolEncountParam_offset, param_1, encounter_tables, unused, overworld_spec);
    }
    static encounter_tables_t FetchSymbolEncountTable(u64 unused, u64* hash) {
        return external<encounter_tables_t>(FetchSymbolEncountTable_offset, unused, hash);
    }
    static bool TryGenerateSymbolEncount(EncountSpawner* encount_spawner, void* param_2) {
        return external<bool>(TryGenerateSymbolEncount_offset);
    }
}
#endif
