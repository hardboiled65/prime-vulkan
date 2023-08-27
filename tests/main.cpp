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

// Vulkan validation layers.
const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
};
#ifdef NDEBUG
const bool enable_validation_layers = false;
#else
const bool enable_validation_layers = true;
#endif
VkLayerProperties *vulkan_layer_properties = NULL;
// Vulkan init.
pr::vk::VkInstance *vulkan_instance = nullptr;
pr::vk::VkPhysicalDevice *vulkan_physical_device = nullptr;
uint32_t graphics_family = 0;
VkPhysicalDeviceFeatures vulkan_device_features;
VkQueue vulkan_graphics_queue = NULL;
// Logical device.
pr::vk::VkDevice *vulkan_device = nullptr;
// Vulkan surface.
pr::vk::VkSurface *vulkan_surface = nullptr;
uint32_t present_family = 0;
VkQueue vulkan_present_queue = NULL;
// Swapchain.
uint32_t *queue_family_indices = NULL;
VkSurfaceCapabilitiesKHR vulkan_capabilities;
VkSurfaceFormatKHR *vulkan_formats = NULL;
VkPresentModeKHR *vulkan_present_modes = NULL;
const char* *vulkan_required_extension_names = NULL; // Not used.
VkSurfaceFormatKHR vulkan_format;
VkPresentModeKHR vulkan_present_mode;
VkExtent2D vulkan_extent;
pr::vk::VkSwapchain *vulkan_swapchain = nullptr;
pr::Vector<pr::vk::VkImage> vulkan_swapchain_images;
// Image views.
pr::Vector<pr::vk::VkImageView> vulkan_image_views;
// Render pass.
VkAttachmentDescription vulkan_attachment_description;
VkAttachmentReference vulkan_attachment_reference;
VkSubpassDescription vulkan_subpass_description;
VkRenderPass vulkan_render_pass;
VkRenderPassCreateInfo vulkan_render_pass_create_info;
VkSubpassDependency vulkan_dependency;
// Shaders.
uint8_t *vert_shader_code = NULL;
uint32_t vert_shader_code_size = 0;
uint8_t *frag_shader_code = NULL;
uint32_t frag_shader_code_size = 0;
VkPipelineShaderStageCreateInfo *vulkan_shader_stage_create_infos = NULL;
VkPipelineColorBlendAttachmentState vulkan_color_blend_attachment_state;
VkPipelineColorBlendStateCreateInfo vulkan_color_blend_state_create_info;
pr::vk::VkPipelineLayout *vulkan_layout = nullptr;
VkGraphicsPipelineCreateInfo vulkan_graphics_pipeline_create_info;
VkPipeline vulkan_graphics_pipeline = NULL;
// Framebuffers.
VkFramebuffer *vulkan_framebuffers = NULL;
// Command pool.
VkCommandPoolCreateInfo vulkan_command_pool_create_info;
VkCommandPool vulkan_command_pool = NULL;
// Command buffer.
VkCommandBufferAllocateInfo vulkan_command_buffer_allocate_info;
VkCommandBuffer vulkan_command_buffer = NULL;
VkCommandBufferBeginInfo vulkan_command_buffer_begin_info; // Unused.
VkRenderPassBeginInfo vulkan_render_pass_begin_info;
VkClearValue vulkan_clear_color;
// Sync objects.
VkSemaphore vulkan_image_available_semaphore = NULL;
VkSemaphore vulkan_render_finished_semaphore = NULL;
VkFence vulkan_in_flight_fence = NULL;


struct wl_subsurface *subsurface;

uint32_t image_width;
uint32_t image_height;
uint32_t image_size;
uint32_t *image_data;

