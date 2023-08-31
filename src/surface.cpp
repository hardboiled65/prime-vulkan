#include <prime-vulkan/surface.h>

namespace pr {
namespace vk {

Surface::WaylandSurfaceCreateInfo::WaylandSurfaceCreateInfo()
{
    this->_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    this->_info.flags = 0;
    this->_info.pNext = nullptr;
    this->_info.display = nullptr;
    this->_info.surface = nullptr;
}

Surface::WaylandSurfaceCreateInfo::WaylandSurfaceCreateInfo(
    struct wl_display *display,
    struct wl_surface *surface)
{
    this->_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    this->_info.flags = 0;
    this->_info.pNext = nullptr;
    this->_info.display = display;
    this->_info.surface = surface;
}

::VkWaylandSurfaceCreateInfoKHR
Surface::WaylandSurfaceCreateInfo::c_struct() const
{
    return this->_info;
}


Surface::Capabilities::Capabilities()
{
}

auto Surface::Capabilities::c_struct() const -> CType
{
    return this->_capabilities;
}


Surface::Surface()
{
    this->_surface = nullptr;
}

auto Surface::c_ptr() const -> CType
{
    return this->_surface;
}


SurfaceFormat::SurfaceFormat()
{
}

::VkFormat SurfaceFormat::format() const
{
    return this->_format.format;
}

::VkColorSpaceKHR SurfaceFormat::color_space() const
{
    return this->_format.colorSpace;
}


} // namespace vk
} // namespace pr
