#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const char *validationLayers[] = {"VK_LAYER_KHRONOS_validation"};

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

    bool isComplete() { return graphicsFamily.has_value(); }
};

class HelloTriangleApplication {
    GLFWwindow *window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;  // implicitly destroyed when `instance` is destroyed
    VkDevice device;
    VkQueue graphicsQueue;  // Queues are implicitly destroyed with the device is destroyed

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
        std::unordered_map<std::string, std::string> supportedExtensionsLut{};

        for (const VkExtensionProperties &extension : extensions) {
            supportedExtensionsLut.insert({extension.extensionName, extension.extensionName});
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

        // Since all we care about is Vulkan support and queue families right now
        bool supportsVulkan1_3 = deviceProperties.apiVersion >= VK_VERSION_1_3;
        QueueFamilyIndices queueFamilies = findQueueFamilies(device);

        return supportsVulkan1_3 && queueFamilies.isComplete();
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        // Find the indices in the queue families of the queues we need
        int i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.graphicsFamily = i;
            if (indices.isComplete()) break;
            i++;
        }

        return indices;
    }

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        // Specifies the queues we want
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        // The specifies what special features we want to use
        VkPhysicalDeviceFeatures deviceFeatures{};

        // Finally create the logical device
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;

        // Similar to VkInstanceCreateInfo but device specific
        // Don't need extensions for now
        createInfo.enabledExtensionCount = 0;

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
    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        pickPhysicalDevice();
        createLogicalDevice();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroyDevice(device, nullptr);
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
