#pragma once

#include <lib/log/svc_logger.hpp>

/* Specify logger implementations here. */
inline exl::log::LoggerMgr<
    exl::log::SvcLogger
> Logging;