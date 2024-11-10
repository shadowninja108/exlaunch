#pragma once

#include <tuple>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <functional>
#include <string_view>
#include <common.hpp>

#include <program/setting.hpp>

#define EXL_LOG_PREFIX "[" EXL_MODULE_NAME "|exlaunch] "

namespace exl::log {

    template<typename... Types>
    class LoggerMgr {
        using TupleT = std::tuple<Types...>;

        TupleT m_Loggers {};

        public:
        template<typename Self>
        ALWAYS_INLINE constexpr void ForEachLogger(this Self &&self, auto func) {
            std::apply([&](auto &&... loggers) {
                (std::invoke(func, loggers), ...);
            }, std::forward<Self>(self).m_Loggers);
        }

        private:
        ALWAYS_INLINE void LogImpl(std::string_view string) {
            this->ForEachLogger([&](auto& logger) {
                logger.LogRaw(string);
            });
        }

        ALWAYS_INLINE void LogImpl(const char* string) {
            LogImpl(std::string_view { string, strlen(string) });
        }

        public:
        ALWAYS_INLINE bool IsEnabled() const {
            return std::tuple_size<TupleT>() != 0;
        }

        template<typename T>
        ALWAYS_INLINE T& GetLogger() {
            return std::get<T>(m_Loggers);
        }

        ALWAYS_INLINE void Log(const char* string) { LogImpl(string); }
        ALWAYS_INLINE void Log(std::string_view string) { LogImpl(string); }

        template<typename... Args>
        ALWAYS_INLINE void Log(const char* fmt, Args&&... args) {
            char buffer[setting::LogBufferSize];
            size_t length = std::snprintf(buffer, sizeof(buffer), fmt, std::forward<Args>(args)...);
            length = std::min(length, sizeof(buffer)-1);
            buffer[length] = '\0';

            LogImpl(std::string_view { buffer, length });
        }

        template<typename... Args>
        ALWAYS_INLINE void VLog(const char* fmt, std::va_list vl) {
            char buffer[setting::LogBufferSize];
            size_t length = std::vsnprintf(buffer, sizeof(buffer), fmt, vl);
            length = std::min(length, sizeof(buffer)-1);
            buffer[length] = '\0';

            LogImpl(std::string_view { buffer, length });
        }
    };

}

#include <program/loggers.hpp>