static const char* vk_present_mode_to_string(VkPresentModeKHR mode)
{
    switch (mode) {
    case VK_PRESENT_MODE_MAILBOX_KHR:
        return "VK_PRESENT_MODE_MAILBOX_KHR";
    case VK_PRESENT_MODE_FIFO_KHR:
        return "VK_PRESENT_MODE_FIFO_KHR";
    default:
        return "Unknown";
    }
}

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
    VkResult result;

    // Check validation layers.
    uint32_t layers = 0;
    result = vkEnumerateInstanceLayerProperties(&layers, NULL);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Layer properties failed!\n");
        return;
    }
    fprintf(stderr, "Number of layers: %d\n", layers);


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
        vulkan_instance = new pr::vk::VkInstance(info);
        fprintf(stderr, "Vulkan instance created!\n");
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to create Vulkan instance.\n");
        pr::println(e.what());
        exit(1);
    }

    // Pick physical device.
    auto physical_devices = pr::vk::VkPhysicalDevice::enumerate(*vulkan_instance);
    if (physical_devices.length() == 0) {
        fprintf(stderr, "No GPUs with Vulkan support!\n");
        return;
    }
    fprintf(stderr, "Physical devices: %ld\n", physical_devices.length());

    for (auto& device: physical_devices) {
        fprintf(stderr, " - Device: %p\n", device.c_ptr());
    }
    vulkan_physical_device = new pr::vk::VkPhysicalDevice(physical_devices[0]);
}

static void create_vulkan_window()
{
    auto surface_create_info =
        pr::vk::VkSurface::WaylandSurfaceCreateInfo(display, surface);

    try {
        vulkan_surface = new pr::vk::VkSurface(
            vulkan_instance->create_wayland_surface(surface_create_info));
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
        vulkan_physical_device->queue_family_properties();
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
            vulkan_physical_device->c_ptr(),
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

    vulkan_device = new pr::vk::VkDevice(
        vulkan_physical_device->create_device(device_create_info));
    /*
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create logical device! reuslt: %d\n",
            result);
        return;
    } else {
        fprintf(stderr, "Logical device created - device: %p\n", vulkan_device);
    }
    */

    vkGetDeviceQueue(vulkan_device->c_ptr(), graphics_family, 0, &vulkan_graphics_queue);
    vkGetDeviceQueue(vulkan_device->c_ptr(), present_family, 0, &vulkan_present_queue);

    // Querying details of swap chain support.
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        vulkan_physical_device->c_ptr(),
        vulkan_surface->c_ptr(),
        &vulkan_capabilities);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to get surface capabilities!\n");
        return;
    }
    fprintf(stderr, "Physical device surface capabilities. - transform: %d\n",
        vulkan_capabilities.currentTransform);

    uint32_t formats;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_physical_device->c_ptr(),
        vulkan_surface->c_ptr(), &formats, NULL);
    fprintf(stderr, "Surface formats: %d\n", formats);

    vulkan_formats = (VkSurfaceFormatKHR*)malloc(
        sizeof(VkSurfaceFormatKHR) * formats
    );
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_physical_device->c_ptr(),
        vulkan_surface->c_ptr(), &formats, vulkan_formats);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to get surface formats!\n");
        return;
    }

    for (uint32_t i = 0; i < formats; ++i) {
        VkSurfaceFormatKHR surface_format = *(vulkan_formats + i);
        pr::println(" - Format: {}, Color space: {}",
            pr::vk::Vulkan::vk_format_to_string(surface_format.format),
            (int)(surface_format.colorSpace));
        if (surface_format.format == VK_FORMAT_B8G8R8A8_SRGB) {
            vulkan_format = surface_format;
        }
    }

    uint32_t modes;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_physical_device->c_ptr(),
        vulkan_surface->c_ptr(), &modes, NULL);
    if (modes == 0) {
        fprintf(stderr, "No surface present modes!\n");
        return;
    }
    fprintf(stderr, "Surface present modes: %d\n", modes);

    vulkan_present_modes = (VkPresentModeKHR*)malloc(
        sizeof(VkPresentModeKHR) * modes
    );

    result = vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_physical_device->c_ptr(),
        vulkan_surface->c_ptr(), &modes, vulkan_present_modes);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to get surface present modes!\n");
        return;
    }

    for (uint32_t i = 0; i < modes; ++i) {
        VkPresentModeKHR present_mode = *(vulkan_present_modes + i);
        fprintf(stderr, " - Present mode: %s\n",
            vk_present_mode_to_string(present_mode));
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            vulkan_present_mode = present_mode;
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

    vulkan_extent.width = WINDOW_WIDTH;
    vulkan_extent.height = WINDOW_HEIGHT;
}

