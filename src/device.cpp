#include <prime-vulkan/device.h>

#include <prime-vulkan/base.h>

namespace pr {
namespace vk {

Device::QueueCreateInfo::QueueCreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    this->_info.flags = 0;
    this->_info.pNext = nullptr;
    this->_info.pQueuePriorities = nullptr;
    this->_info.queueCount = 0;
    this->_info.queueFamilyIndex = 0;

    this->_priorities = nullptr;
}

Device::QueueCreateInfo::QueueCreateInfo(const QueueCreateInfo& other)
{
    this->_info = other._info;

    this->_priorities = new float[this->_info.queueCount];
    for (uint32_t i = 0; i < this->_info.queueCount; ++i) {
        this->_priorities[i] = other._priorities[i];
    }

    this->_info.pQueuePriorities = this->_priorities;
}

Device::QueueCreateInfo::~QueueCreateInfo()
{
    if (this->_priorities != nullptr) {
        delete[] this->_priorities;
    }
}

void Device::QueueCreateInfo::set_queue_count(uint32_t count)
{
    this->_info.queueCount = count;
}

void Device::QueueCreateInfo::set_queue_family_index(uint32_t index)
{
    this->_info.queueFamilyIndex = index;
}

void Device::QueueCreateInfo::set_queue_priorities(const Vector<float>& priorities)
{
    if (this->_priorities != nullptr) {
        delete[] this->_priorities;
    }

    this->_priorities = new float[priorities.length()];
    for (uint64_t i = 0; i < priorities.length(); ++i) {
        this->_priorities[i] = priorities[i];
    }

    this->_info.pQueuePriorities = this->_priorities;
}

::VkDeviceQueueCreateInfo Device::QueueCreateInfo::c_struct() const
{
    return this->_info;
}


Device::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    this->_info.queueCreateInfoCount = 0;
    this->_info.pQueueCreateInfos = nullptr;

    this->_info.pEnabledFeatures = nullptr;

    this->_info.enabledExtensionCount = 0;
    this->_info.ppEnabledExtensionNames = nullptr;

    // Zero or null.
    this->_info.enabledLayerCount = 0;
    this->_info.ppEnabledLayerNames = nullptr;
    this->_info.flags = 0;
    this->_info.pNext = nullptr;

    this->_queue_create_infos = nullptr;
    this->_pp_enabled_extension_names = nullptr;
}

Device::CreateInfo::~CreateInfo()
{
    if (this->_queue_create_infos != nullptr) {
        delete[] this->_queue_create_infos;
    }
    if (this->_pp_enabled_extension_names != nullptr) {
        delete[] this->_pp_enabled_extension_names;
    }
}

void Device::CreateInfo::set_queue_create_infos(
    const Vector<QueueCreateInfo>& infos)
{
    this->_info.queueCreateInfoCount = infos.length();

    this->_queue_create_infos = new ::VkDeviceQueueCreateInfo[infos.length()];
    for (uint64_t i = 0; i < infos.length(); ++i) {
        this->_queue_create_infos[i] = infos[i].c_struct();
    }

    this->_info.pQueueCreateInfos = this->_queue_create_infos;
}

void Device::CreateInfo::set_enabled_features(
    ::VkPhysicalDeviceFeatures features)
{
    this->_enabled_features = features;

    this->_info.pEnabledFeatures = &(this->_enabled_features);
}

void Device::CreateInfo::set_enabled_extension_names(
    const Vector<String>& names)
{
    this->_info.enabledExtensionCount = names.length();

    this->_enabled_extension_names = names;
    this->_pp_enabled_extension_names = new const char*[names.length()];
    for (uint64_t i = 0; i < names.length(); ++i) {
        this->_pp_enabled_extension_names[i] = names[i].c_str();
        // TODO: not name[i] but this->_enabled_extension_names[i].
    }

    this->_info.ppEnabledExtensionNames = this->_pp_enabled_extension_names;
}

::VkDeviceCreateInfo Device::CreateInfo::c_struct() const
{
    return this->_info;
}


Device::Device()
{
}

Device::Device(const Device& other)
{
    this->_device = other._device;
}

Queue Device::queue_for(uint32_t queue_family_index,
                          uint32_t queue_index) const
{
    ::VkQueue queue;
    vkGetDeviceQueue(this->_device, queue_family_index, queue_index, &queue);

    Queue ret;
    ret._queue = queue;

    return ret;
}

