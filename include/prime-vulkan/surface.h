#ifndef _PRIME_VULKAN_SURFACE_H
#define _PRIME_VULKAN_SURFACE_H

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>

namespace pr {
namespace vk {

class VkInstance;

class VkSurface
{
    friend VkInstance;
public:
    class WaylandSurfaceCreateInfo
    {
    public:
        WaylandSurfaceCreateInfo();

        WaylandSurfaceCreateInfo(struct wl_display*, struct wl_surface*);

        ::VkWaylandSurfaceCreateInfoKHR c_struct() const;

    private:
        ::VkWaylandSurfaceCreateInfoKHR _info;
    };

public:
    ::VkSurfaceKHR c_ptr() const;

private:
    VkSurface();

private:
    ::VkSurfaceKHR _surface;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_SURFACE_H
