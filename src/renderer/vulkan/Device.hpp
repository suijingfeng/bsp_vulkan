#pragma once
#include "Base.hpp"
#include <vector>

/*
 *  Vulkan device and swapchain creation
 */

namespace vk
{
    struct SwapChain
    {
        VkSwapchainKHR sc = VK_NULL_HANDLE;
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkExtent2D extent = { 0, 0 };
        std::vector<VkImage> images;
    };


    Device   createDevice(const VkInstance &instance, const VkSurfaceKHR &surface);
    VkResult createSwapChain(const Device &device, const VkSurfaceKHR &surface, SwapChain *swapChain, VkSwapchainKHR oldSwapchain);
}
