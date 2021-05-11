#include "SkeletonRenderer.h"

#include <optional>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <OgreHeaderPrefix.h>

#include <OgreEntity.h>
#include <OgreMaterialManager.h>
#include <OgreMeshManager.h>
#include <OgreQuaternion.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include <OgreHeaderSuffix.h>

SkeletonRenderer::SkeletonRenderer(Ogre::Root& ogreRoot, Ogre::SceneManager& sceneManager,
                                   std::queue<Ogre::Entity*> allocatedSpheres, std::queue<Ogre::Entity*> allocatedLines)
    : m_ogreRoot(&ogreRoot), m_sceneManager(&sceneManager), m_preAllocatedSpheres(std::move(allocatedSpheres)),
      m_preAllocatedLineEntities(std::move(allocatedLines)) {
    m_skeletonRootNode = sceneManager.getRootSceneNode()->createChildSceneNode();
}

SkeletonRenderer::~SkeletonRenderer() {
    for (auto* entity : m_allocatedSpheres)
        m_sceneManager->destroyEntity(entity);

    for (auto* line : m_allocatedLineEntities)
        m_sceneManager->destroyEntity(line);

    while (!m_preAllocatedSpheres.empty()) {
        m_sceneManager->destroyEntity(m_preAllocatedSpheres.front());
        m_preAllocatedSpheres.pop();
    }

    while (!m_preAllocatedLineEntities.empty()) {
        m_sceneManager->destroyEntity(m_preAllocatedLineEntities.front());
        m_preAllocatedLineEntities.pop();
    }

    m_skeletonRootNode->removeAndDestroyAllChildren();
    m_sceneManager->destroySceneNode(m_skeletonRootNode);
}

static glm::vec3 FromVec4(glm::vec4 vec) {
    vec / vec.w;
    return {vec.x, vec.y, vec.z};
}

static Ogre::Vector3 OgreVec3FromVec3(const glm::vec3& vec) { return {vec.x, vec.y, vec.z}; }

static Ogre::Vector3 OgreVec3FromVec4(const glm::vec4& vec) { return OgreVec3FromVec3(FromVec4(vec)); }

namespace {
struct LineMesh {
    LineMesh(Ogre::SceneManager& sceneManager) {
        auto lineMaterial = Ogre::MaterialManager::getSingleton().getByName("Template/GreenNonShaded");
        auto* lineManualObject = sceneManager.createManualObject();
        lineManualObject->begin(lineMaterial, Ogre::RenderOperation::OT_LINE_STRIP);
        lineManualObject->position(Ogre::Vector3(0, 0, 0));
        lineManualObject->position(Ogre::Vector3(1, 0, 0));
        lineManualObject->end();

        lineManualObject->convertToMesh("OgreSkeletonRendererLine.mesh");
    }
};
} // namespace

static LineMesh& GetSingleLineMesh(Ogre::SceneManager& sceneManager) {
    static LineMesh lineMesh(sceneManager);
    return lineMesh;
}

static std::pair<std::optional<std::pair<float, glm::vec3>>, float> GetLineScaleAndOrientation(const glm::vec4& start,
                                                                                               const glm::vec4& end) {
    const auto start3 = FromVec4(start), end3 = FromVec4(end);
    const auto startToEnd = end3 - start3;

    if (startToEnd == glm::vec3(1, 0, 0))
        return {{}, startToEnd.x}; // Only a uniform scale needed, no rotation

    const auto startToEndLength = glm::length(startToEnd);

    if (startToEndLength < std::numeric_limits<float>::epsilon())
        return {{}, 0.f}; // No rotation and no scale

    const auto dotProduct = startToEnd.x;

    auto angleAxis = glm::cross(glm::vec3(1, 0, 0), startToEnd);
    angleAxis /= glm::length(angleAxis);

    const auto angle = std::acos(dotProduct / startToEndLength);

    return {{{angle, glm::vec3(angleAxis)}}, startToEndLength};
}

