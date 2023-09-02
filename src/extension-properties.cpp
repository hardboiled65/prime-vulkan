#include <prime-vulkan/extension-properties.h>

namespace pr {
namespace vk {

ExtensionProperties::ExtensionProperties()
{
}

Vector<ExtensionProperties> ExtensionProperties::enumerate()
{
    Vector<ExtensionProperties> v;

    uint32_t count;
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

    ::VkExtensionProperties *props = new ::VkExtensionProperties[count];

    vkEnumerateInstanceExtensionProperties(nullptr, &count, props);

    for (uint32_t i = 0; i < count; ++i) {
        ExtensionProperties properties;
        properties._extension_name = props[i].extensionName;
        properties._spec_version = props[i].specVersion;
        v.push(properties);
    }

    delete[] props;

    return v;
}

pr::String ExtensionProperties::extension_name() const
{
    return this->_extension_name;
}

uint32_t ExtensionProperties::spec_version() const
{
    return this->_spec_version;
}

} // namespace vk
} // namespace pr
