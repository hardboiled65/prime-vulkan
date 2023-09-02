#ifndef _PRIME_VULKAN_SEMAPHORE_H
#define _PRIME_VULKAN_SEMAPHORE_H

#include <vulkan/vulkan.h>

#include <memory>

namespace pr {
namespace vk {

class Device;

class Semaphore
{
    friend Device;
public:
    using CType = ::VkSemaphore;

    class CreateInfo
    {
    public:
        using CType = ::VkSemaphoreCreateInfo;

    public:
        CreateInfo();

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

        void operator()(CType *semaphore)
        {
            vkDestroySemaphore(this->_p_device, *semaphore, nullptr);
        }

    private:
        ::VkDevice _p_device;
    };

public:
    CType c_ptr() const;

private:
    Semaphore();

private:
    std::shared_ptr<CType> _semaphore;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_SEMAPHORE_H