static void TransformBaseMeshLineToMatchSkeletonLine(Ogre::SceneNode& node, const glm::vec4& start,
                                                     const glm::vec4& end) {
    auto [angleAndAxis, scale] = GetLineScaleAndOrientation(start, end);
    if (angleAndAxis) {
        Ogre::Quaternion quat;
        quat.FromAngleAxis(Ogre::Radian{angleAndAxis->first}, OgreVec3FromVec3(angleAndAxis->second));
        node.setOrientation(quat);
    }
    node.setScale({scale, scale, scale});
    node.setPosition(OgreVec3FromVec4(start));
}

void SkeletonRenderer::Line(const glm::vec4& start, const glm::vec4& end) {
    auto& lineMesh = GetAllocatedOrNewLine();
    m_lineEntities.push_back(&lineMesh);

    auto* lineNode = m_skeletonRootNode->createChildSceneNode();
    lineNode->attachObject(&lineMesh);

    TransformBaseMeshLineToMatchSkeletonLine(*lineNode, start, end);
}

void SkeletonRenderer::JointSphere(CJoint& joint, const glm::vec4& position) {
    auto& sphere = Sphere(position, "Template/Blue");
    m_sphereEntities.push_back(&sphere);
    m_backwardsMapping.emplace(&sphere, &joint);
}

void SkeletonRenderer::TargetSphere(const glm::vec4& position) {
    auto& sphere = Sphere(position, "Template/Red");
    m_sphereEntities.push_back(&sphere);
}

template <typename Entity>
std::queue<Entity*>&& YieldEntities(std::queue<Entity*>& preAllocatedQueue, std::vector<Entity*>& allocatedVec) {
    for (auto* entity : allocatedVec)
        preAllocatedQueue.push(entity);
    allocatedVec.clear();
    return std::move(preAllocatedQueue);
}

std::queue<Ogre::Entity*>&& SkeletonRenderer::YieldAllocatedJointSpheres() && {
    return YieldEntities(m_preAllocatedSpheres, m_allocatedSpheres);
}

std::queue<Ogre::Entity*>&& SkeletonRenderer::YieldAllocatedLines() && {
    return YieldEntities(m_preAllocatedLineEntities, m_allocatedLineEntities);
}

Ogre::Entity& SkeletonRenderer::Sphere(const glm::vec4& position, const std::string& materialName) {
    Ogre::Entity& ent = GetAllocatedOrNewSphere();
    auto material = Ogre::MaterialManager::getSingleton().getByName(materialName);
    ent.setMaterial(material);

    Ogre::SceneNode* node = m_skeletonRootNode->createChildSceneNode();
    node->attachObject(&ent);

    node->setPosition(OgreVec3FromVec4(position));
    node->scale({.02f, .02f, .02f});

    node->_updateBounds(); // Required to make picking work, because scene queries need an up to date bounding box

    return ent;
}

template <typename Entity, typename AllocateFunc>
static Entity& GetOrAllocateNewEntity(std::queue<Entity*>& preAllocatedQueue, std::vector<Entity*>& allocatedVec,
                                      AllocateFunc allocate) {
    Entity* ent;
    if (preAllocatedQueue.empty()) {
        ent = allocate();
    } else {
        ent = preAllocatedQueue.front();
        preAllocatedQueue.pop();
    }
    allocatedVec.push_back(ent);
    return *ent;
}

Ogre::Entity& SkeletonRenderer::GetAllocatedOrNewSphere() {
    return GetOrAllocateNewEntity(m_preAllocatedSpheres, m_allocatedSpheres,
                                  [this]() { return m_sceneManager->createEntity("sphere.mesh"); });
}

Ogre::Entity& SkeletonRenderer::GetAllocatedOrNewLine() {
    return GetOrAllocateNewEntity(m_preAllocatedLineEntities, m_allocatedLineEntities, [this]() {
        (void)GetSingleLineMesh(*m_sceneManager); // The mesh is stored by Ogre
        return m_sceneManager->createEntity("OgreSkeletonRendererLine.mesh");
    });
}
