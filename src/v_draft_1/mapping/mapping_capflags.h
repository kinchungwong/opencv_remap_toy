#ifndef OPENCV_REMAP_DRAFT_1_MAPPING_MAPPING_CAPFLAGS_H
#define OPENCV_REMAP_DRAFT_1_MAPPING_MAPPING_CAPFLAGS_H

#include <cstdint>

namespace opencv_remap::draft_1
{

enum class MappingCapFlags : uint32_t
{
    None = 0u,
    EvalFloatFloat = (1u << 0),
    EvalIntFixed = (1u << 1),
    EvalArrFloatFloat = (1u << 2),
    EvalArrIntFixed = (1u << 3),
    EvalRectFloat = (1u << 4),
    EvalRectFixed = (1u << 5),
    EvalMatFloatFloat = (1u << 6),
    EvalMatIntFixed = (1u << 7),
};

inline constexpr MappingCapFlags operator|(MappingCapFlags lhs, MappingCapFlags rhs)
{
    return static_cast<MappingCapFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline constexpr MappingCapFlags operator&(MappingCapFlags lhs, MappingCapFlags rhs)
{
    return static_cast<MappingCapFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

} // namespace(opencv_remap::draft_1)

#endif // OPENCV_REMAP_DRAFT_1_MAPPING_MAPPING_CAPFLAGS_H
