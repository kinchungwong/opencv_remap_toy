#include <iostream>
#include <utility>
#include <memory>
#include <optional>
#include <opencv2/core.hpp>
#include "simd_caps_info/simd_caps_info.h"
#include "naive_demo/naive_demo.h"
#include "naive_desttile_demo/naive_desttile_demo.h"
#include "naive_desttile_simd_demo/naive_desttile_simd_demo.h"
#include "bicubic_simd_demo/detail/bicubic_simd_detail_main.h"

int print_opencv_build_info(int argc, char** argv)
{
    std::cout << cv::getBuildInformation() << std::endl;
    return 0;
}

std::string str_trim(const std::string& s)
{
    // static const std::string spaces{" \t\n\r"};
    static const std::string spaces{" \t"};
    static constexpr size_t npos = std::string::npos;
    size_t idx_first = s.find_first_not_of(spaces);
    if (idx_first == npos)
    {
        return {};
    }
    size_t idx_last = s.find_first_not_of(spaces);
    return s.substr(idx_first, idx_last - idx_first + 1UL);
}

using MainFuncType = int(*)(int, char**);
struct MainFuncTableEntry
{
    MainFuncType func_ptr;
    const char* func_name;
};

static constexpr const MainFuncTableEntry stc_main_func_table[] =
{
    { &print_opencv_build_info, "print_opencv_build_info" },
    { &simd_caps_info_main, "simd_caps_info_main" },
    { &naive_demo_main, "naive_demo_main" },
    { &naive_desttile_demo_main_1, "naive_desttile_demo_main_1" },
    { &naive_desttile_demo_main_2, "naive_desttile_demo_main_2" },
    { &naive_desttile_demo_main_3, "naive_desttile_demo_main_3" },
    { &naive_desttile_simd_demo_4, "naive_desttile_simd_demo_4" },
    { &naive_desttile_simd_demo_5, "naive_desttile_simd_demo_5" },
    { &tests::bicubic_simd::detail::run_all_tests_main, "tests::bicubic_simd::detail::run_all_tests_main" }
};

int select_execute_return(int argc, char** argv)
{
    std::cout << std::endl;
    int idx = 0;
    for (const auto& entry : stc_main_func_table)
    {
        std::cout << "[" << idx << "] " << entry.func_name << std::endl;
        ++idx;
    }
    std::cout << "[A] run_all" << std::endl;
    std::cout << "Select index:" << std::endl;
    std::string response;
    if (1 < argc && argv[1] != nullptr && argv[1][0] != '\0')
    {
        response = argv[1];
    }
    else
    {
        std::getline(std::cin, response);
    }
    response = str_trim(response);
    if (response == "A")
    {
        for (const auto& entry : stc_main_func_table)
        {
            const auto& name = entry.func_name;
            std::cout << "Running " << name << std::endl;
            int retcode = entry.func_ptr(argc, argv);
            std::cout << "Return code: " << retcode << std::endl;
            if (retcode != 0)
            {
                return retcode;
            }
        }
        return 0;
    }
    else
    {
        bool idx_good = false;
        try
        {
            idx = std::stoi(response, nullptr);
            idx_good = (idx >= 0 && idx < std::size(stc_main_func_table));
        }
        catch(const std::exception& e)
        {
            std::cerr << "Bad response." << std::endl;
            std::cerr << e.what() << '\n';
            return 1;
        }
        if (!idx_good)
        {
            std::cerr << "Bad response." << std::endl;
            return 1;
        }
        const auto& entry = stc_main_func_table[idx];
        int retcode = entry.func_ptr(argc, argv);
        return retcode;
    }
}

int main(int argc, char** argv)
{
    return select_execute_return(argc, argv);
}
