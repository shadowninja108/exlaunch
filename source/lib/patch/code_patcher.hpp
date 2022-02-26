#pragma once

#include "../armv8.hpp"
#include "stream_patcher.hpp"

namespace exl::patch {

    namespace inst = armv8::inst;
    
    class CodePatcher : public StreamPatcher {
        private:
        using InstBitSet = armv8::InstBitSet;

        public:
        inline CodePatcher(uintptr_t start) : StreamPatcher(start) {}

        inline void WriteInst(InstBitSet inst) {
            Write<InstBitSet>(inst);
        }

        /* Special case branches as they are relative to the current position. */
        inline void BranchInstRel(ptrdiff_t address) {
            WriteInst(inst::Branch(address));
        }
        inline void BranchLinkInstRel(ptrdiff_t address) {
            WriteInst(inst::BranchLink(address));
        }
        
        /* Address relative to the base (Ro). */
        inline void BranchInst(uintptr_t address) {
            BranchInstRel(RelativeAddressFromBase(address));
        }
        inline void BranchLinkInst(uintptr_t address) {
            BranchLinkInstRel(RelativeAddressFromBase(address));
        }
        /* Absolute addresses. */
        inline void BranchInst(void* ptr) {
            BranchInstRel(RelativeAddressFromPointer(ptr));
        }
        inline void BranchLinkInst(void* ptr) {
            BranchLinkInstRel(RelativeAddressFromPointer(ptr));
        }
    };
}