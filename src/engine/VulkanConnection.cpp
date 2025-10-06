#include "VulkanConnection.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <set>
#include <stdexcept>

namespace pve {
    /**
     * The `createInstance()` of the Vulkan guide.
     *
     * @param appName Pointer to the string of the application name.
     * @param engineName Pointer to the string of the engine name.
     */
    VulkanConnection::VulkanConnection(const char* appName, const char* engineName) {
        _createInstance(appName, engineName);
        _setupDebugMessenger();
        _physicalDevice.pickPhysicalDevice(_instance);
    }

    VulkanConnection::~VulkanConnection() {
        if (_debugger.isValidationEnabled()) {
            _debugger.destroyDebugUtilsMessengerEXT(_instance, _debugger.debugMessenger, nullptr);
        }
        vkDestroyInstance(_instance, nullptr);
    }

    void VulkanConnection::_createInstance(const char* appName, const char* engineName) {
        if (_debugger.isValidationEnabled() && !_debugger.checkValidationLayerSupport()) {
            throw std::runtime_error("Validation layers requested but not available");
        }

        // Info about the application; optional but can help optimize
        const VkApplicationInfo appInfo{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = appName,
            .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
            .pEngineName = engineName,
            .apiVersion = VK_API_VERSION_1_0,
        };

        // Information about what extensions and validation layers to include
        const auto extensions = _getRequiredExtensions();
        VkInstanceCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &appInfo,
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
        };

        // Create Debugger for createInstance since the main one won't be created till after this is initialized
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

        // Include the validation layers
        if (_debugger.isValidationEnabled()) {
            std::cout << "Validation Layers are enabled.\n";
            createInfo.enabledLayerCount = _debugger.getValidationLayersCount();
            createInfo.ppEnabledLayerNames = _debugger.getValidationLayerNames();

            _populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
            throw std::runtime_error("Failed to create Vulkan Instance");
    }

    void VulkanConnection::_populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        // Details about the messenger and its callback
        createInfo = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = _debugger.debugCallback,
            .pUserData = nullptr,  // optional
        };
    }

    void VulkanConnection::_setupDebugMessenger() {
        if (!_debugger.isValidationEnabled()) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        _populateDebugMessengerCreateInfo(createInfo);

        if (_debugger.createDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugger.debugMessenger) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to setup the debug messenger.");
        }
    }

    std::vector<const char*> VulkanConnection::_getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (_debugger.isValidationEnabled()) {
            // Allows us to specify a debug callback to handle the debugging ourself
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        if (!_areRequiredExtSupported(extensions.data(), glfwExtensionCount))
            throw std::runtime_error("Some of the required extensions are not supported");

        return extensions;
    }

    bool VulkanConnection::_areRequiredExtSupported(const char** requiredExtensions,
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