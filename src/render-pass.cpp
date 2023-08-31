#include <prime-vulkan/render-pass.h>

#include <prime-vulkan/framebuffer.h>

namespace pr {
namespace vk {

AttachmentDescription::AttachmentDescription()
{
    this->_description.flags = 0;
}

void AttachmentDescription::set_format(::VkFormat format)
{
    this->_description.format = format;
}

void AttachmentDescription::set_samples(::VkSampleCountFlagBits samples)
{
    this->_description.samples = samples;
}

void AttachmentDescription::set_load_op(::VkAttachmentLoadOp op)
{
    this->_description.loadOp = op;
}

void AttachmentDescription::set_store_op(::VkAttachmentStoreOp op)
{
    this->_description.storeOp = op;
}

void AttachmentDescription::set_stencil_load_op(::VkAttachmentLoadOp op)
{
    this->_description.stencilLoadOp = op;
}

void AttachmentDescription::set_stencil_store_op(::VkAttachmentStoreOp op)
{
    this->_description.stencilStoreOp = op;
}

void AttachmentDescription::set_initial_layout(::VkImageLayout layout)
{
    this->_description.initialLayout = layout;
}

void AttachmentDescription::set_final_layout(::VkImageLayout layout)
{
    this->_description.finalLayout = layout;
}

auto AttachmentDescription::c_struct() const -> CType
{
    return this->_description;
}


AttachmentReference::AttachmentReference()
{
    this->_reference.attachment = 0;
    this->_reference.layout = VK_IMAGE_LAYOUT_UNDEFINED;
}

AttachmentReference::AttachmentReference(uint32_t attachment,
                                         ::VkImageLayout layout)
{
    this->_reference.attachment = attachment;
    this->_reference.layout = layout;
}

auto AttachmentReference::c_struct() const -> CType
{
    return this->_reference;
}


SubpassDescription::SubpassDescription()
{
    this->_description.colorAttachmentCount = 0;
    this->_description.pColorAttachments = nullptr;
    this->_description.inputAttachmentCount = 0;
    this->_description.pInputAttachments = nullptr;
    this->_description.preserveAttachmentCount = 0;
    this->_description.pPreserveAttachments = nullptr;
    this->_description.pResolveAttachments = nullptr;
    this->_description.pDepthStencilAttachment = nullptr;

    this->_description.flags = 0;

    this->_attachments = nullptr;
}

SubpassDescription::SubpassDescription(const SubpassDescription& other)
{
    this->_description = other._description;

    if (other._attachments != nullptr) {
        uint32_t count = other._description.colorAttachmentCount;
        this->_attachments = new AttachmentReference::CType[count];
        for (uint32_t i = 0; i < count; ++i) {
            this->_attachments[i] = other._attachments[i];
        }
        this->_description.pColorAttachments = this->_attachments;
    }
}

SubpassDescription::~SubpassDescription()
{
    if (this->_attachments != nullptr) {
        delete[] this->_attachments;
    }
}

void SubpassDescription::set_pipeline_bind_point(::VkPipelineBindPoint bind_point)
{
    this->_description.pipelineBindPoint = bind_point;
}

void SubpassDescription::set_color_attachments(
    const pr::Vector<AttachmentReference>& attachments)
{
    uint64_t count = attachments.length();

    this->_description.colorAttachmentCount = count;

    // TODO: nullptr for zero-length vector.
    this->_attachments = new AttachmentReference::CType[count];
    for (uint64_t i = 0; i < count; ++i) {
        this->_attachments[i] = attachments[i].c_struct();
    }
    this->_description.pColorAttachments = this->_attachments;
}

auto SubpassDescription::c_struct() const -> CType
{
    return this->_description;
}


SubpassDependency::SubpassDependency()
{
    this->_dependency.dependencyFlags = 0;
}

void SubpassDependency::set_subpass_src_dst(int32_t src, int32_t dst)
{
    this->_dependency.srcSubpass = src;
    this->_dependency.dstSubpass = dst;
}

void SubpassDependency::set_stage_mask_src_dst(::VkPipelineStageFlags src,
                                               ::VkPipelineStageFlags dst)
{
    this->_dependency.srcStageMask = src;
    this->_dependency.dstStageMask = dst;
}

void SubpassDependency::set_access_mask_src_dst(::VkAccessFlags src,
                                                ::VkAccessFlags dst)
{
    this->_dependency.srcAccessMask = src;
    this->_dependency.dstAccessMask = dst;
}

auto SubpassDependency::c_struct() const -> CType
{
    return this->_dependency;
}


RenderPass::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

RenderPass::CreateInfo::~CreateInfo()
{
}

void RenderPass::CreateInfo::set_attachments(
    const pr::Vector<AttachmentDescription>& vec)
{
    this->_attachments = vec;
    uint64_t count = vec.length();

    this->_info.attachmentCount = count;

    for (uint64_t i = 0; i < count; ++i) {
        this->_p_attachments.push_back(this->_attachments[i].c_struct());
    }
    this->_info.pAttachments = this->_p_attachments.data();
}

void RenderPass::CreateInfo::set_subpasses(
    const pr::Vector<SubpassDescription>& vec)
{
    this->_subpasses = vec;
    uint64_t count = vec.length();

    this->_info.subpassCount = count;
    for (uint64_t i = 0; i < count; ++i) {
        this->_p_subpasses.push_back(this->_subpasses[i].c_struct());
    }

    this->_info.pSubpasses = this->_p_subpasses.data();
}

void RenderPass::CreateInfo::set_dependencies(
    const pr::Vector<SubpassDependency>& vec)
{
    this->_dependencies = vec;
    uint64_t count = vec.length();

    this->_info.dependencyCount = count;

    for (uint64_t i = 0; i < count; ++i) {
        this->_p_dependencies.push_back(this->_dependencies[i].c_struct());
    }
    this->_info.pDependencies = this->_p_dependencies.data();
}

auto RenderPass::CreateInfo::c_struct() const -> CType
{
    return this->_info;
}


RenderPass::BeginInfo::BeginInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    this->_info.pNext = nullptr;
}

void RenderPass::BeginInfo::set_render_pass(const RenderPass& render_pass)
{
    this->_info.renderPass = render_pass.c_ptr();
}

void RenderPass::BeginInfo::set_framebuffer(const Framebuffer& framebuffer)
{
    this->_info.framebuffer = framebuffer.c_ptr();
}

void RenderPass::BeginInfo::set_render_area(::VkRect2D area)
{
    this->_info.renderArea = area;
}

void RenderPass::BeginInfo::set_clear_values(
    const pr::Vector<::VkClearValue>& values)
{
    this->_info.clearValueCount = values.length();

    for (auto& value: values) {
        this->_clear_values.push_back(value);
    }
    this->_info.pClearValues = this->_clear_values.data();
}

auto RenderPass::BeginInfo::c_struct() const -> CType
{
    return this->_info;
}


RenderPass::RenderPass()
{
    this->_render_pass = nullptr;
}

auto RenderPass::c_ptr() const -> CType
{
    return *(this->_render_pass);
}

} // namespace vk
} // namespace pr
