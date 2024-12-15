#pragma once

#include <opencv2/core/core.hpp>

namespace naive_desttile_demo
{

class DestTileGeneratorBase
{
private:
    DestTileGeneratorBase(const DestTileGeneratorBase&) = delete;
    DestTileGeneratorBase(DestTileGeneratorBase&&) = delete;
    DestTileGeneratorBase& operator=(const DestTileGeneratorBase&) = delete;
    DestTileGeneratorBase& operator=(DestTileGeneratorBase&&) = delete;

protected:
    DestTileGeneratorBase(const cv::Mat1b& matsrc, cv::Mat1b& matdest);
    virtual ~DestTileGeneratorBase() = 0;

    /**
     * A function to be implemented by derived class to convert destination
     * coordinates into source coordinates.
     * 
     * The range of destination coordinates is always rectangular, and is
     * specified with the destrect argument.
     * 
     * Two signed 32-bit integer matrices will be allocated by the caller
     * (from the base class) to be populated by this function.
     * 
     * The caller guarantees that these two output matrices have been
     * allocated with the proper size; that is, the following holds:
     * 
     * ... assert(destrect.size() == srcxq.size()); // precond
     * ... assert(destrect.size() == srcyq.size()); // precond
     * 
     * The source (x, y) coordinates can be populated as either nearest 
     * integers or fixed-point fractional values. The output argument 
     * num_q_bits needs to be populated by this function to indicate
     * the number of fractional bits used.
     * 
     * Function arguments
     *      destrect (input) cv::Rect 
     *          The rectangular range of destination coordinates that
     *          must be populated by this function.
     *      num_q_bits (output, byref) int&
     *          Number of fractional bits used in the fixed-point
     *          representation of source coordinates produced by this
     *          function. 
     *          Valid values are from 0 to 8, inclusive. If the 
     *          generated source coordinates are integer-valued,
     *          set this to zero.
     *      srcxq (output, byref) cv::Mat1i&
     *          Output matrix for the source x-coordinates.
     *      srcyq (output, byref) cv::Mat1i&
     *          Output matrix for the source y-coordinates.
     * 
     * Function return value
     *      (return) bool
     *          True if all output arguments are populated normally.
     *          Otherwise, false.
     *          Typically, when fallback functions are implemented,
     *          returning false from this function will cause the
     *          alternative fallback function to be used.
     * 
     * Remarks
     *      This function must use the caller-allocated output matrices;
     *      do not try to re-allocate, resize, or reshape them.
     */
    virtual bool populate_source_coords(cv::Rect destrect, int& num_q_bits, 
        cv::Mat1i& srcxq, cv::Mat1i& srcyq) const;

    virtual bool clamp_source_coords(cv::Rect destrect, int num_q_bits, 
        cv::Mat1i& srcxq, cv::Mat1i& srcyq) const;

    virtual bool clamp_source_coords_scalar(cv::Rect destrect, int num_q_bits, 
        cv::Mat1i& srcxq, cv::Mat1i& srcyq) const;

    virtual bool populate_dest_bilinear(cv::Rect destrect, int num_q_bits, 
        const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const;

    virtual bool populate_dest_bilinear_q(cv::Rect destrect, int num_q_bits, 
        const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const;

    virtual bool populate_dest_bilinear_qzero(cv::Rect destrect, int num_q_bits, 
        const cv::Mat1i& srcxq, const cv::Mat1i& srcyq) const;

public:
    bool operator() (cv::Rect destrect) const;

private:
    void validate_dest_rect_else_throw(const cv::Rect& destrect) const;

protected:
    std::unique_ptr<const cv::Mat1b> m_matsrc;
    std::unique_ptr<cv::Mat1b> m_matdest;
};

} // namespace(naive_desttile_demo)
