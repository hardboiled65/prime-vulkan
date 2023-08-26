#include <prime-vulkan/device.h>

#include <prime-vulkan/base.h>

namespace pr {
namespace vk {

VkDevice::QueueCreateInfo::QueueCreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    this->_info.flags = 0;
    this->_info.pNext = nullptr;
    this->_info.pQueuePriorities = nullptr;
    this->_info.queueCount = 0;
    this->_info.queueFamilyIndex = 0;

    this->_priorities = nullptr;
}

VkDevice::QueueCreateInfo::QueueCreateInfo(const QueueCreateInfo& other)
{
    this->_info = other._info;

    this->_priorities = new float[this->_info.queueCount];
    for (uint32_t i = 0; i < this->_info.queueCount; ++i) {
        this->_priorities[i] = other._priorities[i];
    }

    this->_info.pQueuePriorities = this->_priorities;
}

VkDevice::QueueCreateInfo::~QueueCreateInfo()
{
    if (this->_priorities != nullptr) {
        delete[] this->_priorities;
    }
}

void VkDevice::QueueCreateInfo::set_queue_count(uint32_t count)
{
    this->_info.queueCount = count;
}

void VkDevice::QueueCreateInfo::set_queue_family_index(uint32_t index)
{
    this->_info.queueFamilyIndex = index;
}

void VkDevice::QueueCreateInfo::set_queue_priorities(const Vector<float>& priorities)
{
    if (this->_priorities != nullptr) {
        delete[] this->_priorities;
    }

    this->_priorities = new float[priorities.length()];
    for (uint64_t i = 0; i < priorities.length(); ++i) {
        this->_priorities[i] = priorities[i];
    }

    this->_info.pQueuePriorities = this->_priorities;
}

::VkDeviceQueueCreateInfo VkDevice::QueueCreateInfo::c_struct() const
{
    return this->_info;
}


VkDevice::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    this->_info.queueCreateInfoCount = 0;
    this->_info.pQueueCreateInfos = nullptr;

    this->_info.pEnabledFeatures = nullptr;

    this->_info.enabledExtensionCount = 0;
    this->_info.ppEnabledExtensionNames = nullptr;

    // Zero or null.
    this->_info.enabledLayerCount = 0;
    this->_info.ppEnabledLayerNames = nullptr;
    this->_info.flags = 0;
    this->_info.pNext = nullptr;

    this->_queue_create_infos = nullptr;
    this->_pp_enabled_extension_names = nullptr;
}

VkDevice::CreateInfo::~CreateInfo()
{
    if (this->_queue_create_infos != nullptr) {
        delete[] this->_queue_create_infos;
    }
    if (this->_pp_enabled_extension_names != nullptr) {
        delete[] this->_pp_enabled_extension_names;
    }
}

void VkDevice::CreateInfo::set_queue_create_infos(
    const Vector<QueueCreateInfo>& infos)
{
    this->_info.queueCreateInfoCount = infos.length();

    this->_queue_create_infos = new ::VkDeviceQueueCreateInfo[infos.length()];
    for (uint64_t i = 0; i < infos.length(); ++i) {
        this->_queue_create_infos[i] = infos[i].c_struct();
    }

    this->_info.pQueueCreateInfos = this->_queue_create_infos;
}

void VkDevice::CreateInfo::set_enabled_features(
    ::VkPhysicalDeviceFeatures features)
{
    this->_enabled_features = features;

    this->_info.pEnabledFeatures = &(this->_enabled_features);
}

void VkDevice::CreateInfo::set_enabled_extension_names(
    const Vector<String>& names)
{
    this->_info.enabledExtensionCount = names.length();

    this->_enabled_extension_names = names;
    this->_pp_enabled_extension_names = new const char*[names.length()];
    for (uint64_t i = 0; i < names.length(); ++i) {
        this->_pp_enabled_extension_names[i] = names[i].c_str();
    }

    this->_info.ppEnabledExtensionNames = this->_pp_enabled_extension_names;
}

::VkDeviceCreateInfo VkDevice::CreateInfo::c_struct() const
{
    return this->_info;
}


VkDevice::VkDevice()
{
}

VkDevice::VkDevice(const VkDevice& other)
{
    this->_device = other._device;
}

VkQueue VkDevice::queue_for(uint32_t queue_family_index,
                            uint32_t queue_index) const
{
    ::VkQueue queue;
    vkGetDeviceQueue(this->_device, queue_family_index, queue_index, &queue);

    VkQueue ret;
    ret._queue = queue;

    return ret;
}

VkSwapchain VkDevice::create_swapchain(
    const VkSwapchain::CreateInfo& info) const
{
    ::VkResult result;
    ::VkSwapchainKHR vk_swapchain;

    auto create_info = info.c_struct();
    result = vkCreateSwapchainKHR(this->_device, &create_info,
        nullptr, &vk_swapchain);

    if (result != VK_SUCCESS) {
        // TODO: Throw.
    }
    VkSwapchain swapchain;
    swapchain._swapchain = vk_swapchain;

    return swapchain;
}

Vector<VkImage> VkDevice::images_for(const VkSwapchain& swapchain) const
{
    Vector<VkImage> v;

    ::VkResult result;

    uint32_t count;
    ::VkImage *vk_images;

    result = vkGetSwapchainImagesKHR(this->_device,
        swapchain.c_ptr(), &count,
        nullptr);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    vk_images = new ::VkImage[count];

    result = vkGetSwapchainImagesKHR(this->_device,
        swapchain.c_ptr(), &count, vk_images);

    if (result != VK_SUCCESS) {
        delete[] vk_images;
        throw VulkanError(result);
    } else {
        for (uint64_t i = 0; i < count; ++i) {
            VkImage image;
            image._image = vk_images[i];
            v.push(image);
        }

        delete[] vk_images;
    }

    return v;
}

VkImageView VkDevice::create_image_view(
    const VkImageView::CreateInfo& info) const
{
    ::VkResult result;

    ::VkImageViewCreateInfo vk_info = info.c_struct();
    ::VkImageView view;
    result = vkCreateImageView(this->_device, &vk_info, nullptr, &view);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    VkImageView image_view;
    image_view._view = std::shared_ptr<::VkImageView>(
        new ::VkImageView(view), VkImageView::Deleter(this->_device));

    return image_view;
}

::VkDevice VkDevice::c_ptr()
{
    return this->_device;
}

} // namespace vk
} // namespace pr
