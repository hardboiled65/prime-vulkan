#include <prime-vulkan/pipeline.h>

#include <prime-vulkan/shader-module.h>

namespace pr {
namespace vk {

VkPipeline::ShaderStageCreateInfo::ShaderStageCreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    this->_info.pSpecializationInfo = nullptr;
    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void VkPipeline::ShaderStageCreateInfo::set_stage(::VkShaderStageFlagBits stage)
{
    this->_info.stage = stage;
}

void VkPipeline::ShaderStageCreateInfo::set_module(
    const VkShaderModule& shader_module)
{
    this->_info.module = shader_module.c_ptr();
}

void VkPipeline::ShaderStageCreateInfo::set_name(const pr::String& name)
{
    this->_name = name;
    this->_info.pName = this->_name.c_str();
}

::VkPipelineShaderStageCreateInfo
VkPipeline::ShaderStageCreateInfo::c_struct() const
{
    return this->_info;
}


VkPipeline::DynamicStateCreateInfo::DynamicStateCreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;

    this->_states = nullptr;
}

VkPipeline::DynamicStateCreateInfo::DynamicStateCreateInfo(
    const VkPipeline::DynamicStateCreateInfo& other)
{
    this->_info.sType = other._info.sType;

    this->_info.flags = 0;
    this->_info.pNext = 0;

    if (other._states != nullptr) {
        this->_info.dynamicStateCount = other._info.dynamicStateCount;
        this->_states = new ::VkDynamicState[other._info.dynamicStateCount];
        for (uint64_t i = 0; i < other._info.dynamicStateCount; ++i) {
            this->_states[i] = other._states[i];
        }
        this->_info.pDynamicStates = this->_states;
    } else {
        this->_states = nullptr;
    }
}

VkPipeline::DynamicStateCreateInfo::~DynamicStateCreateInfo()
{
    if (this->_states != nullptr) {
        delete[] this->_states;
    }
}

void VkPipeline::DynamicStateCreateInfo::set_dynamic_states(
    const pr::Vector<::VkDynamicState>& states)
{
    uint64_t count = states.length();
    this->_info.dynamicStateCount = count;

    this->_states = new ::VkDynamicState[count];
    for (uint64_t i = 0; i < count; ++i) {
        this->_states[i] = states[i];
    }
    this->_info.pDynamicStates = this->_states;
}

::VkPipelineDynamicStateCreateInfo
VkPipeline::DynamicStateCreateInfo::c_struct() const
{
    return this->_info;
}


VkPipeline::VertexInputStateCreateInfo::VertexInputStateCreateInfo()
{
    this->_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void VkPipeline::VertexInputStateCreateInfo::set_vertex_binding_descriptions(
    const pr::Vector<::VkVertexInputBindingDescription>& descriptions)
{
    if (descriptions.length() != 0) {
        fprintf(stderr, "[WARN] Description setter with non-zero length Vector is not implemented.\n");
    }

    this->_info.vertexBindingDescriptionCount = 0;
    this->_info.pVertexBindingDescriptions = nullptr;
}

void VkPipeline::VertexInputStateCreateInfo::set_vertex_attribute_descriptions(
    const pr::Vector<::VkVertexInputAttributeDescription>& descriptions)
{
    if (descriptions.length() != 0) {
        fprintf(stderr, "[WARN] Description setter with non-zero length Vector is not implemented.\n");
    }

    this->_info.vertexAttributeDescriptionCount = 0;
    this->_info.pVertexAttributeDescriptions = nullptr;
}

auto VkPipeline::VertexInputStateCreateInfo::c_struct() const -> CType
{
    return this->_info;
}


VkPipeline::InputAssemblyStateCreateInfo::InputAssemblyStateCreateInfo()
{
    this->_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void VkPipeline::InputAssemblyStateCreateInfo::set_topology(
    ::VkPrimitiveTopology topology)
{
    this->_info.topology = topology;
}

void VkPipeline::InputAssemblyStateCreateInfo::set_primitive_restart_enable(
    bool enable)
{
    this->_info.primitiveRestartEnable = (enable) ? VK_TRUE : VK_FALSE;
}

auto VkPipeline::InputAssemblyStateCreateInfo::c_struct() const -> CType
{
    return this->_info;
}


VkPipeline::ViewportStateCreateInfo::ViewportStateCreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    this->_info.pViewports = nullptr;
    this->_info.pScissors = nullptr;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void VkPipeline::ViewportStateCreateInfo::set_viewport_count(uint32_t count)
{
    this->_info.viewportCount = count;
}

void VkPipeline::ViewportStateCreateInfo::set_scissor_count(uint32_t count)
{
    this->_info.scissorCount = count;
}

auto VkPipeline::ViewportStateCreateInfo::c_struct() const -> CType
{
    return this->_info;
}


VkPipeline::RasterizationStateCreateInfo::RasterizationStateCreateInfo()
{
    this->_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void VkPipeline::RasterizationStateCreateInfo::set_depth_clamp_enable(
    bool enable)
{
    this->_info.depthClampEnable = (enable) ? VK_TRUE : VK_FALSE;
}

void VkPipeline::RasterizationStateCreateInfo::set_rasterizer_discard_enable(
    bool enable)
{
    this->_info.rasterizerDiscardEnable = (enable) ? VK_TRUE : VK_FALSE;
}

void VkPipeline::RasterizationStateCreateInfo::set_polygon_mode(
    ::VkPolygonMode mode)
{
    this->_info.polygonMode = mode;
}

void VkPipeline::RasterizationStateCreateInfo::set_line_width(
    float width)
{
    this->_info.lineWidth = width;
}

void VkPipeline::RasterizationStateCreateInfo::set_cull_mode(
    ::VkCullModeFlags cull_mode)
{
    this->_info.cullMode = cull_mode;
}

void VkPipeline::RasterizationStateCreateInfo::set_front_face(
    ::VkFrontFace front_face)
{
    this->_info.frontFace = front_face;
}

void VkPipeline::RasterizationStateCreateInfo::set_depth_bias_enable(
    bool enable)
{
    this->_info.depthBiasEnable = enable;
}

auto VkPipeline::RasterizationStateCreateInfo::c_struct() const -> CType
{
    return this->_info;
}


VkPipeline::MultisampleStateCreateInfo::MultisampleStateCreateInfo()
{
    this->_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    this->_info.pSampleMask = nullptr;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void VkPipeline::MultisampleStateCreateInfo::set_sample_shading_enable(
    bool enable)
{
    this->_info.sampleShadingEnable = (enable) ? VK_TRUE : VK_FALSE;
}

void VkPipeline::MultisampleStateCreateInfo::set_rasterization_samples(
    ::VkSampleCountFlagBits samples)
{
    this->_info.rasterizationSamples = samples;
}

auto VkPipeline::MultisampleStateCreateInfo::c_struct() const -> CType
{
    return this->_info;
}


VkPipeline::ColorBlendAttachmentState::ColorBlendAttachmentState()
{
    this->_state.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    this->_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    this->_state.colorBlendOp = VK_BLEND_OP_ADD;
    this->_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    this->_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    this->_state.alphaBlendOp = VK_BLEND_OP_ADD;
}

void VkPipeline::ColorBlendAttachmentState::set_color_write_mask(
    ::VkColorComponentFlags mask)
{
    this->_state.colorWriteMask = mask;
}

void VkPipeline::ColorBlendAttachmentState::set_blend_enable(bool enable)
{
    this->_state.blendEnable = (enable) ? VK_TRUE : VK_FALSE;
}

auto VkPipeline::ColorBlendAttachmentState::c_struct() const -> CType
{
    return this->_state;
}


VkPipeline::ColorBlendStateCreateInfo::ColorBlendStateCreateInfo()
{
    this->_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    this->_info.attachmentCount = 0;
    this->_info.pAttachments = nullptr;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;

    this->_p_attachments = nullptr;
}

VkPipeline::ColorBlendStateCreateInfo::~ColorBlendStateCreateInfo()
{
    if (this->_p_attachments != nullptr) {
        delete[] this->_p_attachments;
    }
}

void VkPipeline::ColorBlendStateCreateInfo::set_logic_op(
    std::optional<::VkLogicOp> op)
{
    if (op != std::nullopt) {
        this->_info.logicOpEnable = VK_TRUE;
        this->_info.logicOp = op.value();
    } else {
        this->_info.logicOpEnable = VK_FALSE;
    }
}

void VkPipeline::ColorBlendStateCreateInfo::set_attachments(
    const pr::Vector<ColorBlendAttachmentState>& attachments)
{
    uint64_t count = attachments.length();

    this->_info.attachmentCount = count;

    this->_p_attachments = new ::VkPipelineColorBlendAttachmentState[count];
    for (uint64_t i = 0; i < count; ++i) {
        this->_p_attachments[i] = attachments[i].c_struct();
    }
    this->_info.pAttachments = this->_p_attachments;
}

void VkPipeline::ColorBlendStateCreateInfo::set_blend_constants(float r,
                                                                float g,
                                                                float b,
                                                                float a)
{
    this->_info.blendConstants[0] = r;
    this->_info.blendConstants[1] = g;
    this->_info.blendConstants[2] = b;
    this->_info.blendConstants[3] = a;
}

auto VkPipeline::ColorBlendStateCreateInfo::c_struct() const -> CType
{
    return this->_info;
}


VkPipeline::GraphicsPipelineCreateInfo::GraphicsPipelineCreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}


VkPipelineLayout::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void VkPipelineLayout::CreateInfo::set_set_layouts(
    const pr::Vector<::VkDescriptorSetLayout>& set_layouts)
{
    if (set_layouts.length() != 0) {
        fprintf(stderr, "[WARN] Set layout setter with non-zero length Vector is not implemented.\n");
    }

    this->_info.setLayoutCount = 0;
    this->_info.pSetLayouts = nullptr;
}

void VkPipelineLayout::CreateInfo::set_push_constant_range(
    const pr::Vector<::VkPushConstantRange>& push_constant_range)
{
    if (push_constant_range.length() != 0) {
        fprintf(stderr, "[WARN] Push constant range setter with non-zero length Vector is not implemented.\n");
    }

    this->_info.pushConstantRangeCount = 0;
    this->_info.pPushConstantRanges = nullptr;
}

auto VkPipelineLayout::CreateInfo::c_struct() const -> CType
{
    return this->_info;
}


VkPipelineLayout::VkPipelineLayout()
{
    this->_layout = nullptr;
}

auto VkPipelineLayout::c_ptr() const -> CType
{
    return *(this->_layout);
}

} // namespace vk
} // namespace pr