static void create_vulkan_swapchain()
{
    fprintf(stderr, "Capabilities min, max image count: %d, %d\n",
        vulkan_capabilities.minImageCount,
        vulkan_capabilities.maxImageCount);
    uint32_t image_count = vulkan_capabilities.minImageCount + 1;
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
    swapchain_create_info.set_pre_transform(vulkan_capabilities.currentTransform);
    // ?? No alpha?
    swapchain_create_info.set_composite_alpha(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);
    swapchain_create_info.set_present_mode(vulkan_present_mode);
    swapchain_create_info.set_clipped(true);
    // vulkan_swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;
    fprintf(stderr, "Done writing swapchain create info.\n");

    vulkan_swapchain = new pr::vk::VkSwapchain(
        vulkan_device->create_swapchain(swapchain_create_info));
    /*
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create swapchain!\n");
        return;
    }
    fprintf(stderr, "Swapchain created!\n");
    */

    // Images.
    try {
        vulkan_swapchain_images = vulkan_device->images_for(*vulkan_swapchain);
    } catch (const pr::vk::VulkanError& e) {
        fprintf(stderr, "Failed to get swapchain images! %s\n", e.what());
    }

    fprintf(stderr, "Number of images: %ld\n",
        vulkan_swapchain_images.length());

    fprintf(stderr, "Swapchain images got.\n");
}

static void create_vulkan_image_views()
{
    for (uint64_t i = 0; i < vulkan_swapchain_images.length(); ++i) {
        pr::vk::VkImageView::CreateInfo create_info;
        create_info.set_image(vulkan_swapchain_images[i]);
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
            auto image_view = vulkan_device->create_image_view(create_info);
            vulkan_image_views.push(image_view);
        } catch (const pr::vk::VulkanError& e) {
            fprintf(stderr, "Image view creation failed. %s\n", e.what());
            exit(1);
        }

        fprintf(stderr, "Image view created - index: %ld\n", i);
    }
}

static void create_vulkan_render_pass()
{
    VkResult result;

    vulkan_attachment_description.format = vulkan_format.format;
    vulkan_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
    vulkan_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    vulkan_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    vulkan_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    vulkan_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    vulkan_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vulkan_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    vulkan_attachment_reference.attachment = 0;
    vulkan_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    vulkan_subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    vulkan_subpass_description.colorAttachmentCount = 1;
    vulkan_subpass_description.pColorAttachments = &vulkan_attachment_reference;

    // Subpass dependency.
    vulkan_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    vulkan_dependency.dstSubpass = 0;
    vulkan_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    vulkan_dependency.srcAccessMask = 0;
    vulkan_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    vulkan_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    vulkan_render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    vulkan_render_pass_create_info.attachmentCount = 1;
    vulkan_render_pass_create_info.pAttachments = &vulkan_attachment_description;
    vulkan_render_pass_create_info.subpassCount = 1;
    vulkan_render_pass_create_info.pSubpasses = &vulkan_subpass_description;
    vulkan_render_pass_create_info.dependencyCount = 1;
    vulkan_render_pass_create_info.pDependencies = &vulkan_dependency;

    result = vkCreateRenderPass(vulkan_device->c_ptr(), &vulkan_render_pass_create_info,
        NULL, &vulkan_render_pass);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create render pass!\n");
    }
    fprintf(stderr, "Render pass created. - render pass: %p\n",
        vulkan_render_pass);
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
            vulkan_device->create_shader_module(vert_create_info));
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
            vulkan_device->create_shader_module(frag_create_info));
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

    // Color blend attachment.
    vulkan_color_blend_attachment_state.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    vulkan_color_blend_attachment_state.blendEnable = VK_FALSE;

    // Color blending.
    vulkan_color_blend_state_create_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    vulkan_color_blend_state_create_info.logicOpEnable = VK_FALSE;
    vulkan_color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
    vulkan_color_blend_state_create_info.attachmentCount = 1;
    vulkan_color_blend_state_create_info.pAttachments =
        &vulkan_color_blend_attachment_state;
    vulkan_color_blend_state_create_info.blendConstants[0] = 0.0f;
    vulkan_color_blend_state_create_info.blendConstants[1] = 0.0f;
    vulkan_color_blend_state_create_info.blendConstants[2] = 0.0f;
    vulkan_color_blend_state_create_info.blendConstants[3] = 0.0f;

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
            vulkan_device->create_pipeline_layout(pipeline_layout_create_info));
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
    vulkan_graphics_pipeline_create_info.renderPass = vulkan_render_pass;
    vulkan_graphics_pipeline_create_info.subpass = 0;
    vulkan_graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

    result = vkCreateGraphicsPipelines(vulkan_device->c_ptr(), VK_NULL_HANDLE,
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
    VkResult result;

    vulkan_framebuffers = (VkFramebuffer*)malloc(
        sizeof(VkFramebuffer) * vulkan_swapchain_images.length()
    );

    for (uint32_t i = 0; i < vulkan_swapchain_images.length(); ++i) {
        VkImageView attachments[] = {
            vulkan_image_views[i].c_ptr(),
        };

        VkFramebufferCreateInfo create_info;
        create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = vulkan_render_pass;
        create_info.attachmentCount = 1;
        create_info.pAttachments = attachments;
        create_info.width = vulkan_extent.width;
        create_info.height = vulkan_extent.height;
        create_info.layers = 1;
        create_info.flags = 0;
        create_info.pNext = NULL;

        result = vkCreateFramebuffer(vulkan_device->c_ptr(), &create_info, NULL,
            (vulkan_framebuffers + i));
        if (result != VK_SUCCESS) {
            fprintf(stderr, "Failed to create framebuffer.\n");
            return;
        }
        fprintf(stderr, "Framebuffer created. - framebuffer: %p\n",
            vulkan_framebuffers[i]);
    }
}

