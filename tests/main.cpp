#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>

#include <primer/string.h>
#include <primer/vector.h>
#include <primer/io.h>

#include <prime-vulkan/vulkan.h>

// #include <cairo.h>

#include "xdg-shell.h"

struct wl_display *display = NULL;
struct wl_compositor *compositor = NULL;
struct wl_subcompositor *subcompositor = NULL;
struct wl_surface *surface;
struct wl_egl_window *egl_window;
struct wl_region *region;
struct wl_output *output;

struct xdg_wm_base *xdg_wm_base = NULL;
struct xdg_surface *xdg_surface = NULL;
struct xdg_toplevel *xdg_toplevel = NULL;

#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 360

static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

// Vulkan validation layers.
const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
};
#ifdef NDEBUG
const bool enable_validation_layers = false;
#else
const bool enable_validation_layers = true;
#endif

// Vulkan init.
pr::vk::VkInstance *instance = nullptr;
pr::vk::VkPhysicalDevice *physical_device = nullptr;
uint32_t graphics_family = 0;
VkPhysicalDeviceFeatures vulkan_device_features;
// Logical device.
pr::vk::VkDevice *device = nullptr;
pr::vk::Queue *graphics_queue = nullptr;
pr::vk::Queue *present_queue = nullptr;
// Vulkan surface.
pr::vk::Surface *vulkan_surface = nullptr;
uint32_t present_family = 0;
// Swapchain.
uint32_t *queue_family_indices = NULL;
pr::vk::Surface::Capabilities *surface_capabilities = nullptr;
pr::Vector<pr::vk::SurfaceFormat> surface_formats;
VkPresentModeKHR *vulkan_present_modes = NULL;
const char* *vulkan_required_extension_names = NULL; // Not used.
VkSurfaceFormatKHR vulkan_format;
VkPresentModeKHR vulkan_present_mode;
VkExtent2D vulkan_extent;
pr::vk::VkSwapchain *swapchain = nullptr;
pr::Vector<pr::vk::VkImage> swapchain_images;
// Image views.
pr::Vector<pr::vk::VkImageView> image_views;
// Render pass.
pr::vk::RenderPass *render_pass;
// Shaders.
uint8_t *vert_shader_code = NULL;
uint32_t vert_shader_code_size = 0;
uint8_t *frag_shader_code = NULL;
uint32_t frag_shader_code_size = 0;
VkPipelineShaderStageCreateInfo *vulkan_shader_stage_create_infos = NULL;
pr::vk::VkPipelineLayout *vulkan_layout = nullptr;
VkGraphicsPipelineCreateInfo vulkan_graphics_pipeline_create_info;
VkPipeline vulkan_graphics_pipeline = NULL;
// Framebuffers.
pr::Vector<pr::vk::Framebuffer> framebuffers;
// Command pool.
pr::vk::CommandPool *command_pool = nullptr;
// Command buffer.
pr::Vector<pr::vk::CommandBuffer> command_buffers;
VkClearValue vulkan_clear_color;
// Sync objects.
pr::Vector<pr::vk::Semaphore> image_available_semaphores;
pr::Vector<pr::vk::Semaphore> render_finished_semaphores;
pr::Vector<pr::vk::Fence> in_flight_fences;
uint32_t current_frame = 0;


struct wl_subsurface *subsurface;

uint32_t image_width;
uint32_t image_height;
uint32_t image_size;
uint32_t *image_data;

/*
void load_image()
{
    cairo_surface_t *cairo_surface = cairo_image_surface_create_from_png(
        "miku@2x.png");
    cairo_t *cr = cairo_create(cairo_surface);
    image_width = cairo_image_surface_get_width(cairo_surface);
    image_height = cairo_image_surface_get_height(cairo_surface);
    image_size = sizeof(uint32_t) * (image_width * image_height);
    image_data = malloc(image_size);
    memcpy(image_data, cairo_image_surface_get_data(cairo_surface), image_size);

    // Color correct.
    for (uint32_t i = 0; i < (image_width * image_height); ++i) {
        uint32_t color = 0x00000000;
        color += ((image_data[i] & 0xff000000));  // Set alpha.
        color += ((image_data[i] & 0x00ff0000) >> 16);   // Set blue.
        color += ((image_data[i] & 0x0000ff00));   // Set green.
        color += ((image_data[i] & 0x000000ff) << 16);  // Set red.
        image_data[i] = color;
    }

    cairo_surface_destroy(cairo_surface);
    cairo_destroy(cr);
}
*/

