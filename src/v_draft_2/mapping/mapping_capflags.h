#ifndef OPENCV_REMAP_DRAFT_2_MAPPING_MAPPING_CAPFLAGS_H
#define OPENCV_REMAP_DRAFT_2_MAPPING_MAPPING_CAPFLAGS_H

#include <cstdint>
#include <limits>
#include <type_traits>

namespace opencv_remap::draft_2
{

enum class MappingCapFlags : uint64_t
{
    None = 0u,
    Reserved_0 = 1u,
    /**
     * @todo Add flags as needed.
     * @todo Update \p Reserved_Mask to reflect actual bits used.
     */
    Reserved_Mask = std::numeric_limits<uint64_t>::max() & (~Reserved_0)
};

inline constexpr MappingCapFlags operator|(MappingCapFlags lhs, MappingCapFlags rhs)
{
    using enumtype = MappingCapFlags;
    using bittype = std::underlying_type_t<MappingCapFlags>;
    constexpr const auto mask = static_cast<bittype>(enumtype::Reserved_Mask);
    return static_cast<enumtype>(mask & (static_cast<bittype>(lhs) | static_cast<bittype>(rhs)));
}

inline constexpr MappingCapFlags operator&(MappingCapFlags lhs, MappingCapFlags rhs)
{
    using enumtype = MappingCapFlags;
    using bittype = std::underlying_type_t<MappingCapFlags>;
    constexpr const auto mask = static_cast<bittype>(enumtype::Reserved_Mask);
    return static_cast<enumtype>(mask & (static_cast<bittype>(lhs) & static_cast<bittype>(rhs)));
}

inline constexpr MappingCapFlags operator~(MappingCapFlags rhs)
{
    using enumtype = MappingCapFlags;
    using bittype = std::underlying_type_t<MappingCapFlags>;
    constexpr const auto mask = static_cast<bittype>(enumtype::Reserved_Mask);
    return static_cast<enumtype>(mask & ~static_cast<bittype>(rhs));
}

} // namespace(opencv_remap::draft_2)

#endif // OPENCV_REMAP_DRAFT_2_MAPPING_MAPPING_CAPFLAGS_H
