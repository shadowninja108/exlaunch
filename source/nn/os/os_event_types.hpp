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
#include "impl/os_internal_critical_section.hpp"
#include "impl/os_internal_condition_variable.hpp"

namespace nn::os {

    namespace impl {

        class MultiWaitObjectList;

    }

    struct EventType {
        enum State {
            State_NotInitialized = 0,
            State_Initialized    = 1,
        };

        /* List stuff. */
        u8 _0[0x10];

        bool signaled;
        bool initially_signaled;
        u8 clear_mode;
        u8 state;
        u32 broadcast_counter_low;
        u32 broadcast_counter_high;

        detail::InternalCriticalSectionStorage cs_event;
        detail::InternalConditionVariableStorage cv_signaled;
    };
    static_assert(std::is_trivial<EventType>::value);

}