//===========
// Vulkan
//===========

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

static void init_vulkan()
{
    // Check validation layers.
    auto layer_properties = pr::vk::LayerProperties::enumerate();
    fprintf(stderr, "Number of layers: %ld\n", layer_properties.length());
    for (auto& props: layer_properties) {
        fprintf(stderr, " <Layer> %s(%s)\n",
            props.layer_name().c_str(),
            props.description().c_str());
    }

    // Check extensions.
    auto extension_properties = pr::vk::VkExtensionProperties::enumerate();
    fprintf(stderr, "Number of extensions: %ld\n",
        extension_properties.length());

    pr::Vector<pr::String> extension_names = extension_properties.map<pr::String>([](auto& p) {
        return p.extension_name();
    });

    for (auto& properties: extension_properties) {
        fprintf(stderr, " - Extension name: %s\n",
            properties.extension_name().c_str());
    }

    // Create instance.
    pr::vk::VkInstance::CreateInfo info;
    info.set_enabled_extension_names(extension_names);

    try {
        instance = new pr::vk::VkInstance(info);
        fprintf(stderr, "Vulkan instance created!\n");
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create Vulkan instance.\n");
        pr::println(e.what());
        exit(1);
    }

    // Pick physical device.
    auto physical_devices = pr::vk::VkPhysicalDevice::enumerate(*instance);
    if (physical_devices.length() == 0) {
        fprintf(stderr, "No GPUs with Vulkan support!\n");
        return;
    }
    fprintf(stderr, "Physical devices: %ld\n", physical_devices.length());

    for (auto& device: physical_devices) {
        fprintf(stderr, " - Device: %p\n", device.c_ptr());
    }
    physical_device = new pr::vk::VkPhysicalDevice(physical_devices[0]);
}

