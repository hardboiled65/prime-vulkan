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
