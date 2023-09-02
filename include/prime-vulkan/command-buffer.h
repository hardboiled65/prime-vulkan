#ifndef _PRIME_VULKAN_COMMAND_BUFFER_H
#define _PRIME_VULKAN_COMMAND_BUFFER_H

#include <vulkan/vulkan.h>

#include <prime-vulkan/render-pass.h>

namespace pr {
namespace vk {

class CommandPool;

class Pipeline;

class Device;

class CommandBuffer
{
    friend Device;
public:
    using CType = ::VkCommandBuffer;

    class AllocateInfo
    {
    public:
        using CType = ::VkCommandBufferAllocateInfo;

    public:
        AllocateInfo();

        void set_command_pool(const CommandPool& command_pool);

        void set_level(::VkCommandBufferLevel level);

        void set_command_buffer_count(uint32_t count);

        CType c_struct() const;

    private:
        CType _info;
    };

    class BeginInfo
    {
    public:
        using CType = ::VkCommandBufferBeginInfo;

    public:
        BeginInfo();

        CType c_struct() const;

    private:
        CType _info;
    };

public:
    void begin(const BeginInfo& info);

    void reset(::VkCommandBufferResetFlags flags);

    void begin_render_pass(const RenderPass::BeginInfo& info,
                           ::VkSubpassContents contents);

    void bind_pipeline(::VkPipelineBindPoint bind_point,
                       const Pipeline& pipeline);

    void set_viewport(uint32_t first_viewport,
                      const pr::Vector<::VkViewport>& viewports);

    void set_scissor(uint32_t first_scissor,
                     const pr::Vector<::VkRect2D>& scissors);

    void draw(uint32_t vertex_count,
              uint32_t instance_count,
              uint32_t first_vertex,
              uint32_t first_instance);

    void end_render_pass();

    /// Finish recording a command buffer.
    void end();

    CType c_ptr() const;

private:
    CommandBuffer();

private:
    CType _command_buffer;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_COMMAND_BUFFER_H
