#ifndef CMDBUFFER_HPP_
#define CMDBUFFER_HPP_

#include "Base.hpp"
#include "Device.hpp"

/*
 *  Command pool and command buffer related functions
 */

namespace vk
{
    typedef std::vector<VkCommandBuffer> CmdBufferList;

    VkCommandBuffer beginOneTimeCommand(const Device &device, const VkCommandPool &commandPool);
    void endOneTimeCommand(const Device &device, const VkCommandBuffer &commandBuffer, const VkCommandPool &commandPool, const VkQueue &graphicsQueue);
    VkResult createCommandPool(const Device &device, VkCommandPool *commandPool);
    VkResult createCommandBuffers(const Device &device, const VkCommandPool &commandPool, CmdBufferList &commandBuffers, size_t fbCount);
    void freeCommandBuffers(const Device &device, const VkCommandPool &commandPool, CmdBufferList &commandBuffers);
}

#endif
