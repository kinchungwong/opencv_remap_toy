#pragma once
#include <opencv2/core.hpp>
#include "naive_desttile_demo/dest_tile_generator_base.h"

namespace my_mock_rotate
{

template <class BaseClass = naive_desttile_demo::DestTileGeneratorBase>
class MockRotateNearestInt
    : public BaseClass
{
public:
    MockRotateNearestInt(const cv::Mat1b& matsrc, cv::Mat1b& matdest);

    bool populate_source_coords(cv::Rect destrect, int& num_q_bits, 
        cv::Mat1i& srcxq, cv::Mat1i& srcyq) const final;
};

template <class BaseClass = naive_desttile_demo::DestTileGeneratorBase>
class MockRotateBilinearFP
    : public BaseClass
{
public:
    MockRotateBilinearFP(const cv::Mat1b& matsrc, cv::Mat1b& matdest);

    bool populate_source_coords_scalar(float destx, float desty, 
        float& srcx, float& srcy) const final;

private:
    const cv::Size m_srcsz;
};

} // namespace(my_mock_rotate)
