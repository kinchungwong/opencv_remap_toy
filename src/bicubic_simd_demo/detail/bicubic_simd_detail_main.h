// File: src/bicubic_simd_demo/detail/bicubic_simd_detail_main.h
//
#pragma once

namespace tests::bicubic_simd::detail
{

void test_single_nonzero_src_pixel();
void test_single_nonzero_xycoef();
void test_single_nonzero_clamp();

inline void run_all_tests()
{
    test_single_nonzero_src_pixel();
    test_single_nonzero_xycoef();
    test_single_nonzero_clamp();
}

} // namespace(tests::bicubic_simd::detail)
