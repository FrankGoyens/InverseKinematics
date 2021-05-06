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

MinimalOgre::~MinimalOgre() { delete m_cameraMan; }

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
    Ogre::SceneNode* camNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
    camNode->setPosition(0, 0, 15);
    camNode->lookAt(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_PARENT);

    // create the camera
    Ogre::Camera* cam = m_sceneManager->createCamera("myCam");
    cam->setNearClipDistance(5); // specific to this sample
    cam->setAutoAspectRatio(true);
    camNode->attachObject(cam);

    // and tell it to render into the main window
    getRenderWindow()->addViewport(cam);

    m_skeletonRenderer = std::make_unique<SkeletonRenderer>(*getRoot(), *m_sceneManager);
    LoadSkeletonFromDisk();

    // Set up the cameraman
    m_cameraMan = new OgreBites::CameraMan(camNode);
    m_cameraMan->setStyle(OgreBites::CameraStyle::CS_ORBIT);
    m_cameraMan->setYawPitchDist(Ogre::Radian(0), Ogre::Radian(0.3f), 50);

    addInputListener(m_cameraMan);
}

void MinimalOgre::shutdown() {
    delete m_trays;
    m_skeletonRenderer.reset();
    OgreBites::ApplicationContextBase::shutdown();
}

bool MinimalOgre::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    OgreBites::ApplicationContext::frameRenderingQueued(evt);

    m_skeletonRenderer = std::make_unique<SkeletonRenderer>(*getRoot(), *m_sceneManager);

    m_skeleton->draw(*m_skeletonRenderer);

    PickRequest currentPickRequest;
    {
        std::scoped_lock(m_pickRequestMutex);
        currentPickRequest = m_pickRequest;
        m_pickRequest = {};
    }

    CJoint* pickedJoint = nullptr;

    const auto* camera = m_sceneManager->getCamera("myCam");
    if (currentPickRequest && camera) {
        const auto screenWidth = camera->getViewport()->getActualWidth();
        const auto screenHeight = camera->getViewport()->getActualHeight();
        const auto x = static_cast<Ogre::Real>(currentPickRequest->first) / screenWidth;
        const auto y = static_cast<Ogre::Real>(currentPickRequest->second) / screenHeight;
        pickedJoint = SkeletonPicker::Pick(
            SkeletonPicker::PickContext{*m_sceneManager, *camera, m_skeletonRenderer->GetBackwardsMapping()}, x, y);
    }

    if (pickedJoint) {
        std::cout << "I picked the thing" << std::endl;
    }
    return true;
}

bool MinimalOgre::frameEnded(const Ogre::FrameEvent&) { return true; }

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

bool MinimalOgre::mousePressed(const OgreBites::MouseButtonEvent& evt) {
    {
        std::scoped_lock lock(m_pickRequestMutex);
        m_pickRequest = std::make_pair(evt.x, evt.y);
    }
    return true;
}
