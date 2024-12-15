#pragma once

#include <program/version.hpp>

namespace exl::util {

    namespace impl {
        extern UserVersion s_UserVersion;
        void InitVersion();
    }

    inline UserVersion GetUserVersion() { return impl::s_UserVersion; }
}