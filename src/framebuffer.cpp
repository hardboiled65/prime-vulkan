#include <prime-vulkan/framebuffer.h>

#include <prime-vulkan/render-pass.h>
#include <prime-vulkan/swapchain.h>     // ImageView

namespace pr {
namespace vk {

Framebuffer::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void Framebuffer::CreateInfo::set_render_pass(const RenderPass& render_pass)
{
    this->_info.renderPass = render_pass.c_ptr();
}

void Framebuffer::CreateInfo::set_attachments(
    const pr::Vector<VkImageView>& attachments)
{
    uint64_t count = attachments.length();

    this->_info.attachmentCount = count;
    for (uint64_t i = 0; i < count; ++i) {
        this->_attachments.push_back(attachments[i].c_ptr());
    }
    this->_info.pAttachments = this->_attachments.data();
}

void Framebuffer::CreateInfo::set_width(uint32_t width)
{
    this->_info.width = width;
}

void Framebuffer::CreateInfo::set_height(uint32_t height)
{
    this->_info.height = height;
}

void Framebuffer::CreateInfo::set_layers(uint32_t layers)
{
    this->_info.layers = layers;
}

auto Framebuffer::CreateInfo::c_struct() const -> CType
{
    return this->_info;
}


Framebuffer::Framebuffer()
{
    this->_framebuffer = nullptr;
}

auto Framebuffer::c_ptr() const -> CType
{
    return *(this->_framebuffer);
}

} // namespace vk
} // namespace pr
