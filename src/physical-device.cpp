#include <prime-vulkan/physical-device.h>

#include <prime-vulkan/base.h>
#include <prime-vulkan/instance.h>

namespace pr {
namespace vk {

VkQueueFamilyProperties::VkQueueFamilyProperties()
{
}

::VkFlags VkQueueFamilyProperties::queue_flags() const
{
    return this->_properties.queueFlags;
}

uint32_t VkQueueFamilyProperties::queue_count() const
{
    return this->_properties.queueCount;
}


VkPhysicalDevice::VkPhysicalDevice()
{
    this->_device = nullptr;
}

VkPhysicalDevice::VkPhysicalDevice(const VkPhysicalDevice& other)
{
    this->_device = other._device;
}

Vector<VkPhysicalDevice> VkPhysicalDevice::enumerate(const VkInstance& instance)
{
    Vector<VkPhysicalDevice> v;

    uint32_t count;
    vkEnumeratePhysicalDevices(
        const_cast<VkInstance&>(instance).c_ptr(),
        &count,
        NULL
    );
    if (count == 0) {
        fprintf(stderr, "No GPUs with Vulkan support!\n");
    }

    ::VkPhysicalDevice *devices = new ::VkPhysicalDevice[count];

    vkEnumeratePhysicalDevices(
        const_cast<VkInstance&>(instance).c_ptr(), &count, devices);

    for (uint32_t i = 0; i < count; ++i) {
        ::VkPhysicalDevice device = devices[i];
        VkPhysicalDevice physical_device;
        physical_device._device = device;
        v.push(physical_device);
    }

    delete[] devices;

    return v;
}

Vector<VkQueueFamilyProperties> VkPhysicalDevice::queue_family_properties() const
{
    Vector<VkQueueFamilyProperties> v;

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        const_cast<VkPhysicalDevice*>(this)->c_ptr(),
        &count, nullptr);

    ::VkQueueFamilyProperties *properties = new ::VkQueueFamilyProperties[count];
    vkGetPhysicalDeviceQueueFamilyProperties(
        const_cast<VkPhysicalDevice*>(this)->c_ptr(),
        &count, properties);

    for (uint32_t i = 0; i < count; ++i) {
        VkQueueFamilyProperties p;
        p._properties = properties[i];
        v.push(p);
    }

    delete[] properties;

    return v;
}

VkDevice VkPhysicalDevice::create_device(
    const VkDevice::CreateInfo& create_info) const
{
    ::VkResult result;
    ::VkDevice device;

    ::VkDeviceCreateInfo info = create_info.c_struct();
    result = vkCreateDevice(this->_device, &info, nullptr, &device);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    // Construct device class.
    VkDevice vk_device;
    vk_device._device = device;

    return vk_device;
}

::VkPhysicalDevice VkPhysicalDevice::c_ptr()
{
    return this->_device;
}

} // namespace vk
} // namespace pr
