#include "naive_desttile_demo/dest_tile_generator_base.h"

namespace naive_desttile_demo
{

DestTileGeneratorBase::~DestTileGeneratorBase()
{}

DestTileGeneratorBase::DestTileGeneratorBase(const cv::Mat1b& matsrc, cv::Mat1b& matdest)
    : m_matsrc(std::make_unique<const cv::Mat1b>(matsrc))
    , m_matdest(std::make_unique<cv::Mat1b>(matdest))
{}

bool DestTileGeneratorBase::populate_source_coords(cv::Rect destrect, int& num_q_bits, 
    cv::Mat1i& srcxq, cv::Mat1i& srcyq) const
{
    // As a virtual function, this function can be overridden.
    // If that is the case, this constant has no relevance.
    static constexpr const int DEFAULT_NUM_Q_BITS = 8;
    num_q_bits = DEFAULT_NUM_Q_BITS;
    const cv::Size tilesz = destrect.size();
    int q_scale = (1 << num_q_bits);
    for (int row = 0; row < tilesz.height; ++row)
    {
        for (int col = 0; col < tilesz.width; ++col)
        {
            int destx = col + destrect.x;
            int desty = row + destrect.y;
            float srcx, srcy;
            bool good = this->populate_source_coords_scalar(
                static_cast<float>(destx),
                static_cast<float>(desty),
                static_cast<float&>(srcx),
                static_cast<float&>(srcy)
            );
            if (!good)
            {
                return false;
            }
            int& refxq = srcxq.at<int>(row, col);
            refxq = cv::saturate_cast<int>(srcx * q_scale);
            int& refyq = srcyq.at<int>(row, col);
            refyq = cv::saturate_cast<int>(srcy * q_scale);
        }
    }
    return true;
}

bool DestTileGeneratorBase::populate_source_coords_scalar(float destx, float desty, 
    float& srcx, float& srcy) const
{
    return false;
}

bool DestTileGeneratorBase::clamp_source_coords(cv::Rect destrect, int num_q_bits, 
    cv::Mat1i& srcxq, cv::Mat1i& srcyq) const
{
    if (this->clamp_source_coords_scalar(destrect, num_q_bits, srcxq, srcyq))
    {
        return true;
    }
    return false;
}

bool DestTileGeneratorBase::clamp_source_coords_scalar(cv::Rect destrect, int num_q_bits, 
    cv::Mat1i& srcxq, cv::Mat1i& srcyq) const
{
    if (num_q_bits < 0 || num_q_bits > 8)
    {
        return false;
    }
    const cv::Size tilesz = destrect.size();
    const cv::Size srcsz = this->m_matsrc->size();
    const int xmaxi = (srcsz.width - 1);
    const int ymaxi = (srcsz.height - 1);
    if (xmaxi >= (INT_MAX >> num_q_bits) ||
        ymaxi >= (INT_MAX >> num_q_bits))
    {
        return false;
    }
    const int xmaxq = xmaxi << num_q_bits;
    const int ymaxq = ymaxi << num_q_bits;
    for (int row = 0; row < tilesz.height; ++row)
    {
        for (int col = 0; col < tilesz.width; ++col)
        {
            int& refxq = srcxq.at<int>(row, col);
            refxq = std::clamp(refxq, 0, xmaxq);
            int& refyq = srcyq.at<int>(row, col);
            refyq = std::clamp(refyq, 0, ymaxq);
        }
    }
    return true;
}

bool DestTileGeneratorBase::populate_dest_pixels(cv::Rect destrect, int num_q_bits, 
    const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const
{
    if (num_q_bits == 0)
    {
        return this->populate_dest_pixels_qzero(destrect, num_q_bits, srcxq, srcyq);
    }
    else
    {
        return this->populate_dest_pixels_q(destrect, num_q_bits, srcxq, srcyq);
    }
}

bool DestTileGeneratorBase::populate_dest_pixels_q(cv::Rect destrect, int num_q_bits, 
    const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const
{
    if (num_q_bits < 0 || num_q_bits > 8)
    {
        return false;
    }
    const cv::Size tilesz = destrect.size();
    const int q_unit = (1 << num_q_bits);
    const int q_mask = q_unit - 1;
    const int rounding_bias = ((1 << (2 * num_q_bits)) >> 1);
    const cv::Mat1b& matsrc = *(this->m_matsrc);
    cv::Mat1b& matdest = *(this->m_matdest);
    for (int row = 0; row < tilesz.height; ++row)
    {
        for (int col = 0; col < tilesz.width; ++col)
        {
            int xq = srcxq.at<int>(row, col);
            int yq = srcyq.at<int>(row, col);
            int xi = xq >> num_q_bits;
            int yi = yq >> num_q_bits;
            int xf = xq & q_mask;
            int yf = yq & q_mask;
            uchar top_left = matsrc.at<uchar>(yi, xi);
            uchar top_rght = xf ? matsrc.at<uchar>(yi, xi + 1) : top_left;
            uchar bot_left = yf ? matsrc.at<uchar>(yi + 1, xi) : top_left;
            uchar bot_rght = (xf & yf) ? matsrc.at<uchar>(yi + 1, xi + 1) : top_left;
            int top_blended = (top_left << num_q_bits) + (top_rght - top_left) * xf;
            int bot_blended = (bot_left << num_q_bits) + (bot_rght - bot_left) * xf;
            int blended = (top_blended << num_q_bits) + (bot_blended - top_blended) * yf;
            int rounded = (blended + rounding_bias) >> (2 * num_q_bits);
            uchar pix_out = cv::saturate_cast<uchar>(rounded);
            int desty = row + destrect.y;
            int destx = col + destrect.x;
            matdest.at<uchar>(desty, destx) = pix_out;
        }
    }
    return true;
}

bool DestTileGeneratorBase::populate_dest_pixels_qzero(cv::Rect destrect, int num_q_bits, 
    const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const
{
    if (num_q_bits != 0)
    {
        return false;
    }
    const cv::Size tilesz = destrect.size();
    const cv::Mat1b& matsrc = *(this->m_matsrc);
    cv::Mat1b& matdest = *(this->m_matdest);
    for (int row = 0; row < tilesz.height; ++row)
    {
        for (int col = 0; col < tilesz.width; ++col)
        {
            int xi = srcxq.at<int>(row, col);
            int yi = srcyq.at<int>(row, col);
            uchar top_left = matsrc.at<uchar>(yi, xi);
            int desty = row + destrect.y;
            int destx = col + destrect.x;
            matdest.at<uchar>(desty, destx) = top_left;
        }
    }
    return true;
}

bool DestTileGeneratorBase::operator() (cv::Rect destrect) const
{
    static constexpr const char* msg_where_pop = "DestTileGeneratorBase::populate_source_coords()";
    static constexpr const char* msg_where_clamp = "DestTileGeneratorBase::clamp_source_coords()";
    static constexpr const char* msg_where_bilinear = "DestTileGeneratorBase::populate_dest_pixels()";
    static constexpr const char* msg_bad_q = "Bad number of fractional bits";
    this->validate_dest_rect_else_throw(destrect);
    const cv::Size destsz = destrect.size();
    cv::Mat1i srcxq(destsz);
    cv::Mat1i srcyq(destsz);
    int num_q_bits = -1;
    bool good = this->populate_source_coords(destrect, num_q_bits, srcxq, srcyq);
    if (!good)
    {
        throw std::runtime_error(msg_where_pop);
    }
    if (num_q_bits < 0 || num_q_bits > 8)
    {
        throw std::invalid_argument(msg_bad_q);
    }
    good = this->clamp_source_coords(destrect, num_q_bits, srcxq, srcyq);
    if (!good)
    {
        throw std::runtime_error(msg_where_clamp);
    }
    good = this->populate_dest_pixels(destrect, num_q_bits, srcxq, srcyq);
    if (!good)
    {
        throw std::runtime_error(msg_where_bilinear);
    }
    return true;
}

void DestTileGeneratorBase::validate_dest_rect_else_throw(const cv::Rect& destrect) const
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

} // namespace(naive_desttile_demo)
