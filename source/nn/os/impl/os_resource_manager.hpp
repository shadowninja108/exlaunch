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

#include "os_tick_manager_impl.hpp"

namespace nn::os::detail {

    class OsResourceManager {
        private:
        
            TickManager m_tick_manager{};
            
        public:
            OsResourceManager() = default;

            constexpr ALWAYS_INLINE TickManager &GetTickManager() { return m_tick_manager; }
    };

    class ResourceManagerHolder {
        private:
            static util::TypedStorage<OsResourceManager> s_resource_manager_storage;
        private:
            constexpr ResourceManagerHolder() { /* ... */ }
        public:
            static ALWAYS_INLINE void InitializeResourceManagerInstance() {
                /* Construct the resource manager instance. */
                util::ConstructAt(s_resource_manager_storage);
            }

            static ALWAYS_INLINE OsResourceManager &GetResourceManagerInstance() {
                return GetReference(s_resource_manager_storage);
            }
    };

    ALWAYS_INLINE OsResourceManager &GetResourceManager() {
        return ResourceManagerHolder::GetResourceManagerInstance();
    }

    ALWAYS_INLINE TickManager &GetTickManager() {
        return GetResourceManager().GetTickManager();
    }

}