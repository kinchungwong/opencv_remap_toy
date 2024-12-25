// File: src/bicubic_simd_demo/detail/bicubic_simd_detail.h
//
#pragma once
#include <cstdint>
#include <algorithm>
#include <x86intrin.h>

namespace bicubic_simd::detail
{

inline 
__attribute__((always_inline))
void unpack_u8x16_u32x4x4(
    __m128i u8x16,
    __m128i& u32x4_0,
    __m128i& u32x4_1,
    __m128i& u32x4_2,
    __m128i& u32x4_3)
{
    __m128i zero = _mm_setzero_si128();
    __m128i u16x8_0 = _mm_unpacklo_epi8(u8x16, zero);
    __m128i u16x8_1 = _mm_unpackhi_epi8(u8x16, zero);
    u32x4_0 = _mm_unpacklo_epi16(u16x8_0, zero);
    u32x4_1 = _mm_unpackhi_epi16(u16x8_0, zero);
    u32x4_2 = _mm_unpacklo_epi16(u16x8_1, zero);
    u32x4_3 = _mm_unpackhi_epi16(u16x8_1, zero);
}

/**
 * @brief SIMD arithmetic kernel for bicubic interpolation for one uint8 pixel.
 * @param src_u8x4x4 4x4 matrix of source pixel values in uint8.
 * @param coefs_s16q8_xy concatenated weights vector in x and y directions,
 *                       signed 16-bit scaled by 256 (8-bit fixed point).
 * @return uint8_t result of bicubic interpolation in uint8, clamped.
 */
inline 
__attribute__((noinline))
__attribute__((hot))
__attribute__((flatten))
uint8_t 
bicubic_mix_u8_v128_sse41(
    __m128i src_u8x4x4,
    __m128i coefs_s16q8_xy)
{
    __m128i coefs_sgnext = _mm_srai_epi16(coefs_s16q8_xy, 15);
    __m128i xcoef_32q8 = _mm_unpacklo_epi16(coefs_s16q8_xy, coefs_sgnext);
    __m128i ycoef_32q8 = _mm_unpackhi_epi16(coefs_s16q8_xy, coefs_sgnext);
    __m128i row_u32x4_0, row_u32x4_1, row_u32x4_2, row_u32x4_3;
    unpack_u8x16_u32x4x4(src_u8x4x4, row_u32x4_0, row_u32x4_1, row_u32x4_2, row_u32x4_3);
    __m128i ycoef_32q8_splat_0 = _mm_shuffle_epi32(ycoef_32q8, _MM_SHUFFLE(0, 0, 0, 0));
    __m128i row_yprod_s32_0 = _mm_mullo_epi32(row_u32x4_0, ycoef_32q8_splat_0);
    __m128i ycoef_32q8_splat_1 = _mm_shuffle_epi32(ycoef_32q8, _MM_SHUFFLE(1, 1, 1, 1));
    __m128i row_yprod_s32_1 = _mm_mullo_epi32(row_u32x4_1, ycoef_32q8_splat_1);
    __m128i ycoef_32q8_splat_2 = _mm_shuffle_epi32(ycoef_32q8, _MM_SHUFFLE(2, 2, 2, 2));
    __m128i row_yprod_s32_2 = _mm_mullo_epi32(row_u32x4_2, ycoef_32q8_splat_2);
    __m128i ycoef_32q8_splat_3 = _mm_shuffle_epi32(ycoef_32q8, _MM_SHUFFLE(3, 3, 3, 3));
    __m128i row_yprod_s32_3 = _mm_mullo_epi32(row_u32x4_3, ycoef_32q8_splat_3);
    __m128i row_yprod_s32_sums = _mm_add_epi32(
        _mm_add_epi32(row_yprod_s32_0, row_yprod_s32_1),
        _mm_add_epi32(row_yprod_s32_2, row_yprod_s32_3));
    __m128i row_xyprod_s32q16 = _mm_mullo_epi32(row_yprod_s32_sums, xcoef_32q8);
    int32_t row_xyprod_s32q16_values[4];
    _mm_storeu_si128(reinterpret_cast<__m128i*>(row_xyprod_s32q16_values), row_xyprod_s32q16);
    int32_t xyprod_sum_s32q16_biased = (
        +row_xyprod_s32q16_values[0] + 
        +row_xyprod_s32q16_values[1] + 
        +row_xyprod_s32q16_values[2] + 
        +row_xyprod_s32q16_values[3] + 
        +(1 << 15)
    );
    uint32_t result_u32q16_clamplo = static_cast<uint32_t>(std::max(xyprod_sum_s32q16_biased, 0));
    uint32_t result_u32q16_clamphi = std::min(result_u32q16_clamplo, (255u << 16));
    uint8_t result_u8 = static_cast<uint8_t>(result_u32q16_clamphi >> 16);
    return result_u8;
} // bicubic_mix_u8_v128_avx2

} // namespace(bicubic_simd::detail)
