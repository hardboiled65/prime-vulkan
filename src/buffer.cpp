#include <prime-vulkan/buffer.h>

namespace pr {
namespace vk {

Buffer::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    this->_info.queueFamilyIndexCount = 0;
    this->_info.pQueueFamilyIndices = nullptr;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void Buffer::CreateInfo::set_size(::VkDeviceSize size)
{
    this->_info.size = size;
}

void Buffer::CreateInfo::set_usage(::VkBufferUsageFlags usage)
{
    this->_info.usage = usage;
}

void Buffer::CreateInfo::set_sharing_mode(::VkSharingMode mode)
{
    this->_info.sharingMode = mode;
}

auto Buffer::CreateInfo::c_struct() const -> CType
{
    return this->_info;
}


Buffer::Buffer()
{
    this->_buffer = nullptr;
}

auto Buffer::c_ptr() const -> CType
{
    return *(this->_buffer);
}

} // namespace vk
} // namespace pr
