#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <vector>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const char *validationLayers[] = {"VK_LAYER_KHRONOS_validation"};
const char *deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

// Because this function is an extension function, it is not automatically loaded. We have to look up its address
// ourselves
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

// We need to check which queue families are supported by the device
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentationFamily;  // In case the drawing queue and the presentation queue do not overlap

    bool isComplete() const { return graphicsFamily.has_value() && presentationFamily.has_value(); }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR> presentationModes;
};

class HelloTriangleApplication {
    GLFWwindow *window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;  // implicitly destroyed when `instance` is destroyed
    VkDevice device;
    VkQueue graphicsQueue;  // Queues are implicitly destroyed with the device is destroyed
    VkQueue presentationQueue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainsImages;
    VkFormat swapChainImageFormat;  // Needed for later after swap chain creation
    VkExtent2D swapChainExtent;     // Needed for later after swap chain creation
    std::vector<VkImageView> swapChainImageViews;

public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    void initWindow() {
        glfwInit();  // Initialize the GLFW Library

        // Do not create an OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        // Disable resizing for now since it needs special care
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        // Create the actual window
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void createInstance() {
        // This is the connection between my app and the Vulkan lib

        // Included later - Enable Vk Validation Layers
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("Validation layers requested but not available.");
        }

        // Info about our application
        // It's optional but helps the driver optimize
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vk First Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // Not optional and includes information on what Vulkan extensions and validation layers to include
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

        // GLFW will handle windows for us so we need to get the extensions it needs and pass it to Vulkan
        auto extensions = getRequiredExtensions();
        const uint32_t extensionsCount = extensions.size();
        checkSupportedExtensions(extensions.data(), &extensionsCount);

        createInfo.enabledExtensionCount = extensionsCount;
        createInfo.ppEnabledExtensionNames = extensions.data();

        // Now Includes validation layers
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = std::size(validationLayers);
            createInfo.ppEnabledLayerNames = validationLayers;

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        // That's everything specified. Create the Vulkan Instance. If everything goes well, then all the information is
        // stored in the instance handle. vkCreateInstance will either return VK_SUCCESS or an error for us to check
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vk Instance");
        }
    }

    std::vector<const char *> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (enableValidationLayers) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    void checkSupportedExtensions(const char **requiredExtensions, const uint32_t *requiredExtensionCount) {
        // Request just the number of supported Extensions
        uint32_t supportedExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);

        // Get the extension details
        std::vector<VkExtensionProperties> extensions(supportedExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, extensions.data());

        // Extra credit to check if the required extensions are supported
        std::set<std::string> supportedExtensionsLut{};

        for (const VkExtensionProperties &extension : extensions) {
            supportedExtensionsLut.insert({extension.extensionName});
        }

        std::cout << "Required Extensions:\n";
        for (int i = 0; i < *requiredExtensionCount; i++) {
            std::cout << "\t" << requiredExtensions[i];

            if (supportedExtensionsLut.contains(requiredExtensions[i])) {
                std::cout << " (supported)" << "\n";
            } else {
                std::cout << " (not supported)" << "\n";
            }
        }
    }

    bool checkValidationLayerSupport() {
        // Get the number of layer properties
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        // My method again for checking two lists
        std::unordered_map<std::string, bool> requestedValidationLayersLut{};
        for (const char *requestedLayer : validationLayers) {
            requestedValidationLayersLut.insert({requestedLayer, false});
        }

        for (const auto &layer : availableLayers) {
            if (requestedValidationLayersLut.contains(layer.layerName)) {
                requestedValidationLayersLut[layer.layerName] = true;
            }
        }

        for (auto &pair : requestedValidationLayersLut) {
            if (pair.second == false) return false;
        }

        return true;
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;

        // Setup the debugger
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        // Pass it to Vulkan
        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("Failed to set up debug messenger");
        }
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;  // Optional
        createInfo.flags = 0;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData) {
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

        // Indicates if the Vulkan call that triggered the validation layer message should be aborted
        return VK_FALSE;  // Normally keep false
    }

    void createSurface() {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface");
        }
    }

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) throw std::runtime_error("Failed to find GPUs that support Vulkan");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto &device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) throw std::runtime_error("No suitable GPUs");
    }

    bool isDeviceSuitable(VkPhysicalDevice device) {
        // Basic device properties like the name, type and supported Vulkan version
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        // Optional features like texture compression, 64-bit floats and multi viewport rendering
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        // As an example
        // return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
        // We could also score the devices and get the most suitable one

        // We care about supported: vulkan version, queue families, extensions, swap chain
        bool supportsVulkan1_3 = deviceProperties.apiVersion >= VK_VERSION_1_3;
        QueueFamilyIndices queueFamilies = findQueueFamilies(device);
        bool extensionsSupported = checkDeviceExtensionSupport(device);
        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.surfaceFormats.empty() && !swapChainSupport.presentationModes.empty();
        }

        return supportsVulkan1_3 && queueFamilies.isComplete() && extensionsSupported && swapChainAdequate;
    }

    bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(std::begin(deviceExtensions), std::end(deviceExtensions));

        for (const auto &extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.surfaceFormats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.surfaceFormats.data());
        }

        uint32_t presentationModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationModeCount, nullptr);
        if (presentationModeCount != 0) {
            details.presentationModes.resize(presentationModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationModeCount,
                                                      details.presentationModes.data());
        }

        return details;
    }

    // Chooses the best swap chain format; we will prefer 32-bit SRGB colors
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        for (const auto &availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    // The most important part of the swap chain with 4 different modes including immediate, v-sync, triple buffer, etc.
    VkPresentModeKHR chooseSwapPresentationMode(const std::vector<VkPresentModeKHR> &availablePresentationModes) {
        for (const auto &availableMode : availablePresentationModes) {
            // This mode is a good trade-off of low latency and no tearing if energy is not a concern
            if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR) return availableMode;
        }

        return VK_PRESENT_MODE_FIFO_KHR;  // v-sync similar option is the only one guaranteed to be available
    }

    // The resolution of the swap chain images. It's almost always exactly equal to the resolution of the window that
    // we're drawing to in pixels (can differ on high pixel-density displays)
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) return capabilities.currentExtent;

        // If the width was the max, we are dealing with a high density display and need to get true pixel locations
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
        };

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actualExtent;
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        // Find the indices in the queue families of the queues we need
        VkBool32 presentationSupport = false;
        int i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.graphicsFamily = i;

            // Will likely be the graphics family too but this is a more general support
            if (!presentationSupport) {
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);
                if (presentationSupport) indices.presentationFamily = i;
            }

            if (indices.isComplete()) break;
            i++;
        }

        return indices;
    }

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        std::set queueFamilySet = {indices.graphicsFamily.value(), indices.presentationFamily.value()};

        // This is not pre-allocated because values in the set could map to the same key, so the set could be smaller
        // than it appears e.g. graphics and presentation families are typically the same but might not be.
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        // Specifies the queues we want
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : queueFamilySet) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // The specifies what special features we want to use
        VkPhysicalDeviceFeatures deviceFeatures{};

        // Finally create the logical device
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;

        // Similar to VkInstanceCreateInfo but device specific
        createInfo.enabledExtensionCount = std::size(deviceExtensions);
        createInfo.ppEnabledExtensionNames = deviceExtensions;

        if (enableValidationLayers) {  // Newer versions don't need this but good for compatibility
            createInfo.enabledLayerCount = std::size(validationLayers);
            createInfo.ppEnabledLayerNames = validationLayers;
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentationFamily.value(), 0, &graphicsQueue);
    }

    void createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.surfaceFormats);
        VkPresentModeKHR presentMode = chooseSwapPresentationMode(swapChainSupport.presentationModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;

        // We need to specify how many images to keep in the swap chain. There is a required minimum amount, but it is
        // recommended to keep 1 more than the required minimum
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        // We also need to set below the maximum images allowed in the swap chain (0 means there is no max)
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        // Now the Create Info struct
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;  // Amount of layers each image has. Always 1 unless a 3D app
        createInfo.imageUsage =
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;  // What we are going to use the images in the chain for. We are
                                                  // rendering them directly so use this one

        // We now need to handle if images are used across queues. Again, the graphics and presentation queues are
        // typically the same, but it is possible it can differ
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentationFamily.value()};

        if (indices.graphicsFamily != indices.presentationFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;  // Images can be used across multiple queue
                                                                       // families without explicit ownership transfers
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode =
                VK_SHARING_MODE_EXCLUSIVE;  // An image is owned by one queue family at a time and ownership must be
                                            // explicitly transferred before using it in another queue family. This
                                            // option offers the best performance
            createInfo.queueFamilyIndexCount = 0;      // Optional
            createInfo.pQueueFamilyIndices = nullptr;  // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;  // Dont perform any extra transforms
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;             // No blending on the alpha channel
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;  // Clips out pixels if the window is obscured
        createInfo.oldSwapchain =
            VK_NULL_HANDLE;  // Many conditions can cause the swap chain to be invalidated and new chain must be created
                             // (such as window resizing). This is complex so assume only 1 chain for now

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create swap chain");
        }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainsImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainsImages.data());
    }

    // An image view is quite literally a view into an image. It describes how to access the image and which part of the
    // image to access.
    void createImageViews() {
        swapChainImageViews.resize(swapChainsImages.size());

        for (size_t i = 0; i < swapChainsImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainsImages[i];

            // Specify how the image should be interpreted
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;

            // Keep default color mappings
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            // Describe what the image's purpose is and which part of the image should be accessed. Our images will be
            // used as color targets without any mipmapping levels or multiple layers.
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create image views");
            }
        }
    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        vkDestroyDevice(device, nullptr);
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
