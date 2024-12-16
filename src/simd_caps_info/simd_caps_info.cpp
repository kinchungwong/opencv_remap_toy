#include "simd_caps_info/simd_caps_info.h"
#include <opencv2/cvconfig.h>
#include <opencv2/core.hpp>
#include <opencv2/core/simd_intrinsics.hpp>
#include <opencv2/core/hal/intrin.hpp>
#include <iostream>
#include <string>
#include <vector>

void print_simd_defined_macros()
{
    std::vector<std::string> pp_defs;
#if CV_SIMD128
    pp_defs.push_back("CV_SIMD128");
#endif
#if CV_SIMD256
    pp_defs.push_back("CV_SIMD256");
#endif
#if CV_SIMD512
    pp_defs.push_back("CV_SIMD512");
#endif
    for (const auto& pp_def : pp_defs)
    {
        std::cout << pp_def << " (defined)" << std::endl;
    }
}

struct CpuFeatureEntry
{
    ::CpuFeatures feature;
    const char* name;
};

static constexpr const CpuFeatureEntry CpuFeatureTable[] = {
    { CPU_MMX, "CPU_MMX" },
    { CPU_SSE, "CPU_SSE" },
    { CPU_SSE2, "CPU_SSE2" },
    { CPU_SSE3, "CPU_SSE3" },
    { CPU_SSSE3, "CPU_SSSE3" },
    { CPU_SSE4_1, "CPU_SSE4_1" },
    { CPU_SSE4_2, "CPU_SSE4_2" },
    { CPU_POPCNT, "CPU_POPCNT" },
    { CPU_FP16, "CPU_FP16" },
    { CPU_AVX, "CPU_AVX" },
    { CPU_AVX2, "CPU_AVX2" },
    { CPU_FMA3, "CPU_FMA3" },
    ///
    { CPU_AVX_512F, "CPU_AVX_512F" },
    { CPU_AVX_512BW, "CPU_AVX_512BW" },
    { CPU_AVX_512CD, "CPU_AVX_512CD" },
    { CPU_AVX_512DQ, "CPU_AVX_512DQ" },
    { CPU_AVX_512ER, "CPU_AVX_512ER" },
    { CPU_AVX_512IFMA, "CPU_AVX_512IFMA" },
    { CPU_AVX_512PF, "CPU_AVX_512PF" },
    { CPU_AVX_512VBMI, "CPU_AVX_512VBMI" },
    { CPU_AVX_512VL, "CPU_AVX_512VL" },
    { CPU_AVX_512VBMI2, "CPU_AVX_512VBMI2" },
    { CPU_AVX_512VNNI, "CPU_AVX_512VNNI" },
    { CPU_AVX_512BITALG, "CPU_AVX_512BITALG" },
    { CPU_AVX_512VPOPCNTDQ, "CPU_AVX_512VPOPCNTDQ" },
    { CPU_AVX_5124VNNIW, "CPU_AVX_5124VNNIW" },
    { CPU_AVX_5124FMAPS, "CPU_AVX_5124FMAPS" },
    ///
    { CPU_NEON, "CPU_NEON" },
    { CPU_NEON_DOTPROD, "CPU_NEON_DOTPROD" },
    { CPU_NEON_FP16, "CPU_NEON_FP16" },
    { CPU_NEON_BF16, "CPU_NEON_BF16" },
    ///
    { CPU_MSA, "CPU_MSA" },
    ///
    { CPU_RISCVV, "CPU_RISCVV" },
    //
    { CPU_VSX, "CPU_VSX" },
    { CPU_VSX3, "CPU_VSX3" },
    ///
    { CPU_RVV, "CPU_RVV" },
    ///
    { CPU_LSX, "CPU_LSX" },
    { CPU_LASX, "CPU_LASX" },
    ///
    { CPU_AVX512_SKX, "CPU_AVX512_SKX" },
    { CPU_AVX512_COMMON, "CPU_AVX512_COMMON" },
    { CPU_AVX512_KNL, "CPU_AVX512_KNL" },
    { CPU_AVX512_KNM, "CPU_AVX512_KNM" },
    { CPU_AVX512_CNL, "CPU_AVX512_CNL" },
    { CPU_AVX512_CLX, "CPU_AVX512_CLX" },
    { CPU_AVX512_ICL, "CPU_AVX512_ICL" },
};

void print_cpu_features()
{
    for (const auto& entry : CpuFeatureTable)
    {
        if (cv::checkHardwareSupport(entry.feature))
        {
            std::cout << entry.name << " (supported)" << std::endl;
        }
    }
}

void simd_demo_print_squares()
{
    std::cout << "This demo function prints squares of numbers from 0 to 31." << std::endl;
    int16_t buf[32];
    for (int i = 0; i < 32; ++i)
    {
        buf[i] = i;
    }
    cv::v_int16x32 v1 = cv::v512_load(buf);
    v1 = cv::v_mul(v1, v1);
    cv::v_store(buf, v1);
    for (const auto& v : buf)
    {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}

int simd_caps_info_main(int argc, char** argv)
{
    print_simd_defined_macros();
    print_cpu_features();
    simd_demo_print_squares();
    return 0;
}
