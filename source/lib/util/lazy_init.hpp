#pragma once

#include <atomic>

template <typename T>
class LazyInit {
    private:
    T m_Data;
    std::atomic<bool> m_Guard;

    void CheckAccess() {
        /* TODO: Take exclusive access from the guard until Initialize is complete! This is not thread safe! */
        auto init = m_Guard.exchange(true);
        if(!init)
            m_Data.Initialize();
    }

    public:
    
    T* operator->() {
        CheckAccess();
        return &m_Data;
    }

    T& operator&() {
        CheckAccess();
        return m_Data;
    }
};