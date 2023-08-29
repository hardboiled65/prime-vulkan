#ifndef _PRIME_VULKAN_RENDER_PASS_H
#define _PRIME_VULKAN_RENDER_PASS_H

#include <vulkan/vulkan.h>

#include <memory>

#include <primer/vector.h>

namespace pr {
namespace vk {

class VkDevice;

class AttachmentDescription
{
public:
    using CType = ::VkAttachmentDescription;

public:
    AttachmentDescription();

    void set_format(::VkFormat format);

    void set_samples(::VkSampleCountFlagBits samples);

    void set_load_op(::VkAttachmentLoadOp op);

    void set_store_op(::VkAttachmentStoreOp op);

    void set_stencil_load_op(::VkAttachmentLoadOp op);

    void set_stencil_store_op(::VkAttachmentStoreOp op);

    void set_initial_layout(::VkImageLayout layout);

    void set_final_layout(::VkImageLayout layout);

    CType c_struct() const;

private:
    CType _description;
};


class AttachmentReference
{
public:
    using CType = ::VkAttachmentReference;

public:
    AttachmentReference();

    AttachmentReference(uint32_t attachment, ::VkImageLayout layout);

    CType c_struct() const;

private:
    CType _reference;
};


class SubpassDescription
{
public:
    using CType = ::VkSubpassDescription;

public:
    SubpassDescription();

    SubpassDescription(const SubpassDescription& other);

    ~SubpassDescription();

    void set_pipeline_bind_point(::VkPipelineBindPoint bind_point);

    void set_color_attachments(
        const pr::Vector<AttachmentReference>& attachments);

    CType c_struct() const;

private:
    CType _description;

    AttachmentReference::CType *_attachments;
};


class SubpassDependency
{
public:
    using CType = ::VkSubpassDependency;

public:
    SubpassDependency();

    void set_subpass_src_dst(int32_t src, int32_t dst);

    void set_stage_mask_src_dst(::VkPipelineStageFlags src,
                                ::VkPipelineStageFlags dst);

    void set_access_mask_src_dst(::VkAccessFlags src,
                                 ::VkAccessFlags dst);

    CType c_struct() const;

private:
    CType _dependency;
};


class RenderPass
{
    friend VkDevice;
public:
    using CType = ::VkRenderPass;

    class CreateInfo
    {
    public:
        using CType = ::VkRenderPassCreateInfo;

    public:
        CreateInfo();

        ~CreateInfo();

        void set_attachments(const pr::Vector<AttachmentDescription>& vec);

        void set_subpasses(const pr::Vector<SubpassDescription>& vec);

        void set_dependencies(const pr::Vector<SubpassDependency>& vec);

        CType c_struct() const;

    private:
        CType _info;

        AttachmentDescription::CType *_attachments;
        SubpassDescription::CType *_subpasses;
        SubpassDependency::CType *_dependencies;
    };

    class Deleter
    {
    public:
        Deleter(::VkDevice p_device)
        {
            this->_p_device = p_device;
        }

        void operator()(CType *render_pass)
        {
            vkDestroyRenderPass(this->_p_device, *render_pass, nullptr);
        }

    private:
        ::VkDevice _p_device;
    };

public:
    CType c_ptr() const;

private:
    RenderPass();

private:
    std::shared_ptr<CType> _render_pass;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_RENDER_PASS_H
