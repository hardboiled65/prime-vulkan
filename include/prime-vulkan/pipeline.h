#ifndef _PRIME_VULKAN_PIPELINE_H
#define _PRIME_VULKAN_PIPELINE_H

#include <vulkan/vulkan.h>

#include <memory>

#include <primer/string.h>

namespace pr {
namespace vk {

class VkShaderModule;

class VkDevice;

class VkPipeline
{
//=================
// Nested classes
//=================
public:
    class ShaderStageCreateInfo
    {
    public:
        ShaderStageCreateInfo();

        void set_stage(::VkShaderStageFlagBits stage);

        void set_module(const VkShaderModule& shader_module);

        void set_name(const pr::String& name);

        ::VkPipelineShaderStageCreateInfo c_struct() const;

    private:
        ::VkPipelineShaderStageCreateInfo _info;
        pr::String _name;
    };

    class DynamicStateCreateInfo
    {
    public:
        DynamicStateCreateInfo();

        DynamicStateCreateInfo(const DynamicStateCreateInfo& other);

        ~DynamicStateCreateInfo();

        /// Sets `.pDynamicStates` and automatically fills the count field.
        void set_dynamic_states(const pr::Vector<::VkDynamicState>& states);

        ::VkPipelineDynamicStateCreateInfo c_struct() const;

    private:
        ::VkPipelineDynamicStateCreateInfo _info;

        ::VkDynamicState *_states;
    };

//===================
// Public methods
//===================
public:

//===================
// Private methods
//===================
private:
    VkPipeline();

//============
// Members
//============
private:
};


class VkPipelineLayout
{
    friend VkDevice;
//===============
// Type aliases
//===============
public:
    using CType = ::VkPipelineLayout;

//=================
// Nested classes
//=================
public:
    class CreateInfo
    {
    public:
        using CType = ::VkPipelineLayoutCreateInfo;

    public:
        CreateInfo();

        /// Sets the set layout list. Count will automatically filled.
        ///
        /// TODO: Currently only accepts 0 length Vector.
        /// Others not implemented.
        void set_set_layouts(
            const pr::Vector<::VkDescriptorSetLayout>& set_layouts);

        /// Sets the push constant range list. Count will automatically filled.
        ///
        /// TODO: Currently only accepts 0 length Vector.
        /// Others not implemented.
        void set_push_constant_range(
            const pr::Vector<::VkPushConstantRange>& push_constant_range);

        CType c_struct() const;

    private:
        CType _info;
    };

    class Deleter
    {
    public:
        Deleter() = delete;

        Deleter(::VkDevice p_device)
        {
            this->_p_device = p_device;
        }

        void operator()(CType *layout)
        {
            vkDestroyPipelineLayout(this->_p_device, *layout, nullptr);
        }

    private:
        ::VkDevice _p_device;
    };

//=================
// Public methods
//=================
public:
    CType c_ptr() const;

//=================
// Private methods
//=================
private:
    VkPipelineLayout();

//==========
// Members
//==========
private:
    std::shared_ptr<CType> _layout;
};

} // namespace vk
} // namespace pr

#endif // _PRIME_VULKAN_PIPELINE_H
