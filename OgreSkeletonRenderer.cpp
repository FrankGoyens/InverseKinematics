#include "SkeletonRenderer.h"

#include <glm/vec3.hpp>

#include <OgreHeaderPrefix.h>

#include <OgreEntity.h>
#include <OgreMaterialManager.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include <OgreHeaderSuffix.h>

SkeletonRenderer::SkeletonRenderer(Ogre::Root& ogreRoot, Ogre::SceneManager& sceneManager,
                                   std::queue<Ogre::Entity*> allocatedSpheres)
    : m_ogreRoot(&ogreRoot), m_sceneManager(&sceneManager), m_preAllocatedSpheres(std::move(allocatedSpheres)) {
    m_skeletonRootNode = sceneManager.getRootSceneNode()->createChildSceneNode();
}

SkeletonRenderer::~SkeletonRenderer() {
    for (auto* entity : m_allocatedSpheres)
        m_sceneManager->destroyEntity(entity);

    while (!m_preAllocatedSpheres.empty()) {
        m_sceneManager->destroyEntity(m_preAllocatedSpheres.front());
        m_preAllocatedSpheres.pop();
    }

    for (auto* line : m_lineEntities)
        m_sceneManager->destroyManualObject(line);
    m_skeletonRootNode->removeAndDestroyAllChildren();
    m_sceneManager->destroySceneNode(m_skeletonRootNode);
}

static glm::vec3 FromVec4(glm::vec4 vec) {
    vec / vec.w;
    return {vec.x, vec.y, vec.z};
}

static Ogre::Vector3 OgreVec3FromVec3(const glm::vec3& vec) { return {vec.x, vec.y, vec.z}; }

static Ogre::Vector3 OgreVec3FromVec4(const glm::vec4& vec) { return OgreVec3FromVec3(FromVec4(vec)); }

void SkeletonRenderer::Line(const glm::vec4& start, const glm::vec4& end) {
    auto* manualObject_line = m_sceneManager->createManualObject();
    auto lineMaterial = Ogre::MaterialManager::getSingleton().getByName("Template/GreenNonShaded");

    manualObject_line->begin(lineMaterial, Ogre::RenderOperation::OT_LINE_STRIP);
    manualObject_line->position(OgreVec3FromVec4(start));
    manualObject_line->position(OgreVec3FromVec4(end));
    manualObject_line->end();

    auto* lineNode = m_skeletonRootNode->createChildSceneNode();
    lineNode->attachObject(manualObject_line);

    m_lineEntities.push_back(manualObject_line);
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

std::queue<Ogre::Entity*>&& SkeletonRenderer::YieldAllocatedJointSpheres() && {
    for (auto* entity : m_allocatedSpheres)
        m_preAllocatedSpheres.push(entity);
    m_allocatedSpheres.clear();
    return std::move(m_preAllocatedSpheres);
}

Ogre::Entity& SkeletonRenderer::Sphere(const glm::vec4& position, const std::string& materialName) {
    Ogre::Entity* ent;
    if (m_preAllocatedSpheres.empty()) {
        ent = m_sceneManager->createEntity("sphere.mesh");
    } else {
        ent = m_preAllocatedSpheres.front();
        m_preAllocatedSpheres.pop();
    }
    m_allocatedSpheres.push_back(ent);
    auto material = Ogre::MaterialManager::getSingleton().getByName(materialName);
    ent->setMaterial(material);

    Ogre::SceneNode* node = m_skeletonRootNode->createChildSceneNode();
    node->attachObject(ent);

    node->setPosition(OgreVec3FromVec4(position));
    node->scale({.02f, .02f, .02f});

    node->_updateBounds(); // Required to make picking work, because scene queries need an up to date bounding box

    return *ent;
}
