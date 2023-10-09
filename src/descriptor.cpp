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

auto DescriptorSetLayout::c_struct() const -> CType
{
    return *(this->_layout);
}

} // namespace vk
} // namespace pr
