#ifndef _PRIME_VULKAN_SURFACE_H
#define _PRIME_VULKAN_SURFACE_H

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>

namespace pr {
namespace vk {

class Instance;

class PhysicalDevice;

class Surface
{
    friend Instance;
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
        friend PhysicalDevice;
    public:
        using CType = ::VkSurfaceCapabilitiesKHR;

    public:
        ::VkSurfaceTransformFlagBitsKHR current_transform() const;

        uint32_t min_image_count() const;
        uint32_t max_image_count() const;

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


class SurfaceFormat
{
    friend PhysicalDevice;
public:
    using CType = ::VkSurfaceFormatKHR;

public:
    ::VkFormat format() const;

    ::VkColorSpaceKHR color_space() const;

private:
    SurfaceFormat();

private:
    CType _format;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_SURFACE_H
