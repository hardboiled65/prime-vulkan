#include <prime-vulkan/queue.h>

#include <assert.h>

#include <vector>

#include <prime-vulkan/base.h>
#include <prime-vulkan/fence.h>

namespace pr {
namespace vk {

Queue::Queue()
{
    this->_queue = nullptr;
}

void Queue::submit(const pr::Vector<SubmitInfo>& submits, const Fence& fence)
{
    ::VkResult result;

    uint32_t count = submits.length();

    std::vector<SubmitInfo::CType> vk_submits;
    for (uint32_t i = 0; i < count; ++i) {
        vk_submits.push_back(submits[i].c_struct());
    }

    result = vkQueueSubmit(this->_queue,
        count, vk_submits.data(), fence.c_ptr());

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }
}

void Queue::present(const PresentInfo& present_info)
{
    ::VkResult result;

    PresentInfo::CType vk_present_info = present_info.c_struct();

    result = vkQueuePresentKHR(this->_queue, &vk_present_info);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }
}

::VkQueue Queue::c_ptr() const
{
    return this->_queue;
}


SubmitInfo::SubmitInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    this->_info.pNext = nullptr;
}

void SubmitInfo::set_wait_semaphores(const pr::Vector<Semaphore>& semaphores)
{
    uint32_t count = semaphores.length();

    this->_info.waitSemaphoreCount = count;

    for (uint32_t i = 0; i < count; ++i) {
        this->_wait_semaphores.push_back(semaphores[i].c_ptr());
    }
    this->_info.pWaitSemaphores = this->_wait_semaphores.data();
}

void SubmitInfo::set_wait_dst_stage_mask(
    const pr::Vector<::VkPipelineStageFlags>& mask)
{
    uint32_t count = mask.length();

    assert(this->_info.waitSemaphoreCount == count);

    for (uint32_t i = 0; i < count; ++i) {
        this->_wait_dst_stage_mask.push_back(mask[i]);
    }
    this->_info.pWaitDstStageMask = this->_wait_dst_stage_mask.data();
}

void SubmitInfo::set_command_buffers(
    const pr::Vector<CommandBuffer>& command_buffers)
{
    uint32_t count = command_buffers.length();

    this->_info.commandBufferCount = count;

    for (uint32_t i = 0; i < count; ++i) {
        this->_command_buffers.push_back(command_buffers[i].c_ptr());
    }
    this->_info.pCommandBuffers = this->_command_buffers.data();
}

void SubmitInfo::set_signal_semaphores(const pr::Vector<Semaphore>& semaphores)
{
    uint32_t count = semaphores.length();

    this->_info.signalSemaphoreCount = count;

    for (uint32_t i = 0; i < count; ++i) {
        this->_signal_semaphores.push_back(semaphores[i].c_ptr());
    }
    this->_info.pSignalSemaphores = this->_signal_semaphores.data();
}

auto SubmitInfo::c_struct() const -> CType
{
    return this->_info;
}


PresentInfo::PresentInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    this->_info.pResults = nullptr;

    this->_info.pNext = nullptr;
}

void PresentInfo::set_wait_semaphores(const pr::Vector<Semaphore>& semaphores)
{
    this->_info.waitSemaphoreCount = semaphores.length();

    for (auto& semaphore: semaphores) {
        this->_wait_semaphores.push_back(semaphore.c_ptr());
    }
    this->_info.pWaitSemaphores = this->_wait_semaphores.data();
}

void PresentInfo::set_swapchains(const pr::Vector<VkSwapchain>& swapchains)
{
    this->_info.swapchainCount = swapchains.length();

    for (auto& swapchain: swapchains) {
        this->_swapchains.push_back(swapchain.c_ptr());
    }
    this->_info.pSwapchains = this->_swapchains.data();
}

void PresentInfo::set_image_indices(const pr::Vector<uint32_t>& indices)
{
    for (auto& index: indices) {
        this->_indices.push_back(index);
    }
    this->_info.pImageIndices = this->_indices.data();
}

auto PresentInfo::c_struct() const -> CType
{
    return this->_info;
}

} // namespace vk
} // namespace pr
