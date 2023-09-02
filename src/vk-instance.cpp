#include <prime-vulkan/instance.h>

#include <stdio.h>

#include <prime-vulkan/base.h>

namespace pr {
namespace vk {

LayerProperties::LayerProperties()
{
}

pr::Vector<LayerProperties> LayerProperties::enumerate()
{
    pr::Vector<LayerProperties> v;

    uint32_t count;
    vkEnumerateInstanceLayerProperties(&count, nullptr);

    LayerProperties::CType *p = new CType[count];
    vkEnumerateInstanceLayerProperties(&count, p);

    for (uint32_t i = 0; i < count; ++i) {
        LayerProperties properties;
        properties._layerName = p[i].layerName;
        properties._specVersion = p[i].specVersion;
        properties._implementationVersion = p[i].implementationVersion;
        properties._description = p[i].description;

        v.push(properties);
    }

    delete[] p;

    return v;
}

pr::String LayerProperties::layer_name() const
{
    return this->_layerName;
}

uint32_t LayerProperties::spec_version() const
{
    return this->_specVersion;
}

uint32_t LayerProperties::implementation_version() const
{
    return this->_implementationVersion;
}

pr::String LayerProperties::description() const
{
    return this->_description;
}


Instance::CreateInfo::CreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    this->_info.pApplicationInfo = nullptr;
    this->_info.enabledExtensionCount = 0;
    this->_info.enabledLayerCount = 0;
    this->_info.ppEnabledExtensionNames = nullptr;
    this->_info.ppEnabledLayerNames = nullptr;
    this->_info.pNext = nullptr;
    this->_info.flags = 0;
}

Instance::CreateInfo::~CreateInfo()
{
    if (this->_pp_enabled_extension_names != nullptr) {
        delete[] this->_pp_enabled_extension_names;
    }
}

void Instance::CreateInfo::set_enabled_extension_names(
    const pr::Vector<pr::String>& names)
{
    this->_enabled_extension_names = names;

    this->_info.enabledExtensionCount = names.length();

    // Make compatible.
    this->_pp_enabled_extension_names = new const char*[names.length()];
    for (uint64_t i = 0; i < names.length(); ++i) {
        this->_pp_enabled_extension_names[i] =
            this->_enabled_extension_names[i].c_str();
    }
    this->_info.ppEnabledExtensionNames = this->_pp_enabled_extension_names;
}

struct VkInstanceDeleter
{
    void operator()(::VkInstance *instance)
    {
        vkDestroyInstance(*instance, nullptr);
    }
};

Instance::Instance(const Instance::CreateInfo& info)
{
    ::VkInstance instance;
    VkResult result = vkCreateInstance(&(info._info),
        nullptr,
        &instance);

    if (result == VK_SUCCESS) {
        this->_instance = std::shared_ptr<::VkInstance>(
            new ::VkInstance(instance),
            VkInstanceDeleter());
    } else {
        throw VulkanError(result);
    }
}

Instance::~Instance()
{
    // Delete _instance.
}

Surface Instance::create_wayland_surface(
    const Surface::WaylandSurfaceCreateInfo& info
) const
{
    Surface surface;

    ::VkWaylandSurfaceCreateInfoKHR vk_info = info.c_struct();

    Surface::CType vk_surface;
    ::VkResult result = vkCreateWaylandSurfaceKHR(*(this->_instance),
        &vk_info, nullptr, &vk_surface);

    if (result != VK_SUCCESS) {
        throw VulkanError(result);
    }

    surface._surface = vk_surface;

    return surface;
}

::VkInstance Instance::c_ptr()
{
    return *(this->_instance);
}

} // namespace vk
} // namespace pr
