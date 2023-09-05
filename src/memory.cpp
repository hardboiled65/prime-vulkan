#include <prime-vulkan/memory.h>

namespace pr {
namespace vk {

MemoryRequirements::MemoryRequirements()
{
}

::VkDeviceSize MemoryRequirements::size() const
{
    return this->_requirements.size;
}

::VkDeviceSize MemoryRequirements::alignment() const
{
    return this->_requirements.alignment;
}

uint32_t MemoryRequirements::memory_type_bits() const
{
    return this->_requirements.memoryTypeBits;
}

auto MemoryRequirements::c_struct() const -> CType
{
    return this->_requirements;
}


MemoryAllocateInfo::MemoryAllocateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    this->_info.pNext = nullptr;
}

void MemoryAllocateInfo::set_allocation_size(::VkDeviceSize size)
{
    this->_info.allocationSize = size;
}

void MemoryAllocateInfo::set_memory_type_index(uint32_t index)
{
    this->_info.memoryTypeIndex = index;
}

auto MemoryAllocateInfo::c_struct() const -> CType
{
    return this->_info;
}


DeviceMemory::DeviceMemory()
{
    this->_memory = nullptr;
}

auto DeviceMemory::c_ptr() const -> CType
{
    return *(this->_memory);
}

} // namespace vk
} // namespace pr
