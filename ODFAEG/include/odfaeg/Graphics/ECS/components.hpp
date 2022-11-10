#ifndef ODFAEG_ECS_COMPONENTS_HPP
#define ODFAEG_ECS_COMPONENTS_HPP
#include "../../Math/vec4.h"
#include "grid.hpp"
#include "../../Math/transformMatrix.h"
#include "../batcher.h"
#include "../renderWindow.h"
#include "../view.h"
#include "../rectangleShape.h"
#include "../glCheck.h"
#include <GL/glew.h>
#include <vulkan/vulkan.h>
namespace odfaeg {
    namespace graphic {
        namespace ecs {
            struct VkInstanceCp {
                VkInstance instance;
            };
            struct VkDebugMessengerCp {
                VkDebugUtilsMessengerEXT debugMessenger;
            };
            struct VkPhysicalDeviceCp {
                VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
            };
            struct VkLogicalDeviceCp {
                VkDevice device;
                VkQueue graphicsQueue;
                VkQueue presentQueue;
            };
            struct VkSurfaceKHRCp  {
                VkSurfaceKHR surface;
            };
            struct VkSwapchainCp {
                VkSwapchainKHR swapChain;
                std::vector<VkImage> swapChainImages;
                VkFormat swapChainImageFormat;
                VkExtent2D swapChainExtent;
            };
            struct VkImageViewCp {
                std::vector<VkImageView> imagesViews;
            };
            enum WallType {
                    TOP_LEFT, TOP_RIGHT, BOTTOM_RIGHT, BOTTOM_LEFT, TOP_BOTTOM, RIGHT_LEFT, T_TOP, T_RIGHT, T_LEFT, T_BOTTOM, X, NB_WALL_TYPES
            };
            struct ColliderComponent {
            };
            struct ClonableComponent {

            };
            struct MergeableComponent {
            };
            struct EntityInfoComponent {
                std::string groupName;
            };
            struct SceneGridComponent {
                Grid partitions;
            };
            struct TransformComponent {
                math::Vec3f position, center, scale, rotationAxis, size, origin;
                float angle;
                TransformMatrix transformMatrix;
                physic::BoundingBox globalBounds, localBounds;
            };
            struct AnimationComponent {
                bool playing, loop;
                size_t previousFrame, currentFrame, nextFrame, nbFrames, boneIndex;
                size_t interpLevels, interpPerc;
                std::vector<Face> interpolatedFrameFaces;
                std::vector<EntityId> framesFaces;
            };
            struct BoneAnimationComponent {
                std::vector<size_t> selectedBonesIndexes;
                std::vector<AnimationComponent> boneAnimations;
            };
            struct PerPixelLinkedListBindlessPass1RenderComponent  {
                PerPixelLinkedListBindlessPass1RenderComponent(RenderWindow& window) : window(window) {}
                GLuint atomicBuffer, linkedListBuffer, clearBuf, headPtrTex, bindlessTexUBO;
                VertexBuffer vb;
                std::array<VertexBuffer, Batcher::nbPrimitiveTypes> vbBindlessTex;
                Batcher batcher;
                Shader perPixelLinkedListShader;
                std::string expression;
                unsigned int layer;
                RenderWindow& window;
            };
            struct PerPixelLinkedListBindlessPass2RenderComponent {
                PerPixelLinkedListBindlessPass2RenderComponent(RenderWindow& window) : window(window) {}
                size_t linkedListBuffer, headPtrTex;
                Shader perPixelLinkedListShader;
                RenderWindow& window;
                RectangleShape fullScreenQuad;
                std::string expression;
            };
            struct MeshComponent {
                std::vector<Face> faces;
            };
        }
    }
}
#endif
