#include <prime-vulkan/pipeline.h>

#include <prime-vulkan/shader-module.h>
#include <prime-vulkan/render-pass.h>

namespace pr {
namespace vk {

Pipeline::ShaderStageCreateInfo::ShaderStageCreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    this->_info.pSpecializationInfo = nullptr;
    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void Pipeline::ShaderStageCreateInfo::set_stage(::VkShaderStageFlagBits stage)
{
    this->_info.stage = stage;
}

void Pipeline::ShaderStageCreateInfo::set_module(
    const VkShaderModule& shader_module)
{
    this->_info.module = shader_module.c_ptr();
}

void Pipeline::ShaderStageCreateInfo::set_name(const pr::String& name)
{
    this->_name = name;
    this->_info.pName = this->_name.c_str();
}

::VkPipelineShaderStageCreateInfo
Pipeline::ShaderStageCreateInfo::c_struct() const
{
    return this->_info;
}


Pipeline::DynamicStateCreateInfo::DynamicStateCreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;

    this->_states = nullptr;
}

Pipeline::DynamicStateCreateInfo::DynamicStateCreateInfo(
    const Pipeline::DynamicStateCreateInfo& other)
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

Pipeline::DynamicStateCreateInfo::~DynamicStateCreateInfo()
{
    if (this->_states != nullptr) {
        delete[] this->_states;
    }
}

void Pipeline::DynamicStateCreateInfo::set_dynamic_states(
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
Pipeline::DynamicStateCreateInfo::c_struct() const
{
    return this->_info;
}


Pipeline::VertexInputStateCreateInfo::VertexInputStateCreateInfo()
{
    this->_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void Pipeline::VertexInputStateCreateInfo::set_vertex_binding_descriptions(
    const pr::Vector<::VkVertexInputBindingDescription>& descriptions)
{
    if (descriptions.length() != 0) {
        fprintf(stderr, "[WARN] Description setter with non-zero length Vector is not implemented.\n");
    }

    this->_info.vertexBindingDescriptionCount = 0;
    this->_info.pVertexBindingDescriptions = nullptr;
}

void Pipeline::VertexInputStateCreateInfo::set_vertex_attribute_descriptions(
    const pr::Vector<::VkVertexInputAttributeDescription>& descriptions)
{
    if (descriptions.length() != 0) {
        fprintf(stderr, "[WARN] Description setter with non-zero length Vector is not implemented.\n");
    }

    this->_info.vertexAttributeDescriptionCount = 0;
    this->_info.pVertexAttributeDescriptions = nullptr;
}

auto Pipeline::VertexInputStateCreateInfo::c_struct() const -> CType
{
    return this->_info;
}


Pipeline::InputAssemblyStateCreateInfo::InputAssemblyStateCreateInfo()
{
    this->_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void Pipeline::InputAssemblyStateCreateInfo::set_topology(
    ::VkPrimitiveTopology topology)
{
    this->_info.topology = topology;
}

void Pipeline::InputAssemblyStateCreateInfo::set_primitive_restart_enable(
    bool enable)
{
    this->_info.primitiveRestartEnable = (enable) ? VK_TRUE : VK_FALSE;
}

auto Pipeline::InputAssemblyStateCreateInfo::c_struct() const -> CType
{
    return this->_info;
}


Pipeline::ViewportStateCreateInfo::ViewportStateCreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    this->_info.pViewports = nullptr;
    this->_info.pScissors = nullptr;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void Pipeline::ViewportStateCreateInfo::set_viewport_count(uint32_t count)
{
    this->_info.viewportCount = count;
}

void Pipeline::ViewportStateCreateInfo::set_scissor_count(uint32_t count)
{
    this->_info.scissorCount = count;
}

auto Pipeline::ViewportStateCreateInfo::c_struct() const -> CType
{
    return this->_info;
}


Pipeline::RasterizationStateCreateInfo::RasterizationStateCreateInfo()
{
    this->_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void Pipeline::RasterizationStateCreateInfo::set_depth_clamp_enable(
    bool enable)
{
    this->_info.depthClampEnable = (enable) ? VK_TRUE : VK_FALSE;
}

void Pipeline::RasterizationStateCreateInfo::set_rasterizer_discard_enable(
    bool enable)
{
    this->_info.rasterizerDiscardEnable = (enable) ? VK_TRUE : VK_FALSE;
}

void Pipeline::RasterizationStateCreateInfo::set_polygon_mode(
    ::VkPolygonMode mode)
{
    this->_info.polygonMode = mode;
}

void Pipeline::RasterizationStateCreateInfo::set_line_width(
    float width)
{
    this->_info.lineWidth = width;
}

void Pipeline::RasterizationStateCreateInfo::set_cull_mode(
    ::VkCullModeFlags cull_mode)
{
    this->_info.cullMode = cull_mode;
}

void Pipeline::RasterizationStateCreateInfo::set_front_face(
    ::VkFrontFace front_face)
{
    this->_info.frontFace = front_face;
}

void Pipeline::RasterizationStateCreateInfo::set_depth_bias_enable(
    bool enable)
{
    this->_info.depthBiasEnable = enable;
}

auto Pipeline::RasterizationStateCreateInfo::c_struct() const -> CType
{
    return this->_info;
}


Pipeline::MultisampleStateCreateInfo::MultisampleStateCreateInfo()
{
    this->_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    this->_info.pSampleMask = nullptr;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void Pipeline::MultisampleStateCreateInfo::set_sample_shading_enable(
    bool enable)
{
    this->_info.sampleShadingEnable = (enable) ? VK_TRUE : VK_FALSE;
}

void Pipeline::MultisampleStateCreateInfo::set_rasterization_samples(
    ::VkSampleCountFlagBits samples)
{
    this->_info.rasterizationSamples = samples;
}

auto Pipeline::MultisampleStateCreateInfo::c_struct() const -> CType
{
    return this->_info;
}


Pipeline::ColorBlendAttachmentState::ColorBlendAttachmentState()
{
    this->_state.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    this->_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    this->_state.colorBlendOp = VK_BLEND_OP_ADD;
    this->_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    this->_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    this->_state.alphaBlendOp = VK_BLEND_OP_ADD;
}

void Pipeline::ColorBlendAttachmentState::set_color_write_mask(
    ::VkColorComponentFlags mask)
{
    this->_state.colorWriteMask = mask;
}

void Pipeline::ColorBlendAttachmentState::set_blend_enable(bool enable)
{
    this->_state.blendEnable = (enable) ? VK_TRUE : VK_FALSE;
}

auto Pipeline::ColorBlendAttachmentState::c_struct() const -> CType
{
    return this->_state;
}


Pipeline::ColorBlendStateCreateInfo::ColorBlendStateCreateInfo()
{
    this->_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    this->_info.attachmentCount = 0;
    this->_info.pAttachments = nullptr;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;

    this->_p_attachments = nullptr;
}

Pipeline::ColorBlendStateCreateInfo::~ColorBlendStateCreateInfo()
{
    if (this->_p_attachments != nullptr) {
        delete[] this->_p_attachments;
    }
}

void Pipeline::ColorBlendStateCreateInfo::set_logic_op(
    std::optional<::VkLogicOp> op)
{
    if (op != std::nullopt) {
        this->_info.logicOpEnable = VK_TRUE;
        this->_info.logicOp = op.value();
    } else {
        this->_info.logicOpEnable = VK_FALSE;
    }
}

void Pipeline::ColorBlendStateCreateInfo::set_attachments(
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

void Pipeline::ColorBlendStateCreateInfo::set_blend_constants(float r,
                                                                float g,
                                                                float b,
                                                                float a)
{
    this->_info.blendConstants[0] = r;
    this->_info.blendConstants[1] = g;
    this->_info.blendConstants[2] = b;
    this->_info.blendConstants[3] = a;
}

auto Pipeline::ColorBlendStateCreateInfo::c_struct() const -> CType
{
    return this->_info;
}


GraphicsPipelineCreateInfo::GraphicsPipelineCreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    this->_info.basePipelineHandle = nullptr;
    this->_info.pTessellationState = nullptr;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void GraphicsPipelineCreateInfo::set_stages(
    const pr::Vector<Pipeline::ShaderStageCreateInfo>& stages)
{
    uint64_t count = stages.length();

    this->_info.stageCount = count;

    this->_stages = stages;
    this->_vk_stages = {};
    for (uint64_t i = 0; i < count; ++i) {
        this->_vk_stages.push_back(stages[i].c_struct());
    }

    this->_info.pStages = this->_vk_stages.data();
}

void GraphicsPipelineCreateInfo::set_vertex_input_state(
    const Pipeline::VertexInputStateCreateInfo& info)
{
    this->_vertex_input_state = info.c_struct();

    this->_info.pVertexInputState = &(this->_vertex_input_state);
}

void GraphicsPipelineCreateInfo::set_input_assembly_state(
    const Pipeline::InputAssemblyStateCreateInfo& info)
{
    this->_input_assembly_state = info.c_struct();

    this->_info.pInputAssemblyState = &(this->_input_assembly_state);
}

void GraphicsPipelineCreateInfo::set_viewport_state(
    const Pipeline::ViewportStateCreateInfo& info)
{
    this->_viewport_state = info.c_struct();

    this->_info.pViewportState = &(this->_viewport_state);
}

void GraphicsPipelineCreateInfo::set_rasterization_state(
    const Pipeline::RasterizationStateCreateInfo& info)
{
    this->_rasterization_state = info.c_struct();

    this->_info.pRasterizationState = &(this->_rasterization_state);
}

void GraphicsPipelineCreateInfo::set_multisample_state(
    const Pipeline::MultisampleStateCreateInfo& info)
{
    this->_multisample_state = info.c_struct();

    this->_info.pMultisampleState = &(this->_multisample_state);
}

void GraphicsPipelineCreateInfo::set_color_blend_state(
    const Pipeline::ColorBlendStateCreateInfo& info)
{
    this->_color_blend_state = info.c_struct();

    this->_info.pColorBlendState = &(this->_color_blend_state);
}

void GraphicsPipelineCreateInfo::set_dynamic_state(
    const Pipeline::DynamicStateCreateInfo& info)
{
    this->_dynamic_state = info.c_struct();

    this->_info.pDynamicState = &(this->_dynamic_state);
}

void GraphicsPipelineCreateInfo::set_layout(
    const PipelineLayout& layout)
{
    this->_layout = layout.c_ptr();

    this->_info.layout = this->_layout;
}

void GraphicsPipelineCreateInfo::set_render_pass(
    const RenderPass& render_pass)
{
    this->_render_pass = render_pass.c_ptr();

    this->_info.renderPass = this->_render_pass;
}

void GraphicsPipelineCreateInfo::set_subpass(uint32_t subpass)
{
    this->_info.subpass = subpass;
}

void GraphicsPipelineCreateInfo::set_base_pipeline_handle(
    const Pipeline& pipeline_handle)
{
    this->_info.basePipelineHandle = pipeline_handle.c_ptr();
}

auto GraphicsPipelineCreateInfo::c_struct() const -> CType
{
    return this->_info;
}


Pipeline::Pipeline()
{
    this->_pipeline = nullptr;
}

auto Pipeline::c_ptr() const -> CType
{
    return *(this->_pipeline);
}


PipelineLayout::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void PipelineLayout::CreateInfo::set_set_layouts(
    const pr::Vector<::VkDescriptorSetLayout>& set_layouts)
{
    if (set_layouts.length() != 0) {
        fprintf(stderr, "[WARN] Set layout setter with non-zero length Vector is not implemented.\n");
    }

    this->_info.setLayoutCount = 0;
    this->_info.pSetLayouts = nullptr;
}

void PipelineLayout::CreateInfo::set_push_constant_range(
    const pr::Vector<::VkPushConstantRange>& push_constant_range)
{
    if (push_constant_range.length() != 0) {
        fprintf(stderr, "[WARN] Push constant range setter with non-zero length Vector is not implemented.\n");
    }

    this->_info.pushConstantRangeCount = 0;
    this->_info.pPushConstantRanges = nullptr;
}

auto PipelineLayout::CreateInfo::c_struct() const -> CType
{
    return this->_info;
}


PipelineLayout::PipelineLayout()
{
    this->_layout = nullptr;
}

auto PipelineLayout::c_ptr() const -> CType
{
    return *(this->_layout);
}

} // namespace vk
} // namespace pr
