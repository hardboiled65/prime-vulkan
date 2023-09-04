#include "application.h"

#include <stdio.h>

#include <functional>

#include <primer/io.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

static void load_shader(const char *path, uint8_t* *code, uint32_t *size);


//================
// Wayland global
//================
void Application::global(struct wl_registry *registry,
                         uint32_t id,
                         const char *interface,
                         uint32_t version)
{
    if (pr::String(interface) == "wl_compositor"_S) {
        fprintf(stderr, "Interface is wl_compositor\n");
        this->_compositor = (struct wl_compositor*)wl_registry_bind(
            registry,
            id,
            &wl_compositor_interface,
            version);
        fprintf(stderr, "Compositor got!\n");
    } else if (pr::String(interface) == "xdg_wm_base"_S) {
        this->_xdg_wm_base = (struct xdg_wm_base*)wl_registry_bind(registry,
            id, &xdg_wm_base_interface, 1);
        this->_xdg_wm_base_listener.ping =
            Application::ping_callback<&Application::ping>;
        xdg_wm_base_add_listener(this->_xdg_wm_base,
            &this->_xdg_wm_base_listener, (void*)this);
    } else {
        printf("(%d) Got a registry event for <%s> id <%d>\n",
            version, interface, id);
    }
}

void Application::global_remove(struct wl_registry *registry, uint32_t id)
{
    (void)registry;
    (void)id;
}

//===============
// Wayland XDG
//===============
void Application::ping(struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);

    fprintf(stderr, "XDG pong!\n");
}


void Application::configure(struct xdg_surface *surface, uint32_t serial)
{
    (void)surface;

    xdg_surface_ack_configure(this->_xdg_surface, serial);
}


void Application::configure(struct xdg_toplevel *toplevel,
                            int32_t width,
                            int32_t height,
                            struct wl_array *states)
{
    (void)toplevel;
    (void)width;
    (void)height;
    (void)states;
}

void Application::close(struct xdg_toplevel *toplevel)
{
    (void)toplevel;
}


Application::Application()
{
    this->_instance = nullptr;
    this->_physical_device = nullptr;

    this->_current_frame = 0;
}

void Application::init_wayland()
{
    this->_display = wl_display_connect(nullptr);
    if (this->_display == nullptr) {
        fprintf(stderr, "Can't connect to display.\n");
        exit(1);
    }

    this->_registry_listener.global =
        Application::global_callback<&Application::global>;
    this->_registry_listener.global_remove =
        Application::global_remove_callback<&Application::global_remove>;
    struct wl_registry *registry = wl_display_get_registry(this->_display);
    wl_registry_add_listener(registry, &this->_registry_listener, (void*)this);

    wl_display_roundtrip(this->_display); // SEGFAULT HERE!

    if (this->_compositor == nullptr || this->_xdg_wm_base == nullptr) {
        fprintf(stderr, "Can't find compositor or xdg_wm_base.\n");
        exit(1);
    }

    // Check surface.
    this->_wayland_surface = wl_compositor_create_surface(this->_compositor);
    if (this->_wayland_surface == nullptr) {
        fprintf(stderr, "Can't create surface.\n");
        exit(1);
    }

    // Check xdg surface.
    this->_xdg_surface = xdg_wm_base_get_xdg_surface(this->_xdg_wm_base,
        this->_wayland_surface);
    if (this->_xdg_surface == nullptr) {
        fprintf(stderr, "Can't create xdg surface.\n");
        exit(1);
    }
    this->_xdg_surface_listener.configure =
        XdgSurfaceListenable<Application>::configure_callback<&Application::configure>;
    xdg_surface_add_listener(this->_xdg_surface,
        &this->_xdg_surface_listener, (void*)this);

    this->_xdg_toplevel = xdg_surface_get_toplevel(this->_xdg_surface);
    if (this->_xdg_toplevel == nullptr) {
        exit(1);
    }
    this->_xdg_toplevel_listener.configure =
        XdgToplevelListenable<Application>::configure_callback<&Application::configure>;
    this->_xdg_toplevel_listener.close =
        XdgToplevelListenable<Application>::close_callback<&Application::close>;
    xdg_toplevel_add_listener(this->_xdg_toplevel,
        &this->_xdg_toplevel_listener, (void*)this);

    // MUST COMMIT! or not working on weston.
    wl_surface_commit(this->_wayland_surface);
}

