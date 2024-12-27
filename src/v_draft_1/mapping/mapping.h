// File: src/v_draft_1/mapping/mapping.h
//
#ifndef OPENCV_REMAP_DRAFT_1_MAPPING_MAPPING_H
#define OPENCV_REMAP_DRAFT_1_MAPPING_MAPPING_H

#include <cstddef>
#include <cstdint>
#include <opencv2/core.hpp>
#include "v_draft_1/mapping/mapping_capflags.h"

namespace opencv_remap::draft_1
{

/**
 * @brief Pure abstract base for mathematical mappings between two 2D
 *        spaces, for use in opencv-remap.
 */
class Mapping
{
protected:
    Mapping() = default;
    virtual ~Mapping() = 0;

public:
    /**
     * @brief Gets the capabilities of this mapping implementation.
     * 
     * @remark Adapters and decorators are allowed to enhance the
     *         capabilities of underlying (wrapped) implementations.
     */
    virtual MappingCapFlags caps() const = 0;

    /**
     * @brief Gets the fixed-point format for source coordinates computed
     *        by this implementation.
     * 
     * @return The number of fractional bits in the fixed-point format.
     *         Valid range is 0 to 8 inclusive.
     *         Implementations are allowed to return a value outside the
     *         valid range, to indicate that it does not implement any
     *         support for fixed-point source coordinates.
     * 
     * @remark Fixed-point format is not applicable to destination coordinates.
     * 
     * @remark If the implementation supports fixed-point source coordinates,
     *         all relevant member functions such as evaluate_int(...),
     *         evaluate_arr_int(...), evaluate_rect_int(...), and 
     *         evaluate_mat_int(...) must use the same fixed-point format.
     */
    virtual int num_q_bits() const = 0;

    virtual bool evaluate(float dest_x, float dest_y, float& src_x, float& src_y) const = 0;

    virtual bool evaluate_int(int dest_x, int dest_y, int& src_xq, int& src_yq) const = 0;

    /**
     * @brief C-style array floating point evaluation with interleaved array.
     */
    virtual bool evaluate_arr(size_t pair_count, const float* arr_dest_xy, 
        float* arr_src_xy) const = 0;

    /**
     * @brief C-style array integer and fixed-point evaluation with interleaved array.
     */
    virtual bool evaluate_arr_int(size_t pair_count, const int* arr_dest_xy, 
        int* arr_src_xyq) const = 0;

    virtual bool evaluate_rect(const cv::Rect& dest_rect, cv::Mat2f& src_xyf) const = 0;
    
    virtual bool evaluate_rect_int(const cv::Rect& dest_rect, cv::Mat2i& src_xyq) const = 0;

    virtual bool evaluate_mat(const cv::Mat2f& dest_xyf, cv::Mat2f& src_xyf) const = 0;

    virtual bool evaluate_mat_int(const cv::Mat2i& dest_xyi, cv::Mat2i& src_xyq) const = 0;
};

} // namespace(opencv_remap::draft_1)

#endif // OPENCV_REMAP_TOY_DRAFT_1_MAPPING_MAPPING_H
