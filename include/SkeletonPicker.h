#pragma once

#include <optional>

#include <glm/vec3.hpp>

#include "SkeletonRenderer.h" //For BackwardsMapping type

namespace Ogre {
class SceneManager;
class Camera;
} // namespace Ogre

class CJoint;

namespace SkeletonPicker {

struct PickContext {
    Ogre::SceneManager& sceneManager;
    const Ogre::Camera& camera;
    const SkeletonRenderer::BackwardsMapping& backwardsMapping;
};

struct Result {
    std::reference_wrapper<CJoint> joint;
    float depth;
    glm::vec3 pickOrigin, pickDirection;
};

/*! \brief x anf y should be in normalized screen coordinates*/
std::optional<Result> Pick(PickContext&, float x, float y);
}; // namespace SkeletonPicker