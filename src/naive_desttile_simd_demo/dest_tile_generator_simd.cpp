#include <vector>
#include <iostream> // for troubleshoot only
#include "naive_desttile_simd_demo/dest_tile_generator_simd.h"

namespace naive_desttile_simd_demo
{

DestTileGeneratorSimd::DestTileGeneratorSimd(const cv::Mat1b& matsrc, cv::Mat1b& matdest)
    : naive_desttile_demo::DestTileGeneratorBase(matsrc, matdest)
{}

DestTileGeneratorSimd::~DestTileGeneratorSimd() {}

bool DestTileGeneratorSimd::populate_dest_bilinear(cv::Rect destrect, int num_q_bits, 
    const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const
{
    bool has_avx512 = this->get_has_avx512();
    bool unchecked = true; // TODO IMPORTANT CRITICAL: must verify index bounds
    if (num_q_bits == 0)
    {
        if (has_avx512 && unchecked)
        {
            if (this->populate_dest_bilinear_qzero_avx512_unchecked(destrect, num_q_bits, srcxq, srcyq))
            {
                return true;
            }
        }
        return this->populate_dest_bilinear_qzero(destrect, num_q_bits, srcxq, srcyq);
    }
    else if (num_q_bits >= 1 && num_q_bits <= 8)
    {
        if (has_avx512 && unchecked)
        {
            if (this->populate_dest_bilinear_q_avx512_unchecked(destrect, num_q_bits, srcxq, srcyq))
            {
                return true;
            }
        }
        return this->populate_dest_bilinear_q(destrect, num_q_bits, srcxq, srcyq);
    }
    return false;
}

bool DestTileGeneratorSimd::populate_dest_bilinear_q_avx512_unchecked(cv::Rect destrect, 
    int num_q_bits, const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const
{
    return false;
}

bool DestTileGeneratorSimd::populate_dest_bilinear_qzero_avx512_unchecked(cv::Rect destrect, 
    int num_q_bits, const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const
{
    return false;
}

bool DestTileGeneratorSimd::get_has_avx512() const
{
    if (!this->m_has_avx512.has_value())
    {
        std::vector<CpuFeatures> required_features{{
            CPU_AVX2,
            CPU_AVX512_COMMON,
        }};
        bool result = this->verify_cpu_features(required_features);
        this->m_has_avx512 = result;
    }
    return this->m_has_avx512.value();
}

bool DestTileGeneratorSimd::verify_cpu_features(
    const std::vector<CpuFeatures>& features) const
{
    for (const auto& feature : features)
    {
        if (!cv::checkHardwareSupport(feature))
        {
            return false;
        }
    }
    return true;
}

} // namespace(naive_desttile_simd_demo)
