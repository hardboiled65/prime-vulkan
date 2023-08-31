#include <prime-vulkan/swapchain.h>

#include <prime-vulkan/surface.h>

namespace pr {
namespace vk {

VkSwapchain::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    this->_info.surface = nullptr;
    this->_info.minImageCount = 0;
    this->_info.imageFormat = VK_FORMAT_UNDEFINED;
    this->_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    this->_info.imageExtent = ::VkExtent2D();
    this->_info.imageArrayLayers = 0;
    this->_info.imageUsage = 0;
    this->_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    this->_info.queueFamilyIndexCount = 0;
    this->_info.pQueueFamilyIndices = nullptr;
    this->_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    this->_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    this->_info.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    this->_info.clipped = VK_FALSE;
    this->_info.oldSwapchain = VK_NULL_HANDLE;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;

    this->_indices = nullptr;
}

VkSwapchain::CreateInfo::~CreateInfo()
{
    if (this->_indices != nullptr) {
        delete[] this->_indices;
    }
}

void VkSwapchain::CreateInfo::set_surface(const Surface& surface)
{
    this->_info.surface = surface.c_ptr();
}

void VkSwapchain::CreateInfo::set_min_image_count(uint32_t count)
{
    this->_info.minImageCount = count;
}

void VkSwapchain::CreateInfo::set_image_format(::VkFormat format)
{
    this->_info.imageFormat = format;
}

void VkSwapchain::CreateInfo::set_image_color_space(::VkColorSpaceKHR value)
{
    this->_info.imageColorSpace = value;
}

void VkSwapchain::CreateInfo::set_image_extent(::VkExtent2D extent)
{
    this->_info.imageExtent = extent;
}

void VkSwapchain::CreateInfo::set_image_array_layers(uint32_t layers)
{
    this->_info.imageArrayLayers = layers;
}

void VkSwapchain::CreateInfo::set_image_usage(::VkImageUsageFlags usage)
{
    this->_info.imageUsage = usage;
}

void VkSwapchain::CreateInfo::set_image_sharing_mode(::VkSharingMode mode)
{
    this->_info.imageSharingMode = mode;
}

void VkSwapchain::CreateInfo::set_queue_family_indices(
    const Vector<uint32_t>& indices)
{
    auto length = indices.length();
    if (length != 0) {
        this->_info.queueFamilyIndexCount = length;
        this->_indices = new uint32_t[length];
        for (uint64_t i = 0; i < length; ++i) {
            this->_indices[i] = indices[i];
        }
        this->_info.pQueueFamilyIndices = this->_indices;
    } else {
        this->_info.queueFamilyIndexCount = 0;
        this->_info.pQueueFamilyIndices = nullptr;
    }
}

void VkSwapchain::CreateInfo::set_pre_transform(
    ::VkSurfaceTransformFlagBitsKHR transform)
{
    this->_info.preTransform = transform;
}

void VkSwapchain::CreateInfo::set_composite_alpha(
    ::VkCompositeAlphaFlagBitsKHR composite)
{
    this->_info.compositeAlpha = composite;
}

void VkSwapchain::CreateInfo::set_present_mode(
    ::VkPresentModeKHR mode)
{
    this->_info.presentMode = mode;
}

void VkSwapchain::CreateInfo::set_clipped(bool clipped)
{
    this->_info.clipped = (clipped) ? VK_TRUE : VK_FALSE;
}

::VkSwapchainCreateInfoKHR VkSwapchain::CreateInfo::c_struct() const
{
    return this->_info;
}

VkSwapchain::VkSwapchain()
{
    this->_swapchain = nullptr;
}

::VkSwapchainKHR VkSwapchain::c_ptr() const
{
    return this->_swapchain;
}


VkImage::VkImage()
{
    this->_image = nullptr;
}

::VkImage VkImage::c_ptr() const
{
    return this->_image;
}


VkImageView::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void VkImageView::CreateInfo::set_image(const VkImage& image)
{
    this->_info.image = image.c_ptr();
}

void VkImageView::CreateInfo::set_view_type(::VkImageViewType type)
{
    this->_info.viewType = type;
}

void VkImageView::CreateInfo::set_format(::VkFormat format)
{
    this->_info.format = format;
}

void VkImageView::CreateInfo::set_components(::VkComponentSwizzle r,
                                             ::VkComponentSwizzle g,
                                             ::VkComponentSwizzle b,
                                             ::VkComponentSwizzle a)
{
    this->_info.components.r = r;
    this->_info.components.g = g;
    this->_info.components.b = b;
    this->_info.components.a = a;
}

void VkImageView::CreateInfo::set_subresource_range(::VkImageSubresourceRange range)
{
    this->_info.subresourceRange = range;
}

::VkImageViewCreateInfo VkImageView::CreateInfo::c_struct() const
{
    return this->_info;
}


VkImageView::VkImageView()
{
    this->_view = nullptr;
}

VkImageView::~VkImageView()
{
}

::VkImageView VkImageView::c_ptr() const
{
    return *(this->_view);
}

} // namespace vk
} // namespace pr
