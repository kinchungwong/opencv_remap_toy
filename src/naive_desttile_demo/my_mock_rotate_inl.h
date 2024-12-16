#pragma once
#include "naive_desttile_demo/my_mock_rotate.h"

namespace my_mock_rotate
{

template <class BaseClass>
MockRotateNearestInt<BaseClass>::MockRotateNearestInt(const cv::Mat1b& matsrc, cv::Mat1b& matdest)
    : BaseClass(matsrc, matdest)
{}

template <class BaseClass>
bool MockRotateNearestInt<BaseClass>::populate_source_coords(cv::Rect destrect, int& num_q_bits, 
    cv::Mat1i& srcxq, cv::Mat1i& srcyq) const
{
    const cv::Size srcsz = this->m_matsrc->size();
    int src_midx = (srcsz.width - 1) / 2;
    int src_midy = (srcsz.height - 1) / 2;
    const cv::Size tilesz = destrect.size();
    for (int row = 0; row < tilesz.height; ++row)
    {
        for (int col = 0; col < tilesz.width; ++col)
        {
            int destx = col + destrect.x;
            int desty = row + destrect.y;
            int dx = destx - src_midx;
            int dy = desty - src_midy;
            int sx = (dx * 3 + dy) / 5;
            int sy = (-dx + dy * 3) / 5;
            int srcx = sx + src_midx;
            int srcy = sy + src_midy;
            srcxq.at<int>(row, col) = srcx;
            srcyq.at<int>(row, col) = srcy;
        }
    }
    num_q_bits = 0;
    return true;
}

template <class BaseClass>
MockRotateBilinearFP<BaseClass>::MockRotateBilinearFP(const cv::Mat1b& matsrc, cv::Mat1b& matdest)
    : BaseClass(matsrc, matdest)
    , m_srcsz(matsrc.size())
{}

template <class BaseClass>
bool MockRotateBilinearFP<BaseClass>::populate_source_coords_scalar(float destx, float desty, 
    float& srcx, float& srcy) const
{
    int src_midx = (m_srcsz.width - 1) / 2;
    int src_midy = (m_srcsz.height - 1) / 2;
    float dx = destx - src_midx;
    float dy = desty - src_midy;
    float sx = (dx * 3.0f + dy) * (1.0f / 5.0f);
    float sy = (-dx + dy * 3.0f) * (1.0f / 5.0f);
    srcx = sx + src_midx;
    srcy = sy + src_midy;
    return true;
}

} // namespace(my_mock_rotate)
