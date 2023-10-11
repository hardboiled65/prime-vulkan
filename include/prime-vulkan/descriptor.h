#ifndef _PRIME_VULKAN_DESCRIPTOR_H
#define _PRIME_VULKAN_DESCRIPTOR_H

#include <vulkan/vulkan.h>

#include <memory>

#include <primer/vector.h>

namespace pr {
namespace vk {

class Device;

/// \brief Wrapper class for `VkDescriptorSetLayout`.
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
    CType c_ptr() const;

private:
    DescriptorSetLayout();

private:
    std::shared_ptr<CType> _layout;
};


/// \brief Wrapper class for `VkDescriptorPool`.
class DescriptorPool
{
    friend Device;
public:
    using CType = VkDescriptorPool;

public:
    /// \brief Wrapper class for `VkDescriptorPoolSize`.
    class Size
    {
    public:
        using CType = VkDescriptorPoolSize;

    public:
        Size();

        void set_type(VkDescriptorType type);

        void set_descriptor_count(uint32_t count);

        CType c_struct() const;

    private:
        CType _size;
    };

    /// \brief Wrapper class for `VkDescriptorPoolCreateInfo`.
    class CreateInfo
    {
    public:
        using CType = VkDescriptorPoolCreateInfo;

    public:
        CreateInfo();

        void set_pool_sizes(const pr::Vector<DescriptorPool::Size>& sizes);

        void set_max_sets(uint32_t sets);

        CType c_struct() const;

    private:
        CType _info;

        pr::Vector<DescriptorPool::Size::CType> _sizes;
    };

    class Deleter
    {
    public:
        Deleter(VkDevice p_device)
        {
            this->_p_device = p_device;
        }

        void operator()(CType *pool)
        {
            vkDestroyDescriptorPool(this->_p_device, *pool, nullptr);
        }

    private:
        VkDevice _p_device;
    };

public:
    CType c_ptr() const;

private:
    DescriptorPool();

private:
    std::shared_ptr<CType> _pool;
};


/// \brief Wrapper class for `VkDescriptorSet`.
class DescriptorSet
{
    friend Device;
public:
    using CType = VkDescriptorSet;

public:
    /// \brief Wrapper class for `VkDescriptorSetAllocateInfo`.
    class AllocateInfo
    {
    public:
        using CType = VkDescriptorSetAllocateInfo;

    public:
        AllocateInfo();

        void set_descriptor_pool(const DescriptorPool& pool);

        void set_descriptor_set_count(uint32_t count);

        void set_set_layouts(const pr::Vector<DescriptorSetLayout>& layouts);

        CType c_struct() const;

    private:
        CType _info;

        pr::Vector<DescriptorSetLayout::CType> _set_layouts;
    };

public:
    CType c_ptr() const;

private:
    DescriptorSet();

private:
    std::shared_ptr<DescriptorSet::CType> _set;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_DESCRIPTOR_H
