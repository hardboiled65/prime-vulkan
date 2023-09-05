#ifndef _PRIME_VULKAN_MEMORY_H
#define _PRIME_VULKAN_MEMORY_H

#include <vulkan/vulkan.h>

#include <memory>

namespace pr {
namespace vk {

class Device;

class MemoryRequirements
{
    friend Device;
public:
    using CType = ::VkMemoryRequirements;

public:
    ::VkDeviceSize size() const;

    ::VkDeviceSize alignment() const;

    uint32_t memory_type_bits() const;

    CType c_struct() const;

private:
    MemoryRequirements();

private:
    CType _requirements;
};


class MemoryAllocateInfo
{
    friend Device;
public:
    using CType = ::VkMemoryAllocateInfo;

public:
    MemoryAllocateInfo();

    void set_allocation_size(::VkDeviceSize size);

    void set_memory_type_index(uint32_t index);

    CType c_struct() const;

private:
    CType _info;
};


class DeviceMemory
{
    friend Device;
public:
    using CType = ::VkDeviceMemory;

    class Deleter
    {
    public:
        Deleter(::VkDevice p_device)
        {
            this->_p_device = p_device;
        }

        void operator()(CType *memory)
        {
            vkFreeMemory(this->_p_device, *memory, nullptr);
        }

    private:
        ::VkDevice _p_device;
    };

public:
    CType c_ptr() const;

private:
    DeviceMemory();

private:
    std::shared_ptr<CType> _memory;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_MEMORY_H
