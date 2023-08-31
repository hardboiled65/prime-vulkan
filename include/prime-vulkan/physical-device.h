#ifndef _PRIME_VULKAN_PHYSICAL_DEVICE_H
#define _PRIME_VULKAN_PHYSICAL_DEVICE_H

#include <vulkan/vulkan.h>

#include <primer/vector.h>

#include <prime-vulkan/device.h>

namespace pr {
namespace vk {

class VkInstance;

class VkPhysicalDevice;

class VkQueueFamilyProperties
{
    friend VkPhysicalDevice;
public:
    ::VkFlags queue_flags() const;

    uint32_t queue_count() const;

private:
    VkQueueFamilyProperties();

private:
    ::VkQueueFamilyProperties _properties;
};

class VkPhysicalDevice
{
public:
    VkPhysicalDevice(const VkPhysicalDevice& other);

    static Vector<VkPhysicalDevice> enumerate(const VkInstance& instance);

    /// Get the list of queue family properties.
    /// Using `vkGetPhysicalDeviceQueueFamilyProperties` function.
    Vector<VkQueueFamilyProperties> queue_family_properties() const;

    VkDevice create_device(const VkDevice::CreateInfo& create_info) const;

    // vkGetPhysicalDeviceSurfaceCapabilitiesKHR
    // SurfaceCapabilities surface_capabilities() const;

    ::VkPhysicalDevice c_ptr();

private:
    VkPhysicalDevice();

private:
    ::VkPhysicalDevice _device;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_PHYSICAL_DEVICE_H
