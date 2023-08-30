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

namespace pr {
namespace vk {

class VkPhysicalDevice;

class VkDevice
{
    friend VkPhysicalDevice;
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
    VkDevice(const VkDevice& other);

    /// Using `vkGetDeviceQueue`.
    VkQueue queue_for(uint32_t queue_family_index, uint32_t queue_index) const;

    VkSwapchain create_swapchain(const VkSwapchain::CreateInfo& info) const;

    Vector<VkImage> images_for(const VkSwapchain& swapchain) const;

    VkImageView create_image_view(const VkImageView::CreateInfo& info) const;

    VkShaderModule
    create_shader_module(const VkShaderModule::CreateInfo& info) const;

    /// Create a pipeline layout.
    VkPipelineLayout
    create_pipeline_layout(const VkPipelineLayout::CreateInfo& info) const;

    RenderPass create_render_pass(const RenderPass::CreateInfo& info) const;

    Framebuffer create_framebuffer(const Framebuffer::CreateInfo& info) const;

    CommandPool create_command_pool(const CommandPool::CreateInfo& info) const;

    ::VkDevice c_ptr();

private:
    VkDevice();

private:
    ::VkDevice _device;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_DEVICE_H
