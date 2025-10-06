#include "PhysicalDeviceSelection.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>

namespace pve {
    /**
     * Determines if a found device has the features we need.
     * @param device A physical device we are interested in.
     * @return Bool if the device is suitable for our use.
     */
    bool PhysicalDeviceSelection::_isDeviceSuitable(VkPhysicalDevice device) {
        // To evaluate the suitability of a device we can start by querying for some details
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        // We can also query for additional features
        // VkPhysicalDeviceFeatures deviceFeatures;
        // vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        QueueFamilyIndices indices = _findQueueFamilies(device);

        if (indices.isComplete()) {
            std::cout << "Selecting: " << deviceProperties.deviceName << "\n";
            return true;
        }

        return false;
    }

    /**
     * We need to check which queue families are supported by the device and which one of these supports the commands
     * that we want to use.
     * @param device A physical device we want to check support for.
     * @return All indices of the supported families we need.
     */
    PhysicalDeviceSelection::QueueFamilyIndices PhysicalDeviceSelection::_findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        // Query then store the queue family properties
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        // Need to check each found queue family if it was the graphics family. Then we know where to find it
        int i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (indices.isComplete()) break;

            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
            i++;
        }

        return indices;
    }

    /**
     * Looks over the devices on the system to find one we can use with Vulkan.
     * @param instance Reference to the current Vulkan Instance.
     */
    void PhysicalDeviceSelection::pickPhysicalDevice(VkInstance &instance) {
        // Get the number of devices
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        // Error if no devices found
        if (!deviceCount) {
            throw std::runtime_error("Failed to find GPUs with Vulkan Support.");
        }

        // Otherwise, store the list of found devices
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        // We'll need to check the devices to find one that is suitable for all our needs
        for (const auto &device : devices) {
            if (_isDeviceSuitable(device)) {
                // TODO: It's a good idea that we can grade each of the devices and pick the 'best' instead of the first
                _physicalDevice = device;
                break;
            }
        }

        if (_physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find suitable GPU.");
        }
    }

}