// File: src/v_draft_1/mapping/mapping_adapter.cpp

#include "v_draft_1/mapping/mapping_adapter.h"

namespace opencv_remap::draft_1
{

template <class TBase>
MappingAdapter<TBase>::MappingAdapter()
{
    const TBase& base = static_cast<TBase&>(*this);
    auto base_caps = this->m_base_caps = base.caps();
    auto base_min_caps = (base_caps & this->stc_min_caps);
    if (base_min_caps == CapFlags::None)
    {
        static const char* what = "Expects at least one of EvalFloatFloat EvalIntFixed in Base.";
        throw std::logic_error(what);
    }
    this->m_adapted_caps = this->m_base_caps | this->stc_fulfilled_caps;
    this->m_base_q_bits = base.num_q_bits();
    this->m_adapted_q_bits = this->m_base_q_bits;
    if (base_min_caps == CapFlags::EvalFloatFloat)
    {
        // This class implements fixed-point in terms of float. num_q_bits can 
        // be chosen from any valid value (0 to 8 inclusive).
        this->m_adapted_q_bits = 8;
    }
    else
    {
        if (base_min_caps == CapFlags::EvalIntFixed)
        {
            // Float is implemented in terms of fixed-point.
            // Requires base num_q_bits value valid.
            if (this->m_base_q_bits < 0 || this->m_base_q_bits > 8)
            {
                static const char* what = "Invalid base num_q_bits value.";
                throw std::logic_error(what);
            }
        }
        // If this line is reached, base num_q_bits is valid.
    }
}

template <class TBase>
MappingAdapter<TBase>::~MappingAdapter()
{
    // TODO
}

template <class TBase>
MappingCapFlags MappingAdapter<TBase>::caps() const {
    return this->m_adapted_caps;
}

template <class TBase>
int MappingAdapter<TBase>::num_q_bits() const {
    // TODO
    return static_cast<TBase&>(*this).num_q_bits();
}

template <class TBase>
bool MappingAdapter<TBase>::evaluate(float dest_x, float dest_y, float& src_x, float& src_y) const
{
    const auto base_caps = this->m_base_caps;
    const TBase& base = static_cast<TBase&>(*this);
    if (has_all_caps(base_caps, MappingCapFlags::EvalFloatFloat))
    {
        return base.evaluate(dest_x, dest_y, src_x, src_y);
    }
    //
    // __assume(has_all_caps(base_caps, MappingCapFlags::EvalIntFixed))
    // __assume(base_q_bits >= 0 && base_q_bits <= 8)
    // Reason: checked by constructor.
    //
    int src_xq = 0;
    int src_yq = 0;
    if (!base.evaluate_int(
        static_cast<int>(dest_x), static_cast<int>(dest_y), src_xq, src_yq))
    {
        return false;
    }
    int base_q_bits = this->m_base_q_bits;
    src_x = static_cast<float>(src_xq >> base_q_bits);
    src_y = static_cast<float>(src_yq >> base_q_bits);
    return true;
}

template <class TBase>
bool MappingAdapter<TBase>::evaluate_int(int dest_x, int dest_y, int& src_xq, int& src_yq) const
{
    const auto base_caps = this->m_base_caps;
    const TBase& base = static_cast<TBase&>(*this);
    if (has_all_caps(base_caps, MappingCapFlags::EvalIntFixed))
    {
        return base.evaluate_int(dest_x, dest_y, src_xq, src_yq);
    }
    //
    // __assume(has_all_caps(base_caps, MappingCapFlags::EvalFloatFloat))
    // Reason: checked by constructor.
    //
    float src_x = 0.0f;
    float src_y = 0.0f;
    if (!base.evaluate(static_cast<float>(dest_x), static_cast<float>(dest_y), src_x, src_y))
    {
        return false;
    }
    int base_q_bits = this->m_base_q_bits;
    src_xq = static_cast<int>(src_x * (1 << base_q_bits));
    src_yq = static_cast<int>(src_y * (1 << base_q_bits));
    return true;
}

template <class TBase>
bool MappingAdapter<TBase>::evaluate_arr(size_t pair_count, const float* arr_dest_xy, 
    float* arr_src_xy) const
{
    if (!pair_count && !arr_dest_xy && !arr_src_xy)
    {
        // Special case: a validly empty request returns true to indicate
        // supported capability.
        // To be validly empty, all pointers must be null and count must be zero.
        return true;
    }
    if (pair_count && (!arr_dest_xy || !arr_src_xy))
    {
        // Misuses involving null pointers always throw, never just returning 
        // a bool.
        const char* what = (!arr_dest_xy) ? "arr_dest_xy" : "arr_src_xy";
        throw std::invalid_argument(what);
    }
    //
    // TODO: check pair_count for size_t overflow, or unreasonably large values.
    //       These should always throw, as they can be the result of memory
    //       corruptions and/or deliberate misuse.
    //
    for (size_t pair_idx = 0u; pair_idx < pair_count; ++pair_idx)
    {
        float dest_x = arr_dest_xy[pair_idx * 2u];
        float dest_y = arr_dest_xy[pair_idx * 2u + 1u];
        float src_x = 0.0f;
        float src_y = 0.0f;
        if (!this->evaluate(dest_x, dest_y, src_x, src_y))
        {
            return false;
        }
        arr_src_xy[pair_idx * 2u] = src_x;
        arr_src_xy[pair_idx * 2u + 1u] = src_y;
    }
    return true;
}

template <class TBase>
bool MappingAdapter<TBase>::evaluate_arr_int(size_t pair_count, const int* arr_dest_xy, 
    int* arr_src_xyq) const
{
    if (!pair_count && !arr_dest_xy && !arr_src_xyq)
    {
        // Special case: a validly empty request returns true to indicate
        // supported capability.
        // To be validly empty, all pointers must be null and count must be zero.
        return true;
    }
    if (pair_count && (!arr_dest_xy || !arr_src_xyq))
    {
        // Misuses involving null pointers always throw, never just returning 
        // a bool.
        const char* what = (!arr_dest_xy) ? "arr_dest_xy" : "arr_src_xyq";
        throw std::invalid_argument(what);
    }
    //
    // TODO: check pair_count for size_t overflow, or unreasonably large values.
    //       These should always throw, as they can be the result of memory
    //       corruptions and/or deliberate misuse.
    //
    for (size_t pair_idx = 0u; pair_idx < pair_count; ++pair_idx)
    {
        int dest_x = arr_dest_xy[pair_idx * 2u];
        int dest_y = arr_dest_xy[pair_idx * 2u + 1u];
        int src_xq = 0;
        int src_yq = 0;
        if (!this->evaluate_int(dest_x, dest_y, src_xq, src_yq))
        {
            return false;
        }
        arr_src_xyq[pair_idx * 2u] = src_xq;
        arr_src_xyq[pair_idx * 2u + 1u] = src_yq;
    }
    return true;
}

template <class TBase>
bool MappingAdapter<TBase>::evaluate_rect(const cv::Rect& dest_rect, cv::Mat2f& src_xyf) const
{
    if (dest_rect.width == 0 && dest_rect.height == 0 && src_xyf.empty())
    {
        // Special case: a validly empty request returns true to indicate
        // supported capability.
        // To be validly empty, the rectangle coordinates must be exactly zero.
        return true;
    }
    if (dest_rect.empty() || src_xyf.empty() || src_xyf.dims != 2)
    {
        return false;
    }
    const cv::Size sz = dest_rect.size();
    if (sz != src_xyf.size())
    {
        return false;
    }
    const cv::Point tl = dest_rect.tl();
    for (int row = 0; row < sz.height; ++row)
    {
        for (int col = 0; col < sz.width; ++col)
        {
            float dest_x = static_cast<float>(tl.x + col);
            float dest_y = static_cast<float>(tl.y + row);
            float src_x = 0.0f;
            float src_y = 0.0f;
            if (!evaluate(dest_x, dest_y, src_x, src_y))
            {
                return false;
            }
            src_xyf(row, col) = cv::Vec2f(src_x, src_y);
        }
    }
}

template <class TBase>
bool MappingAdapter<TBase>::evaluate_rect_int(const cv::Rect& dest_rect, cv::Mat2i& src_xyq) const
{
    if (dest_rect.width == 0 && dest_rect.height == 0 && src_xyq.empty())
    {
        // Special case: a validly empty request returns true to indicate
        // supported capability.
        // To be validly empty, the rectangle coordinates must be exactly zero.
        return true;
    }
    if (dest_rect.empty() || src_xyq.empty() || src_xyq.dims != 2)
    {
        return false;
    }
    const cv::Size sz = dest_rect.size();
    if (sz != src_xyq.size())
    {
        return false;
    }
    const cv::Point tl = dest_rect.tl();
    for (int row = 0; row < sz.height; ++row)
    {
        for (int col = 0; col < sz.width; ++col)
        {
            int dest_xi = tl.x + col;
            int dest_yi = tl.y + row;
            int src_xq = 0;
            int src_yq = 0;
            if (!evaluate_int(dest_xi, dest_yi, src_xq, src_yq))
            {
                return false;
            }
            src_xyq(row, col) = cv::Vec2i(src_xq, src_yq);
        }
    }
}

template <class TBase>
bool MappingAdapter<TBase>::evaluate_mat(const cv::Mat2f& dest_xyf, cv::Mat2f& src_xyf) const
{
    if (dest_xyf.empty() && src_xyf.empty())
    {
        // Special case: a validly empty request returns true to indicate
        // supported capability.
        return true;
    }
    if (dest_xyf.empty() || dest_xyf.rows != 2 ||
        src_xyf.empty() || src_xyf.rows != 2)
    {
        return false;
    }
    const cv::Size sz = dest_xyf.size();
    if (sz != src_xyf.size())
    {
        return false;
    }
    int safe_width = std::max(sz.width, 0);
    const size_t col_pixel_count = static_cast<size_t>(safe_width);
    for (int row = 0; row < sz.height; ++row)
    {
        const float* dest_xyf_row = dest_xyf.ptr<float>(row);
        float* src_xyf_row = src_xyf.ptr<float>(row);
        if (!this->evaluate_arr(col_pixel_count, dest_xyf_row, src_xyf_row))
        {
            return false;
        }   
    }
    return true;
}

template <class TBase>
bool MappingAdapter<TBase>::evaluate_mat_int(const cv::Mat2i& dest_xyi, cv::Mat2i& src_xyq) const
{
    if (dest_xyi.empty() && src_xyq.empty())
    {
        // Special case: a validly empty request returns true to indicate
        // supported capability.
        return true;
    }
    if (dest_xyi.empty() || dest_xyi.rows != 2 ||
        src_xyq.empty() || src_xyq.rows != 2)
    {
        return false;
    }
    const cv::Size sz = dest_xyi.size();
    if (sz != src_xyq.size())
    {
        return false;
    }
    int safe_width = std::max(sz.width, 0);
    const size_t col_pixel_count = static_cast<size_t>(safe_width);
    for (int row = 0; row < sz.height; ++row)
    {
        const int* dest_xyi_row = dest_xyi.ptr<int>(row);
        int* src_xyq_row = src_xyq.ptr<int>(row);
        if (!this->evaluate_arr_int(col_pixel_count, dest_xyi_row, src_xyq_row))
        {
            return false;
        }
    }
    return true;
}

} // namespace(opencv_remap::draft_1)