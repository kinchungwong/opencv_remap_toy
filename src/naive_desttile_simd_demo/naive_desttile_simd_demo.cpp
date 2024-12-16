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
#include "naive_desttile_simd_demo/dest_tile_generator_simd.h"
#include "demo_utils/chrono_timer.h"
#include "demo_utils/init_background.h"

namespace naive_desttile_simd_demo
{

template <class BaseClass = DestTileGeneratorSimd>
class MyDestTileGen3 final
    : public BaseClass
{
public:
    MyDestTileGen3(const cv::Mat1b& matsrc, cv::Mat1b& matdest)
        : BaseClass(matsrc, matdest)
        , m_srcsz(matsrc.size())
    {}

    bool populate_source_coords_scalar(float destx, float desty, 
        float& srcx, float& srcy) const
    {
        int src_midx = (m_srcsz.width - 1) / 2;
        int src_midy = (m_srcsz.height - 1) / 2;
        float dx = destx - src_midx;
        float dy = desty - src_midy;
        float sx = (dx * 3.0f + dy) * (1.0f / 5.0f);
        float sy = (-dx + dy * 3.0f) * (1.0f / 5.0f);
        srcx = sx + src_midx;
        srcy = sy + src_midy;
        return true;
    }
private:
    const cv::Size m_srcsz;
};

} // namespace(naive_desttile_simd_demo)

int naive_desttile_simd_demo_4(int argc, char** argv)
{
    using namespace naive_desttile_simd_demo;
    const auto srcsz = cv::Size(720, 720);
    cv::Mat1b mat(srcsz);
    init_background(mat, 24);
    const auto destsz = cv::Size(720, 720);
    cv::Mat1b mat2(destsz);
    MyDestTileGen3 destgen(mat, mat2);
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
