// File: src/v_draft_2/mapping/mapping.h
//
#ifndef OPENCV_REMAP_DRAFT_2_MAPPING_MAPPING_H
#define OPENCV_REMAP_DRAFT_2_MAPPING_MAPPING_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <opencv2/core.hpp>
#include "v_draft_2/mapping/mapping_capflags.h"

namespace opencv_remap::draft_2
{

class Mapping;
using MappingPtr = std::shared_ptr<Mapping>;

/**
 * @brief Pure abstract base for mathematical mappings between two 2D
 *        spaces, for use in opencv-remap.
 * 
 * @details This class provides a common interface for mapping from
 *          destination coordinates to source coordinates. This
 *          convention is chosen because the primary use case for
 *          mappings is to produce a remapped image, where each pixel
 *          (in the remapped image, i.e. destination space) is
 *          computed by looking up the corresponding pixel in the
 *          original image (i.e. source space).
 * 
 * @remarks For clarity, the shorthand "dest" refers to destination
 *          space, which are the query inputs to the member functions
 *          on this mapping; the shorthand "src" refers to source
 *          space, which are the query outputs from the member functions.
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
     * @remark If the implementation supports fixed-point source coordinates,
     *         all relevant member functions such as \p eval_iq(...) ,
     *         \p eval_q(...) , \p eval_arr_iq(...), and \p eval_grid_q(...) 
     *         must use the same fixed-point format.
     */
    virtual int num_q_bits() const = 0;

    virtual bool eval_f(float dest_x, float dest_y, float& src_x, float& src_y) const = 0;

    virtual bool eval_iq(int dest_x, int dest_y, int& src_xq, int& src_yq) const = 0;

    virtual bool eval_q(int dest_xq, int dest_yq, int& src_xq, int& src_yq) const = 0;

    /**
     * @brief C-style array floating point evaluation with interleaved array.
     * 
     * @remarks
     *      Each (x, y) pair of destination coordinates are to be be evaluated
     *      independently. Implementations must not make the assumption that
     *      the coordinates might satisfy certain mathematical relations.
     */
    virtual bool eval_arr_f(size_t pair_count, const float* arr_dest_xy, 
        float* arr_src_xy) const = 0;

    /**
     * @brief C-style array integer and fixed-point evaluation with interleaved array.
     */
    virtual bool eval_arr_iq(size_t pair_count, const int* arr_dest_xy, 
        int* arr_src_xyq) const = 0;

    /**
     * @brief
     *      Evaluates for a rectangular region in destination space.
     * 
     * @param dest_left, @param dest_top, @param dest_width, @param dest_height
     *      The rectangular region in destination space to evaluate.
     *      No restrictions are placed on the values of \p dest_left and
     *      \p dest_top. Meanwhile, \p dest_width and \p dest_height
     *      must be positive. (See remarks for a special case.)
     * 
     * @param arrcount_f
     *      Number of float elements that can be safely written to the memory 
     *      buffer specified by \p arr_src_xyf. This value must be at least equal 
     *      to the product of ( 2 * \p dest_width * \p dest_height )
     * 
     * @param arr_src_xyf
     *      A pointer to a memory buffer where the source coordinates
     *      will be written. The number of float elements that can be safely
     *      written with is to be specified via \p arrcount_f.
     * 
     * @remarks
     *      The memory layout of the \p arr_src_xyf buffer is as follows:
     * ~~~~
     *      [
     *          src_x(dest_left, dest_top), src_y(dest_left, dest_top),
     *          src_x(dest_left+1, dest_top), src_y(dest_left+1, dest_top),
     *          ...
     *      ]
     * ~~~~
     * 
     * @remarks
     *      A special case of invocation is when all parameters, including 
     *      the array pointer, are zero-valued. This is a query for the basic
     *      implementation support of this function. Upon seeing this
     *      special invocation, the function should return the bool,
     *      indicating whether the implementation supports this function.
     *      No other side effects are allowed.
     */
    virtual bool eval_grid_f(
        int dest_left, int dest_top, int dest_width, int dest_height,
        ssize_t arrcount_f, float* arr_src_xyf) const = 0;
    
    virtual bool eval_grid_q(
        int dest_left, int dest_top, int dest_width, int dest_height,
        ssize_t arrcount_q, int* arr_src_xyq) const = 0;
};

} // namespace(opencv_remap::draft_2)

#endif // OPENCV_REMAP_TOY_DRAFT_2_MAPPING_MAPPING_H
