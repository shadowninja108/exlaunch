#pragma once

#include "ilogger.hpp"

namespace exl::log {

    struct SvcLogger : public ILogger {
        /* Declaring LogRaw as final allows the compiler to properly optimize. */
        virtual void LogRaw(std::string_view string) final;
    };
}