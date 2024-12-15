#pragma once
#include <functional>
#include <memory>
#include <string>
#include <iostream>

class ChronoTimer
{
public:
    ChronoTimer();
    ~ChronoTimer();

    template <class... Args>
    ChronoTimer(Args&&... args);

public:
    ChronoTimer(const ChronoTimer&) = default;
    ChronoTimer(ChronoTimer&&) = default;
    ChronoTimer& operator=(const ChronoTimer&) = default;
    ChronoTimer& operator=(ChronoTimer&&) = default;

private:
    struct OpaqueTimePoint;

    template <class Arg0, class... Args>
    void set_args(Arg0&& arg0, Args&&... args);

    void set_arg(std::string name);
    void set_arg(std::function<void(double)> result_func);
    void set_arg(std::function<void(const std::string&)> print_func);

private:
    std::shared_ptr<OpaqueTimePoint> m_start;
    std::string m_name;
    std::function<void(double)> m_result_func;
    std::function<void(const std::string&)> m_print_func;
};

template <class... Args>
inline ChronoTimer::ChronoTimer(Args&&... args)
    : ChronoTimer()
{
    if constexpr (sizeof...(args) > 0)
    {
        this->set_args(std::move(args)...);
    }
}

template <class Arg0, class... Args>
inline void ChronoTimer::set_args(Arg0&& arg0, Args&&... args)
{
    this->set_arg(std::move(arg0));
    if constexpr (sizeof...(args) > 0)
    {
        this->set_args(std::move(args)...);
    }
}

class EzTimer
    : private ChronoTimer
{
public:
    EzTimer(const std::string& name)
        : ChronoTimer(name, EzTimer::default_print_func)
    {}

    ~EzTimer(){}

private:
    static void default_print_func(const std::string& msg) {
        std::cout << msg << std::endl;
    }
};
