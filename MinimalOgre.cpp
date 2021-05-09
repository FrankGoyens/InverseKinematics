#include "MinimalOgre.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include <OgreHeaderPrefix.h>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreMaterialManager.h>
#include <OgreRenderWindow.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include <Bites/OgreCameraMan.h>
#include <Bites/OgreTrays.h>

#include <OgreHeaderSuffix.h>

#include <CSkeleton.h>
#include <SkeletonPicker.h>
#include <SkeletonRenderer.h>

MinimalOgre::MinimalOgre() : OgreBites::ApplicationContext("OgreTutorialApp") {}

MinimalOgre::~MinimalOgre() {}

void MinimalOgre::loadResources() {
    enableShaderCache();
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Essential");

    createDummyScene();

    m_trays = new OgreBites::TrayManager("Interface", getRenderWindow());
    addInputListener(m_trays);

    m_trays->showLoadingBar(1, 0);
    OgreBites::ApplicationContext::loadResources();

    m_trays->hideLoadingBar();
    destroyDummyScene();
}

void MinimalOgre::setup() { // do not forget to call the base first
    OgreBites::ApplicationContext::setup();

    // register for input events
    addInputListener(this);

    // get a pointer to the already created root
    Ogre::Root* root = getRoot();
    m_sceneManager = root->createSceneManager();

    // register our scene with the RTSS
    Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(m_sceneManager);

    // Trays
    m_sceneManager->addRenderQueueListener(getOverlaySystem());
    m_trays->showFrameStats(OgreBites::TL_TOPRIGHT);
    m_trays->refreshCursor();

    auto* rs = shadergen->getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    rs->addTemplateSubRenderState(shadergen->createSubRenderState(Ogre::RTShader::PerPixelLighting().getType()));

    m_sceneManager->setAmbientLight(Ogre::ColourValue(.5, .5, .5));

    // without light we would just get a black screen
    Ogre::Light* light = m_sceneManager->createLight("MainLight");
    Ogre::SceneNode* lightNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
    lightNode->setPosition(20, 80, 50);
    lightNode->attachObject(light);

    // also need to tell where we are
    m_cameraNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
    m_cameraNode->setPosition(0, 0, 15);
    m_cameraNode->lookAt(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_PARENT);

    // create the camera
    Ogre::Camera* cam = m_sceneManager->createCamera("myCam");
    cam->setNearClipDistance(5); // specific to this sample
    cam->setAutoAspectRatio(true);
    m_cameraNode->attachObject(cam);

    // and tell it to render into the main window
    getRenderWindow()->addViewport(cam);

    m_skeletonRenderer = std::make_unique<SkeletonRenderer>(*getRoot(), *m_sceneManager);
    LoadSkeletonFromDisk();

    m_cameraMan = std::make_unique<OgreBites::CameraMan>(m_cameraNode);
    m_cameraMan->setStyle(OgreBites::CameraStyle::CS_ORBIT);
    m_cameraMan->setYawPitchDist(Ogre::Radian(0), Ogre::Radian(0.3f), 50);

    AttachCameraMan();
}

void MinimalOgre::shutdown() {
    delete m_trays;
    m_skeletonRenderer.reset();
    OgreBites::ApplicationContextBase::shutdown();
}

bool MinimalOgre::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    OgreBites::ApplicationContext::frameRenderingQueued(evt);

    m_skeletonRenderer = std::make_unique<SkeletonRenderer>(*getRoot(), *m_sceneManager);
    bool cameraManNeeded = true;

    m_skeleton->draw(*m_skeletonRenderer);

    if (m_pickDepth) {
        DragJointToMousePositionAtPickDepth();
        cameraManNeeded = false;
    } else if (const auto pickResult = PickJointIfRequested()) {
        m_pickDepth = pickResult->depth;
    }

    if (cameraManNeeded)
        AttachCameraMan();
    else
        DetachCameraMan();

    return true;
}

bool MinimalOgre::frameEnded(const Ogre::FrameEvent&) { return true; }

