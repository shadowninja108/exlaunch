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

namespace nn::os::detail {

    class InternalConditionVariableImplByHorizon;

    class InternalCriticalSectionImplByHorizon {
        private:

            friend class InternalConditionVariableImplByHorizon;
        private:
            u32 m_thread_handle;
        public:
            constexpr InternalCriticalSectionImplByHorizon() : m_thread_handle(INVALID_HANDLE) { /* ... */ }

            constexpr void Initialize() { m_thread_handle = INVALID_HANDLE; }
            constexpr void Finalize() { /* ... */ }

            void Enter();
            bool TryEnter();
            void Leave();

            bool IsLockedByCurrentThread() const;

            ALWAYS_INLINE void Lock()    { return this->Enter(); }
            ALWAYS_INLINE bool TryLock() { return this->TryEnter(); }
            ALWAYS_INLINE void Unlock()  { return this->Leave(); }

            ALWAYS_INLINE void lock()     { return this->Lock(); }
            ALWAYS_INLINE bool try_lock() { return this->TryLock(); }
            ALWAYS_INLINE void unlock()   { return this->Unlock(); }
    };

}