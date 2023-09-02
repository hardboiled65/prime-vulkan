#ifndef _PRIME_VULKAN_COMMAND_POOL_H
#define _PRIME_VULKAN_COMMAND_POOL_H

#include <vulkan/vulkan.h>

#include <memory>

namespace pr {
namespace vk {

class Device;

class CommandPool
{
    friend Device;
public:
    using CType = ::VkCommandPool;

    class CreateInfo
    {
    public:
        using CType = ::VkCommandPoolCreateInfo;

    public:
        CreateInfo();

        void set_queue_family_index(uint32_t index);

        void set_flags(::VkCommandPoolCreateFlags flags);

        CType c_struct() const;

    private:
        CType _info;
    };

    class Deleter
    {
    public:
        Deleter(::VkDevice p_device)
        {
            this->_p_device = p_device;
        }

        void operator()(CType *command_pool)
        {
            vkDestroyCommandPool(this->_p_device, *command_pool, nullptr);
        }

    private:
        ::VkDevice _p_device;
    };

public:
    CType c_ptr() const;

private:
    CommandPool();

private:
    std::shared_ptr<CType> _command_pool;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_COMMAND_POOL_H
