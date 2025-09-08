#include "PveVulkanConnection.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <set>
#include <stdexcept>

namespace pve {
    /// The `createInstance` function from the Vulkan guide
    PveVulkanConnection::PveVulkanConnection(const char* appName, const char* engineName) {
        if (_debugger.isValidationEnabled() && !_debugger.areValidationLayersSupported()) {
            throw std::runtime_error("Validation layers requested but not available");
        }

        // Info about the application; optional but can help optimize
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appName;
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
        appInfo.pEngineName = engineName;
        appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.apiVersion = VK_API_VERSION_1_4;

        // Information about what extensions and validation layers to include
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        const auto extensions = _getRequiredExtensions();
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        // Include the validation layers
        if (_debugger.isValidationEnabled()) {
            createInfo.enabledLayerCount = _debugger.getValidationLayersCount();
            createInfo.ppEnabledLayerNames = _debugger.getValidationLayerNames();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
            throw std::runtime_error("Failed to create Vulkan Instance");
    }

    std::vector<const char*> PveVulkanConnection::_getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (_debugger.isValidationEnabled()) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        if (!_areRequiredExtSupported(extensions.data(), glfwExtensionCount))
            throw std::runtime_error("Some of the required extensions are not supported");

        return extensions;
    }

    bool PveVulkanConnection::_areRequiredExtSupported(const char** requiredExtensions,
                                                       const uint32_t requiredExtensionCount) {
        // Get the supported extensions
        uint32_t supportedExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(supportedExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, extensions.data());

        // Verify required extensions are supported
        std::set<std::string> supportedExtensionsLut{};
        for (const VkExtensionProperties& ext : extensions) {
            supportedExtensionsLut.insert(ext.extensionName);
        }

        bool supported = true;
        std::cout << "Required Extensions:\n";
        for (int i = 0; i < requiredExtensionCount; i++) {
            std::cout << "\t" << requiredExtensions[i];

            if (supportedExtensionsLut.contains(requiredExtensions[i])) {
                std::cout << " (supported)" << "\n";
            } else {
                std::cout << " (not supported)" << "\n";
                supported = false;
            }
        }
        return supported;
    }

}