#ifndef _PRIME_VULKAN_PIPELINE_H
#define _PRIME_VULKAN_PIPELINE_H

#include <vulkan/vulkan.h>

#include <memory>
#include <optional>

#include <primer/string.h>

namespace pr {
namespace vk {

class VkShaderModule;

class VkDevice;

class VkPipelineLayout;

class VkPipeline
{
//=================
// Nested classes
//=================
public:
    /// A wrapper class for `VkPipelineShaderStageCreateInfo` struct.
    class ShaderStageCreateInfo
    {
    public:
        ShaderStageCreateInfo();

        void set_stage(::VkShaderStageFlagBits stage);

        void set_module(const VkShaderModule& shader_module);

        void set_name(const pr::String& name);

        ::VkPipelineShaderStageCreateInfo c_struct() const;

    private:
        ::VkPipelineShaderStageCreateInfo _info;
        pr::String _name;
    };

    /// A wrapper class for `VkPipelineDynamicStateCreateInfo` struct.
    class DynamicStateCreateInfo
    {
    public:
        DynamicStateCreateInfo();

        DynamicStateCreateInfo(const DynamicStateCreateInfo& other);

        ~DynamicStateCreateInfo();

        /// Sets `.pDynamicStates` and automatically fills the count field.
        void set_dynamic_states(const pr::Vector<::VkDynamicState>& states);

        ::VkPipelineDynamicStateCreateInfo c_struct() const;

    private:
        ::VkPipelineDynamicStateCreateInfo _info;

        ::VkDynamicState *_states;
    };

    /// A wrapper class for `VkPipelineVertexInputStateCreateInfo` struct.
    class VertexInputStateCreateInfo
    {
    public:
        using CType = ::VkPipelineVertexInputStateCreateInfo;

    public:
        VertexInputStateCreateInfo();

        /// Set the vertex input binding descriptions.
        /// Count will automatically filled.
        ///
        /// TODO: Currently only accepts 0 length Vector.
        /// Others not implemented.
        void set_vertex_binding_descriptions(
            const pr::Vector<::VkVertexInputBindingDescription>& descriptions);

        /// Set the vertex input attribute descriptions.
        /// Count will automatically filled.
        ///
        /// TODO: Currently only accepts 0 length Vector.
        /// Others not implemented.
        void set_vertex_attribute_descriptions(
            const pr::Vector<::VkVertexInputAttributeDescription>& descriptions);

        CType c_struct() const;

    private:
        CType _info;
    };

    /// A wrapper class for `VkPipelineInputAssemblyStateCreateInfo` struct.
    class InputAssemblyStateCreateInfo
    {
    public:
        using CType = ::VkPipelineInputAssemblyStateCreateInfo;

    public:
        InputAssemblyStateCreateInfo();

        void set_topology(::VkPrimitiveTopology topology);

        void set_primitive_restart_enable(bool enable);

        CType c_struct() const;

    private:
        CType _info;
    };

    /// A wrapper class for `VkPipelineViewportStateCreateInfo` struct.
    class ViewportStateCreateInfo
    {
    public:
        using CType = ::VkPipelineViewportStateCreateInfo;

    public:
        ViewportStateCreateInfo();

        /// Set viewport count for dynamic state.
        void set_viewport_count(uint32_t count);

        /// Set scissor count for dynamic state.
        void set_scissor_count(uint32_t count);

        CType c_struct() const;

    private:
        CType _info;
    };

    /// A wrapper class for `VkPipelineRasterizationStateCreateInfo` struct.
    class RasterizationStateCreateInfo
    {
    public:
        using CType = ::VkPipelineRasterizationStateCreateInfo;

    public:
        RasterizationStateCreateInfo();

        void set_depth_clamp_enable(bool enable);

        void set_rasterizer_discard_enable(bool enable);

        void set_polygon_mode(::VkPolygonMode mode);

        void set_line_width(float width);

        void set_cull_mode(::VkCullModeFlags cull_mode);

        void set_front_face(::VkFrontFace front_face);

        void set_depth_bias_enable(bool enable);

        CType c_struct() const;

    private:
        CType _info;
    };

    /// A wrapper class for `VkPipelineMultisampleStateCreateInfo` struct.
    class MultisampleStateCreateInfo
    {
    public:
        using CType = ::VkPipelineMultisampleStateCreateInfo;

    public:
        MultisampleStateCreateInfo();

        void set_sample_shading_enable(bool enable);

        void set_rasterization_samples(::VkSampleCountFlagBits samples);

        CType c_struct() const;

    private:
        CType _info;
    };

