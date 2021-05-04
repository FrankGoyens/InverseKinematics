#include "Renderer.h"

#include <glm/vec3.hpp>

#include <OgreHeaderPrefix.h>

#include <OgreEntity.h>
#include <OgreMaterialManager.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include <OgreHeaderSuffix.h>

Renderer::Renderer(Ogre::Root& ogreRoot, Ogre::SceneManager& sceneManager)
    : m_ogreRoot(&ogreRoot), m_sceneManager(&sceneManager) {}

static glm::vec3 FromVec4(glm::vec4 vec) {
    vec / vec.w;
    return {vec.x, vec.y, vec.z};
}

static Ogre::Vector3 OgreVec3FromVec3(const glm::vec3& vec) { return {vec.x, vec.y, vec.z}; }

static Ogre::Vector3 OgreVec3FromVec4(const glm::vec4& vec) { return OgreVec3FromVec3(FromVec4(vec)); }

void Renderer::DrawLine(const glm::vec4& start, const glm::vec4& end) {
    auto* manualObject_line = m_sceneManager->createManualObject();
    auto lineMaterial = Ogre::MaterialManager::getSingleton().getByName("Template/GreenNonShaded");

    manualObject_line->begin(lineMaterial, Ogre::RenderOperation::OT_LINE_STRIP);
    manualObject_line->position(OgreVec3FromVec4(start));
    manualObject_line->position(OgreVec3FromVec4(end));
    manualObject_line->end();

    auto* lineNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
    lineNode->attachObject(manualObject_line);
}
void Renderer::Sphere(const glm::vec4& position) {
    Ogre::Entity* ent = m_sceneManager->createEntity("sphere.mesh");
    auto material = Ogre::MaterialManager::getSingleton().getByName("Template/Blue");
    ent->setMaterial(material);
    Ogre::SceneNode* node = m_sceneManager->getRootSceneNode()->createChildSceneNode();
    node->attachObject(ent);

    node->setPosition(OgreVec3FromVec4(position));
    node->scale({.01f, .01f, .01f});
}