#include <prime-vulkan/render-pass.h>

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

SubpassDescription::~SubpassDescription()
{
    if (this->_attachments != nullptr) {
        delete[] this->_attachments;
    }
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

    this->_attachments = nullptr;
    this->_subpasses = nullptr;
    this->_dependencies = nullptr;
}

RenderPass::CreateInfo::~CreateInfo()
{
    if (this->_attachments != nullptr) {
        delete[] this->_attachments;
    }
    if (this->_subpasses != nullptr) {
        delete[] this->_subpasses;
    }
    if (this->_dependencies != nullptr) {
        delete[] this->_dependencies;
    }
}

void RenderPass::CreateInfo::set_attachments(
    const pr::Vector<AttachmentDescription>& vec)
{
    uint64_t count = vec.length();

    this->_info.attachmentCount = count;

    this->_attachments = new AttachmentDescription::CType[count];
    for (uint64_t i = 0; i < count; ++i) {
        this->_attachments[i] = vec[i].c_struct();
    }
    this->_info.pAttachments = this->_attachments;
}

void RenderPass::CreateInfo::set_subpasses(
    const pr::Vector<SubpassDescription>& vec)
{
    uint64_t count = vec.length();

    this->_info.subpassCount = count;

    this->_subpasses = new SubpassDescription::CType[count];
    for (uint64_t i = 0; i < count; ++i) {
        this->_subpasses[i] = vec[i].c_struct();
    }
    this->_info.pSubpasses = this->_subpasses;
}

void RenderPass::CreateInfo::set_dependencies(
    const pr::Vector<SubpassDependency>& vec)
{
    uint64_t count = vec.length();

    this->_info.dependencyCount = count;

    this->_dependencies = new SubpassDependency::CType[count];
    for (uint64_t i = 0; i < count; ++i) {
        this->_dependencies[i] = vec[i].c_struct();
    }
    this->_info.pDependencies = this->_dependencies;
}

} // namespace vk
} // namespace pr
