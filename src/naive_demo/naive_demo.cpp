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
#include "naive_demo/naive_demo.h"
#include "naive_demo/my_back_func.h"
#include "naive_demo/my_bilinear_sampler.h"

namespace naive_demo
{

void init_background(cv::Mat1b& dest, int band_thickness)
{
    const auto sz = dest.size();
    int rowband = 0;
    for (int rowstart = 0; rowstart < sz.height; rowstart += band_thickness)
    {
        const int rowstop = std::min(rowstart + band_thickness, sz.height);
        int colband = 0;
        for (int colstart = 0; colstart < sz.width; colstart += band_thickness)
        {
            uchar band_color = ((rowband ^ colband) & 1) ? 255 : 0;
            const int colstop = std::min(colstart + band_thickness, sz.width);
            const auto roi = cv::Rect(colstart, rowstart, colstop - colstart, rowstop - rowstart);
            dest(roi).setTo(band_color);
            ++colband;
        }
        ++rowband;
    }
}

} // namespace(naive_demo)

int naive_demo_main(int argc, char** argv)
{
    using namespace naive_demo;
    cv::Mat1b mat(720, 720);
    init_background(mat, 24);
    cv::Mat1b mat2(720, 720);
    const std::vector<float> coefs({
        +1.0f,
        -1.4e-3f,
        +1.0e-6f,
        +1.0e-9f,
        +1.0e-12f,
    });
    MyBackFunc backfun(mat2.size(), coefs);
    MyBilinearSampler sampler(mat, mat2, backfun);
    sampler(cv::Rect(0, 0, 720, 720));
    cv::imshow("opencv_remap_toy", mat2);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