void Application::init_vulkan()
{
    this->_create_instance();
    this->_create_physical_device();
    this->_create_surface();
    this->_create_logical_device();
    this->_get_capabilities();
    this->_create_swapchain();
    this->_create_image_views();
    this->_create_render_pass();
    this->_create_graphics_pipeline();
    this->_create_framebuffers();
    this->_create_command_pool();
    this->_create_command_buffers();
    this->_create_semaphores_and_fences();
}

void Application::run()
{
    this->_draw_frame();

    int res = wl_display_dispatch(this->_display);
    fprintf(stderr, "Initial dispatch.\n");
    while (res != -1) {
        // draw_frame();
        res = wl_display_dispatch(this->_display);
    }
    fprintf(stderr, "wl_display_dispatch() - res: %d\n", res);

    wl_display_disconnect(this->_display);
}

void Application::_create_instance()
{
    auto properties = pr::vk::ExtensionProperties::enumerate();
    auto extension_names = properties.map<pr::String>([](auto& p) {
        return p.extension_name();
    });

    pr::vk::Instance::CreateInfo info;
    info.set_enabled_extension_names(extension_names);
    try {
        this->_instance = new pr::vk::Instance(info);
        fprintf(stderr, "Vulkan instance created!\n");
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create Vulkan instance.\n");
        pr::println(e.what());
        exit(1);
    }
}

void Application::_create_physical_device()
{
    auto physical_devices = pr::vk::PhysicalDevice::enumerate(*this->_instance);
    if (physical_devices.length() == 0) {
        fprintf(stderr, "No GPUs with Vulkan support!\n");
        exit(1);
    }
    fprintf(stderr, "Physical devices: %ld\n", physical_devices.length());

    for (auto& device: physical_devices) {
        fprintf(stderr, " - Device: %p\n", device.c_ptr());
    }
    this->_physical_device = new pr::vk::PhysicalDevice(physical_devices[0]);
}