    /// A wrapper class for `VkPipelineColorBlendAttachmentState` struct.
    class ColorBlendAttachmentState
    {
    public:
        using CType = ::VkPipelineColorBlendAttachmentState;

    public:
        ColorBlendAttachmentState();

        void set_color_write_mask(::VkColorComponentFlags mask);

        void set_blend_enable(bool enable);

        CType c_struct() const;

    private:
        CType _state;
    };

    /// A wrapper class for `VkPipelineColorBlendStateCreateInfo` struct.
    class ColorBlendStateCreateInfo
    {
    public:
        using CType = ::VkPipelineColorBlendStateCreateInfo;

    public:
        ColorBlendStateCreateInfo();

        ~ColorBlendStateCreateInfo();

        /// Set `.logicOp` field.
        ///
        /// If the argument is a value, then `.logicOpEnable` field will be
        /// `VK_TRUE`. Otherwise set as `VK_FALSE` automatically.
        void set_logic_op(std::optional<::VkLogicOp> op);

        void set_attachments(
            const pr::Vector<ColorBlendAttachmentState>& attachments);

        void set_blend_constants(float r, float g, float b, float a);

        CType c_struct() const;

    private:
        CType _info;

        ::VkPipelineColorBlendAttachmentState *_p_attachments;
    };

    /// A wrapper class for `VkGraphicsPipelineCreateInfo` struct.
    class GraphicsPipelineCreateInfo
    {
    public:
        using CType = ::VkGraphicsPipelineCreateInfo;

    public:
        GraphicsPipelineCreateInfo();

        void set_stages(const pr::Vector<ShaderStageCreateInfo>& stages);

        void set_vertex_input_state(const VertexInputStateCreateInfo& info);

        void set_input_assembly_state(const InputAssemblyStateCreateInfo& info);

        void set_viewport_state(const ViewportStateCreateInfo& info);

        void set_rasterization_state(const RasterizationStateCreateInfo& info);

        void set_multisample_state(const MultisampleStateCreateInfo& info);

        void set_color_blend_state(const ColorBlendStateCreateInfo& info);

        void set_dynamic_state(const DynamicStateCreateInfo& info);

        void set_layout(const VkPipelineLayout& layout);

        void set_render_pass(/*const VkRenderPass& render_pass*/);

        void set_subpass(uint32_t subpass);

        void set_base_pipeline_handle(/**/);

        CType c_struct() const;

    private:
        CType _info;

        pr::Vector<ShaderStageCreateInfo> _stages;
        VertexInputStateCreateInfo _vertex_input_state;
        InputAssemblyStateCreateInfo _input_assembly_state;
        ViewportStateCreateInfo _viewport_state;
        RasterizationStateCreateInfo _rasterization_state;
        MultisampleStateCreateInfo _multisample_state;
        ColorBlendStateCreateInfo _color_blend_state;
        DynamicStateCreateInfo _dynamic_state;
    };

//===================
// Public methods
//===================
public:

//===================
// Private methods
//===================
private:
    VkPipeline();

//============
// Members
//============
private:
};


class VkPipelineLayout
{
    friend VkDevice;
//===============
// Type aliases
//===============
public:
    using CType = ::VkPipelineLayout;

//=================
// Nested classes
//=================
public:
    class CreateInfo
    {
    public:
        using CType = ::VkPipelineLayoutCreateInfo;

    public:
        CreateInfo();

        /// Sets the set layout list. Count will automatically filled.
        ///
        /// TODO: Currently only accepts 0 length Vector.
        /// Others not implemented.
        void set_set_layouts(
            const pr::Vector<::VkDescriptorSetLayout>& set_layouts);

        /// Sets the push constant range list. Count will automatically filled.
        ///
        /// TODO: Currently only accepts 0 length Vector.
        /// Others not implemented.
        void set_push_constant_range(
            const pr::Vector<::VkPushConstantRange>& push_constant_range);

        CType c_struct() const;

    private:
        CType _info;
    };

    class Deleter
    {
    public:
        Deleter() = delete;

        Deleter(::VkDevice p_device)
        {
            this->_p_device = p_device;
        }

        void operator()(CType *layout)
        {
            vkDestroyPipelineLayout(this->_p_device, *layout, nullptr);
        }

    private:
        ::VkDevice _p_device;
    };

//=================
// Public methods
//=================
public:
    CType c_ptr() const;

//=================
// Private methods
//=================
private:
    VkPipelineLayout();

//==========
// Members
//==========
private:
    std::shared_ptr<CType> _layout;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_PIPELINE_H
