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

namespace naive_desttile_demo
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

} // namespace(naive_desttile_demo)

int naive_desttile_demo_main(int argc, char** argv)
{
    using namespace naive_desttile_demo;
    cv::Mat1b mat(720, 720);
    init_background(mat, 24);
    cv::Mat1b mat2(720, 720);
    MyDestTileGen destgen(mat, mat2);
    destgen(cv::Rect(0, 0, 720, 720));
    cv::imshow("opencv_remap_toy", mat2);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
