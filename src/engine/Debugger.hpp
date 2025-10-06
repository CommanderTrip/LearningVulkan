#pragma once
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <iterator>

namespace pve {
    class Debugger {
// Changes depending on the build mode.
#ifdef NDEBUG
        const bool _enableValidationLayers = false;
#else
        const bool _enableValidationLayers = true;
#endif
        const char *const _validationLayers[1] = {"VK_LAYER_KHRONOS_validation"};

    public:
        VkDebugUtilsMessengerEXT debugMessenger = nullptr;

        Debugger() = default;
        ~Debugger() = default;

        bool isValidationEnabled() const { return _enableValidationLayers; }
        uint32_t getValidationLayersCount() const { return std::size(_validationLayers); }
        const char *const *getValidationLayerNames() const { return _validationLayers; }
        bool checkValidationLayerSupport() const;

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                            void *pUserData);

        VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                              const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                              const VkAllocationCallbacks *pAllocator,
                                              VkDebugUtilsMessengerEXT *pDebugMessenger);

        void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                           const VkAllocationCallbacks *pAllocator);
    };
}
