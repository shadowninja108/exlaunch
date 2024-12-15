#pragma once

#include <common.hpp>

namespace exl::util {

    /* Add versions here if you want to take advantage of this feature. */
    enum class UserVersion {
        DEFAULT,
        /* Members can be assigned any constant of your choosing. */
        /* OTHER = 123 */
    };

    namespace impl {
        ALWAYS_INLINE UserVersion DetermineUserVersion() {
            /* 
                Implement a method of determining your application version here if you want to take advantage of this feature.
                An example may be apply Murmur3 over the .text and .rodata of the main executable or calling nn::oe::GetDisplayVersion.
                The former would have complications if other mods are in effect, and the latter may have issues with people doing safe downgrades on console.
            */
            return UserVersion::DEFAULT;
        }
    }
}