// File: src/v_draft_1/mapping/mapping_null_base.h
//
#ifndef OPENCV_REMAP_DRAFT_1_MAPPING_MAPPING_NULL_BASE_H
#define OPENCV_REMAP_DRAFT_1_MAPPING_MAPPING_NULL_BASE_H

#include "v_draft_1/mapping/mapping.h"

namespace opencv_remap::draft_1
{

/**
 * @brief Provides a concrete base class for Mapping, with all member
 *        functions refused to implement anything.
 */
class MappingNullBase
    : public Mapping
{
public:
    MappingNullBase() = default;
    ~MappingNullBase() = default;

public:
    MappingCapFlags caps() const override
    {
        return MappingCapFlags::None;
    }

    int num_q_bits() const override
    {
        return -1;
    }

    bool evaluate(float dest_x, float dest_y, float& src_x, float& src_y) const override
    {
        return false;
    }

    bool evaluate_int(int dest_x, int dest_y, int& src_xq, int& src_yq) const override
    {
        return false;
    }

    bool evaluate_arr(size_t pair_count, const float* arr_dest_xy, 
        float* arr_src_xy) const override
    {
        return false;
    }

    bool evaluate_arr_int(size_t pair_count, const int* arr_dest_xy, 
        int* arr_src_xyq) const override
    {
        return false;
    }

    bool evaluate_rect(const cv::Rect& dest_rect, cv::Mat2f& src_xyf) const override
    {
        return false;
    }
    
    bool evaluate_rect_int(const cv::Rect& dest_rect, cv::Mat2i& src_xyq) const override
    {
        return false;
    }

    bool evaluate_mat(const cv::Mat2f& dest_xyf, cv::Mat2f& src_xyf) const override
    {
        return false;
    }

    bool evaluate_mat_int(const cv::Mat2i& dest_xyi, cv::Mat2i& src_xyq) const override
    {
        return false;
    }
};

} // namespace(opencv_remap::draft_1)

#endif // OPENCV_REMAP_DRAFT_1_MAPPING_MAPPING_NULL_BASE_H
