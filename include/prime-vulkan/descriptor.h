#ifndef _PRIME_VULKAN_DESCRIPTOR_H
#define _PRIME_VULKAN_DESCRIPTOR_H

#include <vulkan/vulkan.h>

#include <memory>

#include <primer/vector.h>

namespace pr {
namespace vk {

class Device;

class DescriptorSetLayout
{
    friend Device;
public:
    using CType = VkDescriptorSetLayout;

public:
    class Binding
    {
    public:
        using CType = VkDescriptorSetLayoutBinding;

    public:
        Binding();

        void set_binding(uint32_t binding);

        void set_descriptor_type(VkDescriptorType type);

        void set_descriptor_count(uint32_t count);

        void set_stage_flags(VkShaderStageFlags flags);

        CType c_struct() const;

    private:
        CType _binding;
    };

    class CreateInfo
    {
    public:
        using CType = VkDescriptorSetLayoutCreateInfo;

    public:
        CreateInfo();

        void set_bindings(
            const pr::Vector<DescriptorSetLayout::Binding>& bindings);

        CType c_struct() const;

    private:
        CType _info;

        pr::Vector<VkDescriptorSetLayoutBinding> _bindings;
    };

    class Deleter
    {
    public:
        Deleter(VkDevice p_device)
        {
            this->_p_device = p_device;
        }

        void operator()(CType *set_layout)
        {
            vkDestroyDescriptorSetLayout(this->_p_device, *set_layout, nullptr);
        }

    private:
        VkDevice _p_device;
    };

public:
    CType c_struct() const;

private:
    DescriptorSetLayout();

private:
    std::shared_ptr<CType> _layout;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_DESCRIPTOR_H