bool MinimalOgre::mouseReleased(const OgreBites::MouseButtonEvent& evt) {
    {
        std::scoped_lock(m_pickDepthMutex);
        m_pickDepth = {};
    }
    return true;
}

bool MinimalOgre::mouseMoved(const OgreBites::MouseMotionEvent& evt) {
    if (m_pickDepth)
        // There is a pick depth (from a previous pick) so request picking to drag the target
        RequestPick({evt.x, evt.y});
    return true;
}

bool MinimalOgre::keyPressed(const OgreBites::KeyboardEvent& evt) {
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE) {
        getRoot()->queueEndRendering();
    }
    return true;
}

void MinimalOgre::LoadSkeletonFromDisk() {
    m_skeleton = std::make_unique<CSkeleton>("skeleton.skl");
    m_skeleton->setTransformMatrix(glm::scale(m_skeleton->getTransformMatrix(), {100, 100, 100}));
}

std::pair<float, float> MinimalOgre::MousePositionToScreenSpace(const std::pair<int, int>& mousePos,
                                                                const Ogre::Camera& camera) {
    const auto screenWidth = camera.getViewport()->getActualWidth();
    const auto screenHeight = camera.getViewport()->getActualHeight();
    const auto x = static_cast<Ogre::Real>(mousePos.first) / screenWidth;
    const auto y = static_cast<Ogre::Real>(mousePos.second) / screenHeight;
    return {x, y};
}

void MinimalOgre::AttachCameraMan() {
    if (!m_cameraManAttached) {
        m_cameraManAttached = true;
        addInputListener(m_cameraMan.get());
    }
}

void MinimalOgre::RequestPick(const std::pair<int, int>& mousePosition) {
    std::scoped_lock lock(m_pickRequestMutex);
    m_pickRequest = mousePosition;
}

MinimalOgre::PickRequest MinimalOgre::ConsumePickRequest() {
    std::scoped_lock(m_pickRequestMutex);
    const auto currentPickRequest = m_pickRequest;
    m_pickRequest = {};
    return currentPickRequest;
}

void MinimalOgre::DetachCameraMan() {
    if (m_cameraManAttached) {
        m_cameraManAttached = false;
        removeInputListener(m_cameraMan.get());

        // Do this manually, or the camera man might think the mouse is still down when set up again
        OgreBites::MouseButtonEvent evt;
        evt.button = OgreBites::ButtonType::BUTTON_LEFT;
        m_cameraMan->mouseReleased(evt);
    }
}

void MinimalOgre::DragJointToMousePositionAtPickDepth() {
    if (const auto mousePosition = ConsumePickRequest()) {
        const auto* camera = m_sceneManager->getCamera("myCam");
        if (camera) {
            const auto screenPosition = MousePositionToScreenSpace(*mousePosition, *camera);
            const auto pickRay = camera->getCameraToViewportRay(screenPosition.first, screenPosition.second);
            const auto pickPoint = pickRay.getOrigin() + *m_pickDepth * pickRay.getDirection();
            m_skeletonRenderer->TargetSphere({pickPoint.x, pickPoint.y, pickPoint.z, 1.f});
            RequestPick(*mousePosition); // Set the current mouseposition in case the mouse does not move anymore
        }
    }
}

std::optional<SkeletonPicker::Result> MinimalOgre::PickJointIfRequested() {
    const auto currentPickRequest = ConsumePickRequest();
    const auto* camera = m_sceneManager->getCamera("myCam");
    if (currentPickRequest && camera) {
        auto [x, y] = MousePositionToScreenSpace(*currentPickRequest, *camera);
        return SkeletonPicker::Pick(
            SkeletonPicker::PickContext{*m_sceneManager, *camera, m_skeletonRenderer->GetBackwardsMapping()}, x, y);
    }
    return {};
}

bool MinimalOgre::mousePressed(const OgreBites::MouseButtonEvent& evt) {
    RequestPick({evt.x, evt.y});
    return true;
}
