#ifndef _PRIME_VULKAN_QUEUE_H
#define _PRIME_VULKAN_QUEUE_H

#include <vulkan/vulkan.h>

namespace pr {
namespace vk {

class VkDevice;

class VkQueue
{
    friend VkDevice;
public:
    ::VkQueue c_ptr() const;

private:
    VkQueue();

private:
    ::VkQueue _queue;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_QUEUE_H
