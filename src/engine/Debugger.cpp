#include "Debugger.hpp"

#include <vulkan/vulkan_core.h>

#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace pve {
    bool Debugger::checkValidationLayerSupport() const {
        // Similar to pve::PveVulkanConnection::_areRequiredExtSupported

        // Get the layer properties
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        // Verify validation layers are supported
        std::set<std::string> requestedValidationLayersLut{};
        for (const char *requestedLayer : _validationLayers) {
            requestedValidationLayersLut.insert(requestedLayer);
        }

        for (const auto &layer : availableLayers) {
            if (requestedValidationLayersLut.contains(layer.layerName)) {
                requestedValidationLayersLut.erase(layer.layerName);
            }
        }

        return requestedValidationLayersLut.size() == 0;
    }

    /**
     * @param messageSeverity Specifies the severity of the message.
     * @param messageType Specifies the type of the message.
     * @param pCallbackData Pointer to struct containing the details of the message itself.
     * @param pUserData Pointer that was specified during setup of the callback and allows you to pass your own data.
     * @return Boolean that indicates if the Vulkan call that triggered the validation layer message should be aborted.
     */
    VkBool32 Debugger::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                     const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
        /* messageSeverity Types
         * VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: Diagnostic message
         * VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: Informational message like the creation of a resource
         * VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: Message about behavior that is not necessarily an error, but
         * very likely a bug in your application.
         * VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: Message about behavior that is invalid and may cause crashes
         */

        /* messageType Types
         * VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: Some event has happened that is unrelated to the specification
         * or performance
         * VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: Something has happened that violates the
         * specification or indicates a possible mistake
         * VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: Potential
         * non-optimal use of Vulkan
         */

        /* pCallbackData important members
         * pMessage: The debug message as a null-terminated string
         * pObjects: Array of Vulkan object handles related to the message
         * objectCount: Number of objects in array
         */
        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                std::cout << "[DEBUG]\t" << pCallbackData->pMessage << "\n";
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                std::cout << "[WARN]\t" << pCallbackData->pMessage << "\n";
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                std::cerr << "[ERROR]\t" << pCallbackData->pMessage << "\n";
                break;
            default:
                std::cerr << "[UNKNOWN]\t" << pCallbackData->pMessage << "\n";
        }

        // If the callback returns true, then the call is aborted with the VK_ERROR_VALIDATION_FAILED_EXT error. This is
        // normally only used to test the validation layers themselves, so you should always return VK_FALSE.
        return VK_FALSE;
    }

    /**
     * To create the debugger, we need to pass the debugger create info struct to the vkCreateDebugUtilsMessengerEXT.
     * Because this function is an extension function, it is not automatically loaded. We have to look up its address
     * ourselves.
     * @param instance
     * @param pCreateInfo
     * @param pAllocator
     * @param pDebugMessenger
     * @return
     */
    VkResult Debugger::createDebugUtilsMessengerEXT(VkInstance instance,
                                                    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                    const VkAllocationCallbacks *pAllocator,
                                                    VkDebugUtilsMessengerEXT *pDebugMessenger) {
        auto func =
            (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    /**
     * Similar to the createDebugUtilsMessengerEXT, we need to find the address of the destroy function to clean up the
     * debugger properly.
     * @param instance
     * @param debugMessenger
     * @param pAllocator
     */
    void Debugger::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                                 const VkAllocationCallbacks *pAllocator) {
        auto func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

}