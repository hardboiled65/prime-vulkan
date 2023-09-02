#include <prime-vulkan/shader-module.h>

namespace pr {
namespace vk {

ShaderModule::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    this->_info.flags = 0;
    this->_info.pNext = nullptr;
}

void ShaderModule::CreateInfo::set_code(const uint32_t *code, uint64_t size)
{
    this->_info.pCode = code;
    this->_info.codeSize = size;
}

::VkShaderModuleCreateInfo ShaderModule::CreateInfo::c_struct() const
{
    return this->_info;
}


ShaderModule::ShaderModule()
{
    this->_shader_module = nullptr;
}

::VkShaderModule ShaderModule::c_ptr() const
{
    return *(this->_shader_module);
}

} // namespace vk
} // namespace pr
