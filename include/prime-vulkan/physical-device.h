#ifndef _PRIME_VULKAN_PHYSICAL_DEVICE_H
#define _PRIME_VULKAN_PHYSICAL_DEVICE_H

#include <vulkan/vulkan.h>

#include <primer/vector.h>

#include <prime-vulkan/device.h>
#include <prime-vulkan/surface.h>

namespace pr {
namespace vk {

class Instance;

class PhysicalDevice;

class QueueFamilyProperties
{
    friend PhysicalDevice;
public:
    ::VkFlags queue_flags() const;

    uint32_t queue_count() const;

private:
    QueueFamilyProperties();

private:
    ::VkQueueFamilyProperties _properties;
};

class PhysicalDevice
{
public:
    class MemoryProperties
    {
        friend PhysicalDevice;
    public:
        using CType = ::VkPhysicalDeviceMemoryProperties;

    public:
        pr::Vector<::VkMemoryHeap> memory_heaps() const;

        pr::Vector<::VkMemoryType> memory_types() const;

        CType c_struct() const;

    private:
        MemoryProperties();

    private:
        CType _properties;
    };

public:
    PhysicalDevice(const PhysicalDevice& other);

    static Vector<PhysicalDevice> enumerate(const Instance& instance);

    /// Get the list of queue family properties.
    /// Using `vkGetPhysicalDeviceQueueFamilyProperties` function.
    Vector<QueueFamilyProperties> queue_family_properties() const;

    /// Using `vk_` function.
    MemoryProperties memory_properties() const;

    Device create_device(const Device::CreateInfo& create_info) const;

    /// Using `vkGetPhysicalDeviceSurfaceCapabilitiesKHR` function.
    Surface::Capabilities surface_capabilities_for(
        const Surface& surface) const;

    /// Using `vkGetPhysicalDeviceSurfaceFormatsKHR` function.
    Vector<SurfaceFormat> surface_formats_for(
        const Surface& surface) const;

    /// Using `vkGetPhysicalDeviceSurfacePresentModesKHR` function.
    Vector<::VkPresentModeKHR> present_modes_for(
        const Surface& surface) const;

    /// Using `vkGetPhysicalDeviceSurfaceSupportKHR` function.
    bool surface_support_for(uint32_t queue_family_index,
                             const Surface& surface) const;

    ::VkPhysicalDevice c_ptr();

private:
    PhysicalDevice();

private:
    ::VkPhysicalDevice _device;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_PHYSICAL_DEVICE_H
