#pragma once

#include <vector>

#include "nex_window.hpp"

namespace nex {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR        m_capabilities;
        std::vector<VkSurfaceFormatKHR> m_formats;
        std::vector<VkPresentModeKHR>   m_present_modes;
    };

    struct QueueFamilyIndices {
        uint32_t m_graphics_family;
        uint32_t m_present_family;
        bool     m_graphics_family_has_value = false;
        bool     m_present_family_has_value  = false;
        bool     isComplete() {
            return m_graphics_family_has_value && m_present_family_has_value;
        }
    };

    class NexDevice {
      public:
#ifdef NDEBUG
        const bool m_enable_validation_layers = false;
#else
        const bool m_enable_validation_layers = true;
#endif

        NexDevice(NexWindow& window);
        ~NexDevice();

        // Not copyable or movable
        NexDevice(const NexDevice&)            = delete;
        NexDevice& operator=(const NexDevice&) = delete;
        NexDevice(NexDevice&&)                 = delete;
        NexDevice& operator=(NexDevice&&)      = delete;

        VkCommandPool getCommandPool() {
            return m_command_pool;
        }
        VkDevice device() {
            return m_device;
        }
        VkSurfaceKHR surface() {
            return m_surface;
        }
        VkQueue graphicsQueue() {
            return m_graphics_queue;
        }
        VkQueue presentQueue() {
            return m_present_queue;
        }

        SwapChainSupportDetails getSwapChainSupport() {
            return querySwapChainSupport(m_physical_device);
        }
        uint32_t           findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        QueueFamilyIndices findPhysicalQueueFamilies() {
            return findQueueFamilies(m_physical_device);
        }

        VkSampleCountFlagBits getMaxUsableSamples() {
            return m_msaa_samples;
        }

        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        // Buffer Helper Functions
        void            createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        VkCommandBuffer beginSingleTimeCommands();
        void            endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void            copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void            copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        void createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels = 1, uint32_t layerCount = 1);

        VkPhysicalDeviceProperties m_properties;

      private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();

        // helper functions
        bool                     isDeviceSuitable(VkPhysicalDevice device);
        std::vector<const char*> getRequiredExtensions();
        bool                     checkValidationLayerSupport();
        QueueFamilyIndices       findQueueFamilies(VkPhysicalDevice device);
        void                     populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void                     hasGflwRequiredInstanceExtensions();
        bool                     checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails  querySwapChainSupport(VkPhysicalDevice device);
        VkSampleCountFlagBits    getMaxUsableSampleCount();

        VkInstance               m_instance;
        VkDebugUtilsMessengerEXT m_debug_messenger;
        VkPhysicalDevice         m_physical_device = VK_NULL_HANDLE;
        NexWindow&               m_window;
        VkCommandPool            m_command_pool;

        VkDevice     m_device;
        VkSurfaceKHR m_surface;
        VkQueue      m_graphics_queue;
        VkQueue      m_present_queue;

        VkSampleCountFlagBits m_msaa_samples;

        const std::vector<const char*> m_validation_layers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> m_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };

}  // namespace nex
