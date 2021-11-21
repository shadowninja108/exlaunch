#pragma once

#include "ModuleObject.hpp"

namespace rtld {

struct ModuleObjectList {
    ModuleObject *front;
    ModuleObject *back;

    class Iterator;

    Iterator begin() { return Iterator(this->back, false); }

    Iterator end() { return Iterator((ModuleObject *)this, false); }

    Iterator rbegin() { return Iterator(this->front, true); }

    Iterator rend() { return Iterator((ModuleObject *)this, true); }

    class Iterator {
       public:
        Iterator(ModuleObject *pModule, bool reverted)
            : m_pCurrentModule(pModule), m_Reverted(reverted) {}

        Iterator &operator=(ModuleObject *pModule) {
            m_pCurrentModule = pModule;
            return *this;
        }

        Iterator &operator++() {
            if (m_pCurrentModule) {
                m_pCurrentModule = m_Reverted ? m_pCurrentModule->next
                                              : m_pCurrentModule->prev;
            }
            return *this;
        }

        bool operator!=(const Iterator &iterator) {
            return m_pCurrentModule != iterator.m_pCurrentModule;
        }

        ModuleObject *operator*() { return m_pCurrentModule; }

       private:
        ModuleObject *m_pCurrentModule;
        bool m_Reverted;
    };
};

#ifdef __aarch64__
static_assert(sizeof(ModuleObjectList) == 0x10, "ModuleObjectList isn't valid");
#elif __arm__
static_assert(sizeof(ModuleObjectList) == 0x8, "ModuleObjectList isn't valid");
#endif

}  // namespace rtld