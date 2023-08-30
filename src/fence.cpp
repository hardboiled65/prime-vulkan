#include <prime-vulkan/fence.h>

namespace pr {
namespace vk {

Fence::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

auto Fence::CreateInfo::c_struct() const -> CType
{
    return this->_info;
}


Fence::Fence()
{
    this->_fence = nullptr;
}

auto Fence::c_ptr() const -> CType
{
    return *(this->_fence);
}

} // namespace vk
} // namespace pr
