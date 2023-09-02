#ifndef _PRIME_VULKAN_FRAMEBUFFER_H
#define _PRIME_VULKAN_FRAMEBUFFER_H

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

#include <primer/vector.h>

namespace pr {
namespace vk {

class RenderPass;

class VkImageView;

class Device;

class Framebuffer
{
    friend Device;
public:
    using CType = ::VkFramebuffer;

    class CreateInfo
    {
    public:
        using CType = ::VkFramebufferCreateInfo;

    public:
        CreateInfo();

        void set_render_pass(const RenderPass& render_pass);

        void set_attachments(const pr::Vector<VkImageView>& attachments);

        void set_width(uint32_t width);

        void set_height(uint32_t height);

        void set_layers(uint32_t layers);

        CType c_struct() const;

    private:
        CType _info;

        std::vector<::VkImageView> _attachments;
    };

    class Deleter
    {
    public:
        Deleter(::VkDevice p_device)
        {
            this->_p_device = p_device;
        }

        void operator()(CType *framebuffer)
        {
            vkDestroyFramebuffer(this->_p_device, *framebuffer, nullptr);
        }

    private:
        ::VkDevice _p_device;
    };

public:
    CType c_ptr() const;

private:
    Framebuffer();

private:
    std::shared_ptr<CType> _framebuffer;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_FRAMEBUFFER_H
