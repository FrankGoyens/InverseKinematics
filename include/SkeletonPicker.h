#pragma once

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

/*! \brief x anf y should be in normalized screen coordinates*/
CJoint* Pick(PickContext&, float x, float y);
}; // namespace SkeletonPicker