static void create_vulkan_command_pool()
{
    VkResult result;

    vulkan_command_pool_create_info.sType =
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    vulkan_command_pool_create_info.flags =
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vulkan_command_pool_create_info.queueFamilyIndex = graphics_family;

    result = vkCreateCommandPool(vulkan_device->c_ptr(),
        &vulkan_command_pool_create_info, NULL, &vulkan_command_pool);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create command pool!\n");
        return;
    }
    fprintf(stderr, "Command pool created. - command pool: %p\n",
        vulkan_command_pool);
}

static void create_vulkan_command_buffer()
{
    VkResult result;

    vulkan_command_buffer_allocate_info.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    vulkan_command_buffer_allocate_info.commandPool = vulkan_command_pool;
    vulkan_command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vulkan_command_buffer_allocate_info.commandBufferCount = 1;

    result = vkAllocateCommandBuffers(vulkan_device->c_ptr(),
        &vulkan_command_buffer_allocate_info, &vulkan_command_buffer);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to allocate command buffers!\n");
    }
    fprintf(stderr, "Command buffer allocated. - command buffer: %p\n",
        vulkan_command_buffer);
}

static void create_vulkan_sync_objects()
{
    VkResult result;

    VkSemaphoreCreateInfo semaphore_create_info;
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_create_info.flags = 0;
    semaphore_create_info.pNext = NULL;

    VkFenceCreateInfo fence_create_info;
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = NULL;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    result = vkCreateSemaphore(vulkan_device->c_ptr(), &semaphore_create_info,
        NULL, &vulkan_image_available_semaphore);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create image available semaphore!\n");
        return;
    }
    fprintf(stderr, "Image available semaphore created.\n");
    result = vkCreateSemaphore(vulkan_device->c_ptr(), &semaphore_create_info,
        NULL, &vulkan_render_finished_semaphore);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create render finished semaphore!\n");
        return;
    }
    fprintf(stderr, "Render finished semaphore created.\n");
    result = vkCreateFence(vulkan_device->c_ptr(), &fence_create_info,
        NULL, &vulkan_in_flight_fence);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create fence!\n");
        return;
    }
    fprintf(stderr, "Fence created.\n");
}

