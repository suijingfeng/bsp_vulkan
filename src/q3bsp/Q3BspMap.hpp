#ifndef Q3BSPMAP_INCLUDED
#define Q3BSPMAP_INCLUDED

#include "Q3Bsp.hpp"

#include "../Frustum.hpp"
#include "../BspMap.hpp"
#include "../renderer/RenderContext.hpp"
#include <vector>
#include <map>

class  GameTexture;
struct Q3BspPatch;

/*
 *  Quake III map
 */

class Q3BspMap : public BspMap
{
public:
    static const int   s_tesselationLevel; // level of curved surface tesselation
    static const float s_worldScale;       // scale down factor for the map

    Q3BspMap(bool bspValid) : BspMap(bspValid) {}
    ~Q3BspMap();

    void Init();
    void OnRender();
    void OnWindowChanged();

    bool ClusterVisible(int cameraCluster, int testCluster)   const;
    int  FindCameraLeaf(const Math::Vector3f &cameraPosition) const;
    void CalculateVisibleFaces(const Math::Vector3f &cameraPosition);
    void ToggleRenderFlag(int flag);

    // bsp data
    Q3BspHeader     header;
    Q3BspEntityLump entities;
    std::vector<Q3BspTextureLump>   textures;
    std::vector<Q3BspPlaneLump>     planes;
    std::vector<Q3BspNodeLump>      nodes;
    std::vector<Q3BspLeafLump>      leaves;
    std::vector<Q3BspLeafFaceLump>  leafFaces;
    std::vector<Q3BspLeafBrushLump> leafBrushes;
    std::vector<Q3BspModelLump>     models;
    std::vector<Q3BspBrushLump>     brushes;
    std::vector<Q3BspBrushSideLump> brushSides;
    std::vector<Q3BspVertexLump>    vertices;
    std::vector<Q3BspMeshVertLump>  meshVertices;
    std::vector<Q3BspEffectLump>    effects;
    std::vector<Q3BspFaceLump>      faces;
    std::vector<Q3BspLightMapLump>  lightMaps;
    std::vector<Q3BspLightVolLump>  lightVols;
    Q3BspVisDataLump                visData;
private:
    void LoadTextures();
    void LoadLightmaps();
    void SetLightmapGamma(float gamma);
    void CreatePatch(const Q3BspFaceLump &f);

    // Vulkan draw call builder and pipeline builder
    void RecordCommandBuffers();
    void RebuildPipelines();

    // Vulkan buffer creation
    void CreateBuffersForFace(const Q3BspFaceLump &face, int idx);
    void CreateBuffersForPatch(int idx);
    void CreateDescriptorSetLayout();
    void CreateDescriptor(const vk::Texture **textures, vk::Descriptor *descriptor);

    // render data
    std::vector<Q3LeafRenderable>   m_renderLeaves;   // bsp leaves in "renderable format"
    std::vector<Q3FaceRenderable>   m_renderFaces;    // bsp faces in "renderable format"
    std::vector<Q3BspPatch *>       m_patches;        // curved surfaces
    std::vector<GameTexture *>      m_textures;       // loaded in-game textures
    std::vector<Q3FaceRenderable *> m_visibleFaces;   // list of visible surfaces to render
    std::vector<int>                m_visiblePatches; // list of visible patches to render
    vk::Texture *m_lightmapTextures = nullptr;        // bsp lightmaps

    Frustum  m_frustum; // view frustum

    // helper textures
    GameTexture *m_missingTex = nullptr; // rendered if an in-game texture is missing
    vk::Texture  m_whiteTex;             // used if no lightmap specified for a face

    // rendering Vulkan buffers and pipelines
    RenderBuffers m_renderBuffers;
    UniformBufferObject m_ubo;
    vk::Pipeline   m_facesPipeline; // used for rendering standard faces
    vk::Pipeline   m_patchPipeline; // used for rendering curves/patches
    vk::RenderPass m_renderPass;
    VkCommandPool  m_commandPool = VK_NULL_HANDLE;
    vk::CmdBufferList m_commandBuffers;

    // all faces and patches use shared vertex buffer info and descriptor set layout
    vk::VertexBufferInfo  m_vbInfo;
    VkDescriptorSetLayout m_dsLayout;
};

#endif
