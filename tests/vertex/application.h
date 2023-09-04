#include <prime-vulkan/vulkan.h>

// C
#include <stdint.h>

// Wayland
#include <wayland-client.h>
#include "xdg-shell.h"

// C++
#include <memory>

// Primer
#include <primer/vector.h>
#include <primer/string.h>

#include "wayland.h"

class Application : public WlRegistryListenable<Application>,
                    public XdgWmBaseListenable<Application>,
                    public XdgSurfaceListenable<Application>,
                    public XdgToplevelListenable<Application>
{
public:
    Application();

    void init_wayland();

    void init_vulkan();

    void run();


    virtual void global(struct wl_registry *registry,
                        uint32_t id,
                        const char *interface,
                        uint32_t version) override;

    virtual void global_remove(struct wl_registry *registry,
                               uint32_t id) override;


    virtual void ping(struct xdg_wm_base *xdg_wm_base,
                      uint32_t serial) override;


    virtual void configure(struct xdg_surface *surface,
                           uint32_t serial) override;


    virtual void configure(struct xdg_toplevel *toplevel,
                           int32_t width,
                           int32_t height,
                           struct wl_array *states) override;

    virtual void close(struct xdg_toplevel *toplevel) override;

private:

    void _create_instance();

    void _create_physical_device();

    void _create_surface();

    void _create_logical_device();

    void _get_capabilities();

    void _create_swapchain();

    void _create_image_views();

    void _create_render_pass();

    void _create_graphics_pipeline();

    void _create_framebuffers();

    void _create_command_pool();

    void _create_vertex_buffer();

    void _create_command_buffers();

    void _create_semaphores_and_fences();

    void _record_command_buffer(pr::vk::CommandBuffer& command_buffer,
                                uint32_t image_index);

    void _draw_frame();

private:
    // Wayland.
    struct wl_display *_display;
    struct wl_compositor *_compositor;
    struct wl_surface *_wayland_surface;
    struct xdg_wm_base *_xdg_wm_base;
    struct xdg_surface *_xdg_surface;
    struct xdg_toplevel *_xdg_toplevel;

    struct wl_registry_listener _registry_listener;
    struct xdg_wm_base_listener _xdg_wm_base_listener;
    struct xdg_surface_listener _xdg_surface_listener;
    struct xdg_toplevel_listener _xdg_toplevel_listener;

    // Instance.
    pr::vk::Instance *_instance;
    // Physical device.
    pr::vk::PhysicalDevice *_physical_device;
    // Vulkan surface.
    pr::vk::Surface *_surface;
    // Logical device.
    struct {
        uint32_t graphics;
        uint32_t present;
    } _queue_families;
    pr::Vector<uint32_t> _queue_family_indices;
    pr::vk::Device *_device;
    std::shared_ptr<pr::vk::Queue> _graphics_queue;
    std::shared_ptr<pr::vk::Queue> _present_queue;
    // Capabilities.
    std::shared_ptr<pr::vk::Surface::Capabilities> _surface_capabilities;
    std::shared_ptr<pr::vk::SurfaceFormat> _surface_format;
    ::VkPresentModeKHR _present_mode;
    // Swapchain.
    ::VkExtent2D _extent;
    std::shared_ptr<pr::vk::Swapchain> _swapchain;
    pr::Vector<pr::vk::Image> _swapchain_images;
    // Image views.
    pr::Vector<pr::vk::ImageView> _image_views;
    // Render pass.
    std::shared_ptr<pr::vk::RenderPass> _render_pass;
    // Shaders.
    uint8_t *_vert_shader_code = nullptr;
    uint32_t _vert_shader_code_size = 0;
    uint8_t *_frag_shader_code = nullptr;
    uint32_t _frag_shader_code_size = 0;
    // Graphics pipeline.
    std::shared_ptr<pr::vk::PipelineLayout> _pipeline_layout;
    std::shared_ptr<pr::vk::Pipeline> _graphics_pipeline;
    // Framebuffers.
    pr::Vector<pr::vk::Framebuffer> _framebuffers;
    // Command pool.
    std::shared_ptr<pr::vk::CommandPool> _command_pool;
    // Vertex buffer.
    std::shared_ptr<pr::vk::Buffer> _vertex_buffer;
    // Command buffers.
    pr::Vector<pr::vk::CommandBuffer> _command_buffers;
    // Semaphores and fences.
    pr::Vector<pr::vk::Semaphore> _image_available_semaphores;
    pr::Vector<pr::vk::Semaphore> _render_finished_semaphores;
    pr::Vector<pr::vk::Fence> _in_flight_fences;
    // Drawing.
    uint32_t _current_frame;
};
