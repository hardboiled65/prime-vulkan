#include <prime-vulkan/physical-device.h>

#include <prime-vulkan/base.h>
#include <prime-vulkan/instance.h>

namespace pr {
namespace vk {

QueueFamilyProperties::QueueFamilyProperties()
{
}

::VkFlags QueueFamilyProperties::queue_flags() const
{
    return this->_properties.queueFlags;
}

uint32_t QueueFamilyProperties::queue_count() const
{
    return this->_properties.queueCount;
}


PhysicalDevice::MemoryProperties::MemoryProperties()
{
}

pr::Vector<::VkMemoryHeap>
PhysicalDevice::MemoryProperties::memory_heaps() const
{
    pr::Vector<::VkMemoryHeap> v;

    for (uint32_t i = 0; i < this->_properties.memoryHeapCount; ++i) {
        v.push(this->_properties.memoryHeaps[i]);
    }

    return v;
}

pr::Vector<::VkMemoryType>
PhysicalDevice::MemoryProperties::memory_types() const
{
    pr::Vector<::VkMemoryType> v;

    for (uint32_t i = 0; i < this->_properties.memoryTypeCount; ++i) {
        v.push(this->_properties.memoryTypes[i]);
    }

    return v;
}

auto PhysicalDevice::MemoryProperties::c_struct() const -> CType
{
    return this->_properties;
}


PhysicalDevice::PhysicalDevice()
{
    this->_device = nullptr;
}

PhysicalDevice::PhysicalDevice(const PhysicalDevice& other)
{
    this->_device = other._device;
}

Vector<PhysicalDevice> PhysicalDevice::enumerate(const Instance& instance)
{
    Vector<PhysicalDevice> v;

    uint32_t count;
    vkEnumeratePhysicalDevices(
        const_cast<Instance&>(instance).c_ptr(),
        &count,
        NULL
    );
    if (count == 0) {
        fprintf(stderr, "No GPUs with Vulkan support!\n");
    }

    ::VkPhysicalDevice *devices = new ::VkPhysicalDevice[count];

    vkEnumeratePhysicalDevices(
        const_cast<Instance&>(instance).c_ptr(), &count, devices);

    for (uint32_t i = 0; i < count; ++i) {
        ::VkPhysicalDevice device = devices[i];
        PhysicalDevice physical_device;
        physical_device._device = device;
        v.push(physical_device);
    }

    delete[] devices;

    return v;
}

Vector<QueueFamilyProperties> PhysicalDevice::queue_family_properties() const
{
    Vector<QueueFamilyProperties> v;

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        const_cast<PhysicalDevice*>(this)->c_ptr(),
        &count, nullptr);

    ::VkQueueFamilyProperties *properties = new ::VkQueueFamilyProperties[count];
    vkGetPhysicalDeviceQueueFamilyProperties(
        const_cast<PhysicalDevice*>(this)->c_ptr(),
        &count, properties);

    for (uint32_t i = 0; i < count; ++i) {
        QueueFamilyProperties p;
        p._properties = properties[i];
        v.push(p);
    }

    delete[] properties;

    return v;
}

PhysicalDevice::MemoryProperties PhysicalDevice::memory_properties() const
{
    MemoryProperties::CType vk_props;
    vkGetPhysicalDeviceMemoryProperties(this->_device, &vk_props);

    MemoryProperties props;
    props._properties = vk_props;

    return props;
}

Device PhysicalDevice::create_device(
    const Device::CreateInfo& create_info) const
{
    ::VkResult result;
    ::VkDevice device;

    ::VkDeviceCreateInfo info = create_info.c_struct();
    result = vkCreateDevice(this->_device, &info, nullptr, &device);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    // Construct device class.
    Device vk_device;
    vk_device._device = device;

    return vk_device;
}

Surface::Capabilities PhysicalDevice::surface_capabilities_for(
    const Surface& surface) const
{
    ::VkResult result;

    Surface::Capabilities::CType vk_capabilities;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        this->_device,
        surface.c_ptr(),
        &vk_capabilities);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to get surface capabilities!\n");
        throw VulkanError(result);
    }

    Surface::Capabilities capabilities;
    capabilities._capabilities = vk_capabilities;

    return capabilities;
}

Vector<SurfaceFormat> PhysicalDevice::surface_formats_for(
    const Surface& surface) const
{
    ::VkResult result;
    pr::Vector<SurfaceFormat> v;

    uint32_t formats;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(this->_device,
        surface.c_ptr(), &formats, nullptr);
    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    ::VkSurfaceFormatKHR *vk_formats = new ::VkSurfaceFormatKHR[formats];
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(this->_device,
        surface.c_ptr(), &formats, vk_formats);
    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    for (uint32_t i = 0; i < formats; ++i) {
        SurfaceFormat surface_format;
        surface_format._format = vk_formats[i];
        v.push(surface_format);
    }

    delete[] vk_formats;

    return v;
}

Vector<::VkPresentModeKHR> PhysicalDevice::present_modes_for(
    const Surface& surface) const
{
    ::VkResult result;
    pr::Vector<::VkPresentModeKHR> v;

    uint32_t count;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(this->_device,
        surface.c_ptr(), &count, nullptr);
    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    ::VkPresentModeKHR *vk_modes = new ::VkPresentModeKHR[count];
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(this->_device,
        surface.c_ptr(), &count, vk_modes);
    if (result != VK_SUCCESS) {
        // De-allocate memory.
        delete[] vk_modes;

        throw VulkanError(result);
    }

    for (uint32_t i = 0; i < count; ++i) {
        v.push(vk_modes[i]);
    }

    delete[] vk_modes;

    return v;
}

bool PhysicalDevice::surface_support_for(uint32_t queue_family_index,
                                           const Surface& surface) const
{
    ::VkResult result;
    ::VkBool32 vk_bool;

    result = vkGetPhysicalDeviceSurfaceSupportKHR(this->_device,
        queue_family_index, surface.c_ptr(), &vk_bool);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    return (vk_bool == VK_TRUE) ? true : false;
}

::VkPhysicalDevice PhysicalDevice::c_ptr()
{
    return this->_device;
}

} // namespace vk
} // namespace pr