Swapchain Device::create_swapchain(
    const Swapchain::CreateInfo& info) const
{
    ::VkResult result;
    ::VkSwapchainKHR vk_swapchain;

    auto create_info = info.c_struct();
    result = vkCreateSwapchainKHR(this->_device, &create_info,
        nullptr, &vk_swapchain);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }
    Swapchain swapchain;
    swapchain._swapchain = std::shared_ptr<Swapchain::CType>(
        new Swapchain::CType(vk_swapchain),
        Swapchain::Deleter(this->_device));

    return swapchain;
}

Vector<Image> Device::images_for(const Swapchain& swapchain) const
{
    Vector<Image> v;

    ::VkResult result;

    uint32_t count;
    ::VkImage *vk_images;

    result = vkGetSwapchainImagesKHR(this->_device,
        swapchain.c_ptr(), &count,
        nullptr);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    vk_images = new ::VkImage[count];

    result = vkGetSwapchainImagesKHR(this->_device,
        swapchain.c_ptr(), &count, vk_images);

    if (result != VK_SUCCESS) {
        delete[] vk_images;
        throw VulkanError(result);
    } else {
        for (uint64_t i = 0; i < count; ++i) {
            Image image;
            image._image = vk_images[i];
            v.push(image);
        }

        delete[] vk_images;
    }

    return v;
}

ImageView Device::create_image_view(
    const ImageView::CreateInfo& info) const
{
    ::VkResult result;

    ::VkImageViewCreateInfo vk_info = info.c_struct();
    ::VkImageView view;
    result = vkCreateImageView(this->_device, &vk_info, nullptr, &view);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    ImageView image_view;
    image_view._view = std::shared_ptr<::VkImageView>(
        new ::VkImageView(view), ImageView::Deleter(this->_device));

    return image_view;
}

ShaderModule Device::create_shader_module(
    const ShaderModule::CreateInfo& info) const
{
    ::VkResult result;

    ::VkShaderModuleCreateInfo vk_info = info.c_struct();
    ::VkShaderModule module;
    result = vkCreateShaderModule(this->_device, &vk_info, nullptr, &module);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    ShaderModule shader_module;
    shader_module._shader_module = std::shared_ptr<::VkShaderModule>(
        new ::VkShaderModule(module), ShaderModule::Deleter(this->_device));

    return shader_module;
}

PipelineLayout Device::create_pipeline_layout(
    const PipelineLayout::CreateInfo& info) const
{
    ::VkResult result;

    ::VkPipelineLayoutCreateInfo vk_info = info.c_struct();
    ::VkPipelineLayout c_layout;
    result = vkCreatePipelineLayout(this->_device,
        &vk_info, nullptr, &c_layout);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    PipelineLayout layout;
    layout._layout = std::shared_ptr<::VkPipelineLayout>(
        new ::VkPipelineLayout(c_layout),
        PipelineLayout::Deleter(this->_device));

    return layout;
}

pr::Vector<Pipeline> Device::create_graphics_pipelines(
    const pr::Vector<GraphicsPipelineCreateInfo>& infos) const
{
    pr::Vector<Pipeline> v;
    uint32_t count = infos.length();
    GraphicsPipelineCreateInfo::CType *vk_infos =
        new GraphicsPipelineCreateInfo::CType[count];
    for (uint32_t i = 0; i < count; ++i) {
        vk_infos[i] = infos[i].c_struct();
    }

    Pipeline::CType *vk_pipelines = new Pipeline::CType[count];
    ::VkResult result = vkCreateGraphicsPipelines(this->_device,
        nullptr, count, vk_infos, nullptr, vk_pipelines);

    if (result != VK_SUCCESS) {
        // Free memories.
        delete[] vk_pipelines;
        delete[] vk_infos;

        throw VulkanError(result);
    }

    for (uint32_t i = 0; i < count; ++i) {
        Pipeline pipeline;
        pipeline._pipeline = std::shared_ptr<Pipeline::CType>(
            new Pipeline::CType(vk_pipelines[i]));
        v.push(pipeline);
    }
    delete[] vk_pipelines;
    delete[] vk_infos;

    return v;
}

RenderPass Device::create_render_pass(
    const RenderPass::CreateInfo& info) const
{
    ::VkResult result;

    RenderPass::CreateInfo::CType vk_info = info.c_struct();
    RenderPass::CType c_render_pass;
    result = vkCreateRenderPass(this->_device, &vk_info, nullptr,
        &c_render_pass);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    RenderPass render_pass;
    render_pass._render_pass = std::shared_ptr<RenderPass::CType>(
        new RenderPass::CType(c_render_pass),
        RenderPass::Deleter(this->_device));

    return render_pass;
}

