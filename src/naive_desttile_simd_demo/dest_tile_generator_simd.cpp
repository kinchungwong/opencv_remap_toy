#include <vector>
#include <iostream> // for troubleshoot only
#include <opencv2/core/simd_intrinsics.hpp>
#include <opencv2/core/hal/intrin.hpp>
#include "naive_desttile_simd_demo/dest_tile_generator_simd.h"

namespace naive_desttile_simd_demo
{

DestTileGeneratorSimd::DestTileGeneratorSimd(const cv::Mat1b& matsrc, cv::Mat1b& matdest)
    : naive_desttile_demo::DestTileGeneratorBase(matsrc, matdest)
    , m_tsc_total(0)
{}

DestTileGeneratorSimd::~DestTileGeneratorSimd()
{
    int64_t tsc_total = this->m_tsc_total;
    if (tsc_total)
    {
        std::cout << "(DestTileGeneratorSimd) TSC total: " << tsc_total << std::endl;
    }
}

bool DestTileGeneratorSimd::populate_dest_pixels(cv::Rect destrect, int num_q_bits, 
    const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const
{
    bool has_avx512 = this->get_has_avx512();
    bool unchecked = true; // TODO IMPORTANT CRITICAL: must verify index bounds
    if (num_q_bits == 0)
    {
        if (has_avx512 && unchecked)
        {
            if (this->populate_dest_pixels_qzero_avx512_unchecked(destrect, num_q_bits, srcxq, srcyq))
            {
                return true;
            }
        }
        return this->populate_dest_pixels_qzero(destrect, num_q_bits, srcxq, srcyq);
    }
    else if (num_q_bits >= 1 && num_q_bits <= 8)
    {
        if (has_avx512 && unchecked)
        {
            if (this->populate_dest_pixels_q_avx512_unchecked(destrect, num_q_bits, srcxq, srcyq))
            {
                return true;
            }
        }
        return this->populate_dest_pixels_q(destrect, num_q_bits, srcxq, srcyq);
    }
    return false;
}

bool DestTileGeneratorSimd::populate_dest_pixels_q_avx512_unchecked(cv::Rect destrect, 
    int num_q_bits, const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const
{
    return false;
}

bool DestTileGeneratorSimd::populate_dest_pixels_qzero_avx512_unchecked(cv::Rect destrect, 
    int num_q_bits, const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const
{
    if (num_q_bits != 0)
    {
        return false;
    }
#if CV_SIMD512
#pragma message "DestTileGeneratorSimd::populate_dest_pixels_qzero_avx512_unchecked(...)"
#pragma message "This function contains missing critical code and may lead to invalid memory accesses and/or crashes."
    uint64_t tsc_start = __rdtsc();
    const cv::Size tilesz = destrect.size();
    const cv::Mat1b& matsrc = *(this->m_matsrc);
    cv::Mat1b& matdest = *(this->m_matdest);
    const size_t src_step = matsrc.step[0];
    const __m512i src_step_vec = _mm512_set1_epi32(src_step);
    // For each source pixel, AVX512 gather will read at least 4 bytes.
    // Therefore:
    // (1) The source address need to be far enough from the end of the
    // ... image to avoid buffer read overrun (segmentation fault).
    // (2) The gathered 32-bit values need to be masked and packed into
    // ... 8-bit values for writing.
    const __m512i overread_mask = _mm512_set1_epi32(255uL);
    for (int row = 0; row < tilesz.height; ++row)
    {
        int desty = row + destrect.y;
        const int* srcxq_row = srcxq.ptr<int>(row);
        const int* srcyq_row = srcyq.ptr<int>(row);
        uchar* matdest_row = matdest.ptr<uchar>(desty);
        for (int colstart = 0; colstart < tilesz.width; colstart += 16)
        {
            if (colstart + 16 <= tilesz.width)
            {
                const int* srcxq_colstart = srcxq_row + colstart;
                const int* srcyq_colstart = srcyq_row + colstart;
                uchar* matdest_colstart = matdest_row + colstart;
                __m512i srcxq_vec = _mm512_loadu_si512(srcxq_colstart);
                __m512i srcyq_vec = _mm512_loadu_si512(srcyq_colstart);
                //
                // TODO CRITICAL: implement range check here.
                //
                __m512i srcaddr_vec = _mm512_add_epi32(srcxq_vec, _mm512_mullo_epi32(srcyq_vec, src_step_vec));
                __m512i srcval_vec = _mm512_i32gather_epi32(srcaddr_vec, matsrc.data, 1);
                // Because the image is 8-bit, only the lowest 8 bits of each 32-bit element are valid.
                // Pack the valid 8-bit elements into a vector.
                __m128i srcval_vec_u8 = _mm512_cvtepi32_epi8(_mm512_and_epi32(srcval_vec, overread_mask));
                _mm_storeu_si128((__m128i*)matdest_colstart, srcval_vec_u8);
            }
            else
            {
                //
                // TODO CRITICAL: implement non-vectorized code here.
                //
            }
        }
    }
    uint64_t tsc_stop = __rdtsc();
    this->m_tsc_total += (tsc_stop - tsc_start);
    return true;
#else // is_zero(CV_SIMD512)
    return false;
#endif
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
        return result;
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
