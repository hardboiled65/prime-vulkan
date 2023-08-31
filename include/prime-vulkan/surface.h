#ifndef _PRIME_VULKAN_SURFACE_H
#define _PRIME_VULKAN_SURFACE_H

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>

namespace pr {
namespace vk {

class VkInstance;

class VkPhysicalDevice;

class Surface
{
    friend VkInstance;
public:
    using CType = ::VkSurfaceKHR;

    class WaylandSurfaceCreateInfo
    {
    public:
        WaylandSurfaceCreateInfo();

        WaylandSurfaceCreateInfo(struct wl_display*, struct wl_surface*);

        ::VkWaylandSurfaceCreateInfoKHR c_struct() const;

    private:
        ::VkWaylandSurfaceCreateInfoKHR _info;
    };

    class Capabilities
    {
        friend VkPhysicalDevice;
    public:
        using CType = ::VkSurfaceCapabilitiesKHR;

    public:
        CType c_struct() const;

    private:
        Capabilities();

    private:
        CType _capabilities;
    };

public:
    CType c_ptr() const;

private:
    Surface();

private:
    CType _surface;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_SURFACE_H
