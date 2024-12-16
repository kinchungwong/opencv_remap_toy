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
#include "demo_utils/chrono_timer.h"
#include "demo_utils/init_background.h"

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
    {
        EzTimer timer("naive_demo_main");
        sampler(cv::Rect(0, 0, 720, 720));
    }
    cv::imshow("opencv_remap_toy", mat2);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
