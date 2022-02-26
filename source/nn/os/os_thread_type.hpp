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

#include "nn/nn_common.hpp"
#include "os_thread_api.hpp"
#include "os_types.hpp"
#include "impl/os_internal_condition_variable.hpp"
#include "impl/os_internal_critical_section.hpp"

namespace nn::os {

    struct ThreadType {

        enum State : u8 {
            State_NotInitialized         = 0,
            State_Initialized            = 1,
            State_DestroyedBeforeStarted = 2,
            State_Started                = 3,
            State_Terminated             = 4,
        };

        /* List stuff. */
        u8 _0[0x40];

        State state;
        u8 _41;
        u8 _42;
        u8 suspend_count;
        s16 priority;
        void* user_stack;
        void* aliased_stack;
        size_t stack_size;
        void *argument;
        ThreadFunction function;

        /* Unknown? */
        u8 _88[0x110];

        char name_buffer[ThreadNameLengthMax];
        char* name_pointer;

        mutable detail::InternalCriticalSectionStorage cs_thread;
        mutable detail::InternalConditionVariableStorage cv_thread;

        Handle handle;
        ThreadId thread_id;
    };
    static_assert(sizeof(ThreadType) == 0x1C0, "");

    constexpr inline s32 IdealCoreDontCare   = -1;
    constexpr inline s32 IdealCoreUseDefault = -2;
    constexpr inline s32 IdealCoreNoUpdate   = -3;

};