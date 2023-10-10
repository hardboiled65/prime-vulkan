#include <prime-vulkan/descriptor.h>

namespace pr {
namespace vk {

DescriptorSetLayout::Binding::Binding()
{
    this->_binding.pImmutableSamplers = nullptr;
}

void DescriptorSetLayout::Binding::set_binding(uint32_t binding)
{
    this->_binding.binding = binding;
}

void DescriptorSetLayout::Binding::set_descriptor_type(VkDescriptorType type)
{
    this->_binding.descriptorType = type;
}

void DescriptorSetLayout::Binding::set_descriptor_count(uint32_t count)
{
    this->_binding.descriptorCount = count;
}

void DescriptorSetLayout::Binding::set_stage_flags(VkShaderStageFlags flags)
{
    this->_binding.stageFlags = flags;
}

auto DescriptorSetLayout::Binding::c_struct() const -> CType
{
    return this->_binding;
}


DescriptorSetLayout::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void DescriptorSetLayout::CreateInfo::set_bindings(
    const pr::Vector<DescriptorSetLayout::Binding>& bindings)
{
    this->_info.bindingCount = bindings.length();
    for (auto& binding: bindings) {
        this->_bindings.push(binding.c_struct());
    }
    this->_info.pBindings = this->_bindings.c_ptr();
}

auto DescriptorSetLayout::CreateInfo::c_struct() const -> CType
{
    return this->_info;
}


DescriptorSetLayout::DescriptorSetLayout()
{
    this->_layout = nullptr;
}

auto DescriptorSetLayout::c_ptr() const -> CType
{
    return *(this->_layout);
}


DescriptorPool::Size::Size()
{
    this->_size.type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
    this->_size.descriptorCount = 0;
}

void DescriptorPool::Size::set_type(VkDescriptorType type)
{
    this->_size.type = type;
}

void DescriptorPool::Size::set_descriptor_count(uint32_t count)
{
    this->_size.descriptorCount = count;
}

auto DescriptorPool::Size::c_struct() const -> CType
{
    return this->_size;
}


DescriptorPool::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    this->_info.poolSizeCount = 0;
    this->_info.pPoolSizes = nullptr;
    this->_info.maxSets = 0;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void DescriptorPool::CreateInfo::set_pool_sizes(
    const pr::Vector<DescriptorPool::Size>& sizes)
{
    this->_info.poolSizeCount = sizes.length();

    for (auto& size: sizes) {
        this->_sizes.push(size.c_struct());
    }
    this->_info.pPoolSizes = this->_sizes.c_ptr();
}

void DescriptorPool::CreateInfo::set_max_sets(uint32_t sets)
{
    this->_info.maxSets = sets;
}

auto DescriptorPool::CreateInfo::c_struct() const -> CType
{
    return this->_info;
}


DescriptorPool::DescriptorPool()
{
    this->_pool = nullptr;
}

auto DescriptorPool::c_ptr() const -> CType
{
    return *(this->_pool);
}

} // namespace vk
} // namespace pr
