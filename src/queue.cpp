#include <prime-vulkan/queue.h>

namespace pr {
namespace vk {

VkQueue::VkQueue()
{
    this->_queue = nullptr;
}

::VkQueue VkQueue::c_ptr() const
{
    return this->_queue;
}

} // namespace vk
} // namespace pr
