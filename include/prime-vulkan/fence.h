#ifndef _PRIME_VULKAN_FENCE_H
#define _PRIME_VULKAN_FENCE_H

#include <vulkan/vulkan.h>

#include <memory>

namespace pr {
namespace vk {

class Device;

class Fence
{
    friend Device;
public:
    using CType = ::VkFence;

    class CreateInfo
    {
    public:
        using CType = ::VkFenceCreateInfo;

    public:
        CreateInfo();

        void set_flags(::VkFenceCreateFlags flags);

        CType c_struct() const;

    private:
        CType _info;
    };

    class Deleter
    {
    public:
        Deleter() = delete;

        Deleter(::VkDevice p_device)
        {
            this->_p_device = p_device;
        }

        void operator()(CType *fence)
        {
            vkDestroyFence(this->_p_device, *fence, nullptr);
        }

    private:
        ::VkDevice _p_device;
    };

public:
    CType c_ptr() const;

private:
    Fence();

private:
    std::shared_ptr<CType> _fence;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_FENCE_H
