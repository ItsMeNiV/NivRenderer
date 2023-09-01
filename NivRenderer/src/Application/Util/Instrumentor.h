#pragma once
#include "Base.h"
#include <chrono>

class Instrumentor
{
public:
    Instrumentor(Instrumentor const&) = delete;
    void operator=(Instrumentor const&) = delete;

    static Instrumentor& GetInstance()
    {
        static Instrumentor instance;

        return instance;
    }

    void AddTiming(const char* name, int64_t timing)
    {
        std::string stringName(name);
        if (m_Timings.contains(stringName))
            m_Timings[std::string(name)] = (m_Timings[std::string(name)] + timing) / 2;
        else
            m_Timings[std::string(name)] = timing;
    }
    std::unordered_map<std::string, int64_t>& GetTimings() { return m_Timings; }

private:
    Instrumentor() {}

    std::unordered_map<std::string, int64_t> m_Timings;
};

class Timer
{
public:
    Timer(const char* name)
        : m_Name(name), m_Stopped(false)
    {
        m_StartTimepoint = std::chrono::steady_clock::now();
    }

    ~Timer()
    { 
        if (!m_Stopped)
            Stop();
    }

    void Stop()
    {
        const auto endTimepoint = std::chrono::steady_clock::now();
        const auto highResStart = std::chrono::duration<double, std::micro>{m_StartTimepoint.time_since_epoch()};
        const auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() -
            std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

        Instrumentor::GetInstance().AddTiming(m_Name, elapsedTime.count());

        m_Stopped = true;
    }

private:
    const char* m_Name;
    std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
    bool m_Stopped;

};

#define PROFILE_FUNCTION() Timer timer##__LINE__(__func__);