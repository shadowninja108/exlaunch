#include "version.hpp"

#include <cstdio>
#include <lib/log/logger_mgr.hpp>
#include <program/loggers.hpp>

namespace exl::util {

    namespace impl {
        UserVersion s_UserVersion;

        void InitVersion() {
            Logging.Log(EXL_LOG_PREFIX "Determining user version...");
            s_UserVersion = impl::DetermineUserVersion();
            Logging.Log(EXL_LOG_PREFIX "User version: %d", s_UserVersion);
        }
    }
}