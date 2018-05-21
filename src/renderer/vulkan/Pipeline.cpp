#include "Pipeline.hpp"
#include "../../Utils.hpp"
#include <fstream>

namespace vk
{
    struct ShaderProgram
    {
        VkShaderModule vertShader = VK_NULL_HANDLE;
        VkShaderModule fragShader = VK_NULL_HANDLE;
    };

    static VkShaderModule createShaderModule(const Device &device, const uint32_t *shaderSrc, size_t codeSize)
    {
        VkShaderModule shaderModule = VK_NULL_HANDLE;

        VkShaderModuleCreateInfo smCreateInfo = {};
        smCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        smCreateInfo.codeSize = codeSize;
        smCreateInfo.pCode = shaderSrc;

        VkResult result = vkCreateShaderModule(device.logical, &smCreateInfo, nullptr, &shaderModule);
        LOG_MESSAGE_ASSERT(result == VK_SUCCESS, "Could not create shader module: " << result);

        return shaderModule;
    }

    static uint32_t *ReadShaderFromFile(const char *filename, size_t *buffSize)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            LOG_MESSAGE_ASSERT(false, "Cannot open input file: " << filename);
        }

        *buffSize = (size_t)file.tellg();
        uint32_t *outBuffer = new uint32_t[*buffSize << 2];

        file.seekg(0);
        // is alignment ok here?
        file.read((char*)outBuffer, *buffSize);
        file.close();

        return outBuffer;
    }

    static ShaderProgram loadShader(const Device &device, const char* vshFilename, const char *fshFilename)
    {
        size_t vShaderSize = 0, fShaderSize = 0;
        ShaderProgram shader;

        uint32_t *vShaderSrc = ReadShaderFromFile(vshFilename, &vShaderSize);
        uint32_t *fShaderSrc = ReadShaderFromFile(fshFilename, &fShaderSize);

        shader.vertShader = createShaderModule(device, vShaderSrc, vShaderSize);
        shader.fragShader = createShaderModule(device, fShaderSrc, fShaderSize);

        delete[] vShaderSrc;
        delete[] fShaderSrc;

        return shader;
    }

    VkResult createPipeline(const Device &device, const SwapChain &swapChain, const RenderPass &renderPass, const VkDescriptorSetLayout &descriptorLayout, const VertexBufferInfo *vbInfo, Pipeline *pipeline, const char **shaders)
    {
        ShaderProgram shader = loadShader(device, shaders[0], shaders[1]);

        VkPipelineShaderStageCreateInfo vssCreateInfo = {};
        vssCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vssCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vssCreateInfo.module = shader.vertShader;
        vssCreateInfo.pName = "main";
        VkPipelineShaderStageCreateInfo fssCreateInfo = {};
        fssCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fssCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fssCreateInfo.module = shader.fragShader;
        fssCreateInfo.pName = "main";

        VkPipelineShaderStageCreateInfo ssCreateInfos[] = { vssCreateInfo, fssCreateInfo };

        // fixed functions setup
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = vbInfo ? (uint32_t)vbInfo->bindingDescriptions.size() : 0;
        vertexInputInfo.pVertexBindingDescriptions = vbInfo ? vbInfo->bindingDescriptions.data() : nullptr;
        vertexInputInfo.vertexAttributeDescriptionCount = vbInfo ? (uint32_t)vbInfo->attributeDescriptions.size() : 0;
        vertexInputInfo.pVertexAttributeDescriptions = vbInfo ? vbInfo->attributeDescriptions.data() : nullptr;

        VkPipelineInputAssemblyStateCreateInfo iaCreateInfo = {};
        iaCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        iaCreateInfo.topology = pipeline->topology;
        iaCreateInfo.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport = {};
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = (float)swapChain.extent.width;
        viewport.height = (float)swapChain.extent.height;
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;

        VkRect2D scissor = {};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = swapChain.extent;

        VkPipelineViewportStateCreateInfo vpCreateInfo = {};
        vpCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        vpCreateInfo.viewportCount = 1;
        vpCreateInfo.pViewports = &viewport;
        vpCreateInfo.scissorCount = 1;
        vpCreateInfo.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rCreateInfo = {};
        rCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rCreateInfo.depthClampEnable = VK_FALSE;
        rCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rCreateInfo.polygonMode = pipeline->mode;
        rCreateInfo.lineWidth = 1.f;
        rCreateInfo.cullMode = pipeline->cullMode;
        rCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rCreateInfo.depthBiasEnable = VK_FALSE;
        rCreateInfo.depthBiasClamp = 0.f;
        rCreateInfo.depthBiasConstantFactor = 0.f;
        rCreateInfo.depthBiasSlopeFactor = 0.f;

        VkPipelineMultisampleStateCreateInfo  msCreateInfo = {};
        msCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        msCreateInfo.sampleShadingEnable = VK_FALSE;
        msCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        msCreateInfo.minSampleShading = 1.f;
        msCreateInfo.pSampleMask = nullptr;
        msCreateInfo.alphaToCoverageEnable = VK_FALSE;
        msCreateInfo.alphaToOneEnable = VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo dCreateInfo = {};
        dCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        dCreateInfo.depthTestEnable = pipeline->depthTestEnable;
        dCreateInfo.depthWriteEnable = VK_TRUE;
        dCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        dCreateInfo.depthBoundsTestEnable = VK_FALSE;
        dCreateInfo.minDepthBounds = 0.f;
        dCreateInfo.maxDepthBounds = 1.f;
        dCreateInfo.stencilTestEnable = VK_FALSE;
        dCreateInfo.front = {};
        dCreateInfo.back = {};

        VkPipelineColorBlendAttachmentState cbaState = {};
        cbaState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        cbaState.blendEnable = pipeline->blendMode != VK_BLEND_FACTOR_ZERO ? VK_TRUE : VK_FALSE;
        cbaState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        cbaState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        cbaState.alphaBlendOp = VK_BLEND_OP_ADD;
        cbaState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        cbaState.dstColorBlendFactor = pipeline->blendMode;
        cbaState.colorBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo cbsCreateInfo = {};
        cbsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        cbsCreateInfo.logicOpEnable = VK_FALSE;
        cbsCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        cbsCreateInfo.attachmentCount = 1;
        cbsCreateInfo.pAttachments = &cbaState;
        cbsCreateInfo.blendConstants[0] = 0.f;
        cbsCreateInfo.blendConstants[1] = 0.f;
        cbsCreateInfo.blendConstants[2] = 0.f;
        cbsCreateInfo.blendConstants[3] = 0.f;

        VkPipelineDynamicStateCreateInfo dsCreateInfo = {};

        VkPipelineLayoutCreateInfo plCreateInfo = {};
        plCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        plCreateInfo.setLayoutCount = 1;
        plCreateInfo.pSetLayouts = &descriptorLayout;
        plCreateInfo.pushConstantRangeCount = 0;
        plCreateInfo.pPushConstantRanges = nullptr;

        VkResult plResult = vkCreatePipelineLayout(device.logical, &plCreateInfo, nullptr, &pipeline->layout);
        if (plResult != VK_SUCCESS)
            return plResult;

        // create THE pipeline
        VkGraphicsPipelineCreateInfo pCreateInfo = {};
        pCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pCreateInfo.stageCount = 2;
        pCreateInfo.pStages = ssCreateInfos;
        pCreateInfo.pVertexInputState = &vertexInputInfo;
        pCreateInfo.pInputAssemblyState = &iaCreateInfo;
        pCreateInfo.pViewportState = &vpCreateInfo;
        pCreateInfo.pRasterizationState = &rCreateInfo;
        pCreateInfo.pMultisampleState = &msCreateInfo;
        pCreateInfo.pDepthStencilState = &dCreateInfo;
        pCreateInfo.pColorBlendState = &cbsCreateInfo;
        pCreateInfo.pDynamicState = nullptr;
        pCreateInfo.layout = pipeline->layout;
        pCreateInfo.renderPass = renderPass.renderPass;
        pCreateInfo.subpass = 0;
        pCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pCreateInfo.basePipelineIndex = -1;

        VkResult pResult = vkCreateGraphicsPipelines(device.logical, VK_NULL_HANDLE, 1, &pCreateInfo, nullptr, &pipeline->pipeline);
        vkDestroyShaderModule(device.logical, shader.vertShader, nullptr);
        vkDestroyShaderModule(device.logical, shader.fragShader, nullptr);

        return pResult;
    }

    void destroyPipeline(const Device &device, Pipeline &pipeline)
    {
        if (pipeline.layout != VK_NULL_HANDLE)
            vkDestroyPipelineLayout(device.logical, pipeline.layout, nullptr);
        if (pipeline.pipeline != VK_NULL_HANDLE)
            vkDestroyPipeline(device.logical, pipeline.pipeline, nullptr);
    }

    VkResult createRenderPass(const Device &device, const SwapChain &swapChain, RenderPass *renderPass)
    {
        VkAttachmentDescription attachmentDesc = {};
        attachmentDesc.format = swapChain.format;
        attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDesc.loadOp = renderPass->colorLoadOp;
        attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference attachmentRef = {};
        attachmentRef.attachment = 0;
        attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachmentDesc = {};
        depthAttachmentDesc.format = VK_FORMAT_D32_SFLOAT;
        depthAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef = {};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDesc = {};
        subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDesc.colorAttachmentCount = 1;
        subpassDesc.pColorAttachments = &attachmentRef;
        subpassDesc.pDepthStencilAttachment = &depthAttachmentRef;

        VkAttachmentDescription attachments[] = { attachmentDesc, depthAttachmentDesc };
        VkRenderPassCreateInfo rpCreateInfo = {};
        rpCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rpCreateInfo.attachmentCount = 2;
        rpCreateInfo.pAttachments = attachments;
        rpCreateInfo.subpassCount = 1;
        rpCreateInfo.pSubpasses = &subpassDesc;

        // subpass depencency: wait for color stage
        VkSubpassDependency spDep = {};
        spDep.srcSubpass = VK_SUBPASS_EXTERNAL;
        spDep.dstSubpass = 0;
        spDep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        spDep.srcAccessMask = 0;
        spDep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        spDep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        rpCreateInfo.dependencyCount = 1;
        rpCreateInfo.pDependencies = &spDep;

        return vkCreateRenderPass(device.logical, &rpCreateInfo, nullptr, &renderPass->renderPass);
    }

    void destroyRenderPass(const Device &device, RenderPass &renderPass)
    {
        if (renderPass.renderPass != VK_NULL_HANDLE)
            vkDestroyRenderPass(device.logical, renderPass.renderPass, nullptr);
    }
}

