#include "lib.hpp"
#include "external.hpp"
#include "symbols.hpp"

HOOK_DEFINE_TRAMPOLINE(SwapSymbolTableHash) {
    static void Callback(void* param_1, encounter_tables_t* encounter_tables, u64 unused, void* overworld_spec) {
        // pull the hidden table based on the player's current location
        u64 hidden_encount_table_hash = Field::FetchAreaHash();
        *encounter_tables = OverworldEncount::FetchSymbolEncountTable(0, &hidden_encount_table_hash);

        Orig(param_1, encounter_tables, unused, overworld_spec);
    }
};

HOOK_DEFINE_INLINE(DisableEmptyTableCheck) {
    static void Callback(exl::hook::nx64::InlineCtx* ctx) {
        // patch first check to always treat the encounter table as valid
        // this allows spawners with empty/invalid tables to make it to the SwapSymbolTableHash trampoline
        // & is harmless as GenerateSymbolEncountParam re-checks validity
        ctx->W[8] = true;
    }
};

HOOK_DEFINE_INLINE(SwapSymbolTableAccess) {
    static void Callback(exl::hook::nx64::InlineCtx* ctx) {
        // replace offset that points to the symbol hashmap (+0x2a8) to the hidden hashmap (+0x198)
        // causes all spawners to be initialized with empty tables as their symbol hash won't be in the hidden map
        // this is fine as the table is refreshed every spawn
        ctx->X[8] = *reinterpret_cast<long*>(ctx->X[20] + 0x198);
    }
};

void install_underworld_patch() {
    SwapSymbolTableHash::InstallAtOffset(OverworldEncount::GenerateSymbolEncountParam_offset);
    SwapSymbolTableAccess::InstallAtOffset(OverworldEncount::FetchSymbolEncountTable_offset + 0x54);
    DisableEmptyTableCheck::InstallAtOffset(OverworldEncount::TryGenerateSymbolEncount_offset + 0x1C);
}