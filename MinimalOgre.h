#pragma once

#include <OgreHeaderPrefix.h>

#include <OgreRoot.h>

#include <Bites/OgreApplicationContext.h>

#include <OgreHeaderSuffix.h>

namespace OgreBites {
class TrayManager;
class CameraMan;
} // namespace OgreBites

class MinimalOgre final : public OgreBites::ApplicationContext, public OgreBites::InputListener {
  public:
    MinimalOgre();
    ~MinimalOgre() override;

    void loadResources() override;
    void setup() override;
    void shutdown() override;

    bool keyPressed(const OgreBites::KeyboardEvent& evt) override;

  private:
    OgreBites::TrayManager* m_trays;
    OgreBites::CameraMan* m_cameraMan;
};
