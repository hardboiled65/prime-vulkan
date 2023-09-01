#include <prime-vulkan/base.h>

#include <primer/format.h>

namespace pr {
namespace vk {

pr::String Vulkan::vk_format_to_string(::VkFormat format)
{
    switch (format) {
    case VK_FORMAT_R5G6B5_UNORM_PACK16:
        return "VK_FORMAT_R5G6B5_UNORM_PACK16"_S;
    case VK_FORMAT_R8G8B8A8_UNORM:
        return "VK_FORMAT_R8G8B8A8_UNORM"_S;
    case VK_FORMAT_R8G8B8A8_SRGB:
        return "VK_FORMAT_R8G8B8A8_SRGB"_S;
    case VK_FORMAT_B8G8R8A8_UNORM:
        return "VK_FORMAT_B8G8R8A8_UNORM"_S;
    case VK_FORMAT_B8G8R8A8_SRGB:
        return "VK_FORMAT_B8G8R8A8_SRGB"_S;
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
        return "VK_FORMAT_A2R10G10B10_UNORM_PACK32"_S;
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
        return "VK_FORMAT_A2B10G10R10_UNORM_PACK32"_S;
    default:
        return "Unknown"_S;
    }
}

pr::String Vulkan::vk_present_mode_to_string(::VkPresentModeKHR mode)
{
    switch (mode) {
    case VK_PRESENT_MODE_IMMEDIATE_KHR:
        return "VK_PRESENT_MODE_IMMEDIATE_KHR"_S;
    case VK_PRESENT_MODE_MAILBOX_KHR:
        return "VK_PRESENT_MODE_MAILBOX_KHR"_S;
    case VK_PRESENT_MODE_FIFO_KHR:
        return "VK_PRESENT_MODE_FIFO_KHR"_S;
    default:
        return "Unknown"_S;
    }
    // TODO: More enum values.
}


VulkanError::VulkanError(::VkResult result)
{
    this->_vk_result = result;
}

const char* VulkanError::what() const noexcept
{
    pr::String msg = pr::format("VulkanError. Error code: {}",
        static_cast<int>(this->_vk_result));
    return msg.c_str();
}

::VkResult VulkanError::vk_result() const noexcept
{
    return this->_vk_result;
}

} // namespace vk
} // namespace pr
