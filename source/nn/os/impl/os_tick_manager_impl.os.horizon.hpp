/*
 * Copyright (c) Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <nn/nn_common.hpp>
#include <nn/os/os_tick.hpp>

#include <limits>

namespace nn::os::detail {

    class TickManagerImpl {
        public:
            constexpr TickManagerImpl() { /* ... */ }

            ALWAYS_INLINE Tick GetTick() const {
                s64 tick;
                __asm__ __volatile__("mrs %[tick], cntpct_el0" : [tick]"=&r"(tick) :: "memory");
                return Tick(tick);
            }

            ALWAYS_INLINE Tick GetSystemTickOrdered() const {
                s64 tick;
                __asm__ __volatile__("dsb ish\n"
                                        "isb\n"
                                        "mrs %[tick], cntpct_el0\n"
                                        "isb"
                                        : [tick]"=&r"(tick)
                                        :
                                        : "memory");
                return Tick(tick);
            }

            static constexpr inline const s64 TicksPerSecond = 19'200'000;

            static constexpr ALWAYS_INLINE s64 GetTickFrequency() {
                return static_cast<s64>(TicksPerSecond);
            }

            static constexpr ALWAYS_INLINE s64 GetMaxTick() {
                static_assert(GetTickFrequency() <= TimeSpan::FromSeconds(1).GetNanoSeconds());
                return (std::numeric_limits<s64>::max() / TimeSpan::FromSeconds(1).GetNanoSeconds()) * GetTickFrequency();
            }

            static constexpr ALWAYS_INLINE s64 GetMaxTimeSpanNs() {
                static_assert(GetTickFrequency() <= TimeSpan::FromSeconds(1).GetNanoSeconds());
                return TimeSpan::FromNanoSeconds(std::numeric_limits<s64>::max()).GetNanoSeconds();
            }
    };

}