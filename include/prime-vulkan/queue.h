#ifndef _PRIME_VULKAN_QUEUE_H
#define _PRIME_VULKAN_QUEUE_H

#include <vulkan/vulkan.h>

#include <vector>

#include <primer/vector.h>

#include <prime-vulkan/semaphore.h>
#include <prime-vulkan/command-buffer.h>

namespace pr {
namespace vk {

class SubmitInfo;

class Fence;

class VkDevice;

class VkQueue
{
    friend VkDevice;
public:
    using CType = ::VkQueue;

public:
    void submit(const pr::Vector<SubmitInfo>& submits, const Fence& fence);

    ::VkQueue c_ptr() const;

private:
    VkQueue();

private:
    ::VkQueue _queue;
};


class SubmitInfo
{
public:
    using CType = ::VkSubmitInfo;

public:
    SubmitInfo();

    void set_wait_semaphores(const pr::Vector<Semaphore>& semaphores);

    void set_wait_dst_stage_mask(
        const pr::Vector<::VkPipelineStageFlags>& mask);

    void set_command_buffers(const pr::Vector<CommandBuffer>& command_buffers);

    void set_signal_semaphores(const pr::Vector<Semaphore>& semaphores);

    CType c_struct() const;

private:
    CType _info;

    std::vector<Semaphore::CType> _wait_semaphores;
    std::vector<::VkPipelineStageFlags> _wait_dst_stage_mask;
    std::vector<CommandBuffer::CType> _command_buffers;
    std::vector<Semaphore::CType> _signal_semaphores;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_QUEUE_H
