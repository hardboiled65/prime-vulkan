#ifndef _PRIME_VULKAN_SHADER_MODULE_H
#define _PRIME_VULKAN_SHADER_MODULE_H

#include <vulkan/vulkan.h>

#include <memory>

namespace pr {
namespace vk {

class Device;

class VkShaderModule
{
    friend Device;
public:
    class CreateInfo
    {
    public:
        CreateInfo();

        void set_code(const uint32_t *code, uint64_t size);

        ::VkShaderModuleCreateInfo c_struct() const;

    private:
        ::VkShaderModuleCreateInfo _info;
    };

    class Deleter
    {
    public:
        Deleter(::VkDevice p_device)
        {
            this->_p_device = p_device;
        }

        void operator()(::VkShaderModule *shader_module)
        {
            vkDestroyShaderModule(this->_p_device, *shader_module, nullptr);
        }

    private:
        ::VkDevice _p_device;
    };

public:
    ::VkShaderModule c_ptr() const;

private:
    VkShaderModule();

private:
    std::shared_ptr<::VkShaderModule> _shader_module;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_SHADER_MODULE_H
