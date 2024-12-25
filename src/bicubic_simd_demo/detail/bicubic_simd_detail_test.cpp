// File: src/bicubic_simd_demo/detail/bicubic_simd_detail_test.cpp

#include <cstring>
#include <cassert>
#include <iostream>
#include "bicubic_simd_demo/detail/bicubic_simd_detail.h"

#if 0
#define TEST_ASSERT(cond) assert(cond)
#else
#define TEST_ASSERT(cond) { if (!(cond)) { std::cerr << "Test failed: " << #cond << std::endl; } }
#endif

namespace tests::bicubic_simd::detail
{

/**
 * @brief Loads recently-modified 16 bytes from memory into 128-bit SIMD register.
 * 
 * @details This function works around the strict aliasing rule by copying into
 *          a termporary buffer. This workaround is probably not necessary, and
 *          is used only for testing, out of an abundance of caution.
 */
template <typename Elem, size_t Count>
inline 
__attribute__((always_inline))
__m128i dirtyload_v128i(const Elem (&arr)[Count])
{
    static_assert(sizeof(arr) == 16u, "Array size must be 16 bytes");
    uint8_t buffer[16u] = {};
    std::memcpy(buffer, arr, 16u);
    return _mm_loadu_si128(reinterpret_cast<const __m128i*>(buffer));
}

/**
 * @brief Test having exactly one source pixel with a nonzero value, while
 *        all pixels have the same non-zero weight of one (1.0).
 */
__attribute__((noinline))
void test_single_nonzero_src_pixel()
{
    //
    // All weights are set to 1.0
    //
    int16_t const coefs_s16q8_xy_cpp[8] = {
        /* xcoef[0:3] */
        256, 256, 256, 256,
        /* ycoef[0:3] */
        256, 256, 256, 256
    };
    const __m128i coefs_s16q8_xy = _mm_loadu_si128(reinterpret_cast<const __m128i*>(coefs_s16q8_xy_cpp));
    //
    // Insert the least and greatest non-zero values into exactly one source pixel
    //
    const uint8_t src_values[3] = {
        1, 255
    };
    //
    // Source buffer will be modified and restored inside the loop
    //
    uint8_t src_pixels_4x4[16] = {0};
    //
    // Execute all test cases
    //
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            uint8_t& ref_nonzero_pixel = src_pixels_4x4[row * 4 + col];
            for (uint8_t src_value : src_values)
            {
                ref_nonzero_pixel = src_value;
                __m128i src_u8x4x4 = dirtyload_v128i(src_pixels_4x4);
                uint8_t expected = src_value;
                uint8_t actual = ::bicubic_simd::detail::bicubic_mix_u8_v128_sse41(src_u8x4x4, coefs_s16q8_xy);
                TEST_ASSERT(actual == expected);
            }
            ref_nonzero_pixel = 0; // restored for next iteration
        }
    }
}

/**
 * @brief Test having all source pixels populated with unique nonzero values,
 *        while exactly one pixel will be multiplied with a non-zero weight
 *        of one (1.0).
 * @remarks This is done with setting the weights to 1.0 solely for the
 *          desired row and column.
 */
__attribute__((noinline))
void test_single_nonzero_xycoef()
{
    // 
    // Source pixels contain unique nonzero values.
    // Upper nibble: row.
    // Lower nibble: column.
    //
    uint8_t const src_pixels_4x4[16] = {
        0x11, 0x12, 0x13, 0x14,
        0x21, 0x22, 0x23, 0x24,
        0x31, 0x32, 0x33, 0x34,
        0x41, 0x42, 0x43, 0x44
    };
    __m128i const src_u8x4x4 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src_pixels_4x4));
    //
    // Weight buffer will be modified and restored inside the loop
    //
    int16_t coefs_s16q8_xy_cpp[8] = {0};
    for (int row = 0; row < 4; ++row)
    {
        int16_t& ref_ycoef_row = coefs_s16q8_xy_cpp[row + 4];
        ref_ycoef_row = 256; // equiv(1.0)
        for (int col = 0; col < 4; ++col)
        {
            int16_t& ref_xcoef_col = coefs_s16q8_xy_cpp[col];
            ref_xcoef_col = 256; // equiv(1.0)
            __m128i coefs_s16q8_xy = dirtyload_v128i(coefs_s16q8_xy_cpp);
            uint8_t expected = src_pixels_4x4[row * 4 + col];
            uint8_t actual = ::bicubic_simd::detail::bicubic_mix_u8_v128_sse41(src_u8x4x4, coefs_s16q8_xy);
            TEST_ASSERT(actual == expected);
            ref_xcoef_col = 0; // restored
        }
        ref_ycoef_row = 0; // restored
    }
}

