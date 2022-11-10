#ifndef MODEL_FACTORY_HPP
#define MODEL_FACTORY_HPP
#include "components.hpp"
#include "../rectangleShape.h"
#include <GLFW/glfw3.h>
#include <set>
namespace odfaeg {
    namespace graphic {
        namespace ecs {
        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };
        const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        #ifdef NDEBUG
        const bool enableValidationLayers = false;
        #else
        const bool enableValidationLayers = true;
        #endif
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr) {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr) {
                func(instance, debugMessenger, pAllocator);
            }
        }
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

            return VK_FALSE;
        }
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
            createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = debugCallback;
        }

        std::vector<const char*> getRequiredExtensions() {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

            if (enableValidationLayers) {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            return extensions;
        }

        bool checkValidationLayerSupport() {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const char* layerName : validationLayers) {
                bool layerFound = false;

                for (const auto& layerProperties : availableLayers) {
                    if (strcmp(layerName, layerProperties.layerName) == 0) {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound) {
                    return false;
                }
            }

            return true;
        }
        bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

            std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

            for (const auto& extension : availableExtensions) {
                requiredExtensions.erase(extension.extensionName);
            }

            return requiredExtensions.empty();
        }
        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;

            bool isComplete() {
                return graphicsFamily.has_value() && presentFamily.has_value();
            }
        };
        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };
        template <typename DynamicTuple>
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, DynamicTuple& tuple, ComponentMapping renderer, EntityId vkRendererId) {
            QueueFamilyIndices indices;

            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

            int i = 0;
            for (const auto& queueFamily : queueFamilies) {
                if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    indices.graphicsFamily = i;
                }
                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, renderer.getAgregate<VkSurfaceKHRCp>(tuple, vkRendererId), &presentSupport);
                if (presentSupport) {
                    indices.presentFamily = i;
                }
                if (indices.isComplete()) {
                    break;
                }

                i++;
            }

            return indices;
        }
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
            for (const auto& availableFormat : availableFormats) {
                if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    return availableFormat;
                }
            }

            return availableFormats[0];
        }

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
            for (const auto& availablePresentMode : availablePresentModes) {
                if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    return availablePresentMode;
                }
            }

            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
            if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
                return capabilities.currentExtent;
            } else {
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);

                VkExtent2D actualExtent = {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
                };

                actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

                return actualExtent;
            }
        }
        template <typename DynamicTuple>
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, DynamicTuple& tuple, ComponentMapping renderer, EntityId vkRendererId) {
            SwapChainSupportDetails details;

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, renderer.getAgregate<VkSurfaceKHRCp>(tuple, vkRendererId).surface, &details.capabilities);

            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, renderer.getAgregate<VkSurfaceKHRCp>(tuple, vkRendererId).surface, &formatCount, nullptr);

            if (formatCount != 0) {
                details.formats.resize(formatCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, renderer.getAgregate<VkSurfaceKHRCp>(tuple, vkRendererId).surface, &formatCount, details.formats.data());
            }

            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, renderer.getAgregate<VkSurfaceKHRCp>(tuple, vkRendererId).surface, &presentModeCount, nullptr);

            if (presentModeCount != 0) {
                details.presentModes.resize(presentModeCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, renderer.getAgregate<VkSurfaceKHRCp>(tuple, vkRendererId).surface, &presentModeCount, details.presentModes.data());
            }

            return details;
        }
        template <typename DynamicTuple>
        bool isDeviceSuitable(VkPhysicalDevice device, DynamicTuple& tuple, ComponentMapping renderer, EntityId vkRendererId) {
            QueueFamilyIndices indices = findQueueFamilies(device, tuple, renderer, vkRendererId);
            bool extensionsSupported = checkDeviceExtensionSupport(device);
            return indices.isComplete() && extensionsSupported;
        }
        class ModelFactory {
            struct uint64_to_uint128 {
                uint64_t handle;
                uint64_t padding;
            };
            struct Samplers {
                std::vector<uint64_to_uint128> texs;
            };
            public :
            template <typename DynamicTuple, typename Factory>
            static auto createVkInstance (DynamicTuple& tuple, ComponentMapping renderer, EntityId vkRendererId, VkApplicationInfo appInfo, Factory& factory, VkInstanceCreateInfo createInfo) {
                VkInstanceCp instanceCp;
                DynamicTuple newtuple = renderer.addFlag(vkRendererId, tuple, instanceCp, factory);
                if (enableValidationLayers && !checkValidationLayerSupport()) {
                    throw std::runtime_error("validation layers requested, but not available!");
                }
                createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                createInfo.pApplicationInfo = &appInfo;

                auto extensions = getRequiredExtensions();
                createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
                createInfo.ppEnabledExtensionNames = extensions.data();

                VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
                if (enableValidationLayers) {
                    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                    createInfo.ppEnabledLayerNames = validationLayers.data();

                    populateDebugMessengerCreateInfo(debugCreateInfo);
                    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
                } else {
                    createInfo.enabledLayerCount = 0;

                    createInfo.pNext = nullptr;
                }

                if (vkCreateInstance(&createInfo, nullptr, &renderer.getAgregate<VkInstanceCp>(newtuple, vkRendererId).instance) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create instance!");
                }
                return newtuple;
            }
            template <typename DynamicTuple, typename Factory>
            static auto setupDebugMessenger (DynamicTuple& tuple, ComponentMapping renderer, EntityId vkRendererId, Factory& factory) {
                if (!enableValidationLayers) return;
                VkDebugMessengerCp debugMessengerCp;
                DynamicTuple newtuple = renderer.addFlag(vkRendererId, tuple, debugMessengerCp, factory);
                VkDebugUtilsMessengerCreateInfoEXT createInfo;
                populateDebugMessengerCreateInfo(createInfo);

                if (CreateDebugUtilsMessengerEXT(renderer.getAgregate<VkInstanceCp>(newtuple, vkRendererId).instance, &renderer.getAgregate<VkDebugMessengerCp>(newtuple, vkRendererId).debugMessenger, nullptr, &renderer.getAgregate<VkDebugMessengerCp>(newtuple, vkRendererId).debugMessenger) != VK_SUCCESS) {
                    throw std::runtime_error("failed to set up debug messenger!");
                }
                return newtuple;
            }
            template <typename DynamicTuple, typename Factory>
            static auto pickPhysicalDevice (DynamicTuple& tuple, ComponentMapping renderer, EntityId vkRendererId, Factory& factory) {
                uint32_t deviceCount = 0;
                vkEnumeratePhysicalDevices(renderer.getAgregate<VkInstanceCp>(tuple, vkRendererId).instance, &deviceCount, nullptr);

                if (deviceCount == 0) {
                    throw std::runtime_error("failed to find GPUs with Vulkan support!");
                }
                VkPhysicalDeviceCp physicalDevice;
                DynamicTuple newtuple = renderer.addFlag(vkRendererId, tuple, physicalDevice, factory);
                std::vector<VkPhysicalDevice> devices(deviceCount);
                vkEnumeratePhysicalDevices(renderer.getAgregate<VkInstanceCp>(newtuple, vkRendererId).instance, &deviceCount, devices.data());

                for (const auto& device : devices) {
                    if (isDeviceSuitable(device, newtuple, renderer, vkRendererId)) {
                        renderer.getAgregate<VkPhysicalDeviceCp>(newtuple, vkRendererId) = device;
                        break;
                    }
                }

                if (renderer.getAgregate<VkPhysicalDeviceCp>(newtuple, vkRendererId) == VK_NULL_HANDLE) {
                    throw std::runtime_error("failed to find a suitable GPU!");
                }
                return newtuple;
            }
            template <typename DynamicTuple, typename Factory>
            static auto createLogicalDevice (DynamicTuple& tuple, ComponentMapping renderer, EntityId vkRendererId, Factory& factory) {
                VkLogicalDeviceCp logicalDevice;
                DynamicTuple newtuple = renderer.addFlag(vkRendererId, tuple, logicalDevice, factory);
                QueueFamilyIndices indices = findQueueFamilies(renderer.getAgregate<VkPhysicalDeviceCp>(newtuple, vkRendererId).physicalDevice);
                std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
                std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
                float queuePriority = 1.0f;
                for (uint32_t queueFamily : uniqueQueueFamilies) {
                    VkDeviceQueueCreateInfo queueCreateInfo{};
                    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                    queueCreateInfo.queueFamilyIndex = queueFamily;
                    queueCreateInfo.queueCount = 1;
                    queueCreateInfo.pQueuePriorities = &queuePriority;
                    queueCreateInfos.push_back(queueCreateInfo);
                }

                VkPhysicalDeviceFeatures deviceFeatures{};

                VkDeviceCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

                createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
                createInfo.pQueueCreateInfos = queueCreateInfos.data();

                createInfo.pEnabledFeatures = &deviceFeatures;

                createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
                createInfo.ppEnabledExtensionNames = deviceExtensions.data();

                if (enableValidationLayers) {
                    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                    createInfo.ppEnabledLayerNames = validationLayers.data();
                } else {
                    createInfo.enabledLayerCount = 0;
                }

                if (vkCreateDevice(renderer.getAgregate<VkPhysicalDeviceCp>(newtuple, vkRendererId).physicalDevice, &createInfo, nullptr, &renderer.getAgregate<VkLogicalDeviceCp>(newtuple, vkRendererId).device) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create logical device!");
                }

                vkGetDeviceQueue(renderer.getAgregate<VkLogicalDeviceCp>(newtuple, vkRendererId).device, indices.graphicsFamily.value(), 0, &renderer.getAgregate<VkLogicalDeviceCp>(newtuple, vkRendererId).graphicsQueue);
                vkGetDeviceQueue(renderer.getAgregate<VkLogicalDeviceCp>(newtuple, vkRendererId).device, indices.graphicsFamily.value(), 0, &renderer.getAgregate<VkLogicalDeviceCp>(newtuple, vkRendererId).presentQueue);
            }
            template <typename DynamicTuple, typename Factory>
            static auto createSurface (DynamicTuple tuple, ComponentMapping renderer, EntityId vkRendererId, Factory& factory,GLFWwindow* window) {
                VkSurfaceKHRCp surface;
                DynamicTuple newtuple = renderer.addFlag(vkRendererId, tuple, surface, factory);
                if (glfwCreateWindowSurface(renderer.getAgregate<VkInstanceCp>(newtuple, vkRendererId), window, nullptr, &renderer.getAgregate<VkSurfaceKHRCp>(newtuple,vkRendererId).surface) != VK_SUCCESS) {
                    throw std::runtime_error("échec de la création de la window surface!");
                }
            }
            template <typename DynamicTuple, typename Factory>
            static auto createSwapchain (DynamicTuple& tuple, ComponentMapping renderer, EntityId vkRendererId, Factory& factory, GLFWwindow* window) {
                VkSwapchainCp swapChain;

                SwapChainSupportDetails swapChainSupport = querySwapChainSupport(renderer.getAgregate<VkPhysicalDeviceCp>(tuple, vkRendererId).physicalDevice, tuple, renderer, vkRendererId);

                VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
                VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
                VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

                uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
                if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
                    imageCount = swapChainSupport.capabilities.maxImageCount;
                }

                VkSwapchainCreateInfoKHR createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                createInfo.surface = renderer.getAgregate<VkSurfaceKHRCp>(tuple, vkRendererId);

                createInfo.minImageCount = imageCount;
                createInfo.imageFormat = surfaceFormat.format;
                createInfo.imageColorSpace = surfaceFormat.colorSpace;
                createInfo.imageExtent = extent;
                createInfo.imageArrayLayers = 1;
                createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

                QueueFamilyIndices indices = findQueueFamilies(renderer.getAgregate<VkPhysicalDeviceCp>(tuple, vkRendererId).physicalDevice, tuple, renderer, vkRendererId);
                uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

                if (indices.graphicsFamily != indices.presentFamily) {
                    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                    createInfo.queueFamilyIndexCount = 2;
                    createInfo.pQueueFamilyIndices = queueFamilyIndices;
                } else {
                    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                }

                createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
                createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                createInfo.presentMode = presentMode;
                createInfo.clipped = VK_TRUE;

                createInfo.oldSwapchain = VK_NULL_HANDLE;

                if (vkCreateSwapchainKHR(renderer.getAgregate<VkLogicalDeviceCp>(tuple, vkRendererId).device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create swap chain!");
                }

                vkGetSwapchainImagesKHR(renderer.getAgregate<VkLogicalDeviceCp>(tuple, vkRendererId).device, swapChain, &imageCount, nullptr);
                swapChain.swapChainImages.resize(imageCount);
                vkGetSwapchainImagesKHR(renderer.getAgregate<VkLogicalDeviceCp>(tuple, vkRendererId).device, swapChain, &imageCount, swapChain.swapChainImages.data());

                swapChain.swapChainImageFormat = surfaceFormat.format;
                swapChain.swapChainExtent = extent;
                DynamicTuple newtuple = renderer.addAgregate(vkRendererId, tuple, swapChain, factory);
                return newtuple;
            }
            template <typename DynamicTuple, typename Factory>
            static auto createImagesViews (DynamicTuple& tuple, ComponentMapping renderer, EntityId vkRendererId, Factory& factory) {
                VkImageViewCp imageView;
                imageView.imagesViews.resize(renderer.getAgregate<VkSwapchainCp>(tuple, vkRendererId).swapChainImages.size());
                std::vector<VkImageView> swapChainImages = renderer.getAgregate<VkImageViewCp>(tuple, vkRendererId).imagesViews;
                for (size_t i = 0; i < swapChainImages.size(); i++) {
                    VkImageViewCreateInfo createInfo{};
                    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    createInfo.image = swapChainImages[i];
                    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    createInfo.format = renderer.getAgregate<VkSwapchainCp>(tuple, vkRendererId).swapChainImageFormat;
                    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    createInfo.subresourceRange.baseMipLevel = 0;
                    createInfo.subresourceRange.levelCount = 1;
                    createInfo.subresourceRange.baseArrayLayer = 0;
                    createInfo.subresourceRange.layerCount = 1;

                    if (vkCreateImageView(renderer.getAgregate<VkLogicalDeviceCp>(tuple, vkRendererId).device, &createInfo, nullptr, &renderer.getAgregate<VkImageViewCp>(tuple, vkRendererId).imagesViews[i]) != VK_SUCCESS) {
                        throw std::runtime_error("failed to create image views!");
                    }
                }
            }
            template <typename DynamicTuple>
            static void cleanup(DynamicTuple& tuple, ComponentMapping renderer, EntityId vkRendererId) {
                for (auto imageView : renderer.getAgregate<VkImageViewCp>(tuple, vkRendererId).swapChainImageViews) {
                    vkDestroyImageView(renderer.getAgregate<VkLogicalDeviceCp>(tuple, vkRendererId).device, imageView, nullptr);
                }

                vkDestroySwapchainKHR(renderer.getAgregate<VkLogicalDeviceCp>(tuple, vkRendererId).device, renderer.getAgregate<VkSwapchainCp>(tuple, vkRendererId).swapChain, nullptr);
                vkDestroyDevice(renderer.getAgregate<VkLogicalDeviceCp>(tuple, vkRendererId).device, nullptr);

                if (enableValidationLayers) {
                    DestroyDebugUtilsMessengerEXT(renderer.getAgregate<VkInstanceCp>(tuple, vkRendererId), renderer.getAgregate<VkDebugMessengerCp>(tuple, vkRendererId).debugMessenger, nullptr);
                }


                vkDestroySurfaceKHR(renderer.getAgregate<VkInstanceCp>(tuple, vkRendererId).instance, renderer.getAgregate<VkSurfaceKHRCp>(tuple,vkRendererId).surface, nullptr);
                vkDestroyInstance(renderer.getAgregate<VkInstanceCp>(tuple, vkRendererId).instance, nullptr);
            }
            template <typename World, typename DynamicArray, typename Factory>
            static auto createPerPixelLinkedListBindlessRenderComponent(World& world, Factory factory, EntityId rendererId, DynamicArray& da, RenderWindow& window, int layer, std::string expression) {
                PerPixelLinkedListBindlessPass1RenderComponent* perPixelLinkedListBindlessPass1 = new PerPixelLinkedListBindlessPass1RenderComponent(window);
                perPixelLinkedListBindlessPass1->expression = expression;
                GLuint maxNodes = 20 * window.getView().getSize().x * window.getView().getSize().y;
                GLint nodeSize = 5 * sizeof(GLfloat) + sizeof(GLuint);
                sf::Vector3i resolution = sf::Vector3i((int) window.getSize().x, (int) window.getSize().y, window.getView().getSize().z);
                GLuint atomicBuffer, linkedListBuffer, headPtrTex, clearBuf;
                glCheck(glGenBuffers(1, &atomicBuffer));
                glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicBuffer));
                glCheck(glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW));
                glCheck(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));
                glCheck(glGenBuffers(1, &linkedListBuffer));
                glCheck(glBindBuffer(GL_SHADER_STORAGE_BUFFER, linkedListBuffer));
                glCheck(glBufferData(GL_SHADER_STORAGE_BUFFER, maxNodes * nodeSize, nullptr, GL_DYNAMIC_DRAW));
                glCheck(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
                glCheck(glGenTextures(1, &headPtrTex));
                glCheck(glBindTexture(GL_TEXTURE_2D, headPtrTex));
                glCheck(glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, window.getView().getSize().x, window.getView().getSize().y));
                glCheck(glBindImageTexture(0, headPtrTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));
                glCheck(glBindTexture(GL_TEXTURE_2D, 0));
                std::vector<GLuint> headPtrClearBuf(window.getView().getSize().x*window.getView().getSize().y, 0xffffffff);
                glCheck(glGenBuffers(1, &clearBuf));
                glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf));
                glCheck(glBufferData(GL_PIXEL_UNPACK_BUFFER, headPtrClearBuf.size() * sizeof(GLuint),
                &headPtrClearBuf[0], GL_STATIC_COPY));
                glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));
                perPixelLinkedListBindlessPass1->atomicBuffer = atomicBuffer;
                perPixelLinkedListBindlessPass1->linkedListBuffer = linkedListBuffer;
                perPixelLinkedListBindlessPass1->headPtrTex = headPtrTex;
                perPixelLinkedListBindlessPass1->clearBuf = clearBuf;
                perPixelLinkedListBindlessPass1->layer = layer;
                const std::string  vertexShader = R"(#version 460
                                                        layout (location = 0) in vec3 position;
                                                        layout (location = 1) in vec4 color;
                                                        layout (location = 2) in vec2 texCoords;
                                                        layout (location = 3) in vec3 normals;
                                                        layout (location = 4) in uint textureIndex;
                                                        uniform mat4 textureMatrix[)"+core::conversionUIntString(Texture::getAllTextures().size())+R"(];
                                                        uniform mat4 projectionMatrix;
                                                        uniform mat4 viewMatrix;
                                                        uniform vec3 resolution;
                                                        out vec2 fTexCoords;
                                                        out vec4 frontColor;
                                                        out uint texIndex;
                                                        void main () {
                                                            gl_Position = projectionMatrix * viewMatrix * vec4(position.xyz, 1.f);
                                                            fTexCoords = (textureIndex != 0) ? (textureMatrix[textureIndex-1] * vec4(texCoords, 1.f, 1.f)).xy : texCoords;
                                                            frontColor = color;
                                                            texIndex = textureIndex;
                                                        })";
                const std::string fragmentShader = R"(#version 460
                                                  #extension GL_ARB_bindless_texture : enable
                                                  struct NodeType {
                                                      vec4 color;
                                                      float depth;
                                                      uint next;
                                                  };
                                                  layout(binding = 0, offset = 0) uniform atomic_uint nextNodeCounter;
                                                  layout(binding = 0, r32ui) uniform uimage2D headPointers;
                                                  layout(binding = 0, std430) buffer linkedLists {
                                                      NodeType nodes[];
                                                  };
                                                  layout(std140, binding = 0) uniform ALL_TEXTURES {
                                                      sampler2D textures[)"+core::conversionUIntString(Texture::getAllTextures().size())+R"(];
                                                  };
                                                  uniform uint maxNodes;
                                                  uniform float haveTexture;
                                                  uniform sampler2D currentTex;
                                                  in vec4 frontColor;
                                                  in vec2 fTexCoords;
                                                  in flat uint texIndex;
                                                  layout (location = 0) out vec4 fcolor;
                                                  void main() {
                                                       uint nodeIdx = atomicCounterIncrement(nextNodeCounter);
                                                       vec4 color = (texIndex != 0) ? frontColor * texture(textures[texIndex-1], fTexCoords.xy) : frontColor;
                                                       if (nodeIdx < maxNodes) {
                                                            uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx);
                                                            nodes[nodeIdx].color = color;
                                                            nodes[nodeIdx].depth = gl_FragCoord.z;
                                                            nodes[nodeIdx].next = prevHead;
                                                       }
                                                       fcolor = vec4(0, 0, 0, 0);
                                                  })";
                 Shader& perPixelLinkedList = perPixelLinkedListBindlessPass1->perPixelLinkedListShader;
                 if (!perPixelLinkedList.loadFromMemory(vertexShader, fragmentShader)) {
                     throw core::Erreur(54, "Failed to load per pixel linked list shader");
                 }
                 std::vector<Texture*> allTextures = Texture::getAllTextures();
                 Samplers allSamplers{};
                 std::vector<math::Matrix4f> textureMatrices;
                 for (unsigned int i = 0; i < allTextures.size(); i++) {
                     textureMatrices.push_back(allTextures[i]->getTextureMatrix());
                     GLuint64 handle_texture = allTextures[i]->getTextureHandle();
                     allTextures[i]->makeTextureResident(handle_texture);
                     uint64_to_uint128 texHandle;
                     texHandle.handle = handle_texture;
                     allSamplers.texs.push_back(texHandle);
                 }

                 perPixelLinkedList.setParameter("textureMatrix", textureMatrices);
                 perPixelLinkedList.setParameter("maxNodes", maxNodes);
                 GLuint ubo;
                 glCheck(glGenBuffers(1, &ubo));
                 unsigned int ubid;
                 glCheck(ubid = glGetUniformBlockIndex(perPixelLinkedList.getHandle(), "ALL_TEXTURES"));
                 glCheck(glUniformBlockBinding(perPixelLinkedList.getHandle(),    ubid, 0));
                 glCheck(ubid = glGetUniformBlockIndex(perPixelLinkedList.getHandle(), "ALL_TEXTURES"));
                 glCheck(glUniformBlockBinding(perPixelLinkedList.getHandle(),    ubid, 0));
                 glCheck(glBindBuffer(GL_UNIFORM_BUFFER, ubo));
                 glCheck(glBufferData(GL_UNIFORM_BUFFER, sizeof(Samplers),allSamplers.texs.data(), GL_STATIC_DRAW));
                 glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));

                 glCheck(glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo));
                 glCheck(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicBuffer));
                 glCheck(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, linkedListBuffer));

                 for (unsigned int i = 0; i < Batcher::nbPrimitiveTypes; i++) {
                     perPixelLinkedListBindlessPass1->vbBindlessTex[i].setPrimitiveType(static_cast<sf::PrimitiveType>(i));
                 }
                 window.setActive(false);
                 auto da2 = world.addRendererFlag(da, rendererId, perPixelLinkedListBindlessPass1, factory);
                 return createPerPixelLinkedListBindlessRenderComponentPass2(world, factory, rendererId, da2, window, layer, expression);
             }
             template <typename World, typename DynamicArray, typename Factory>
             static auto createPerPixelLinkedListBindlessRenderComponentPass2(World& world, Factory factory, EntityId rendererId, DynamicArray& da, RenderWindow& window, int layer, std::string expression) {
                 RectangleShape quad(math::Vec3f(window.getView().getSize().x, window.getView().getSize().y, window.getSize().y * 0.5f));
                 quad.move(math::Vec3f(-window.getView().getSize().x * 0.5f, -window.getView().getSize().y * 0.5f, 0));
                 PerPixelLinkedListBindlessPass2RenderComponent* perPixelLinkedListBindlessPass2 = new PerPixelLinkedListBindlessPass2RenderComponent(window);
                 const std::string  vertexShader = R"(#version 460
                                                    layout (location = 0) in vec3 position;
                                                    layout (location = 1) in vec4 color;
                                                    layout (location = 2) in vec2 texCoords;
                                                    layout (location = 3) in vec3 normals;
                                                    uniform mat4 projectionMatrix;
                                                    uniform mat4 viewMatrix;
                                                    uniform mat4 worldMat;
                                                    void main () {
                                                        gl_Position = projectionMatrix * viewMatrix * worldMat * vec4(position, 1.f);
                                                    })";
                 const std::string fragmentShader =
                 R"(
                 #version 460
                 #define MAX_FRAGMENTS 20
                 struct NodeType {
                    vec4 color;
                    float depth;
                    uint next;
                 };
                 layout(binding = 0, r32ui) uniform uimage2D headPointers;
                 layout(binding = 0, std430) buffer linkedLists {
                     NodeType nodes[];
                 };
                 uniform vec4 bg_color;
                 layout(location = 0) out vec4 fcolor;
                 void main() {
                    NodeType frags[MAX_FRAGMENTS];
                    int count = 0;
                    uint n = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;
                    while( n != 0xffffffffu && count < MAX_FRAGMENTS) {
                        frags[count] = nodes[n];
                        n = frags[count].next;
                        count++;
                    }
                    //Insertion sort.
                    for (int i = 0; i < count - 1; i++) {
                      for (int j = i + 1; j > 0; j--) {
                          if (frags[j - 1].depth > frags[j].depth) {
                             NodeType tmp = frags[j - 1];
                             frags[j - 1] = frags[j];
                             frags[j] = tmp;
                          }
                       }
                    }
                    vec4 color = bg_color;
                    for( int i = 0; i < count; i++)
                    {
                      color.rgb = frags[i].color.rgb * frags[i].color.a + color.rgb * (1 - frags[i].color.a);
                      color.a = frags[i].color.a + color.a * (1 - frags[i].color.a);
                    }
                    fcolor = color;
                 })";
                 Shader& perPixelLinkedList = perPixelLinkedListBindlessPass2->perPixelLinkedListShader;
                 if (!perPixelLinkedList.loadFromMemory(vertexShader, fragmentShader)) {
                     throw core::Erreur(56, "Failed to load per pixel linked list 2 shader");
                 }
                 math::Matrix4f viewMatrix = window.getDefaultView().getViewMatrix().getMatrix().transpose();
                 math::Matrix4f projMatrix = window.getDefaultView().getProjMatrix().getMatrix().transpose();
                 perPixelLinkedList.setParameter("viewMatrix", viewMatrix);
                 perPixelLinkedList.setParameter("projectionMatrix", projMatrix);
                 math::Matrix4f matrix = quad.getTransform().getMatrix().transpose();
                 perPixelLinkedList.setParameter("worldMat", matrix);
                 perPixelLinkedList.setParameter("bg_color", 0, 0, 0, 0);
                 perPixelLinkedListBindlessPass2->fullScreenQuad = quad;
                 EntityId childId = factory.createEntity();
                 auto da3 = world.addSubRendererFlag(da, rendererId, childId, perPixelLinkedListBindlessPass2, factory);
                 return da3;
                }
                template <typename World, typename DynamicArray, typename Factory>
                static auto createTileModel(World& world, DynamicArray& da, Factory& factory, EntityId entity, const Texture *image, math::Vec3f position, math::Vec3f size, sf::IntRect subRect,  sf::Color color = sf::Color::White, Entity* parent = nullptr) {
                    TransformComponent tc;
                    TransformMatrix tm;
                    tc.localBounds = physic::BoundingBox(0, 0, 0, size.x, size.y, size.z);
                    tc.position = position;
                    tc.origin = size * 0.5f;
                    tc.center = position + tc.origin;
                    tm.setOrigin(tc.origin);
                    tm.setRotation(math::Vec3f::zAxis, 0);
                    tm.setTranslation(tc.center);
                    math::Vec3f scale(1.f, 1.f, 1.f);
                    tm.setScale(scale);
                    tc.globalBounds = tc.localBounds.transform(tm);
                    tc.transformMatrix = tm;
                    auto da2 = world.addEntityComponentFlag(da, entity, tc, factory);
                    MeshComponent mesh;
                    VertexArray va(sf::Quads, 4);
                    Vertex v1(sf::Vector3f(0, 0, 0), color);
                    Vertex v2(sf::Vector3f(size.x, 0, 0), color);
                    Vertex v3(sf::Vector3f(size.x, size.y, 0), color);
                    Vertex v4(sf::Vector3f(0, size.y, 0), color);
                    v1.texCoords = sf::Vector2f(subRect.left, subRect.top);
                    v2.texCoords = sf::Vector2f(subRect.left + subRect.width, subRect.top);
                    v3.texCoords = sf::Vector2f(subRect.left + subRect.width, subRect.top + subRect.height);
                    v4.texCoords = sf::Vector2f(subRect.left, subRect.top + subRect.height);
                    //std::cout<<"tex coords : "<<v2.texCoords.x<<" "<<v2.texCoords.y<<std::endl;
                    //v1.color = v2.color = v3.color = v4.color = color;


                    va[0] = v1;
                    va[1] = v2;
                    va[2] = v3;
                    va[3] = v4;
                    Material material;
                    material.addTexture(nullptr, sf::IntRect(0, 0, 0, 0));
                    Face face(va, material, tm);
                    mesh.faces.push_back(face);
                    auto da3 = world.addEntityComponentFlag(da2, entity, mesh, factory);
                    ClonableComponent clonable;
                    auto da4 = world.addEntityComponentFlag(da3, entity, clonable, factory);
                    EntityInfoComponent entityInfo;
                    entityInfo.groupName = "E_TILE";
                    auto da5 = world.addEntityComponentFlag(da4, entity, entityInfo, factory);
                    return da5;
                }
                template <typename World, typename DynamicArray, typename Factory>
                static auto createBigTileModel(World& world, DynamicArray& da, Factory& factory, EntityId entity, math::Vec3f position) {
                    TransformMatrix tm;
                    TransformComponent tc;
                    tc.localBounds = physic::BoundingBox(0, 0, 0, 0, 0, 0);
                    tc.position = position;
                    tc.origin = tc.size * 0.5f;
                    tc.center = position + tc.origin;
                    tm.setOrigin(tc.origin);
                    tm.setRotation(math::Vec3f::zAxis, 0);
                    tm.setTranslation(tc.center);
                    math::Vec3f scale(1.f, 1.f, 1.f);
                    tm.setScale(scale);
                    tc.globalBounds = tc.localBounds.transform(tm);
                    tc.transformMatrix = tm;
                    auto da1 = world.addEntityComponentFlag(da, entity, tc, factory);
                    return da1;
                }
                template <typename World, typename DynamicArray, typename Factory>
                static auto createGridSceneModel(World& world, DynamicArray& da, Factory& factory, EntityId sceneId, std::string name, unsigned int gridWidth, unsigned int gridHeight, unsigned int gridDepth) {
                    Grid grid(gridWidth, gridHeight, gridDepth);
                    SceneGridComponent sceneGrid;
                    sceneGrid.partitions = grid;
                    auto da2 = world.addSceneFlag(da, sceneId, sceneGrid, factory);
                    return da2;
                }
            };



        }
    }
}
#endif
