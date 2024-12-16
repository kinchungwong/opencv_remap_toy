#include <opencv2/imgproc/imgproc.hpp>

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
#include "naive_desttile_demo/naive_desttile_demo.h"
#include "naive_desttile_demo/dest_tile_generator_base.h"
#include "naive_desttile_demo/my_mock_rotate.h"
#include "demo_utils/chrono_timer.h"
#include "demo_utils/init_background.h"

/// Class template member function inline definitions
#include "naive_desttile_demo/my_mock_rotate_inl.h"

int naive_desttile_demo_main_1(int argc, char** argv)
{
    using namespace naive_desttile_demo;
    using my_mock_rotate::MockRotateNearestInt;
    cv::Mat1b mat(720, 720);
    init_background(mat, 24);
    cv::Mat1b mat2(720, 720);
    MockRotateNearestInt destgen(mat, mat2);
    {
        EzTimer timer("naive_desttile_demo_main_1");
        destgen(cv::Rect(0, 0, 720, 720));
    }
    cv::imshow("opencv_remap_toy", mat2);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}

int naive_desttile_demo_main_2(int argc, char** argv)
{
    using namespace naive_desttile_demo;
    using my_mock_rotate::MockRotateBilinearFP;
    cv::Mat1b mat(720, 720);
    init_background(mat, 24);
    cv::Mat1b mat2(720, 720);
    MockRotateBilinearFP destgen(mat, mat2);
    {
        EzTimer timer("naive_desttile_demo_main_2");
        destgen(cv::Rect(0, 0, 720, 720));
    }
    cv::imshow("opencv_remap_toy", mat2);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}

int naive_desttile_demo_main_3(int argc, char** argv)
{
    using namespace naive_desttile_demo;
    using my_mock_rotate::MockRotateNearestInt;
    const auto srcsz = cv::Size(720, 720);
    cv::Mat1b mat(srcsz);
    init_background(mat, 24);
    const auto destsz = cv::Size(720, 720);
    cv::Mat1b mat2(destsz);
    MockRotateNearestInt destgen(mat, mat2);
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
            EzTimer timer("naive_desttile_demo_main_3");
            cv::parallel_for_(full_range, strip_func, nstripes);
        }
    }
    cv::imshow("opencv_remap_toy", mat2);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
