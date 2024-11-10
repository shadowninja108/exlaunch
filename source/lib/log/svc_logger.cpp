#include "svc_logger.hpp"

#include <common.hpp>

namespace exl::log {

    void SvcLogger::LogRaw(std::string_view string) {
        svcOutputDebugString(string.data(), string.size());
    }
}