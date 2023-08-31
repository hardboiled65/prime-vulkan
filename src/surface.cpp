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


Surface::Surface()
{
    this->_surface = nullptr;
}

auto Surface::c_ptr() const -> CType
{
    return this->_surface;
}

} // namespace vk
} // namespace pr
