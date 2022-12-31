#pragma once

#include <common.hpp>
#include "util/modules.hpp"

namespace exl::util::pointer_path {
    
        /* Core pointer path following. */
    template<uintptr_t... Offsets>
    inline uintptr_t FollowSafe(uintptr_t ptr) {
        uintptr_t curOffset = ptr;

        static constexpr auto offsets = std::array{Offsets...};

        /* Nothing to do if there's no arguments. */
        if constexpr (offsets.size() == 0)
            return curOffset;

        /* Null ptr should just be returned. */
        if(curOffset == 0)
            return 0;

        /* Skip dereferencing the first argument itself. */
        curOffset += offsets[0];
        /* Iterate through rest of offsets. */
        for(size_t i = 1; i < offsets.size(); i++) 
        {
            curOffset = *(uintptr_t*)(curOffset);

            /* Check we can keep following. */
            if(curOffset == 0)
                return 0;

            curOffset += offsets[i];
        }
        return curOffset;
    }
    template<uintptr_t... Offsets>
    inline uintptr_t Follow(uintptr_t ptr) {
        uintptr_t curOffset = ptr;

        static constexpr auto offsets = std::array{Offsets...};

        /* Nothing to do if there's no arguments. */
        if constexpr (offsets.size() == 0)
            return curOffset;
            
        /* Null ptr should just be returned. */
        if(curOffset == 0)
            return 0;

        /* Skip dereferencing the first argument itself. */
        curOffset += offsets[0];
        /* Iterate through rest of offsets. */
        for(size_t i = 1; i < offsets.size(); i++) 
        {
            curOffset = *(uintptr_t*)(curOffset);
            curOffset += offsets[i];
        }
        return curOffset;
    }

    /* Pointer helpers. */
    template<uintptr_t... Offsets>
    inline uintptr_t FollowSafe(void const* ptr) {
        return FollowSafe<Offsets...>(reinterpret_cast<uintptr_t>(ptr));
    }
    template<uintptr_t... Offsets>
    inline uintptr_t Follow(void const* ptr) {
        return Follow<Offsets...>(reinterpret_cast<uintptr_t>(ptr));
    }

    /* Target relative helpers. */
    template<uintptr_t... Offsets>
    inline uintptr_t FollowSafe() {
        return FollowSafe<Offsets...>(modules::GetTargetStart());
    }
    template<uintptr_t... Offsets>
    inline uintptr_t Follow() {
        return Follow<Offsets...>(modules::GetTargetStart());
    }

    /* Casting helpers.  */
    template<typename EndType, uintptr_t... Offsets>
    inline EndType* FollowSafe() {
        auto offset = FollowSafe<Offsets...>();
        return (EndType*) offset;
    }
    template<typename EndType, uintptr_t... Offsets>
    inline EndType Follow() {
        auto offset = Follow<Offsets...>();
        return *(EndType*) offset;
    }
    template<typename EndType, uintptr_t... Offsets>
    inline EndType* FollowSafe(uintptr_t ptr) {
        auto offset = FollowSafe<Offsets...>(ptr);
        return (EndType*) offset;
    }
    template<typename EndType, uintptr_t... Offsets>
    inline EndType Follow(uintptr_t ptr) {
        auto offset = Follow<Offsets...>(ptr);
        return *(EndType*) offset;
    }
    template<typename EndType, uintptr_t... Offsets>
    inline EndType* FollowSafe(void const* ptr) {
        auto offset = FollowSafe<Offsets...>(ptr);
        return (EndType*) offset;
    }
    template<typename EndType, uintptr_t... Offsets>
    inline EndType Follow(void const* ptr) {
        auto offset = Follow<Offsets...>(ptr);
        return *(EndType*) offset;
    }
}