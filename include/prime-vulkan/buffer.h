#ifndef _PRIME_VULKAN_BUFFER_H
#define _PRIME_VULKAN_BUFFER_H

#include <vulkan/vulkan.h>

#include <memory>

namespace pr {
namespace vk {

class Device;

class Buffer
{
    friend Device;
public:
    using CType = ::VkBuffer;

    class CreateInfo
    {
    public:
        using CType = ::VkBufferCreateInfo;

    public:
        CreateInfo();

        void set_size(::VkDeviceSize size);

        void set_usage(::VkBufferUsageFlags usage);

        void set_sharing_mode(::VkSharingMode mode);

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

        void operator()(CType *buffer)
        {
            vkDestroyBuffer(this->_p_device, *buffer, nullptr);
        }

    private:
        ::VkDevice _p_device;
    };

public:
    CType c_ptr() const;

private:
    Buffer();

private:
    std::shared_ptr<CType> _buffer;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_BUFFER_H
