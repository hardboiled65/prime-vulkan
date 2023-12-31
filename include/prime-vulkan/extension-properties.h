#ifndef _PRIME_VULKAN_EXTENSION_PROPERTIES_H
#define _PRIME_VULKAN_EXTENSION_PROPERTIES_H

#include <vulkan/vulkan.h>

#include <primer/vector.h>
#include <primer/string.h>

namespace pr {
namespace vk {

class ExtensionProperties
{
public:
    static Vector<ExtensionProperties> enumerate();

    pr::String extension_name() const;
    uint32_t spec_version() const;

private:
    ExtensionProperties();

private:
    pr::String _extension_name;
    uint32_t _spec_version;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_EXTENSION_PROPERTIES_H
