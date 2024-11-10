#pragma once

namespace exl::util {

    enum class ModuleIndex : uint {
        Rtld,
        Main,
        Subsdk0,
        Subsdk1,
        Subsdk2,
        Subsdk3,
        Subsdk4,
        Subsdk5,
        Subsdk6,
        Subsdk7,
        Subsdk8,
        Subsdk9,
        Sdk,
        End,

        Start = Rtld
    };
    constexpr int s_ModuleIndexBitCount = 4;
    static_assert((static_cast<int>(ModuleIndex::End) & ((1 << s_ModuleIndexBitCount) - 1)) == static_cast<int>(ModuleIndex::End), "Cannot fit the module index into BitCount!");

}