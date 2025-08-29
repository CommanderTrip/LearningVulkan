#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
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

class HelloTriangleApplication {
    GLFWwindow *window;
    VkInstance instance;

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

        // Inserted later - Enable Vk Validation Layers
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

        // Now Includes validation layers
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = std::size(validationLayers);
            createInfo.ppEnabledLayerNames = validationLayers;
        } else {
            createInfo.enabledLayerCount = 0;
        }

        // GLFW will handle windows for us so we need to get the extensions it needs and pass it to Vulkan
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        checkSupportedExtensions(glfwExtensions, &glfwExtensionCount);
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        // Leaving validation layers empty for now
        createInfo.enabledLayerCount = 0;

        // That's everything specified. Create the Vulkan Instance. If everything goes well, then all the information is
        // stored in the instance handle. vkCreateInstance will either return VK_SUCCESS or an error for us to check
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vk Instance");
        }
    }

    void checkSupportedExtensions(const char **requiredExtensions, const uint32_t *extensionCount) {
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
        for (int i = 0; i < *extensionCount; i++) {
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
        std::unordered_map<std::string, int> requestedValidationLayersLut{};
        for (const char *requestedLayer : validationLayers) {
            requestedValidationLayersLut.insert({requestedLayer, 0});
        }

        for (const auto &layer : availableLayers) {
            if (requestedValidationLayersLut.contains(layer.layerName)) {
                requestedValidationLayersLut[layer.layerName]++;
            }
        }

        for (auto &pair : requestedValidationLayersLut) {
            if (pair.second == 0) return false;
        }

        return true;
    }

    void initVulkan() { createInstance(); }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
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
