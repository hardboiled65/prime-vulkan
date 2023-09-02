#ifndef _PRIME_VULKAN_SWAPCHAIN_H
#define _PRIME_VULKAN_SWAPCHAIN_H

#include <vulkan/vulkan.h>

#include <memory>

#include <primer/vector.h>

namespace pr {
namespace vk {

class Surface;

class Device;

class VkSwapchain
{
    friend Device;
public:
    using CType = ::VkSwapchainKHR;

    class CreateInfo
    {
    public:
        CreateInfo();

        ~CreateInfo();

        /// Since raw pointers are used internally, the surface object must
        /// live longer than the create info.
        void set_surface(const Surface& surface);

        void set_min_image_count(uint32_t count);

        void set_image_format(::VkFormat format);

        void set_image_color_space(::VkColorSpaceKHR color_space);

        void set_image_extent(::VkExtent2D extent);

        void set_image_array_layers(uint32_t layers);

        void set_image_usage(::VkImageUsageFlags usage);

        void set_image_sharing_mode(::VkSharingMode mode);

        /// Set queue family indices.
        ///
        /// `.queueFamilyIndexCount` will automatically set.
        /// If an empty vector passed, `.pQueueFamilyIndices` willl be nullptr.
        void set_queue_family_indices(const Vector<uint32_t>& indices);

        void set_pre_transform(::VkSurfaceTransformFlagBitsKHR transform);

        void set_composite_alpha(::VkCompositeAlphaFlagBitsKHR composite);

        void set_present_mode(::VkPresentModeKHR mode);

        void set_clipped(bool clipped);

        void set_old_swapchain(/* TODO */);

        ::VkSwapchainCreateInfoKHR c_struct() const;

    private:
        ::VkSwapchainCreateInfoKHR _info;

        uint32_t *_indices;
    };

    class Deleter
    {
    public:
        Deleter() = delete;

        Deleter(::VkDevice p_device)
        {
            this->_p_device = p_device;
        }

        void operator()(CType *swapchain)
        {
            vkDestroySwapchainKHR(this->_p_device, *swapchain, nullptr);
        }

    private:
        ::VkDevice _p_device;
    };

public:
    ::VkSwapchainKHR c_ptr() const;

private:
    VkSwapchain();

private:
    std::shared_ptr<CType> _swapchain;
};


class VkImage
{
    friend Device;
public:
    ::VkImage c_ptr() const;

private:
    VkImage();

private:
    ::VkImage _image;
};


class VkImageView
{
    friend Device;
public:
    class CreateInfo
    {
    public:
        CreateInfo();

        void set_image(const VkImage& image);

        void set_view_type(::VkImageViewType type);

        void set_format(::VkFormat format);

        void set_components(::VkComponentSwizzle r,
                            ::VkComponentSwizzle g,
                            ::VkComponentSwizzle b,
                            ::VkComponentSwizzle a);

        void set_subresource_range(::VkImageSubresourceRange range);

        ::VkImageViewCreateInfo c_struct() const;

    private:
        ::VkImageViewCreateInfo _info;
    };

    class Deleter
    {
    public:
        Deleter() = delete;

        Deleter(::VkDevice p_device)
        {
            this->_p_device = p_device;
        }

        void operator()(::VkImageView *image_view)
        {
            vkDestroyImageView(this->_p_device, *image_view, nullptr);
        }

    private:
        ::VkDevice _p_device;
    };

public:
    ~VkImageView();

    ::VkImageView c_ptr() const;

private:
    VkImageView();

private:
    std::shared_ptr<::VkImageView> _view;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_SWAPCHAIN_H
