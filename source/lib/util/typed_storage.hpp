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

#include <utility>
#include <new>
#include <memory>

namespace exl::util {

    template<typename T, size_t Size = sizeof(T), size_t Align = alignof(T)>
    struct TypedStorage {
        typename std::aligned_storage<Size, Align>::type _storage;
    };

    template<typename T>
    static ALWAYS_INLINE T *GetPointer(TypedStorage<T> &ts) {
        return std::launder(reinterpret_cast<T *>(std::addressof(ts._storage)));
    }

    template<typename T>
    static ALWAYS_INLINE const T *GetPointer(const TypedStorage<T> &ts) {
        return std::launder(reinterpret_cast<const T *>(std::addressof(ts._storage)));
    }

    template<typename T>
    static ALWAYS_INLINE T &GetReference(TypedStorage<T> &ts) {
        return *GetPointer(ts);
    }

    template<typename T>
    static ALWAYS_INLINE const T &GetReference(const TypedStorage<T> &ts) {
        return *GetPointer(ts);
    }

    namespace impl {

        template<typename T>
        static ALWAYS_INLINE T *GetPointerForConstructAt(TypedStorage<T> &ts) {
            return reinterpret_cast<T *>(std::addressof(ts._storage));
        }

    }

    template<typename T, typename... Args>
    static ALWAYS_INLINE T *ConstructAt(TypedStorage<T> &ts, Args &&... args) {
        return std::construct_at(impl::GetPointerForConstructAt(ts), std::forward<Args>(args)...);
    }

    template<typename T>
    static ALWAYS_INLINE void DestroyAt(TypedStorage<T> &ts) {
        return std::destroy_at(GetPointer(ts));
    }

}