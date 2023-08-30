#include <prime-vulkan/command-buffer.h>

#include <prime-vulkan/command-pool.h>

namespace pr {
namespace vk {

CommandBuffer::AllocateInfo::AllocateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    this->_info.pNext = nullptr;
}

void CommandBuffer::AllocateInfo::set_command_pool(
    const CommandPool& command_pool)
{
    this->_info.commandPool = command_pool.c_ptr();
}

void CommandBuffer::AllocateInfo::set_level(::VkCommandBufferLevel level)
{
    this->_info.level = level;
}

void CommandBuffer::AllocateInfo::set_command_buffer_count(uint32_t count)
{
    this->_info.commandBufferCount = count;
}

auto CommandBuffer::AllocateInfo::c_struct() const -> CType
{
    return this->_info;
}

} // namespace vk
} // namespace pr
