#include "demo_utils/init_background.h"

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
