#ifndef _PRIME_VULKAN_SHADER_MODULE_H
#define _PRIME_VULKAN_SHADER_MODULE_H

#include <vulkan/vulkan.h>

namespace pr {
namespace vk {

class VkShaderModule
{
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

private:

private:
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_SHADER_MODULE_H
