#pragma once
#include <optional>
#include <opencv2/core.hpp>
#include "naive_desttile_demo/dest_tile_generator_base.h"

namespace naive_desttile_simd_demo
{

class DestTileGeneratorSimd 
    : public naive_desttile_demo::DestTileGeneratorBase
{
protected:
    DestTileGeneratorSimd(const cv::Mat1b& matsrc, cv::Mat1b& matdest);
    ~DestTileGeneratorSimd();

protected:
    bool populate_dest_bilinear(cv::Rect destrect, int num_q_bits, 
        const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const override;

    virtual bool populate_dest_bilinear_q_avx512_unchecked(cv::Rect destrect, 
        int num_q_bits, const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const;

    virtual bool populate_dest_bilinear_qzero_avx512_unchecked(cv::Rect destrect, 
        int num_q_bits, const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const;

    virtual bool get_has_avx512() const;

    virtual bool verify_cpu_features(const std::vector<CpuFeatures>& features) const;

protected:
    mutable std::optional<bool> m_has_avx512;
};

} // namespace(naive_desttile_simd_demo)
