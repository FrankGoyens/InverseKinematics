#pragma once

#include <memory>

#include <OgreFrameListener.h>

namespace Ogre {
class Root;
class Camera;
class RenderWindow;
} // namespace Ogre

namespace CEGUI {
class Renderer;
class ImageCodec;
class ResourceProvider;
} // namespace CEGUI

class InverseKinematicsFrameListener;
class WndEvtListener;

class OgreMainWindow final : Ogre::FrameListener {
  public:
    OgreMainWindow();
    ~OgreMainWindow();
    OgreMainWindow(const OgreMainWindow&) = delete;
    OgreMainWindow(OgreMainWindow&&) = delete;
    OgreMainWindow& operator=(const OgreMainWindow&) = delete;
    OgreMainWindow& operator=(OgreMainWindow&&) = delete;

    bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    void Run();

  private:
    std::unique_ptr<Ogre::Root> d_ogreRoot;
    Ogre::Camera* d_camera = nullptr;
    Ogre::RenderWindow* d_window = nullptr;

    std::unique_ptr<InverseKinematicsFrameListener> m_frameListener;
    std::unique_ptr<WndEvtListener> d_windowEventListener;

    void MakeGUI();
};
