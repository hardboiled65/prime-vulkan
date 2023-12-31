#ifndef _PRIME_VULKAN_BASE_H
#define _PRIME_VULKAN_BASE_H

#include <exception>

#include <vulkan/vulkan.h>

#include <primer/string.h>

namespace pr {
namespace vk {

class Vulkan
{
public:
    static pr::String vk_format_to_string(::VkFormat format);

    static pr::String vk_present_mode_to_string(::VkPresentModeKHR mode);

    static pr::String queue_flags_to_string(VkFlags flags);
};

class VulkanError : public std::exception
{
public:
    VulkanError(::VkResult vk_result);

    const char* what() const noexcept;

    ::VkResult vk_result() const noexcept;

private:
    ::VkResult _vk_result;
};

} // namespace vk
} // namespace pr

#endif // _PIME_VULKAN_BASE_H