static void record_command_buffer(VkCommandBuffer command_buffer,
        uint32_t image_index)
{
    VkResult result;

    VkCommandBufferBeginInfo command_buffer_begin_info;
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = 0;
    command_buffer_begin_info.pInheritanceInfo = NULL;
    command_buffer_begin_info.pNext = NULL;

    result = vkBeginCommandBuffer(command_buffer,
        &command_buffer_begin_info);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to begin recording command buffer!\n");
        return;
    }
    fprintf(stderr, "Begin command buffer.\n");
    fprintf(stderr, "Using framebuffer: %p\n", vulkan_framebuffers[image_index]);

    vulkan_render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    vulkan_render_pass_begin_info.renderPass = vulkan_render_pass;
    vulkan_render_pass_begin_info.framebuffer = vulkan_framebuffers[image_index];
    vulkan_render_pass_begin_info.renderArea.offset.x = 0;
    vulkan_render_pass_begin_info.renderArea.offset.y = 0;
    vulkan_render_pass_begin_info.renderArea.extent = vulkan_extent;

    vulkan_clear_color.color.float32[0] = 0.0f;
    vulkan_clear_color.color.float32[1] = 0.0f;
    vulkan_clear_color.color.float32[2] = 0.0f;
    vulkan_clear_color.color.float32[3] = 1.0f;

    vulkan_render_pass_begin_info.clearValueCount = 1;
    vulkan_render_pass_begin_info.pClearValues = &vulkan_clear_color;

    fprintf(stderr, "vkCmdBeginRenderPass() - command buffer: %p\n",
        command_buffer);
    vkCmdBeginRenderPass(command_buffer, &vulkan_render_pass_begin_info,
        VK_SUBPASS_CONTENTS_INLINE);
    fprintf(stderr, "Begin render pass.\n");

    //==============
    // In Commands
    //==============
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        vulkan_graphics_pipeline);

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)vulkan_extent.width;
    viewport.height = (float)vulkan_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = vulkan_extent;
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    vkCmdDraw(command_buffer, 3, 1, 0, 0);
    //===============
    // Out Commands
    //===============

    vkCmdEndRenderPass(command_buffer);

    result = vkEndCommandBuffer(command_buffer);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to record command buffer!\n");
        return;
    }
    fprintf(stderr, "End command buffer.\n");
}

void draw_frame()
{
    VkResult result;

    vkWaitForFences(vulkan_device->c_ptr(), 1, &vulkan_in_flight_fence,
        VK_TRUE, UINT64_MAX);
    vkResetFences(vulkan_device->c_ptr(), 1, &vulkan_in_flight_fence);

    uint32_t image_index;
    result = vkAcquireNextImageKHR(vulkan_device->c_ptr(), vulkan_swapchain->c_ptr(), UINT64_MAX,
        vulkan_image_available_semaphore, VK_NULL_HANDLE, &image_index);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to acquire next image!\n");
        return;
    }
    fprintf(stderr, "Acquired next image. - image index: %d\n", image_index);

    vkResetCommandBuffer(vulkan_command_buffer, 0);
    record_command_buffer(vulkan_command_buffer, image_index);

    VkSubmitInfo submit_info;
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {
        vulkan_image_available_semaphore,
    };
    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &vulkan_command_buffer;

    VkSemaphore signal_semaphores[] = {
        vulkan_render_finished_semaphore,
    };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    // Set zero or null.
    submit_info.pNext = NULL;

    result = vkQueueSubmit(vulkan_graphics_queue, 1, &submit_info,
        vulkan_in_flight_fence);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to submit draw command buffer!\n");
        return;
    }

    VkPresentInfoKHR present_info;
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapchains[] = {
        vulkan_swapchain->c_ptr(),
    };
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;

    present_info.pImageIndices = &image_index;

    // Set zero or null.
    present_info.pNext = NULL;

    vkQueuePresentKHR(vulkan_present_queue, &present_info);
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
        res = wl_display_dispatch(display);
    }
    fprintf(stderr, "wl_display_dispatch() - res: %d\n", res);

    wl_display_disconnect(display);
    printf("Disconnected from display.\n");

    return 0;
}
