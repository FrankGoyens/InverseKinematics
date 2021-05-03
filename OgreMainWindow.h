#pragma once

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

class OgreMainWindow final {
  public:
    OgreMainWindow();
    ~OgreMainWindow();
    OgreMainWindow(const OgreMainWindow&) = delete;
    OgreMainWindow(OgreMainWindow&&) = delete;
    OgreMainWindow& operator=(const OgreMainWindow&) = delete;
    OgreMainWindow& operator=(OgreMainWindow&&) = delete;

  private:
    Ogre::Root* d_ogreRoot = nullptr;
    Ogre::Camera* d_camera = nullptr;
    Ogre::RenderWindow* d_window = nullptr;

    CEGUI::Renderer* d_renderer = nullptr;
    CEGUI::ImageCodec* d_imageCodec = nullptr;
    CEGUI::ResourceProvider* d_resourceProvider = nullptr;
};
