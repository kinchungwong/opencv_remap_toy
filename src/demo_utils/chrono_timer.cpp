#include "chrono_timer.h"
#include <stdio.h>
#include <chrono>

struct ClockTrait
{
    using ClockType = std::chrono::high_resolution_clock;
    using TimePoint = ClockType::time_point;
    using Duration = ClockType::duration;
};

struct ChronoTimer::OpaqueTimePoint
    : ClockTrait
{
    OpaqueTimePoint()
        : m_value(ClockType::now())
    {}

    int64_t elapsed_nsecs() const
    {
        auto dur = ClockType::now() - m_value;
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
        return nanos.count();
    }

    TimePoint m_value;
};

ChronoTimer::~ChronoTimer()
{
    int64_t nsecs = this->m_start->elapsed_nsecs();
    if (this->m_result_func)
    {
        this->m_result_func(nsecs * 1.0e-9);
    }
    if (this->m_print_func)
    {
        auto str_nsecs = std::to_string(nsecs);
        char buf[1024] = {0};
        snprintf(buf, std::size(buf) - 1U, "Timer %s total %s nsecs", this->m_name.c_str(), str_nsecs.c_str());
        const auto s = std::string(buf);
        this->m_print_func(s);
    }
}

ChronoTimer::ChronoTimer()
    : m_start(std::make_shared<OpaqueTimePoint>())
    , m_name()
    , m_result_func()
    , m_print_func()
{}

void ChronoTimer::set_arg(std::string name)
{
    this->m_name = std::move(name);
}

void ChronoTimer::set_arg(std::function<void(double)> result_func)
{
    this->m_result_func = std::move(result_func);
}

void ChronoTimer::set_arg(std::function<void(const std::string&)> print_func)
{
    this->m_print_func = std::move(print_func);
}