static void create_vulkan_window()
{
    auto surface_create_info =
        pr::vk::Surface::WaylandSurfaceCreateInfo(display, surface);

    try {
        vulkan_surface = new pr::vk::Surface(
            instance->create_wayland_surface(surface_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create window surface! %s\n", e.what());
        exit(1);
    }

    fprintf(stderr, "Vulkan surface created.\n");
}

static void create_vulkan_logical_device()
{
    VkResult result;

    // Find queue families.

    auto queue_family_properties_list =
        physical_device->queue_family_properties();
    fprintf(stderr, "List of queue family properties: %ld\n",
        queue_family_properties_list.length());

    for (uint32_t i = 0; i < queue_family_properties_list.length(); ++i) {
        pr::vk::VkQueueFamilyProperties properties =
            queue_family_properties_list[i];
        fprintf(stderr, " - Queue count: %d\n", properties.queue_count());
        if (properties.queue_flags() & VK_QUEUE_GRAPHICS_BIT) {
            fprintf(stderr, " -- Has queue graphics bit. index: %d\n", i);
            graphics_family = i;
//            continue;
        }

        // Presentation support.
        VkBool32 present_support = VK_FALSE;
        result = vkGetPhysicalDeviceSurfaceSupportKHR(
            physical_device->c_ptr(),
            i, vulkan_surface->c_ptr(), &present_support);
        if (result != VK_SUCCESS) {
            fprintf(stderr, "Error!\n");
            return;
        }
        if (present_support == VK_TRUE) {
            fprintf(stderr, "Present support. index: %d\n", i);
            present_family = i;
            break;
        }
    }
    queue_family_indices = (uint32_t*)malloc(sizeof(uint32_t) * 2);
    queue_family_indices[0] = graphics_family;
    queue_family_indices[1] = present_family;
    fprintf(stderr, "[DEBUG] graphics/present queue family indices: %d, %d\n",
        graphics_family,
        present_family);

    // Creating the presentation queue.
    pr::Vector<float> queue_priorities;
    queue_priorities.push(1.0f);

    pr::vk::VkDevice::QueueCreateInfo graphics_queue_create_info;
    graphics_queue_create_info.set_queue_family_index(graphics_family);
    graphics_queue_create_info.set_queue_count(1);
    graphics_queue_create_info.set_queue_priorities(queue_priorities);

    pr::vk::VkDevice::QueueCreateInfo present_queue_create_info;
    present_queue_create_info.set_queue_family_index(present_family);
    present_queue_create_info.set_queue_count(1);
    present_queue_create_info.set_queue_priorities(queue_priorities);

    // Logical device.

    pr::Vector<pr::String> extension_names = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    pr::vk::VkDevice::CreateInfo device_create_info;
    device_create_info.set_queue_create_infos({
        graphics_queue_create_info,
        present_queue_create_info,
    });
    device_create_info.set_enabled_features(vulkan_device_features);
    device_create_info.set_enabled_extension_names(extension_names);

    try {
        device = new pr::vk::VkDevice(
            physical_device->create_device(device_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create logical device. %s\n", e.what());
        exit(1);
    }

    graphics_queue = new pr::vk::Queue(
        device->queue_for(graphics_family, 0));
    present_queue = new pr::vk::Queue(
        device->queue_for(present_family, 0));

    // Querying details of swapchain support.
    try {
        surface_capabilities = new pr::vk::Surface::Capabilities(
            physical_device->surface_capabilities_for(*vulkan_surface)
        );
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to get surface capabilities. %s\n", e.what());
        exit(1);
    }

    auto vulkan_capabilities = surface_capabilities->c_struct();
    fprintf(stderr, "Physical device surface capabilities. - transform: %d\n",
        vulkan_capabilities.currentTransform);

    // Get surface formats and select one.
    try {
        surface_formats = physical_device->surface_formats_for(*vulkan_surface);
    } catch (const pr::vk::VulkanError& e) {
        exit(1);
    }

    for (uint32_t i = 0; i < surface_formats.length(); ++i) {
        pr::println(" - Format: {}, Color space: {}",
            pr::vk::Vulkan::vk_format_to_string(surface_formats[i].format()),
            (int)(surface_formats[i].color_space()));
        if (surface_formats[i].format() == VK_FORMAT_B8G8R8A8_SRGB) {
            vulkan_format.format = surface_formats[i].format();
            vulkan_format.colorSpace = surface_formats[i].color_space();
        }
    }

    // Get present modes and select one.
    pr::Vector<::VkPresentModeKHR> present_modes;
    try {
        present_modes = physical_device->present_modes_for(*vulkan_surface);
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to get surface present modes. %s\n", e.what());
        exit(1);
    }

    for (uint64_t i = 0; i < present_modes.length(); ++i) {
        fprintf(stderr, " - Present mode: %s\n",
            pr::vk::Vulkan::vk_present_mode_to_string(present_modes[i]).c_str());
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            vulkan_present_mode = present_modes[i];
        }
    }

    // Swap extent.
    fprintf(stderr, "Current extent - %ux%u\n",
        vulkan_capabilities.currentExtent.width,
        vulkan_capabilities.currentExtent.height);
    fprintf(stderr, "Min image extent - %dx%d\n",
        vulkan_capabilities.minImageExtent.width,
        vulkan_capabilities.minImageExtent.height);
    fprintf(stderr, "Max image extent - %dx%d\n",
        vulkan_capabilities.maxImageExtent.width,
        vulkan_capabilities.maxImageExtent.height);
}

static void create_vulkan_swapchain(bool init = true)
{
    if (init) {
        vulkan_extent.width = WINDOW_WIDTH;
        vulkan_extent.height = WINDOW_HEIGHT;
    } else {
        // Do nothing.
    }

    auto vk_capabilities = surface_capabilities->c_struct();
    fprintf(stderr, "Capabilities min, max image count: %d, %d\n",
        vk_capabilities.minImageCount,
        vk_capabilities.maxImageCount);
    uint32_t image_count = vk_capabilities.minImageCount + 1;
    fprintf(stderr, "Image count: %d\n", image_count);

    pr::vk::VkSwapchain::CreateInfo swapchain_create_info;
    swapchain_create_info.set_surface(*vulkan_surface);
    swapchain_create_info.set_min_image_count(image_count);
    swapchain_create_info.set_image_format(vulkan_format.format);
    swapchain_create_info.set_image_color_space(vulkan_format.colorSpace);
    swapchain_create_info.set_image_extent(vulkan_extent);
    swapchain_create_info.set_image_array_layers(1);
    swapchain_create_info.set_image_usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

    if (graphics_family != present_family) {
        fprintf(stderr, "NOT SAME!\n");
        swapchain_create_info.set_image_sharing_mode(VK_SHARING_MODE_CONCURRENT);
        pr::Vector<uint32_t> indices = {
            queue_family_indices[0],
            queue_family_indices[1],
        };
        swapchain_create_info.set_queue_family_indices(indices);
    } else {
        swapchain_create_info.set_image_sharing_mode(VK_SHARING_MODE_EXCLUSIVE);
        swapchain_create_info.set_queue_family_indices(pr::Vector<uint32_t>());
    }
    swapchain_create_info.set_pre_transform(vk_capabilities.currentTransform);
    // ?? No alpha?
    swapchain_create_info.set_composite_alpha(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);
    swapchain_create_info.set_present_mode(vulkan_present_mode);
    swapchain_create_info.set_clipped(true);
    // vulkan_swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;
    fprintf(stderr, "Done writing swapchain create info.\n");

    // Create swapchain.
    try {
        swapchain = new pr::vk::VkSwapchain(
            device->create_swapchain(swapchain_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create swapchain! %d\n", e.vk_result());
        exit(1);
    }
    fprintf(stderr, "Swapchain created!\n");

    // Images.
    try {
        swapchain_images = device->images_for(*swapchain);
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to get swapchain images! %s\n", e.what());
    }

    fprintf(stderr, "Number of images: %ld\n",
        swapchain_images.length());

    fprintf(stderr, "Swapchain images got.\n");
}

static void create_vulkan_image_views();
static void create_vulkan_framebuffers();

static void recreate_swapchain()
{
    fprintf(stderr, " === Recreating swapchain... ===\n");
    device->wait_idle();

    framebuffers = {};
    image_views = {};

    delete swapchain;

    create_vulkan_swapchain(false);
    create_vulkan_image_views();
    create_vulkan_framebuffers();
}

static void create_vulkan_image_views()
{
    for (uint64_t i = 0; i < swapchain_images.length(); ++i) {
        pr::vk::VkImageView::CreateInfo create_info;
        create_info.set_image(swapchain_images[i]);
        create_info.set_view_type(VK_IMAGE_VIEW_TYPE_2D);
        create_info.set_format(vulkan_format.format);

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
            auto image_view = device->create_image_view(create_info);
            image_views.push(image_view);
        } catch (const pr::vk::VulkanError& e) {
            fprintf(stderr, "Image view creation failed. %s\n", e.what());
            exit(1);
        }

        fprintf(stderr, "Image view created - index: %ld\n", i);
    }
}

static void create_vulkan_render_pass()
{
    // Render pass create info.
    pr::vk::RenderPass::CreateInfo render_pass_create_info;
    // Set attachments.
    render_pass_create_info.set_attachments({
        []() {
            pr::vk::AttachmentDescription desc;
            desc.set_format(vulkan_format.format);
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
        render_pass = new pr::vk::RenderPass(
            device->create_render_pass(render_pass_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create render pass. %s\n", e.what());
    }

    fprintf(stderr, "Render pass created. - render pass: %p\n",
        render_pass->c_ptr());
}

static void create_vulkan_graphics_pipeline()
{
    VkResult result;

    // Load shader codes.
    load_shader("vert.spv", &vert_shader_code, &vert_shader_code_size);
    load_shader("frag.spv", &frag_shader_code, &frag_shader_code_size);

    // Create vertex shader module.
    pr::vk::VkShaderModule::CreateInfo vert_create_info;
    vert_create_info.set_code((const uint32_t*)vert_shader_code,
        vert_shader_code_size);

    pr::vk::VkShaderModule *vert_shader_module;
    try {
        vert_shader_module = new pr::vk::VkShaderModule(
            device->create_shader_module(vert_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create vertex shader module!\n");
        exit(1);
    }
    fprintf(stderr, "Vertex shader module created.\n");

    // Create fragment shader module.
    pr::vk::VkShaderModule::CreateInfo frag_create_info;
    frag_create_info.set_code((const uint32_t*)frag_shader_code,
        frag_shader_code_size);

    pr::vk::VkShaderModule *frag_shader_module;
    try {
        frag_shader_module = new pr::vk::VkShaderModule(
            device->create_shader_module(frag_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create fragment shader module!\n");
        exit(1);
    }
    fprintf(stderr, "Fragment shader module created.\n");

    // Write shader stage create infos.
    pr::vk::VkPipeline::ShaderStageCreateInfo vert_shader_stage_create_info;
    vert_shader_stage_create_info.set_stage(VK_SHADER_STAGE_VERTEX_BIT);
    vert_shader_stage_create_info.set_module(*vert_shader_module);
    vert_shader_stage_create_info.set_name("main"_S);

    pr::vk::VkPipeline::ShaderStageCreateInfo frag_shader_stage_create_info;
    frag_shader_stage_create_info.set_stage(VK_SHADER_STAGE_FRAGMENT_BIT);
    frag_shader_stage_create_info.set_module(*frag_shader_module);
    frag_shader_stage_create_info.set_name("main"_S);

    // Shader stage.
    vulkan_shader_stage_create_infos = new ::VkPipelineShaderStageCreateInfo[2];

    vulkan_shader_stage_create_infos[0] = vert_shader_stage_create_info.c_struct();
    vulkan_shader_stage_create_infos[1] = frag_shader_stage_create_info.c_struct();

    // Vertex input.
    pr::vk::VkPipeline::VertexInputStateCreateInfo vert_input_state_create_info;
    vert_input_state_create_info.set_vertex_binding_descriptions(
        pr::Vector<::VkVertexInputBindingDescription>());
    vert_input_state_create_info.set_vertex_attribute_descriptions(
        pr::Vector<::VkVertexInputAttributeDescription>());
    auto vulkan_vert_input_state_create_info = vert_input_state_create_info.c_struct();

    // Input assembly.
    pr::vk::VkPipeline::InputAssemblyStateCreateInfo input_assembly_state_create_info;
    input_assembly_state_create_info.set_topology(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    input_assembly_state_create_info.set_primitive_restart_enable(false);
    auto vulkan_input_assembly_state_create_info = input_assembly_state_create_info.c_struct();

    // Viewport.
    pr::vk::VkPipeline::ViewportStateCreateInfo viewport_state_create_info;
    viewport_state_create_info.set_viewport_count(1);
    viewport_state_create_info.set_scissor_count(1);
    auto vulkan_viewport_state_create_info = viewport_state_create_info.c_struct();

    // Rasterization.
    pr::vk::VkPipeline::RasterizationStateCreateInfo rasterization_state_create_info;
    rasterization_state_create_info.set_depth_clamp_enable(false);
    rasterization_state_create_info.set_rasterizer_discard_enable(false);
    rasterization_state_create_info.set_polygon_mode(VK_POLYGON_MODE_FILL);
    rasterization_state_create_info.set_line_width(1.0f);
    rasterization_state_create_info.set_cull_mode(VK_CULL_MODE_BACK_BIT);
    rasterization_state_create_info.set_front_face(VK_FRONT_FACE_CLOCKWISE);
    rasterization_state_create_info.set_depth_bias_enable(false);
    auto vulkan_rasterization_state_create_info = rasterization_state_create_info.c_struct();

    // Multisampling.
    pr::vk::VkPipeline::MultisampleStateCreateInfo multisample_state_create_info;
    multisample_state_create_info.set_sample_shading_enable(false);
    multisample_state_create_info.set_rasterization_samples(VK_SAMPLE_COUNT_1_BIT);
    auto vulkan_multisample_state_create_info = multisample_state_create_info.c_struct();

    // Color blending.
    pr::vk::VkPipeline::ColorBlendStateCreateInfo color_blend_state_create_info;
    // color_blend_state_create_info.set_logic_op(VK_LOGIC_OP_COPY);
    // Color blend attachments.
    color_blend_state_create_info.set_logic_op(VK_LOGIC_OP_COPY);
    color_blend_state_create_info.set_attachments({
        []() {
            pr::vk::VkPipeline::ColorBlendAttachmentState state;
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
    auto vulkan_color_blend_state_create_info = color_blend_state_create_info.c_struct();

    // Dynamic states.
    pr::Vector<::VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    pr::vk::VkPipeline::DynamicStateCreateInfo dynamic_state_create_info;
    dynamic_state_create_info.set_dynamic_states(dynamic_states);

    //-------------
    auto vulkan_dynamic_state_create_info = dynamic_state_create_info.c_struct();
    //-------------

    // Layout.
    pr::vk::VkPipelineLayout::CreateInfo pipeline_layout_create_info;
    pipeline_layout_create_info.set_set_layouts(
        pr::Vector<::VkDescriptorSetLayout>());
    pipeline_layout_create_info.set_push_constant_range(
        pr::Vector<::VkPushConstantRange>());

    try {
        vulkan_layout = new pr::vk::VkPipelineLayout(
            device->create_pipeline_layout(pipeline_layout_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create pipeline layout. %s\n", e.what());
        exit(1);
    }
    fprintf(stderr, "Pipeline layout created.\n");

    vulkan_graphics_pipeline_create_info.sType =
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    vulkan_graphics_pipeline_create_info.stageCount = 2;
    vulkan_graphics_pipeline_create_info.pStages = vulkan_shader_stage_create_infos;
    vulkan_graphics_pipeline_create_info.pVertexInputState =
        &vulkan_vert_input_state_create_info;
    vulkan_graphics_pipeline_create_info.pInputAssemblyState =
        &vulkan_input_assembly_state_create_info;
    vulkan_graphics_pipeline_create_info.pViewportState =
        &vulkan_viewport_state_create_info;
    vulkan_graphics_pipeline_create_info.pRasterizationState =
        &vulkan_rasterization_state_create_info;
    vulkan_graphics_pipeline_create_info.pMultisampleState =
        &vulkan_multisample_state_create_info;
    vulkan_graphics_pipeline_create_info.pColorBlendState =
        &vulkan_color_blend_state_create_info;
    vulkan_graphics_pipeline_create_info.pDynamicState =
        &vulkan_dynamic_state_create_info;
    vulkan_graphics_pipeline_create_info.layout = vulkan_layout->c_ptr();
    vulkan_graphics_pipeline_create_info.renderPass = render_pass->c_ptr();
    vulkan_graphics_pipeline_create_info.subpass = 0;
    vulkan_graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

    vulkan_graphics_pipeline_create_info.flags = 0;
    vulkan_graphics_pipeline_create_info.pNext = nullptr;

    result = vkCreateGraphicsPipelines(device->c_ptr(), VK_NULL_HANDLE,
        1, &vulkan_graphics_pipeline_create_info, NULL,
        &vulkan_graphics_pipeline);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create graphics pipeline!\n");
        return;
    }
    fprintf(stderr, "Graphics pipeline created - pipeline: %p\n",
        vulkan_graphics_pipeline);

    /*
    vkDestroyShaderModule(vulkan_device, frag_shader_module, NULL);
    vkDestroyShaderModule(vulkan_device, vert_shader_module, NULL);
    */
    delete frag_shader_module;
    delete vert_shader_module;
}

static void create_vulkan_framebuffers()
{
    for (uint32_t i = 0; i < swapchain_images.length(); ++i) {
        pr::Vector<pr::vk::VkImageView> attachments = {
            image_views[i],
        };

        pr::vk::Framebuffer::CreateInfo create_info;
        create_info.set_render_pass(*render_pass);
        create_info.set_attachments(attachments);
        create_info.set_width(vulkan_extent.width);
        create_info.set_height(vulkan_extent.height);
        create_info.set_layers(1);

        try {
            pr::vk::Framebuffer fb =
                device->create_framebuffer(create_info);
            framebuffers.push(fb);
        } catch (const pr::vk::VulkanError& e) {
            fprintf(stderr, "Failed to create framebuffer. %s\n", e.what());
        }

        fprintf(stderr, "Framebuffer created. - framebuffer: %p\n",
            framebuffers[i].c_ptr());
    }
}

static void create_vulkan_command_pool()
{
    pr::vk::CommandPool::CreateInfo command_pool_create_info;
    command_pool_create_info.set_queue_family_index(graphics_family);
    command_pool_create_info.set_flags(
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    try {
        command_pool = new pr::vk::CommandPool(
            device->create_command_pool(command_pool_create_info));
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create command pool! %s\n", e.what());
    }

    fprintf(stderr, "Command pool created. - command pool: %p\n",
        command_pool->c_ptr());
}

static void create_vulkan_command_buffer()
{
    pr::vk::CommandBuffer::AllocateInfo command_buffer_alloc_info;
    command_buffer_alloc_info.set_command_pool(*command_pool);
    command_buffer_alloc_info.set_level(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    command_buffer_alloc_info.set_command_buffer_count(1);

    try {
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            command_buffers.push(
                device->allocate_command_buffers(command_buffer_alloc_info));
            fprintf(stderr, "Command buffer allocated. - command buffer: %p\n",
                command_buffers[i].c_ptr());
        }
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to allocate command buffers. %s\n", e.what());
        exit(1);
    }
}

static void create_vulkan_sync_objects()
{
    pr::vk::Semaphore::CreateInfo semaphore_create_info;

    pr::vk::Fence::CreateInfo fence_create_info;
    fence_create_info.set_flags(VK_FENCE_CREATE_SIGNALED_BIT);

    try {
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            image_available_semaphores.push(
                device->create_semaphore(semaphore_create_info));
        }
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create image available semaphore!\n");
        exit(1);
    }

    try {
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            render_finished_semaphores.push(
                device->create_semaphore(semaphore_create_info));
        }
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create render finished semaphore!\n");
        exit(1);
    }
    fprintf(stderr, "Render finished semaphores created.\n");

    try {
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            in_flight_fences.push(
                device->create_fence(fence_create_info));
        }
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create fence. %s\n", e.what());
        exit(1);
    }

    fprintf(stderr, "Fences created.\n");
}

static void record_command_buffer(pr::vk::CommandBuffer& command_buffer,
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
    fprintf(stderr, "Using framebuffer: %p\n", framebuffers[image_index].c_ptr());

    pr::vk::RenderPass::BeginInfo render_pass_begin_info;
    render_pass_begin_info.set_render_pass(*render_pass);
    render_pass_begin_info.set_framebuffer(framebuffers[image_index]);
    render_pass_begin_info.set_render_area([]() {
        ::VkRect2D area;
        area.offset.x = 0;
        area.offset.y = 0;
        area.extent = vulkan_extent;
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

    fprintf(stderr, "vkCmdBeginRenderPass() - command buffer: %p\n",
        command_buffer.c_ptr());
    command_buffer.begin_render_pass(render_pass_begin_info,
        VK_SUBPASS_CONTENTS_INLINE);
    fprintf(stderr, "Begin render pass.\n");

    //==============
    // In Commands
    //==============
    auto vk_command_buffer = command_buffer.c_ptr();
    vkCmdBindPipeline(vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        vulkan_graphics_pipeline);
//    command_buffer.bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS,
//        *graphics_pipeline);

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)vulkan_extent.width;
    viewport.height = (float)vulkan_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    command_buffer.set_viewport(0, {
        viewport,
    });

    VkRect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = vulkan_extent;
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

void draw_frame()
{
    try {
        device->wait_for_fences({
            in_flight_fences[current_frame],
        }, true, UINT64_MAX);
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to wait for fences. %s\n", e.what());
        exit(1);
    }

    try {
        device->reset_fences({
            in_flight_fences[current_frame],
        });
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to reset fences. %s\n", e.what());
        exit(1);
    }

    uint32_t image_index;
    try {
        image_index = device->acquire_next_image(*swapchain,
            UINT64_MAX,
            image_available_semaphores[current_frame]);
    } catch (const pr::vk::VulkanError& e) {
        if (e.vk_result() == VK_ERROR_OUT_OF_DATE_KHR) {
            fprintf(stderr, "Recreate swapchain required.\n");
            recreate_swapchain();
        } else {
            fprintf(stderr, "Failed to acquire next image. %s\n", e.what());
            exit(1);
        }
    }
    fprintf(stderr, "Acquired next image. - image index: %d\n", image_index);

    try {
        command_buffers[current_frame].reset(0);
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to reset command buffer. %s\n", e.what());
        exit(1);
    }

    record_command_buffer(command_buffers[current_frame], image_index);

    pr::vk::SubmitInfo submit_info;
    submit_info.set_wait_semaphores({
        image_available_semaphores[current_frame],
    });
    submit_info.set_wait_dst_stage_mask({
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    });
    submit_info.set_command_buffers({
        command_buffers[current_frame],
    });
    submit_info.set_signal_semaphores({
        render_finished_semaphores[current_frame],
    });

    try {
        graphics_queue->submit({
            submit_info,
        }, in_flight_fences[current_frame]);
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to submit draw command buffer!\n");
        exit(1);
    }
    fprintf(stderr, "Queue submitted.\n");

    pr::vk::PresentInfo present_info;
    present_info.set_wait_semaphores({
        render_finished_semaphores[current_frame],
    });
    present_info.set_swapchains({
        *swapchain,
    });
    present_info.set_image_indices({
        image_index,
    });

    try {
        present_queue->present(present_info);
    } catch (const pr::vk::VulkanError& e) {
        if (e.vk_result() == VK_ERROR_OUT_OF_DATE_KHR ||
            e.vk_result() == VK_SUBOPTIMAL_KHR) {
            fprintf(stderr, "Swapchain recreate required.\n");
            recreate_swapchain();
        } else {
            fprintf(stderr, "Queue present failed. %s\n", e.what());
        }
    }
    fprintf(stderr, "vkQueuePresentKHR called\n");

    current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

//===========
// XDG
//===========
static void xdg_wm_base_ping_handler(void *data,
        struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping_handler,
};

static void xdg_surface_configure_handler(void *data,
        struct xdg_surface *xdg_surface, uint32_t serial)
{
    fprintf(stderr, "xdg_surface_configure_handler()\n");
    xdg_surface_ack_configure(xdg_surface, serial);
}

const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure_handler,
};

static void xdg_toplevel_configure_handler(void *data,
        struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height,
        struct wl_array *states)
{
    (void)data;
    (void)xdg_toplevel;
    (void)states;

    fprintf(stderr, "xdg_toplevel_configure_handler()\n");
    fprintf(stderr, " - width: %d, height: %d\n", width, height);

    if (width == 0 && height == 0) {
        return;
    }
    if (swapchain == nullptr) {
        return;
    }
    vulkan_extent.width = width;
    vulkan_extent.height = height;
    recreate_swapchain();
}

static void xdg_toplevel_close_handler(void *data,
        struct xdg_toplevel *xdg_toplevel)
{
}

const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_configure_handler,
    .close = xdg_toplevel_close_handler,
};

//==============
// Output
//==============

//==============
// Global
//==============
static void global_registry_handler(void *data, struct wl_registry *registry,
        uint32_t id, const char *interface, uint32_t version)
{
    if (strcmp(interface, "wl_compositor") == 0) {
        compositor = (struct wl_compositor*)wl_registry_bind(
            registry,
            id,
            &wl_compositor_interface,
            version
        );
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        xdg_wm_base = (struct xdg_wm_base*)wl_registry_bind(registry,
            id, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(xdg_wm_base, &xdg_wm_base_listener, NULL);
    } else {
        printf("(%d) Got a registry event for <%s> id <%d>\n",
            version, interface, id);
    }
}

static void global_registry_remover(void *data, struct wl_registry *registry,
        uint32_t id)
{
    printf("Got a registry losing event for <%d>\n", id);
}

static const struct wl_registry_listener registry_listener = {
    global_registry_handler,
    global_registry_remover
};


int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    display = wl_display_connect(NULL);
    if (display == NULL) {
        fprintf(stderr, "Can't connect to display.\n");
        exit(1);
    }
    printf("Connected to display.\n");

    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);

    wl_display_roundtrip(display);
    fprintf(stderr, "wl_display_roundtrip()\n");

    if (compositor == NULL || xdg_wm_base == NULL) {
        fprintf(stderr, "Can't find compositor or xdg_wm_base.\n");
        exit(1);
    }

    // Check surface.
    surface = wl_compositor_create_surface(compositor);
    if (surface == NULL) {
        fprintf(stderr, "Can't create surface.\n");
        exit(1);
    }

    // Check xdg surface.
    xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, surface);
    if (xdg_surface == NULL) {
        fprintf(stderr, "Can't create xdg surface.\n");
        exit(1);
    }
    xdg_surface_add_listener(xdg_surface, &xdg_surface_listener, NULL);

    xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
    if (xdg_toplevel == NULL) {
        exit(1);
    }
    xdg_toplevel_add_listener(xdg_toplevel, &xdg_toplevel_listener, NULL);


    // MUST COMMIT! or not working on weston.
    wl_surface_commit(surface);

    wl_display_roundtrip(display);

    // Vulkan init.
    init_vulkan();
    create_vulkan_window();
    create_vulkan_logical_device();
    create_vulkan_swapchain();
    create_vulkan_image_views();
    create_vulkan_render_pass();
    create_vulkan_graphics_pipeline();
    create_vulkan_framebuffers();
    create_vulkan_command_pool();
    create_vulkan_command_buffer();
    create_vulkan_sync_objects();

    draw_frame();

    wl_surface_commit(surface);

    int res = wl_display_dispatch(display);
    fprintf(stderr, "Initial dispatch.\n");
    while (res != -1) {
        draw_frame();
        res = wl_display_dispatch(display);
    }
    fprintf(stderr, "wl_display_dispatch() - res: %d\n", res);

    wl_display_disconnect(display);
    printf("Disconnected from display.\n");

    return 0;
}