void Application::_create_surface()
{
    auto surface_create_info = pr::vk::Surface::WaylandSurfaceCreateInfo(
        this->_display, this->_wayland_surface);

    try {
        this->_surface = new pr::vk::Surface(
            this->_instance->create_wayland_surface(surface_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create window surface! %s\n", e.what());
        exit(1);
    }

    fprintf(stderr, "Vulkan surface created.\n");
}

void Application::_create_logical_device()
{
    auto queue_family_props = this->_physical_device->queue_family_properties();

    for (uint32_t i = 0; i < queue_family_props.length(); ++i) {
        auto properties = queue_family_props[i];
        if (properties.queue_flags() & VK_QUEUE_GRAPHICS_BIT) {
            fprintf(stderr, " -- Has queue graphics bit. index: %d\n", i);
            this->_queue_families.graphics = i;
        }

        // Presentation support.
        bool present_support = false;
        try {
            present_support = this->_physical_device->surface_support_for(i,
                *_surface);
        } catch (const pr::vk::VulkanError& e) {
            fprintf(stderr, "Error. %d\n", e.vk_result());
            exit(1);
        }

        if (present_support == true) {
            fprintf(stderr, "Present support. index: %d\n", i);
            this->_queue_families.present = i;
            break;
        }
    }
    this->_queue_family_indices = {
        this->_queue_families.graphics,
        this->_queue_families.present,
    };

    // Queue create infos.
    pr::Vector<float> queue_priorities;
    queue_priorities.push(1.0f);

    pr::vk::Device::QueueCreateInfo graphics_queue_create_info;
    graphics_queue_create_info.set_queue_family_index(
        this->_queue_families.graphics);
    graphics_queue_create_info.set_queue_count(1);
    graphics_queue_create_info.set_queue_priorities(queue_priorities);

    pr::vk::Device::QueueCreateInfo present_queue_create_info;
    present_queue_create_info.set_queue_family_index(
        this->_queue_families.present);
    present_queue_create_info.set_queue_count(1);
    present_queue_create_info.set_queue_priorities(queue_priorities);

    // Create logical device.
    pr::Vector<pr::String> extension_names = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    ::VkPhysicalDeviceFeatures device_features = {};

    pr::vk::Device::CreateInfo device_create_info;
    device_create_info.set_queue_create_infos({
        graphics_queue_create_info,
        present_queue_create_info,
    });
    device_create_info.set_enabled_features(device_features);
    device_create_info.set_enabled_extension_names(extension_names);

    try {
        this->_device = new pr::vk::Device(
            this->_physical_device->create_device(device_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create logical device. %s\n", e.what());
        exit(1);
    }

    // Create queues.
    this->_graphics_queue = std::make_shared<pr::vk::Queue>(
        this->_device->queue_for(this->_queue_families.graphics, 0));
    this->_present_queue = std::make_shared<pr::vk::Queue>(
        this->_device->queue_for(this->_queue_families.present, 0));
}

void Application::_get_capabilities()
{
    // Querying details of swapchain support.
    try {
        this->_surface_capabilities =
            std::make_shared<pr::vk::Surface::Capabilities>(
                this->_physical_device->surface_capabilities_for(*this->_surface)
            );
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to get surface capabilities. %s\n", e.what());
        exit(1);
    }

    fprintf(stderr, "Physical device surface capabilities. - transform: %d\n",
        this->_surface_capabilities->current_transform());

    // Get surface formats and select one.
    pr::Vector<pr::vk::SurfaceFormat> surface_formats;
    try {
        surface_formats =
            this->_physical_device->surface_formats_for(*_surface);
    } catch (const pr::vk::VulkanError& e) {
        exit(1);
    }

    for (uint32_t i = 0; i < surface_formats.length(); ++i) {
        pr::println(" - Format: {}, Color space: {}",
            pr::vk::Vulkan::vk_format_to_string(surface_formats[i].format()),
            (int)(surface_formats[i].color_space()));
        if (surface_formats[i].format() == VK_FORMAT_B8G8R8A8_SRGB) {
            this->_surface_format = std::make_shared<pr::vk::SurfaceFormat>(
                surface_formats[i]);
        }
    }

    // Get present modes and select one.
    pr::Vector<::VkPresentModeKHR> present_modes;
    try {
        present_modes =
            this->_physical_device->present_modes_for(*_surface);
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to get surface present modes. %s\n", e.what());
        exit(1);
    }

    for (uint64_t i = 0; i < present_modes.length(); ++i) {
        fprintf(stderr, " - Present mode: %s\n",
            pr::vk::Vulkan::vk_present_mode_to_string(present_modes[i]).c_str());
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            this->_present_mode = present_modes[i];
        }
    }
}

void Application::_create_swapchain()
{
    this->_extent.width = WINDOW_WIDTH;
    this->_extent.height = WINDOW_HEIGHT;

    uint32_t image_count = this->_surface_capabilities->min_image_count() + 1;

    // Create info.
    pr::vk::Swapchain::CreateInfo create_info;
    create_info.set_surface(*this->_surface);
    create_info.set_min_image_count(image_count);
    create_info.set_image_format(this->_surface_format->format());
    create_info.set_image_color_space(this->_surface_format->color_space());
    create_info.set_image_extent(this->_extent);
    create_info.set_image_array_layers(1);
    create_info.set_image_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    create_info.set_pre_transform(
        this->_surface_capabilities->current_transform());
    create_info.set_composite_alpha(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);
    create_info.set_present_mode(this->_present_mode);
    create_info.set_clipped(true);
    if (this->_queue_families.graphics != this->_queue_families.present) {
        // Not same.
        create_info.set_image_sharing_mode(VK_SHARING_MODE_CONCURRENT);
        pr::Vector<uint32_t> indices = {
            this->_queue_family_indices[0],
            this->_queue_family_indices[1],
        };
        create_info.set_queue_family_indices(indices);
    } else {
        // Same.
        create_info.set_image_sharing_mode(VK_SHARING_MODE_EXCLUSIVE);
        create_info.set_queue_family_indices(pr::Vector<uint32_t>());
    }

    // Create swapchain.
    try {
        this->_swapchain = std::make_shared<pr::vk::Swapchain>(
            this->_device->create_swapchain(create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create swapchain! %d\n", e.vk_result());
        exit(1);
    }
    fprintf(stderr, "Swapchain created!\n");

    // Images.
    try {
        this->_swapchain_images = this->_device->images_for(*this->_swapchain);
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to get swapchain images! %s\n", e.what());
    }

    fprintf(stderr, "Number of images: %ld\n",
        this->_swapchain_images.length());
}

void Application::_create_image_views()
{
    for (uint64_t i = 0; i < this->_swapchain_images.length(); ++i) {
        pr::vk::ImageView::CreateInfo create_info;
        create_info.set_image(this->_swapchain_images[i]);
        create_info.set_view_type(VK_IMAGE_VIEW_TYPE_2D);
        create_info.set_format(this->_surface_format->format());
        create_info.set_components(VK_COMPONENT_SWIZZLE_IDENTITY,
                                   VK_COMPONENT_SWIZZLE_IDENTITY,
                                   VK_COMPONENT_SWIZZLE_IDENTITY,
                                   VK_COMPONENT_SWIZZLE_IDENTITY);

        ::VkImageSubresourceRange range;
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.baseMipLevel = 0;
        range.levelCount = 1;
        range.baseArrayLayer = 0;
        range.layerCount = 1;
        create_info.set_subresource_range(range);

        try {
            auto image_view = this->_device->create_image_view(create_info);
            this->_image_views.push(image_view);
        } catch (const pr::vk::VulkanError& e) {
            fprintf(stderr, "Image view creation failed. %s\n", e.what());
            exit(1);
        }

        fprintf(stderr, "Image view created - index: %ld\n", i);
    }
}

void Application::_create_render_pass()
{
    // Render pass create info.
    pr::vk::RenderPass::CreateInfo render_pass_create_info;
    // Set attachments.
    render_pass_create_info.set_attachments({
        [this]() {
            pr::vk::AttachmentDescription desc;
            desc.set_format(this->_surface_format->format());
            desc.set_samples(VK_SAMPLE_COUNT_1_BIT);
            desc.set_load_op(VK_ATTACHMENT_LOAD_OP_CLEAR);
            desc.set_store_op(VK_ATTACHMENT_STORE_OP_STORE);
            desc.set_stencil_load_op(VK_ATTACHMENT_LOAD_OP_DONT_CARE);
            desc.set_stencil_store_op(VK_ATTACHMENT_STORE_OP_DONT_CARE);
            desc.set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED);
            desc.set_final_layout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

            return desc;
        }(),
    });
    // Set subpasses.
    render_pass_create_info.set_subpasses({
        []() {
            pr::vk::SubpassDescription desc;
            desc.set_pipeline_bind_point(VK_PIPELINE_BIND_POINT_GRAPHICS);
            desc.set_color_attachments({
                pr::vk::AttachmentReference(0,
                                            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),
            });
            return desc;
        }(),
    });
    // Set dependencies.
    render_pass_create_info.set_dependencies({
        []() {
            pr::vk::SubpassDependency dep;
            dep.set_subpass_src_dst(VK_SUBPASS_EXTERNAL, 0);
            dep.set_stage_mask_src_dst(
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
            dep.set_access_mask_src_dst(0,
                                        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
            return dep;
        }(),
    });

    try {
        this->_render_pass = std::make_shared<pr::vk::RenderPass>(
            this->_device->create_render_pass(render_pass_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create render pass. %s\n", e.what());
    }
}

void Application::_create_graphics_pipeline()
{
    // Load shader codes.
    load_shader("vert.spv",
        &this->_vert_shader_code, &this->_vert_shader_code_size);
    load_shader("frag.spv",
        &this->_frag_shader_code, &this->_frag_shader_code_size);

    // Create vertex shader module.
    pr::vk::ShaderModule::CreateInfo vert_create_info;
    vert_create_info.set_code((const uint32_t*)this->_vert_shader_code,
        this->_vert_shader_code_size);

    pr::vk::ShaderModule *vert_shader_module;
    try {
        vert_shader_module = new pr::vk::ShaderModule(
            this->_device->create_shader_module(vert_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create vertex shader module!\n");
        exit(1);
    }

    // Create fragment shader module.
    pr::vk::ShaderModule::CreateInfo frag_create_info;
    frag_create_info.set_code((const uint32_t*)this->_frag_shader_code,
        this->_frag_shader_code_size);

    pr::vk::ShaderModule *frag_shader_module;
    try {
        frag_shader_module = new pr::vk::ShaderModule(
            this->_device->create_shader_module(frag_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create fragment shader module!\n");
        exit(1);
    }

    // Write shader stage create infos.
    pr::vk::Pipeline::ShaderStageCreateInfo vert_shader_stage_create_info;
    vert_shader_stage_create_info.set_stage(VK_SHADER_STAGE_VERTEX_BIT);
    vert_shader_stage_create_info.set_module(*vert_shader_module);
    vert_shader_stage_create_info.set_name("main"_S);

    pr::vk::Pipeline::ShaderStageCreateInfo frag_shader_stage_create_info;
    frag_shader_stage_create_info.set_stage(VK_SHADER_STAGE_FRAGMENT_BIT);
    frag_shader_stage_create_info.set_module(*frag_shader_module);
    frag_shader_stage_create_info.set_name("main"_S);

    // Vertex input.
    pr::vk::Pipeline::VertexInputStateCreateInfo vert_input_state_create_info;
    vert_input_state_create_info.set_vertex_binding_descriptions({});
    vert_input_state_create_info.set_vertex_attribute_descriptions({});

    // Input assembly.
    pr::vk::Pipeline::InputAssemblyStateCreateInfo input_assembly_state_create_info;
    input_assembly_state_create_info.set_topology(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    input_assembly_state_create_info.set_primitive_restart_enable(false);

    // Viewport.
    pr::vk::Pipeline::ViewportStateCreateInfo viewport_state_create_info;
    viewport_state_create_info.set_viewport_count(1);
    viewport_state_create_info.set_scissor_count(1);

    // Rasterization.
    pr::vk::Pipeline::RasterizationStateCreateInfo rasterization_state_create_info;
    rasterization_state_create_info.set_depth_clamp_enable(false);
    rasterization_state_create_info.set_rasterizer_discard_enable(false);
    rasterization_state_create_info.set_polygon_mode(VK_POLYGON_MODE_FILL);
    rasterization_state_create_info.set_line_width(1.0f);
    rasterization_state_create_info.set_cull_mode(VK_CULL_MODE_BACK_BIT);
    rasterization_state_create_info.set_front_face(VK_FRONT_FACE_CLOCKWISE);
    rasterization_state_create_info.set_depth_bias_enable(false);

    // Multisampling.
    pr::vk::Pipeline::MultisampleStateCreateInfo multisample_state_create_info;
    multisample_state_create_info.set_sample_shading_enable(false);
    multisample_state_create_info.set_rasterization_samples(VK_SAMPLE_COUNT_1_BIT);

    // Color blending.
    pr::vk::Pipeline::ColorBlendStateCreateInfo color_blend_state_create_info;
    color_blend_state_create_info.set_logic_op(VK_LOGIC_OP_COPY);
    // Color blend attachments.
    color_blend_state_create_info.set_attachments({
        []() {
            pr::vk::Pipeline::ColorBlendAttachmentState state;
            state.set_color_write_mask(
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT);
            state.set_blend_enable(false);
            return state;
        }(),
    });
    color_blend_state_create_info.set_blend_constants(0.0f, 0.0f, 0.0f, 0.0f);

    // Dynamic states.
    pr::Vector<::VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    pr::vk::Pipeline::DynamicStateCreateInfo dynamic_state_create_info;
    dynamic_state_create_info.set_dynamic_states(dynamic_states);

    // Layout.
    pr::vk::PipelineLayout::CreateInfo pipeline_layout_create_info;
    pipeline_layout_create_info.set_set_layouts(
        pr::Vector<::VkDescriptorSetLayout>());
    pipeline_layout_create_info.set_push_constant_range(
        pr::Vector<::VkPushConstantRange>());

    try {
        this->_pipeline_layout = std::make_shared<pr::vk::PipelineLayout>(
            this->_device->create_pipeline_layout(pipeline_layout_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create pipeline layout. %s\n", e.what());
        exit(1);
    }
    fprintf(stderr, "Pipeline layout created.\n");

    pr::vk::GraphicsPipelineCreateInfo create_info;
    // Shader stage.
    create_info.set_stages({
        vert_shader_stage_create_info,
        frag_shader_stage_create_info,
    });
    create_info.set_vertex_input_state(vert_input_state_create_info);
    create_info.set_input_assembly_state(input_assembly_state_create_info);
    create_info.set_viewport_state(viewport_state_create_info);
    create_info.set_rasterization_state(rasterization_state_create_info);
    create_info.set_multisample_state(multisample_state_create_info);
    create_info.set_color_blend_state(color_blend_state_create_info);
    create_info.set_dynamic_state(dynamic_state_create_info);
    create_info.set_layout(*this->_pipeline_layout);
    create_info.set_render_pass(*this->_render_pass);
    create_info.set_subpass(0);

    try {
        auto graphics_pipelines = this->_device->create_graphics_pipelines({
            create_info,
        });
        if (graphics_pipelines.length() < 1) {
            fprintf(stderr, "Zero graphics pipelines!\n");
            exit(1);
        }
        this->_graphics_pipeline =
            std::make_shared<pr::vk::Pipeline>(graphics_pipelines[0]);
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create graphics pipeline. %d\n",
                e.vk_result());
        exit(1);
    }

    // Delete shader modules.
    delete frag_shader_module;
    delete vert_shader_module;
}

void Application::_create_framebuffers()
{
    for (uint64_t i = 0; i < this->_swapchain_images.length(); ++i) {
        pr::Vector<pr::vk::ImageView> attachments = {
            this->_image_views[i],
        };

        pr::vk::Framebuffer::CreateInfo create_info;
        create_info.set_render_pass(*this->_render_pass);
        create_info.set_attachments(attachments);
        create_info.set_width(this->_extent.width);
        create_info.set_height(this->_extent.height);
        create_info.set_layers(1);

        try {
            pr::vk::Framebuffer fb =
                this->_device->create_framebuffer(create_info);
            this->_framebuffers.push(fb);
        } catch (const pr::vk::VulkanError& e) {
            fprintf(stderr, "Failed to create framebuffer. %s\n", e.what());
        }
    }
}

void Application::_create_command_pool()
{
    pr::vk::CommandPool::CreateInfo command_pool_create_info;
    command_pool_create_info.set_queue_family_index(
        this->_queue_families.graphics); // ??????????
    command_pool_create_info.set_flags(
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    try {
        this->_command_pool = std::make_shared<pr::vk::CommandPool>(
            this->_device->create_command_pool(command_pool_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create command pool! %s\n", e.what());
    }
}

void Application::_create_command_buffers()
{
    pr::vk::CommandBuffer::AllocateInfo alloc_info;
    alloc_info.set_command_pool(*this->_command_pool);
    alloc_info.set_level(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    alloc_info.set_command_buffer_count(1);

    try {
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            this->_command_buffers.push(
                this->_device->allocate_command_buffers(alloc_info));
        }
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to allocate command buffers. %s\n", e.what());
        exit(1);
    }
}

void Application::_create_semaphores_and_fences()
{
    pr::vk::Semaphore::CreateInfo semaphore_create_info;

    pr::vk::Fence::CreateInfo fence_create_info;
    fence_create_info.set_flags(VK_FENCE_CREATE_SIGNALED_BIT);

    try {
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            this->_image_available_semaphores.push(
                this->_device->create_semaphore(semaphore_create_info));
        }
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create image available semaphore!\n");
        exit(1);
    }

    try {
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            this->_render_finished_semaphores.push(
                this->_device->create_semaphore(semaphore_create_info));
        }
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create render finished semaphore!\n");
        exit(1);
    }
    fprintf(stderr, "Render finished semaphores created.\n");

    try {
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            this->_in_flight_fences.push(
                this->_device->create_fence(fence_create_info));
        }
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create fence. %s\n", e.what());
        exit(1);
    }
}

void Application::_record_command_buffer(pr::vk::CommandBuffer& command_buffer,
                                         uint32_t image_index)
{
    pr::vk::CommandBuffer::BeginInfo command_buffer_begin_info;

    try {
        command_buffer.begin(command_buffer_begin_info);
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to begin recording command buffer!\n");
        exit(1);
    }

    fprintf(stderr, "Begin command buffer.\n");
    fprintf(stderr, "Using framebuffer: %p\n",
        this->_framebuffers[image_index].c_ptr());

    pr::vk::RenderPass::BeginInfo render_pass_begin_info;
    render_pass_begin_info.set_render_pass(*this->_render_pass);
    render_pass_begin_info.set_framebuffer(this->_framebuffers[image_index]);
    render_pass_begin_info.set_render_area([this]() {
        ::VkRect2D area;
        area.offset.x = 0;
        area.offset.y = 0;
        area.extent = this->_extent;
        return area;
    }());
    render_pass_begin_info.set_clear_values({
        []() {
            ::VkClearValue value;
            value.color.float32[0] = 0.0f;
            value.color.float32[1] = 0.0f;
            value.color.float32[2] = 0.0f;
            value.color.float32[3] = 1.0f;
            return value;
        }(),
    });

    command_buffer.begin_render_pass(render_pass_begin_info,
        VK_SUBPASS_CONTENTS_INLINE);
    fprintf(stderr, "Begin render pass.\n");

    //==============
    // In Commands
    //==============
    command_buffer.bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS,
        *this->_graphics_pipeline);

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)this->_extent.width;
    viewport.height = (float)this->_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    command_buffer.set_viewport(0, {
                                       viewport,
                                   });

    VkRect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = this->_extent;
    command_buffer.set_scissor(0, {
                                      scissor,
                                  });

    command_buffer.draw(3, 1, 0, 0);

    //===============
    // Out Commands
    //===============
    command_buffer.end_render_pass();

    try {
        command_buffer.end();
    } catch(const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to record command bufer. %s\n", e.what());
        exit(1);
    }
    fprintf(stderr, "End command buffer.\n");
}

void Application::_draw_frame()
{
    try {
        this->_device->wait_for_fences({
            this->_in_flight_fences[this->_current_frame],
        }, true, UINT64_MAX);
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to wait for fences. %s\n", e.what());
        exit(1);
    }

    try {
        this->_device->reset_fences({
            this->_in_flight_fences[this->_current_frame],
        });
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to reset fences. %s\n", e.what());
        exit(1);
    }

    uint32_t image_index;
    try {
        image_index = this->_device->acquire_next_image(*this->_swapchain,
            UINT64_MAX,
            this->_image_available_semaphores[this->_current_frame]);
    } catch (const pr::vk::VulkanError& e) {
        if (e.vk_result() == VK_ERROR_OUT_OF_DATE_KHR) {
            fprintf(stderr, "Recreate swapchain required.\n");
//            recreate_swapchain();
        } else {
            fprintf(stderr, "Failed to acquire next image. %s\n", e.what());
            exit(1);
        }
    }
    fprintf(stderr, "Acquired next image. - image index: %d\n", image_index);

    try {
        this->_command_buffers[this->_current_frame].reset(0);
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to reset command buffer. %s\n", e.what());
        exit(1);
    }

    this->_record_command_buffer(this->_command_buffers[this->_current_frame],
        image_index);

    pr::vk::SubmitInfo submit_info;
    submit_info.set_wait_semaphores({
        this->_image_available_semaphores[this->_current_frame],
    });
    submit_info.set_wait_dst_stage_mask({
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    });
    submit_info.set_command_buffers({
        this->_command_buffers[this->_current_frame],
    });
    submit_info.set_signal_semaphores({
        this->_render_finished_semaphores[this->_current_frame],
    });

    try {
        this->_graphics_queue->submit({
            submit_info,
        }, this->_in_flight_fences[this->_current_frame]);
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to submit draw command buffer!\n");
        exit(1);
    }
    fprintf(stderr, "Queue submitted.\n");

    pr::vk::PresentInfo present_info;
    present_info.set_wait_semaphores({
        this->_render_finished_semaphores[this->_current_frame],
    });
    present_info.set_swapchains({
        *this->_swapchain,
    });
    present_info.set_image_indices({
        image_index,
    });

    try {
        this->_present_queue->present(present_info);
    } catch (const pr::vk::VulkanError& e) {
        if (e.vk_result() == VK_ERROR_OUT_OF_DATE_KHR ||
            e.vk_result() == VK_SUBOPTIMAL_KHR) {
            fprintf(stderr, "Swapchain recreate required.\n");
//            recreate_swapchain();
        } else {
            fprintf(stderr, "Queue present failed. %s\n", e.what());
        }
    }
    fprintf(stderr, "vkQueuePresentKHR called\n");

    this->_current_frame = (this->_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}


static void load_shader(const char *path, uint8_t* *code, uint32_t *size)
{
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);

    *code = (uint8_t*)malloc(sizeof(uint8_t) * *size);
    fread(*code, *size, 1, f);

    fclose(f);
}