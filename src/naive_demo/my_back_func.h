#pragma once

#include <cmath>
#include <vector>
#include <opencv2/core/core.hpp>

class MyBackFunc
{
public:
    MyBackFunc(cv::Size sz, const std::vector<float>& coefs)
        : m_sz(sz), m_coefs(coefs.begin(), coefs.end())
    {
        m_midx = (m_sz.width - 1.0) * 0.5;
        m_midy = (m_sz.height - 1.0) * 0.5;
    }

    MyBackFunc(const MyBackFunc&) = default;
    MyBackFunc(MyBackFunc&&) = default;
    MyBackFunc& operator=(const MyBackFunc&) = default;
    MyBackFunc& operator=(MyBackFunc&&) = default;
    ~MyBackFunc() = default;

    auto operator()(float destx, float desty, float& srcx, float& srcy) const -> void
    {
        float dx = destx - m_midx;
        float dy = desty - m_midy;
        float dr = std::hypot(dx, dy);
        float factor = 0.0;
        float drn = 1.0;
        for (const auto& coef : m_coefs)
        {
            factor += drn * coef;
            drn *= dr;
        }
        srcx = m_midx + dx * factor;
        srcy = m_midy + dy * factor;
    }
private:
    cv::Size m_sz;
    std::vector<float> m_coefs;
    float m_midx;
    float m_midy;
};
