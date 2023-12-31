#ifndef _PRIME_VULKAN_DEVICE_H
#define _PRIME_VULKAN_DEVICE_H

#include <vulkan/vulkan.h>

#include <primer/vector.h>
#include <primer/string.h>

#include <prime-vulkan/queue.h>
#include <prime-vulkan/swapchain.h>
#include <prime-vulkan/shader-module.h>
#include <prime-vulkan/pipeline.h>
#include <prime-vulkan/render-pass.h>
#include <prime-vulkan/framebuffer.h>
#include <prime-vulkan/command-pool.h>
#include <prime-vulkan/command-buffer.h>
#include <prime-vulkan/semaphore.h>
#include <prime-vulkan/fence.h>
#include <prime-vulkan/buffer.h>
#include <prime-vulkan/memory.h>
#include <prime-vulkan/descriptor.h>

namespace pr {
namespace vk {

class PhysicalDevice;

class Device
{
    friend PhysicalDevice;
public:
    using CType = ::VkDevice;

public:
    class QueueCreateInfo
    {
    public:
        QueueCreateInfo();

        QueueCreateInfo(const QueueCreateInfo& other);

        ~QueueCreateInfo();

        void set_queue_count(uint32_t count);

        void set_queue_family_index(uint32_t index);

        void set_queue_priorities(const Vector<float>& priorities);

        ::VkDeviceQueueCreateInfo c_struct() const;

    private:
        ::VkDeviceQueueCreateInfo _info;

        float *_priorities;
    };

    class CreateInfo
    {
    public:
        CreateInfo();

        ~CreateInfo();

        // void set_queue_create_info_count(uint32_t count);

        /// Set pQueueCreateInfos field. queueCreateInfoCount automatically set.
        void set_queue_create_infos(const Vector<QueueCreateInfo>& infos);

        void set_enabled_features(::VkPhysicalDeviceFeatures features);

        void set_enabled_extension_names(const Vector<String>& names);

        ::VkDeviceCreateInfo c_struct() const;

    private:
        ::VkDeviceCreateInfo _info;

        ::VkDeviceQueueCreateInfo *_queue_create_infos;
        ::VkPhysicalDeviceFeatures _enabled_features;
        Vector<String> _enabled_extension_names;
        const char **_pp_enabled_extension_names;
    };

public:
    Device(const Device& other);

    /// Using `vkGetDeviceQueue`.
    Queue queue_for(uint32_t queue_family_index, uint32_t queue_index) const;

    Swapchain create_swapchain(const Swapchain::CreateInfo& info) const;

    Vector<Image> images_for(const Swapchain& swapchain) const;

    ImageView create_image_view(const ImageView::CreateInfo& info) const;

    ShaderModule
    create_shader_module(const ShaderModule::CreateInfo& info) const;

    /// Create a pipeline layout.
    PipelineLayout
    create_pipeline_layout(const PipelineLayout::CreateInfo& info) const;

    /// `vkCreateGraphicsPipelines`.
    pr::Vector<Pipeline> create_graphics_pipelines(
        const pr::Vector<GraphicsPipelineCreateInfo>& infos) const;

    RenderPass create_render_pass(const RenderPass::CreateInfo& info) const;

    Framebuffer create_framebuffer(const Framebuffer::CreateInfo& info) const;

    CommandPool create_command_pool(const CommandPool::CreateInfo& info) const;

    CommandBuffer
    allocate_command_buffers(const CommandBuffer::AllocateInfo& info) const;

    Semaphore create_semaphore(const Semaphore::CreateInfo& info) const;

    Fence create_fence(const Fence::CreateInfo& info) const;

    Buffer create_buffer(const Buffer::CreateInfo& info) const;

    DescriptorSetLayout
    create_descriptor_set_layout(
        const DescriptorSetLayout::CreateInfo& info) const;

    DescriptorPool create_descriptor_pool(
        const DescriptorPool::CreateInfo& info) const;

    pr::Vector<DescriptorSet> allocate_descriptor_sets(
        const DescriptorSet::AllocateInfo& info) const;

    void wait_for_fences(const Vector<Fence>& fences,
                         bool wait_all,
                         uint64_t timeout) const;

    void reset_fences(const Vector<Fence>& fences) const;

    /// Call `vkAcquireNextImageKHR` function without a fence.
    uint32_t acquire_next_image(const Swapchain& swapchain,
                                uint64_t timeout,
                                const Semaphore& semaphore) const;

    MemoryRequirements memory_requirements_for(const Buffer& buffer) const;

    DeviceMemory allocate_memory(const MemoryAllocateInfo& info) const;

    /// Bind device memory to a buffer object.
    void bind_buffer_memory(Buffer& buffer,
                            DeviceMemory& memory,
                            ::VkDeviceSize offset);
    /// Alias to `bind_buffer_memory`.
    void bind_memory_to_buffer(Buffer&, DeviceMemory&, ::VkDeviceSize);

    void map_memory(DeviceMemory& memory,
                    ::VkDeviceSize offset,
                    ::VkDeviceSize size,
                    ::VkMemoryMapFlags flags,
                    void **data);

    void unmap_memory(DeviceMemory& memory);

    void wait_idle();

    CType c_ptr();

private:
    Device();

private:
    CType _device;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_DEVICE_H
