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
#include "demo_utils/chrono_timer.h"
#include "demo_utils/init_background.h"

namespace naive_desttile_demo
{

class MyDestTileGen final
    : public DestTileGeneratorBase
{
public:
    MyDestTileGen(const cv::Mat1b& matsrc, cv::Mat1b& matdest)
        : DestTileGeneratorBase(matsrc, matdest)
    {}

    bool populate_source_coords(cv::Rect destrect, int& num_q_bits, 
        cv::Mat1i& srcxq, cv::Mat1i& srcyq) const final
    {
        const cv::Size srcsz = this->m_matsrc->size();
        int src_midx = (srcsz.width - 1) / 2;
        int src_midy = (srcsz.height - 1) / 2;
        const cv::Size tilesz = destrect.size();
        for (int row = 0; row < tilesz.height; ++row)
        {
            for (int col = 0; col < tilesz.width; ++col)
            {
                int destx = col + destrect.x;
                int desty = row + destrect.y;
                int dx = destx - src_midx;
                int dy = desty - src_midy;
                int sx = (dx * 3 + dy) / 5;
                int sy = (-dx + dy * 3) / 5;
                int srcx = sx + src_midx;
                int srcy = sy + src_midy;
                srcxq.at<int>(row, col) = srcx;
                srcyq.at<int>(row, col) = srcy;
            }
        }
        num_q_bits = 0;
        return true;
    }
};

class MyDestTileGen2 final
    : public DestTileGeneratorBase
{
public:
    MyDestTileGen2(const cv::Mat1b& matsrc, cv::Mat1b& matdest)
        : DestTileGeneratorBase(matsrc, matdest)
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

} // namespace(naive_desttile_demo)

int naive_desttile_demo_main_1(int argc, char** argv)
{
    using namespace naive_desttile_demo;
    cv::Mat1b mat(720, 720);
    init_background(mat, 24);
    cv::Mat1b mat2(720, 720);
    MyDestTileGen destgen(mat, mat2);
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
    cv::Mat1b mat(720, 720);
    init_background(mat, 24);
    cv::Mat1b mat2(720, 720);
    MyDestTileGen2 destgen(mat, mat2);
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
    const auto srcsz = cv::Size(720, 720);
    cv::Mat1b mat(srcsz);
    init_background(mat, 24);
    const auto destsz = cv::Size(720, 720);
    cv::Mat1b mat2(destsz);
    MyDestTileGen2 destgen(mat, mat2);
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
