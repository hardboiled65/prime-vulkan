#include <prime-vulkan/command-pool.h>

namespace pr {
namespace vk {

CommandPool::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void CommandPool::CreateInfo::set_queue_family_index(uint32_t index)
{
    this->_info.queueFamilyIndex = index;
}

void CommandPool::CreateInfo::set_flags(::VkCommandPoolCreateFlags flags)
{
    this->_info.flags = flags;
}

auto CommandPool::CreateInfo::c_struct() const -> CType
{
    return this->_info;
}


CommandPool::CommandPool()
{
    this->_command_pool = nullptr;
}

auto CommandPool::c_ptr() const -> CType
{
    return *(this->_command_pool);
}

} // namespace vk
} // namespace pr
