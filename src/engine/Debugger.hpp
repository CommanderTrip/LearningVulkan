#pragma once
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <iterator>

namespace pve {
    class Debugger {
#ifdef NDEBUG
        const bool _enableValidationLayers = false;
#else
        const bool _enableValidationLayers = true;
#endif
        const char *const _validationLayers[1] = {"VK_LAYER_KHRONOS_validation"};

    public:
        Debugger() = default;
        ~Debugger();

        bool isValidationEnabled() const { return _enableValidationLayers; }
        uint32_t getValidationLayersCount() const { return std::size(_validationLayers); }
        const char *const *getValidationLayerNames() const { return _validationLayers; }
        bool areValidationLayersSupported() const;

        // Because this function is an extension function, it is not automatically loaded, and we have to find it
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                              const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                              const VkAllocationCallbacks *pAllocator,
                                              VkDebugUtilsMessengerEXT *pDebugMessenger);
    };
}
