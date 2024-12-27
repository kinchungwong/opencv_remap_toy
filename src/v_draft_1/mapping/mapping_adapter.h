// File: src/v_draft_1/mapping/mapping_adapter.h
//
#ifndef OPENCV_REMAP_DRAFT_1_MAPPING_MAPPING_ADAPTER_H
#define OPENCV_REMAP_DRAFT_1_MAPPING_MAPPING_ADAPTER_H

#include <type_traits>
#include <stdexcept>
#include "v_draft_1/mapping/mapping.h"

namespace opencv_remap::draft_1
{

/**
 * @brief Automatic adapter for user-defined mappings, where all missing
 *        capabilities will be fulfilled in a straightforward way.
 */
template <class TBase>
class MappingAdapter
    : public Mapping
{
    static_assert(
        std::is_base_of_v<Mapping, TBase>,
        "Expects Base to implement Mapping.");

public:
    using Base = TBase;
    using CapFlags = MappingCapFlags;
    MappingAdapter();
    ~MappingAdapter();

public:
    /**
     * @brief Capability flags of the adapter class, after fulfilling as many
     *        of the member methods as possible.
     */
    MappingCapFlags caps() const override;

    /**
     * @return Number of fractional bits used for fixed-point source coordinates.
     * 
     * @remark Provided that the base class is valid, this adapter always returns
     *         a valid value (from 0 to 8, inclusive) from this function.
     */
    int num_q_bits() const override;

    /**
     * @remark Base class must implement at least one of evaluate() or evaluate_int().
     *         Note that evaluate_int() requires num_q_bits() to be implemented.
     * 
     * @remark If only evaluate_int() is implemented by the base class, this function
     *         will round the destination coordinates to the nearest integer, then 
     *         evaluate with the base class, and convert the resulting fixed-point
     *         source coordinates back to floating-point.
     *         In this case, the rounding of the destination coordinates could make
     *         the result poorly usable for some applications.
     */
    bool evaluate(float dest_x, float dest_y, float& src_x, float& src_y) const override;

    /**
     * @remark Base class must implement at least one of evaluate() or evaluate_int().
     */
    bool evaluate_int(int dest_x, int dest_y, int& src_xq, int& src_yq) const override;

    /**
     * @brief Provides a default implementation by looping.
     */
    bool evaluate_arr(size_t pair_count, const float* arr_dest_xy, 
        float* arr_src_xy) const override;

    /**
     * @brief Provides a default implementation by looping.
     */
    bool evaluate_arr_int(size_t pair_count, const int* arr_dest_xy, 
        int* arr_src_xyq) const override;

    /**
     * @brief Provides a default implementation by looping.
     */
    bool evaluate_rect(const cv::Rect& dest_rect, cv::Mat2f& src_xyf) const override;

    /**
     * @brief Provides a default implementation by looping.
     */
    bool evaluate_rect_int(const cv::Rect& dest_rect, cv::Mat2i& src_xyq) const override;

    /**
     * @brief Provides a default implementation by looping.
     */
    bool evaluate_mat(const cv::Mat2f& dest_xyf, cv::Mat2f& src_xyf) const override;

    /**
     * @brief Provides a default implementation by looping.
     */
    bool evaluate_mat_int(const cv::Mat2i& dest_xyi, cv::Mat2i& src_xyq) const override;

protected:
    CapFlags m_base_caps = CapFlags::None;
    CapFlags m_adapted_caps = CapFlags::None;
    int m_base_q_bits = -1;
    int m_adapted_q_bits = -1;

public:
    static constexpr const CapFlags stc_min_caps = (
        CapFlags::EvalFloatFloat | 
        CapFlags::EvalIntFixed
    );
    static constexpr const CapFlags stc_fulfilled_caps = (
        CapFlags::EvalFloatFloat | 
        CapFlags::EvalIntFixed |
        CapFlags::EvalArrFloatFloat |
        CapFlags::EvalArrIntFixed |
        CapFlags::EvalRectFloat |
        CapFlags::EvalRectFixed |
        CapFlags::EvalMatFloatFloat |
        CapFlags::EvalMatIntFixed
    );

private:
    inline static bool has_all_caps(CapFlags actual, CapFlags expected)
    {
        return (actual & expected) == expected;
    }
};

} // namespace(opencv_remap::draft_1)

#endif // OPENCV_REMAP_DRAFT_1_MAPPING_MAPPING_ADAPTER_H
