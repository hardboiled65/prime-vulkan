#ifndef _PRIME_VULKAN_INSTANCE_H
#define _PRIME_VULKAN_INSTANCE_H

#include <vulkan/vulkan.h>

#include <memory>

#include <primer/vector.h>
#include <primer/string.h>

#include <prime-vulkan/surface.h>

namespace pr {
namespace vk {

class LayerProperties
{
public:
    using CType = ::VkLayerProperties;

public:
    static pr::Vector<LayerProperties> enumerate();

    pr::String layer_name() const;

    uint32_t spec_version() const;

    uint32_t implementation_version() const;

    pr::String description() const;

private:
    LayerProperties();

private:
    pr::String _layerName;
    uint32_t _specVersion;
    uint32_t _implementationVersion;
    pr::String _description;
};


class VkInstance
{
public:
    class CreateInfo
    {
        friend VkInstance;

    public:
        CreateInfo();

        ~CreateInfo();

        void set_enabled_extension_names(const pr::Vector<pr::String>& names);

    private:
        ::VkInstanceCreateInfo _info;
        pr::Vector<pr::String> _enabled_extension_names;
        const char **_pp_enabled_extension_names;
    };

public:
    VkInstance(const VkInstance::CreateInfo& info);

    ~VkInstance();

    /// Create a Wayland specific surface with the given info.
    VkSurface create_wayland_surface(
        const VkSurface::WaylandSurfaceCreateInfo& info
    ) const;

    ::VkInstance c_ptr();

private:
    std::shared_ptr<::VkInstance> _instance;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_INSTANCE_H
