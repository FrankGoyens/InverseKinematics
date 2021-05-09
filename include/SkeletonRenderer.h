#pragma once

#include <memory>
#include <unordered_map>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace Ogre {
class Root;
class SceneManager;
class MovableObject;
class Entity;
class SceneNode;
class ManualObject;
} // namespace Ogre

class CJoint;

class SkeletonRenderer final {
  public:
    SkeletonRenderer(Ogre::Root&, Ogre::SceneManager&);
    ~SkeletonRenderer();
    SkeletonRenderer(const SkeletonRenderer&) = delete;
    SkeletonRenderer(SkeletonRenderer&&) = delete;
    SkeletonRenderer& operator=(const SkeletonRenderer&) = delete;
    SkeletonRenderer& operator=(SkeletonRenderer&&) = delete;

    void Line(const glm::vec4& start, const glm::vec4& end);
    void JointSphere(CJoint& joint, const glm::vec4& position);

    //! \brief the sphere that indicates where the joint is being dragged to
    void TargetSphere(const glm::vec4& position);

    using BackwardsMapping = std::unordered_map<const Ogre::MovableObject*, CJoint*>;
    const BackwardsMapping& GetBackwardsMapping() const { return m_backwardsMapping; }

  private:
    Ogre::Root* m_ogreRoot = nullptr;
    Ogre::SceneNode* m_skeletonRootNode = nullptr;
    Ogre::SceneManager* m_sceneManager = nullptr;

    BackwardsMapping m_backwardsMapping;
    std::vector<Ogre::ManualObject*> m_lineEntities;
    std::vector<Ogre::Entity*> m_sphereEntities;

    Ogre::Entity& Sphere(const glm::vec4& position, const std::string& materialName);
};