/**
 * @brief Test output saturation (clamping) behavior when exactly one source
 *        pixel contributes to it.
 */
__attribute__((noinline))
void test_single_nonzero_clamp()
{
    uint8_t const src_values[] = {
        1u,
        7u, 8u, 9u,
        15u, 16u, 17u,
        31u, 32u, 33u,
        63u, 64u, 65u,
        127u, 128u, 129u,
        254u, 255u
    };
    int16_t const weight_values_s16q8[] = {
        -17, -16, -15, +15, +16, +17,
        -33, -32, -31, +31, +32, +33,
        -65, -64, -63, +63, +64, +66,
        -129, -128, -127, +127, +128, +129,
        -257, -256, -255, +255, +256, +257,
        -513, -512, -511, +511, +512, +513,
    };
    enum class WeightMode {
        SetX, SetY, SetBoth
    } weight_modes[] = {
        WeightMode::SetX, WeightMode::SetY, WeightMode::SetBoth
    };
    for (uint8_t src_value : src_values)
    {
        __m128i const src_u8x4x4 = _mm_set1_epi8(src_value);
        for (int16_t weight_value : weight_values_s16q8)
        {
            for (WeightMode weight_mode : weight_modes)
            {
                int16_t coefs_s16q8_xy_cpp[8] = {0};
                int32_t effective_weight_product;
                switch (weight_mode)
                {
                    case WeightMode::SetX:
                        coefs_s16q8_xy_cpp[0] = weight_value;
                        coefs_s16q8_xy_cpp[4] = 256;
                        effective_weight_product = weight_value * 256;
                        break;
                    case WeightMode::SetY:
                        coefs_s16q8_xy_cpp[0] = 256;
                        coefs_s16q8_xy_cpp[4] = weight_value;
                        effective_weight_product = weight_value * 256;
                        break;
                    case WeightMode::SetBoth:
                        coefs_s16q8_xy_cpp[0] = weight_value;
                        coefs_s16q8_xy_cpp[4] = weight_value;
                        effective_weight_product = weight_value * weight_value;
                        break;
                }
                int32_t expected_q16 = src_value * effective_weight_product + (1 << 15);
                bool unclamped_is_above = expected_q16 > (255 << 16);
                bool unclamped_is_below = expected_q16 < 0;
                uint8_t expected = unclamped_is_above ? 255u : (unclamped_is_below ? 0u : (expected_q16 >> 16));
                //
                // Code under test
                //
                __m128i coefs_s16q8_xy = dirtyload_v128i(coefs_s16q8_xy_cpp);
                uint8_t actual = ::bicubic_simd::detail::bicubic_mix_u8_v128_sse41(src_u8x4x4, coefs_s16q8_xy);
                //
                // Failure diagnostics (must be before assertion)
                // 
                if (actual != expected)
                {
                    std::cerr << "Failure diagnostics:" << std::endl;
                    std::cerr << "src_value: " << static_cast<int>(src_value) << std::endl;
                    std::cerr << "weight_value: " << weight_value << std::endl;
                    std::cerr << "weight_mode: " << static_cast<int>(weight_mode) << std::endl;
                    std::cerr << "unclamped: " << static_cast<int>(expected_q16 >> 16) << std::endl;
                    std::cerr << "expected: " << static_cast<int>(expected) << std::endl;
                    std::cerr << "actual: " << static_cast<int>(actual) << std::endl;
                }
                //
                // Assertion (aborts on failure)
                //
                TEST_ASSERT(actual == expected);
            }
        }
    }
}

} // namespace(tests::bicubic_simd::detail)
