#include <prime-vulkan/semaphore.h>

namespace pr {
namespace vk {

Semaphore::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

auto Semaphore::CreateInfo::c_struct() const -> CType
{
    return this->_info;
}


Semaphore::Semaphore()
{
    this->_semaphore = nullptr;
}

auto Semaphore::c_ptr() const -> CType
{
    return *(this->_semaphore);
}

} // namespace vk
} // namespace pr