Framebuffer Device::create_framebuffer(
    const Framebuffer::CreateInfo& info) const
{
    ::VkResult result;

    Framebuffer::CreateInfo::CType vk_info = info.c_struct();
    Framebuffer::CType c_framebuffer;
    result = vkCreateFramebuffer(this->_device, &vk_info, nullptr,
        &c_framebuffer);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    Framebuffer framebuffer;
    framebuffer._framebuffer = std::shared_ptr<Framebuffer::CType>(
        new Framebuffer::CType(c_framebuffer),
        Framebuffer::Deleter(this->_device));

    return framebuffer;
}

CommandPool Device::create_command_pool(
    const CommandPool::CreateInfo& info) const
{
    ::VkResult result;

    CommandPool::CreateInfo::CType vk_info = info.c_struct();
    CommandPool::CType c_command_pool;
    result = vkCreateCommandPool(this->_device, &vk_info, nullptr,
        &c_command_pool);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    CommandPool command_pool;
    command_pool._command_pool = std::shared_ptr<CommandPool::CType>(
        new CommandPool::CType(c_command_pool),
        CommandPool::Deleter(this->_device));

    return command_pool;
}

CommandBuffer Device::allocate_command_buffers(
    const CommandBuffer::AllocateInfo& info) const
{
    ::VkResult result;

    CommandBuffer::AllocateInfo::CType vk_info = info.c_struct();
    CommandBuffer::CType c_command_buffer;
    result = vkAllocateCommandBuffers(this->_device, &vk_info,
        &c_command_buffer);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    CommandBuffer command_buffer;
    // TODO: shared_ptr with custom deleter, should I?
    command_buffer._command_buffer = c_command_buffer;

    return command_buffer;
}

Semaphore Device::create_semaphore(const Semaphore::CreateInfo& info) const
{
    ::VkResult result;

    Semaphore::CreateInfo::CType vk_info = info.c_struct();
    Semaphore::CType c_semaphore;
    result = vkCreateSemaphore(this->_device, &vk_info, nullptr, &c_semaphore);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    Semaphore semaphore;
    semaphore._semaphore = std::shared_ptr<Semaphore::CType>(
        new Semaphore::CType(c_semaphore),
        Semaphore::Deleter(this->_device));

    return semaphore;
}

Fence Device::create_fence(const Fence::CreateInfo& info) const
{
    ::VkResult result;

    Fence::CreateInfo::CType vk_info = info.c_struct();
    Fence::CType c_fence;
    result = vkCreateFence(this->_device, &vk_info, nullptr, &c_fence);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    Fence fence;
    fence._fence = std::shared_ptr<Fence::CType>(
        new Fence::CType(c_fence),
        Fence::Deleter(this->_device));

    return fence;
}

Buffer Device::create_buffer(const Buffer::CreateInfo& info) const
{
    ::VkResult result;

    Buffer::CreateInfo::CType vk_info = info.c_struct();
    Buffer::CType vk_buffer;
    result = vkCreateBuffer(this->_device, &vk_info, nullptr, &vk_buffer);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    Buffer buffer;
    buffer._buffer = std::shared_ptr<Buffer::CType>(
        new Buffer::CType(vk_buffer),
        Buffer::Deleter(this->_device));

    return buffer;
}

void Device::wait_for_fences(const Vector<Fence>& fences,
                               bool wait_all,
                               uint64_t timeout) const
{
    ::VkResult result;

    uint64_t count = fences.length();
    Fence::CType *vk_fences = new Fence::CType[count];
    for (uint64_t i = 0; i < count; ++i) {
        vk_fences[i] = fences[i].c_ptr();
    }

    ::VkBool32 vk_wait_all = (wait_all) ? VK_TRUE : VK_FALSE;

    result = vkWaitForFences(this->_device,
        count, vk_fences, vk_wait_all, timeout);

    delete[] vk_fences;

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }
}

void Device::reset_fences(const Vector<Fence>& fences) const
{
    ::VkResult result;

    uint64_t count = fences.length();
    Fence::CType *vk_fences = new Fence::CType[count];
    for (uint64_t i = 0; i < count; ++i) {
        vk_fences[i] = fences[i].c_ptr();
    }

    result = vkResetFences(this->_device, count, vk_fences);

    delete[] vk_fences;

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }
}

uint32_t Device::acquire_next_image(const Swapchain& swapchain,
                                      uint64_t timeout,
                                      const Semaphore& semaphore) const
{
    ::VkResult result;
    uint32_t index;

    result = vkAcquireNextImageKHR(this->_device, swapchain.c_ptr(), timeout,
        semaphore.c_ptr(), nullptr, &index);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    return index;
}

void Device::wait_idle()
{
    vkDeviceWaitIdle(this->_device);
    // TODO: Throw exception.
}

auto Device::c_ptr() -> CType
{
    return this->_device;
}

} // namespace vk
} // namespace pr
