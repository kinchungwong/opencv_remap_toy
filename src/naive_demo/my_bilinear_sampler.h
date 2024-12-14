#pragma once

#include <cmath>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <opencv2/core/core.hpp>

class MyBilinearSampler
{
public:
    typedef std::function<void(float, float, float&, float&)> MapFuncType;
    MyBilinearSampler(cv::Mat1b& matsrc, cv::Mat1b& matdest, MapFuncType mapfunc)
        : m_matsrc(std::make_unique<cv::Mat1b>(matsrc))
        , m_matdest(std::make_unique<cv::Mat1b>(matdest))
        , m_mapfunc(mapfunc)
    {}

    auto operator() (cv::Rect destrect) const -> void
    {
        validate_dest_rect_else_throw(destrect);
        const auto& matsrc = *m_matsrc;
        auto& matdest = *m_matdest;
        const auto srcsz = matsrc.size();
        for (int offy = 0; offy < destrect.height; ++offy)
        {
            int desty = destrect.y + offy;
            for (int offx = 0; offx < destrect.width; ++offx)
            {
                int destx = destrect.x + offx;
                float fsrcx, fsrcy;
                m_mapfunc(
                    static_cast<float>(destx),
                    static_cast<float>(desty),
                    static_cast<float&>(fsrcx),
                    static_cast<float&>(fsrcy));
                int isrcx = static_cast<int>(std::floor(fsrcx));
                int isrcy = static_cast<int>(std::floor(fsrcy));
                if (isrcx < 0 || 
                    isrcy < 0 ||
                    isrcx + 1 >= srcsz.width ||
                    isrcy + 1 >= srcsz.height)
                {
                    continue;
                }
                fsrcx -= isrcx;
                fsrcy -= isrcy;
                uchar pix_topleft = matsrc.at<uchar>(isrcy, isrcx);
                uchar pix_toprght = matsrc.at<uchar>(isrcy, isrcx + 1);
                uchar pix_botleft = matsrc.at<uchar>(isrcy + 1, isrcx);
                uchar pix_botrght = matsrc.at<uchar>(isrcy + 1, isrcx + 1);
                float fpix_top = pix_topleft + (pix_toprght - pix_topleft) * fsrcx;
                float fpix_bot = pix_botleft + (pix_botrght - pix_botleft) * fsrcx;
                float fpix = fpix_top + (fpix_bot - fpix_top) * fsrcy;
                uchar pix_out = cv::saturate_cast<uchar>(fpix);
                matdest.at<uchar>(desty, destx) = pix_out;
            }
        }
    }

private:
    void validate_dest_rect_else_throw(const cv::Rect& destrect) const
    {
        static constexpr const char* msg = "Bad dest rect";
        const auto destsz = m_matdest->size();
        const auto br = destrect.br();
        if (destrect.width < 0 || 
            destrect.height < 0 ||
            destrect.x < 0 ||
            destrect.y < 0 ||
            br.x > destsz.width ||
            br.y > destsz.height)
        {
            throw std::invalid_argument(msg);
        }
    }

private:
    std::unique_ptr<cv::Mat1b> m_matsrc;
    std::unique_ptr<cv::Mat1b> m_matdest;
    MapFuncType m_mapfunc;
};
