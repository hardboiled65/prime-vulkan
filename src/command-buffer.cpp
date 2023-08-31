#include <prime-vulkan/command-buffer.h>

#include <prime-vulkan/base.h>
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


CommandBuffer::BeginInfo::BeginInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    this->_info.pInheritanceInfo = nullptr;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

auto CommandBuffer::BeginInfo::c_struct() const -> CType
{
    return this->_info;
}


CommandBuffer::CommandBuffer()
{
    this->_command_buffer = nullptr;
}

void CommandBuffer::begin(const CommandBuffer::BeginInfo& info)
{
    ::VkResult result;

    BeginInfo::CType vk_info = info.c_struct();
    result = vkBeginCommandBuffer(this->_command_buffer, &vk_info);
    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }
}

void CommandBuffer::reset(::VkCommandBufferResetFlags flags)
{
    ::VkResult result;

    result = vkResetCommandBuffer(this->_command_buffer, flags);
    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }
}

auto CommandBuffer::c_ptr() const -> CType
{
    return this->_command_buffer;
}

} // namespace vk
} // namespace pr
