#pragma once

#include <shared_mutex>
#include <mutex>

template <typename T>
class LockedContainer
{
public:
    LockedContainer() = default;
    LockedContainer(const T& data) : m_data(data) {};

    class ReadContainer
    {
    public:
        ReadContainer(const T& data, std::shared_mutex& mutex) : m_lock(mutex), m_data(data) {};
        const T& operator->() { return m_data; };
        const T& operator*() { return m_data; };
    private:
        const T& m_data;
        std::shared_lock<std::shared_mutex> m_lock;
    };

    class WriteContainer
    {
    public:
        WriteContainer(T& data, std::shared_mutex& mutex) : m_lock(mutex), m_data(data) {};
        T& operator->() { return m_data; };
        T& operator*() { return m_data; };
    private:
        T& m_data;
        std::unique_lock<std::shared_mutex> m_lock;
    };

    ReadContainer Get()
    {
        return ReadContainer(m_data, m_mutex);
    }

    WriteContainer Access()
    {
        return WriteContainer(m_data, m_mutex);
    }

    void operator=(const T& val)
    {
        *(Access()) = val;
    }

private:
    T m_data;
    std::shared_mutex m_mutex;
};