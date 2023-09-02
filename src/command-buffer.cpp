#include <prime-vulkan/command-buffer.h>

#include <vector>

#include <prime-vulkan/base.h>
#include <prime-vulkan/command-pool.h>
#include <prime-vulkan/pipeline.h>

namespace pr {
namespace vk {

CommandBuffer::AllocateInfo::AllocateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    this->_info.pNext = nullptr;
}

void CommandBuffer::AllocateInfo::set_command_pool(
    const CommandPool& command_pool)
{
    this->_info.commandPool = command_pool.c_ptr();
}

void CommandBuffer::AllocateInfo::set_level(::VkCommandBufferLevel level)
{
    this->_info.level = level;
}

void CommandBuffer::AllocateInfo::set_command_buffer_count(uint32_t count)
{
    this->_info.commandBufferCount = count;
}

auto CommandBuffer::AllocateInfo::c_struct() const -> CType
{
    return this->_info;
}


CommandBuffer::BeginInfo::BeginInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    this->_info.pInheritanceInfo = nullptr;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

auto CommandBuffer::BeginInfo::c_struct() const -> CType
{
    return this->_info;
}


CommandBuffer::CommandBuffer()
{
    this->_command_buffer = nullptr;
}

void CommandBuffer::begin(const CommandBuffer::BeginInfo& info)
{
    ::VkResult result;

    BeginInfo::CType vk_info = info.c_struct();
    result = vkBeginCommandBuffer(this->_command_buffer, &vk_info);
    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }
}

void CommandBuffer::reset(::VkCommandBufferResetFlags flags)
{
    ::VkResult result;

    result = vkResetCommandBuffer(this->_command_buffer, flags);
    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }
}

void CommandBuffer::begin_render_pass(const RenderPass::BeginInfo& info,
                                      ::VkSubpassContents contents)
{
    auto vk_info = info.c_struct();
    vkCmdBeginRenderPass(this->_command_buffer, &vk_info, contents);
}

void CommandBuffer::bind_pipeline(::VkPipelineBindPoint bind_point,
                                  const Pipeline& pipeline)
{
    vkCmdBindPipeline(this->_command_buffer,
        bind_point,
        pipeline.c_ptr());
}

void CommandBuffer::set_viewport(uint32_t first_viewport,
                  const pr::Vector<::VkViewport>& viewports)
{
    uint32_t count = viewports.length();

    std::vector<::VkViewport> vk_viewports;
    for (auto& viewport: viewports) {
        vk_viewports.push_back(viewport);
    }

    vkCmdSetViewport(this->_command_buffer,
        first_viewport, count, vk_viewports.data());
}

void CommandBuffer::set_scissor(uint32_t first_scissor,
                                const pr::Vector<::VkRect2D>& scissors)
{
    uint32_t count = scissors.length();

    std::vector<::VkRect2D> vk_scissors;
    for (auto& scissor: scissors) {
        vk_scissors.push_back(scissor);
    }

    vkCmdSetScissor(this->_command_buffer,
        first_scissor, count, vk_scissors.data());
}

void CommandBuffer::draw(uint32_t vertex_count,
                         uint32_t instance_count,
                         uint32_t first_vertex,
                         uint32_t first_instance)
{
    vkCmdDraw(this->_command_buffer,
        vertex_count, instance_count, first_vertex, first_instance);
}

void CommandBuffer::end_render_pass()
{
    vkCmdEndRenderPass(this->_command_buffer);
}

void CommandBuffer::end()
{
    ::VkResult result = vkEndCommandBuffer(this->_command_buffer);
    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }
}

auto CommandBuffer::c_ptr() const -> CType
{
    return this->_command_buffer;
}

} // namespace vk
} // namespace pr
