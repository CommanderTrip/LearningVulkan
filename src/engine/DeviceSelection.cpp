#include "DeviceSelection.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>

namespace pve {
    /**
     * Determines if a found device has the features we need.
     * @param device A physical device we are interested in.
     * @return Bool if the device is suitable for our use.
     */
    bool DeviceSelection::_isDeviceSuitable(VkPhysicalDevice device) {
        // To evaluate the suitability of a device we can start by querying for some details
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        // We can also query for additional features
        // VkPhysicalDeviceFeatures deviceFeatures;
        // vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        QueueFamilyIndices indices = _findQueueFamilies(device);

        if (indices.isComplete()) {
            // TODO: Should move this print out after this function call but I wanted to keep the code above here
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
    DeviceSelection::QueueFamilyIndices DeviceSelection::_findQueueFamilies(VkPhysicalDevice device) {
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
    void DeviceSelection::pickPhysicalDevice(VkInstance &instance) {
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

    /**
     * Picks a logical device to interface with the physical device.
     */
    void DeviceSelection::pickLogicalDevice(Debugger debugger) {
        // Why are we doing this again? Can't we save the results from the last time we did this?
        QueueFamilyIndices indices = _findQueueFamilies(_physicalDevice);

        float queuePriority = 1.0f;  // 0 to 1
        VkDeviceQueueCreateInfo queueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = indices.graphicsFamily.value(),
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,
        };

        // Don't need anything special now, but we might want to later
        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo deviceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queueCreateInfo,
            .pEnabledFeatures = &deviceFeatures,
        };

        /*
         * This is similar to the VkInstance createInfo where we defined extensions but this time it is device specific
         */

        // There used to be a difference between instance and device validation layers but that is no longer the case.
        // Still good to include it for backwards compatibility.
        deviceCreateInfo.enabledExtensionCount = 0;

        if (debugger.isValidationEnabled()) {
            deviceCreateInfo.enabledLayerCount = debugger.getValidationLayersCount();
            deviceCreateInfo.ppEnabledLayerNames = debugger.getValidationLayerNames();
        } else {
            deviceCreateInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr, &_device) != VK_FALSE) {
            throw std::runtime_error("Failed to create logical device.");
        }

        vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    }

}