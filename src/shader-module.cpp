#include <prime-vulkan/shader-module.h>

namespace pr {
namespace vk {

VkShaderModule::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void VkShaderModule::CreateInfo::set_code(const uint32_t *code, uint64_t size)
{
    this->_info.pCode = code;
    this->_info.codeSize = size;
}

::VkShaderModuleCreateInfo VkShaderModule::CreateInfo::c_struct() const
{
    return this->_info;
}

} // namespace vk
} // namespace pr
