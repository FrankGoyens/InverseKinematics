#pragma once

#include <mutex>
#include <optional>

#include <OgreHeaderPrefix.h>

#include <OgreRoot.h>

#include <Bites/OgreApplicationContext.h>

#include <OgreHeaderSuffix.h>

namespace OgreBites {
class TrayManager;
class SceneManager;
class CameraMan;
} // namespace OgreBites

class CSkeleton;
class CJoint;
class SkeletonRenderer;

class MinimalOgre final : public OgreBites::ApplicationContext, public OgreBites::InputListener {
  public:
    MinimalOgre();
    ~MinimalOgre() override;

    void loadResources() override;
    void setup() override;
    void shutdown() override;

    bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
    bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;

    bool frameRenderingQueued(const Ogre::FrameEvent&) override;
    bool frameEnded(const Ogre::FrameEvent&) override;

  private:
    OgreBites::TrayManager* m_trays = nullptr;
    OgreBites::CameraMan* m_cameraMan = nullptr;

    Ogre::SceneManager* m_sceneManager = nullptr;

    std::unique_ptr<CSkeleton> m_skeleton;
    std::unique_ptr<SkeletonRenderer> m_skeletonRenderer;

    std::mutex m_pickRequestMutex;
    using PickRequest = std::optional<std::pair<int, int>>;
    // This would be set from the mouse input thread and read on the rendering thread
    // so lock m_pickRequestMutex for reading/writing
    PickRequest m_pickRequest;

    void LoadSkeletonFromDisk();

    CJoint* PickJointIfRequested();
    PickRequest ConsumePickRequest();
};
