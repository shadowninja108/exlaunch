#pragma once

#include <string_view>

namespace exl::log {

    struct ILogger {
        virtual void LogRaw(std::string_view data) = 0;
    };
}