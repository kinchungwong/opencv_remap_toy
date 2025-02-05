// File: src/v_draft_2/mapping/mapping_null_base.h
//
#ifndef OPENCV_REMAP_DRAFT_2_MAPPING_MAPPING_NULL_BASE_H
#define OPENCV_REMAP_DRAFT_2_MAPPING_MAPPING_NULL_BASE_H

#include "v_draft_2/mapping/mapping.h"

namespace opencv_remap::draft_2
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

    /**
     * TODO: Implement the rest of the pure virtual functions
     * with no-op and returning false.
     */
};

} // namespace(opencv_remap::draft_2)

#endif // OPENCV_REMAP_DRAFT_2_MAPPING_MAPPING_NULL_BASE_H
