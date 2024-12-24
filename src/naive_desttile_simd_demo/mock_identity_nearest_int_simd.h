#pragma once
#include <atomic>
#include <opencv2/core.hpp>
#include "naive_desttile_simd_demo/dest_tile_generator_simd.h"

template <class BaseClass = naive_desttile_simd_demo::DestTileGeneratorSimd>
class MockIdentityNearestIntSimd
    : public BaseClass
{
public:
    MockIdentityNearestIntSimd(const cv::Mat1b& matsrc, cv::Mat1b& matdest);

    ~MockIdentityNearestIntSimd();

    bool populate_source_coords(cv::Rect destrect, int& num_q_bits, 
        cv::Mat1i& srcxq, cv::Mat1i& srcyq) const final;

private:
    mutable std::atomic<uint64_t> m_tsc_total;
};
