#include <vulkan/vulkan_core.h>

#include <set>
#include <string>
#include <vector>

#include "Debugger.hpp"

namespace pve {
    Debugger::~Debugger() {}

    bool Debugger::areValidationLayersSupported() const {
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
}