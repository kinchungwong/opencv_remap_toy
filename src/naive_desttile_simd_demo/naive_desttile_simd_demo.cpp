#include <utility>
#include <vector>
#include <cmath>
#include <functional>
#include <stdexcept>
#include <memory>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "naive_desttile_simd_demo/naive_desttile_simd_demo.h"
#include "naive_desttile_demo/dest_tile_generator_base.h"
#include "naive_desttile_demo/my_mock_rotate.h"
#include "naive_desttile_simd_demo/dest_tile_generator_simd.h"
#include "demo_utils/chrono_timer.h"
#include "demo_utils/init_background.h"

/// Class template member function inline definitions
#include "naive_desttile_demo/my_mock_rotate_inl.h"
#include "naive_desttile_simd_demo/mock_identity_nearest_int_simd_inl.h"

int naive_desttile_simd_demo_4(int argc, char** argv)
{
    using naive_desttile_simd_demo::DestTileGeneratorSimd;
    using my_mock_rotate::MockRotateNearestInt;
    using MyDestGen = MockRotateNearestInt<DestTileGeneratorSimd>;
    const auto srcsz = cv::Size(720, 720);
    cv::Mat1b mat(srcsz);
    init_background(mat, 24);
    const auto destsz = cv::Size(720, 720);
    cv::Mat1b mat2(destsz);
    MyDestGen destgen(mat, mat2);
    {
        auto strip_func = [destsz, &destgen](const cv::Range& strip_row_range)
        {
            int rowstart = strip_row_range.start;
            int rowstop =  strip_row_range.end;
            cv::Rect strip_rect(0, rowstart, destsz.width, rowstop - rowstart);
            destgen(strip_rect);
        };
        const auto full_range = cv::Range(0, destsz.height);
        double nstripes = cv::getNumberOfCPUs();
        {
            EzTimer timer("naive_desttile_simd_demo_4");
            cv::parallel_for_(full_range, strip_func, nstripes);
        }
    }
    cv::imshow("opencv_remap_toy", mat2);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}

int naive_desttile_simd_demo_5(int argc, char** argv)
{
    using naive_desttile_simd_demo::DestTileGeneratorSimd;
    using MyDestGen = MockIdentityNearestIntSimd<DestTileGeneratorSimd>;
    const auto srcsz = cv::Size(720, 720);
    cv::Mat1b mat(srcsz);
    init_background(mat, 24);
    const auto destsz = cv::Size(720, 720);
    cv::Mat1b mat2(destsz);
    MyDestGen destgen(mat, mat2);
    {
        auto strip_func = [destsz, &destgen](const cv::Range& strip_row_range)
        {
            int rowstart = strip_row_range.start;
            int rowstop =  strip_row_range.end;
            cv::Rect strip_rect(0, rowstart, destsz.width, rowstop - rowstart);
            destgen(strip_rect);
        };
        const auto full_range = cv::Range(0, destsz.height);
        double nstripes = cv::getNumberOfCPUs();
        nstripes = 360.0;
        {
            EzTimer timer("naive_desttile_simd_demo_5");
            cv::parallel_for_(full_range, strip_func, nstripes);
        }
    }
    cv::imshow("opencv_remap_toy", mat2);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
