#pragma once
#include <iostream>
#include "naive_desttile_simd_demo/mock_identity_nearest_int_simd.h"

template <class BaseClass>
MockIdentityNearestIntSimd<BaseClass>::MockIdentityNearestIntSimd(const cv::Mat1b& matsrc, cv::Mat1b& matdest)
    : BaseClass(matsrc, matdest)
    , m_tsc_total(0)
{}

template <class BaseClass>
MockIdentityNearestIntSimd<BaseClass>::~MockIdentityNearestIntSimd()
{
    uint64_t tsc_total = m_tsc_total.load();
    if (tsc_total)
    {
        std::cout << "(MockIdentityNearestIntSimd) Total TSC: " << tsc_total << std::endl;
    }
}

template <class BaseClass>
bool MockIdentityNearestIntSimd<BaseClass>::populate_source_coords(cv::Rect destrect, int& num_q_bits, 
    cv::Mat1i& srcxq, cv::Mat1i& srcyq) const
{
#pragma message "MockIdentityNearestIntSimd::populate_source_coords(...)"
#pragma message "The correctness of this function has not been verified."
    uint64_t tsc_start = __rdtsc();
    const cv::Size tilesz = destrect.size();
    const __m128i one = _mm_set1_epi8(1);
    const __m128i iota1 = _mm_add_epi8(one, _mm_slli_si128(one, 1));
    const __m128i iota3 = _mm_add_epi8(iota1, _mm_slli_si128(iota1, 2));
    const __m128i iota7 = _mm_add_epi8(iota3, _mm_slli_si128(iota3, 4));
    const __m128i iota15 = _mm_add_epi8(iota7, _mm_slli_si128(iota7, 8));
    const __m128i iota = _mm_sub_epi8(iota15, one);
    const __m512i iota512 = _mm512_cvtepi8_epi32(iota);
    const __m512i const_16 = _mm512_set1_epi32(16);
    for (int row = 0; row < tilesz.height; ++row)
    {
        __m512i x_fill = _mm512_set1_epi32(destrect.x);
        x_fill = _mm512_add_epi32(iota512, x_fill);
        __m512i y_fill = _mm512_set1_epi32(row + destrect.y);
        int* srcxq_row = srcxq.ptr<int>(row);
        int* srcyq_row = srcyq.ptr<int>(row);
        int col = 0;
        while (col + 64 <= tilesz.width)
        {
            _mm512_storeu_si512((__m512i*)(srcxq_row + col), x_fill);
            _mm512_storeu_si512((__m512i*)(srcyq_row + col), y_fill);
            x_fill = _mm512_add_epi32(x_fill, const_16);
            col += 16;
            _mm512_storeu_si512((__m512i*)(srcxq_row + col), x_fill);
            _mm512_storeu_si512((__m512i*)(srcyq_row + col), y_fill);
            x_fill = _mm512_add_epi32(x_fill, const_16);
            col += 16;
            _mm512_storeu_si512((__m512i*)(srcxq_row + col), x_fill);
            _mm512_storeu_si512((__m512i*)(srcyq_row + col), y_fill);
            x_fill = _mm512_add_epi32(x_fill, const_16);
            col += 16;
            _mm512_storeu_si512((__m512i*)(srcxq_row + col), x_fill);
            _mm512_storeu_si512((__m512i*)(srcyq_row + col), y_fill);
            x_fill = _mm512_add_epi32(x_fill, const_16);
            col += 16;
        }
        while (col + 16 <= tilesz.width)
        {
            _mm512_storeu_si512((__m512i*)(srcxq_row + col), x_fill);
            _mm512_storeu_si512((__m512i*)(srcyq_row + col), y_fill);
            x_fill = _mm512_add_epi32(x_fill, const_16);
            col += 16;
        }
        while (col < tilesz.width)
        {
            int destx = col + destrect.x;
            int desty = row + destrect.y;
            srcxq_row[col] = destx;
            srcyq_row[col] = desty;
            ++col;
        }
    }
    num_q_bits = 0;
    uint64_t tsc_stop = __rdtsc();
    this->m_tsc_total += tsc_stop - tsc_start;
    return true;
}
